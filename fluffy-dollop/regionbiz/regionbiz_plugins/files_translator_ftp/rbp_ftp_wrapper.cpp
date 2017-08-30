#include "rbp_ftp_wrapper.h"

#include <iostream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <regionbiz/rb_files_translator.h>

using namespace regionbiz;

FtpWrapper::FtpWrapper()
{
    initFtp();

    // NOTE can't indicate ftp loading process
    // connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
            //         this, SLOT(updateDataTransferProgress(qint64,qint64)));
}

FtpWrapper::~FtpWrapper()
{
    if( _ftp )
        delete _ftp;
}

void FtpWrapper::loginByUrl(QUrl url)
{
    //std::cerr << "Login: " << url.userName().toLatin1().data()
    //          << " " << url.password().toLatin1().data() << std::endl;

    int id;
    if (!url.userName().isEmpty())
        id = _ftp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
    else
        id = _ftp->login();
    _commands[ id ] = QFtp::Login;
}

void FtpWrapper::connectByUrl(QUrl url)
{
    initFtp();

    int id_conn = _ftp->connectToHost( url.host(), url.port(21) );
    _commands[ id_conn ] = QFtp::ConnectToHost;
}

void FtpWrapper::ftpCommandFinished( int id, bool error )
{
    auto iter = _commands.find( id );
    if( _commands.end() == iter )
    {
        std::cerr << "Can't find Ftp command with id = " << id << std::endl;
        return;
    }

    // erase command
    QFtp::Command cmnd = (*iter).second;
    _commands.erase( iter );

    // check command type
    switch( cmnd )
    {
    case QFtp::ConnectToHost:
    {
        if( error )
        {
            std::cerr << "Can't connect to Ftp host: "
                      << _ftp->errorString().toUtf8().data() << std::endl;
            break;
        }
        else
            std::cerr << "Connect to Ftp host OK" << std::endl;

        // restart sync
        restartSync();

        break;
    }

    case QFtp::Login:
    {
        if( error )
        {
            std::cerr << "Can't login to Ftp host: "
                      << _ftp->errorString().toUtf8().data() << std::endl;
            break;

            // connect again
            closeConnection();
        }
        else
            // say about connect
            _callback_connect();

        break;
    }

    case QFtp::List:
    {
        if( error )
        {
            std::cerr << "Can't list on Ftp: "
                      << _ftp->errorString().toUtf8().data() << std::endl;
            break;
        }

        // free all old childs
        _current_node->freeNonUpdatedChilds();

        // through all childs if needed
        while( true )
        {
            bool find_dir = false;
            while( !find_dir )
            {
                FtpTreeNodePtr child = _current_node->getNextChild();
                if( child )
                {
                    QUrlInfo ch_info = child->getInfo();
                    if( ch_info.isDir() )
                    {
                        // set child nodes for update check
                        child->startUpdateChilds();

                        QString path_child = getFullPath( child );
                        int id_cd = _ftp->cd( path_child );
                        _commands[ id_cd ] = QFtp::Cd;
                        _current_node = child;

                        int id = _ftp->list();
                        _commands[ id ] = QFtp::List;

                        find_dir = true;
                    }
                }
                else
                    break;
            }

            // skip while cd on child dir
            if( find_dir )
                break;

            // return to parent dir
            if( !find_dir )
            {
                if( _current_node == _root_node )
                {
                    // finish ftp check
                    _block_update = false;

                    // save sync info to file
                    saveTreeToFile();

                    break;
                }
                _current_node = _current_node->getParent();
            }
        }
        break;
    }

    case QFtp::Close:
    {
        connectByUrl( _url );
        break;
    }

    case QFtp::Put:
    {
        auto iter = _put_get_files.find( id );
        auto file = (*iter).second;
        _put_get_files.erase( iter );

        if( !error )
        {
            if( iter != _put_get_files.end() )
            {
                if( _callback_add )
                    _callback_add( file );
            }
        }
        else
        {
            std::cerr << "Can't put on Ftp: "
                      << _ftp->errorString().toUtf8().data() << std::endl;

            deleteFile( file );
            putFile( file, file->getType() );
        }
        break;
    }

    case QFtp::Get:
    {
        if( !error )
        {
            // erase info and say aboot finish
            auto iter = _put_get_files.find( id );
            if( iter != _put_get_files.end() )
            {
                auto file = (*iter).second;
                _put_get_files.erase( iter );
                if( _callback_sync )
                    _callback_sync( file );
            }
        }
        else
        {
            std::cerr << "Can't get on Ftp: "
                      << _ftp->errorString().toUtf8().data() << std::endl;
        }

        // close input file
        _input_file.flush();
        _input_file.close();
        break;
    }

    case QFtp::Rmdir:
    {
        if( !error )
        {
            auto iter = _put_get_files.find( id );
            if( iter != _put_get_files.end() )
            {
                auto file = (*iter).second;
                _put_get_files.erase( iter );
                if( _callback_del )
                    _callback_del( file );
            }
        }
        else
        {
            std::cerr << "Some wrong with Rmdir: "
                      << _ftp->errorString().toUtf8().data() << std::endl;
        }

        break;
    }

    case QFtp::Cd:
    case QFtp::None:
    case QFtp::SetTransferMode:
    case QFtp::SetProxy:
    case QFtp::RawCommand:
    case QFtp::Remove:
    {
        if( error )
        {
            std::cerr << "Some wrong with Ftp: "
                      << _ftp->errorString().toUtf8().data() << std::endl;
            break;
        }
        break;
    }

    case QFtp::Mkdir:
    case QFtp::Rename:
    {
        // TODO add this Ftp options
        if( error )
        {
            std::cerr << "Some wrong with Ftp: "
                      << _ftp->errorString().toUtf8().data() << std::endl;
            error = false;
        }

        break;
    }

    }

    // if somth wrong - reconnect
    if( error )
        closeConnection();
}

void FtpWrapper::restartSync()
{
    if( _block_update )
        return;

    _block_update = true;

    loginByUrl( _url );

    //_commands.clear();
    int id_cd = _ftp->cd( "/" );
    _commands[ id_cd ] = QFtp::Cd;

    // show root dir
    int id = _ftp->list( "/" );
    _commands[ id ] = QFtp::List;

    // init nodes
    if( !_root_node )
        _root_node = FtpTreeNodePtr( new FtpTreeNode );
    _current_node = _root_node;
}

void FtpWrapper::addToList(QUrlInfo info)
{
    _current_node->appendUrlInfo( info, _current_node );
}

bool FtpWrapper::isValid()
{
    return _root_node != nullptr;
}

FtpTreeNodePtr FtpWrapper::getNodeByPath(QString ftp_path)
{
    QStringList node_names = ftp_path.split( SEPARATOR,
                                             QString::SkipEmptyParts );
    FtpTreeNodePtr cur_node = _root_node;
    for( QString name: node_names )
    {
        auto childs = cur_node->getChilds();
        bool finded = false;
        for( FtpTreeNodePtr child: (*childs) )
        {
            if( child->getInfo().name() == name )
            {
                finded = true;
                cur_node = child;
            }
        }
        if( !finded )
            // if don't exists in tree
            return nullptr;
    }

    return cur_node;
}

void FtpWrapper::putFile(BaseFileKeeperPtr file, BaseFileKeeper::FileType type)
{
    bool local( true );
    QString inner_local_path = getFullPath( file, local );

    QFile inner_file( inner_local_path );
    inner_file.open( QFile::ReadOnly );
    QByteArray data = inner_file.readAll();

    // TODO use specific ftp wrapper
    QString category = getFileCategoryByType( type );
    int id_cd = _ftp->cd( SEPARATOR + category );
    _commands[ id_cd ] = QFtp::Cd;

    int make_id = _ftp->mkdir( file->getPath().split( "/", QString::SkipEmptyParts ).at( 0 ));
    _commands[ make_id ] = QFtp::Mkdir;

    QString outer_path = getFullPath( file, !local );
    int id = _ftp->put( data, outer_path );
    _commands[ id ] = QFtp::Put;
    _put_get_files[ id ] = file;
}

void FtpWrapper::deleteFile( BaseFileKeeperPtr file )
{
    bool local( true );
    QString ftp_path = getFullPath( file, !local );

    // cd to needed dir
    int pos_separator = 0;
    int next_pos_separator = 0;
    while ( next_pos_separator != -1 )
    {
        pos_separator = next_pos_separator;
        next_pos_separator = ftp_path.indexOf( SEPARATOR, pos_separator + 1 );
    }
    QString ftp_dir = ftp_path.left( pos_separator );
    QString ftp_file = ftp_path.right( ftp_path.size() - pos_separator - 1 );
    int id_cd = _ftp->cd( ftp_dir );
    _commands[ id_cd ] = QFtp::Cd;

    int id_del = _ftp->remove( ftp_file );
    _commands[ id_del ] = QFtp::Remove;

    int id_rm = _ftp->rmdir( ftp_dir );
    _commands[ id_rm ] = QFtp::Rmdir;

    _put_get_files[ id_rm ] = file;
}

void FtpWrapper::getFile( BaseFileKeeperPtr file )
{
    // create folder
    bool local( true );
    QString ftp_path = getFullPath( file, !local );
    QString local_path = getFullPath( file, local );
    QFileInfo local_file( local_path );
    if( !local_file.dir().exists() )
        local_file.dir().mkpath( "." );

    // cd to needed dir
    int pos_separator = 0;
    int next_pos_separator = 0;
    while ( next_pos_separator != -1 )
    {
        pos_separator = next_pos_separator;
        next_pos_separator = ftp_path.indexOf( SEPARATOR, pos_separator + 1 );
    }
    QString ftp_dir = ftp_path.left( pos_separator );
    QString ftp_file = ftp_path.right( ftp_path.size() - pos_separator - 1 );
    int id_cd = _ftp->cd( ftp_dir );
    _commands[ id_cd ] = QFtp::Cd;

    // create and open file
    _input_file.setFileName( local_path );
    _input_file.open( QFile::WriteOnly );

    // start geting
    int id = _ftp->get( ftp_file, &_input_file );
    _commands[ id ] = QFtp::Get;
    _put_get_files[ id ] = file;
}

QString FtpWrapper::getFullPath(FtpTreeNodePtr node)
{
    if( node == _root_node )
        return "/";

    QString path = node->getInfo().name();
    QString parent_path = getFullPath( node->getParent() );
    if( "/" == parent_path )
        parent_path = "";
    return parent_path + "/" + path;
}

void FtpWrapper::closeConnection()
{
    // clear tree
    _root_node = nullptr;
    _current_node = nullptr;
    _block_update = false;

    // close connect
    int id = _ftp->close();
    _commands[ id ] = QFtp::Close;
}

QString FtpWrapper::getFileCategoryByType(BaseFileKeeper::FileType type)
{
    switch( type )
    {
    case BaseFileKeeper::FT_PLAN:
        return "plans";
    case BaseFileKeeper::FT_IMAGE:
        return "images";
    case BaseFileKeeper::FT_DOCUMENT:
        return "documents";
    case BaseFileKeeper::FT_NONE:
        return "";
    }
    return "";
}

QString FtpWrapper::getFileCategoryByType(BaseFileKeeperPtr file)
{
    return getFileCategoryByType( file->getType() );
}

QString FtpWrapper::getFullPath(BaseFileKeeperPtr file_ptr, bool local)
{
    QString rel_path = file_ptr->getPath();
    if( local )
    {
        QStringList list = rel_path.split( SEPARATOR, QString::SkipEmptyParts );
        rel_path = list.first();
        list.pop_front();
        for( QString str: list )
            rel_path += QDir::separator() + str;
    }
    QString cahce_dir = _cache_dir.path();
    QString category = getFileCategoryByType( file_ptr );

    QString abs_path = ( local ? cahce_dir : "" ) + ( local ? QString( QDir::separator() ) : SEPARATOR )
            + category + ( local ? QString( QDir::separator() ) : SEPARATOR ) + rel_path;

    return abs_path;
}

void FtpWrapper::loadTreeFromFile()
{
    // read data from file
    QFile json_file( _cache_dir.path() + QDir::separator() + _tree_file );
    if( !json_file.open( QFile::ReadOnly ))
        return;
    QByteArray json_data = json_file.readAll();
    json_file.close();

    // init root node
    if( !_root_node )
        _root_node = FtpTreeNodePtr( new FtpTreeNode );

    // read json
    QJsonDocument json_doc = QJsonDocument::fromJson( json_data );
    QJsonArray main_array = json_doc.array();
    for( QJsonValue obj: main_array )
        recursiveReadFromFile( obj.toObject(), _root_node );
}

void FtpWrapper::recursiveReadFromFile(QJsonObject obj, FtpTreeNodePtr node)
{
    // if dir
    if( obj.contains( "childs" ))
    {
        QUrlInfo info;
        info.setDir( true );
        info.setName( obj[ "name" ].toString() );
        node->appendUrlInfo( info, node );
        auto childs = node->getChilds();
        FtpTreeNodePtr child_node = childs->at( childs->size() - 1 );

        for( QJsonValue child: obj[ "childs" ].toArray() )
            recursiveReadFromFile( child.toObject(), child_node );
    }
    // if file
    else
    {
        QUrlInfo info;
        info.setFile( true );
        info.setName( obj[ "name" ].toString() );
        info.setLastModified( QDateTime::fromString( obj[ "last_modify" ].toString() ));
        info.setSize( obj[ "size" ].toInt() );
        node->appendUrlInfo( info, node );
    }
}

void FtpWrapper::saveTreeToFile()
{
    QJsonArray main_array;
    for( FtpTreeNodePtr main_node: *( _root_node->getChilds() ) )
    {
        QJsonObject main_json_node;
        recursiveSaveToFile( main_json_node, main_node );
        main_array.append( main_json_node );
    }

    QJsonDocument doc;
    doc.setArray( main_array );

    // create folder for file
    if( !_cache_dir.exists() )
        _cache_dir.mkpath( "." );

    // write to file
    QByteArray json_data = doc.toJson();
    QFile json_file( _cache_dir.path() + QDir::separator() + _tree_file );
    if( json_file.open( QFile::WriteOnly ))
    {
        json_file.write( json_data );
        json_file.flush();
        json_file.close();
    }
}

void FtpWrapper::recursiveSaveToFile(QJsonObject &obj, FtpTreeNodePtr node)
{
    // if dir processed
    if( node->getInfo().isDir() )
    {
        obj["name"] = node->getInfo().name();

        QJsonArray childs;
        for( FtpTreeNodePtr child: *( node->getChilds() ))
        {
            QJsonObject ch_obj;
            recursiveSaveToFile( ch_obj, child );
            childs.append( ch_obj );
        }

        obj[ "childs" ] = childs;
    }
    // if file processed
    else
    {
        obj["name"] = node->getInfo().name();
        obj["size"] = node->getInfo().size();
        obj["last_modify"] = node->getInfo().lastModified().toString();
    }
}

void FtpWrapper::initFtp()
{
    if( _ftp )
        delete _ftp;
    _ftp = new QFtp();

    QObject::connect( _ftp, SIGNAL( commandFinished( int, bool )),
                      this, SLOT( ftpCommandFinished( int, bool )));
    QObject::connect( _ftp, SIGNAL(listInfo(QUrlInfo)),
                      this, SLOT(addToList(QUrlInfo)));
}

void FtpWrapper::appendElement( QStringList names,
                                QDateTime last_modif,
                                uint64_t size )
{
    FtpTreeNodePtr node = _root_node;

    while( names.size() )
    {
        QString name = names.front();
        names.pop_front();

        // if dir
        if( names.size() )
        {
            QUrlInfo info;
            info.setDir( true );
            info.setName( name );
            node->appendUrlInfo( info, node );

            auto vec = node->getChilds();
            for( FtpTreeNodePtr ch: *vec )
                if( ch->getInfo().name() == info.name() )
                {
                    node = ch;
                    break;
                }
        }
        // if file
        else
        {
            QUrlInfo info;
            info.setFile( true );
            info.setName( name );
            info.setLastModified( last_modif );
            info.setSize( size );
            node->appendUrlInfo( info, node );
        }
    }

    saveTreeToFile();
}

void FtpTreeNode::appendUrlInfo( QUrlInfo info, FtpTreeNodePtr current_node )
{
    FtpTreeNodePtr child;
    for( FtpTreeNodePtr ch: _childs )
        if( ch->getInfo().name() == info.name() )
        {
            child = ch;
            break;
        }

    if( !child )
    {
        child = FtpTreeNodePtr( new FtpTreeNode );
        _childs.push_back( child );
        child->_parent = current_node;
    }
    child->_updated = true;
    child->_info = info;
}

FtpTreeNodePtr FtpTreeNode::getParent()
{
    return _parent;
}

FtpTreeNodePtr FtpTreeNode::getNextChild()
{
    if( _childs.size() <= _current_child_num )
    {
        _current_child_num = 0;
        return nullptr;
    }

    auto child = _childs.at( _current_child_num );
    ++_current_child_num;
    return child;
}

QUrlInfo FtpTreeNode::getInfo()
{
    return _info;
}

std::vector<FtpTreeNodePtr>* FtpTreeNode::getChilds()
{
    return &_childs;
}

void FtpTreeNode::startUpdateChilds()
{
    for( FtpTreeNodePtr ch: *( getChilds() ))
        ch->_updated = false;
}

void FtpTreeNode::freeNonUpdatedChilds()
{
    if( getChilds()->empty() )
        return;

    for( auto iter = getChilds()->begin(); iter != getChilds()->end(); ++iter )
    {
        if( getChilds()->empty() )
            return;

        if( !((*iter)->_updated ))
        {
            getChilds()->erase( iter );
            iter = getChilds()->begin();
        }
    }
}
