#include "rbp_files_translator_ftp.h"

#include <iostream>
#include <QTimer>

using namespace regionbiz;

// register translator
REGISTER_TRANSLATOR(FilesTranslatorFtp)

QString FilesTranslatorFtp::getTranslatorName()
{
    return "ftp";
}

void FilesTranslatorFtp::loadFunctions()
{
    _get_file = std::bind( &FilesTranslatorFtp::getFile, this, std::placeholders::_1 );
    _get_file_state = std::bind( &FilesTranslatorFtp::getFileState, this, std::placeholders::_1 );
    _sync_file = std::bind( &FilesTranslatorFtp::syncFile, this, std::placeholders::_1 );
    _add_file = std::bind( &FilesTranslatorFtp::addFile, this,
                           std::placeholders::_1, std::placeholders::_2,
                           std::placeholders::_3, std::placeholders::_4 );
}

bool regionbiz::FilesTranslatorFtp::initBySettings(QVariantMap settings)
{
    QObject::connect( &_ftp, SIGNAL( commandFinished( int, bool )),
                      this, SLOT( ftpCommandFinished( int, bool )));
    QObject::connect( &_ftp, SIGNAL(listInfo(QUrlInfo)),
                      this, SLOT(addToList(QUrlInfo)));

    // NOTE can't indicate ftp loading process
    // connect(ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
    //         this, SLOT(updateDataTransferProgress(qint64,qint64)));

    if( settings.contains( "cache_dir" ))
    {
        QString cache_dir_str = settings[ "cache_dir" ].toString();
        _cache_dir = QDir( cache_dir_str );
    }
    else
        _cache_dir = QDir( DEFAULT_CACHE_DIR );

    if( !settings.contains( "url" ))
        return false;

    QUrl url( settings[ "url" ].toString() );
    if ( url.isValid() )
    {
        _url = url;
        connectByUrl( _url );

        uint delay = DEFAULT_UPDATE_DELAY;
        if( settings.contains( "update_delay" ))
            delay = settings[ "update_delay" ].toUInt();

        QTimer* timer = new QTimer( this );
        QObject::connect( timer, SIGNAL( timeout() ), SLOT( restartSync() ));
        timer->start( delay );
    }

    return true;
}

void FilesTranslatorFtp::ftpCommandFinished( int id, bool error )
{
    // TODO use specific ftp wrapper
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
                      << _ftp.errorString().toUtf8().data() << std::endl;
            break;
        }
        else
            std::cerr << "Connect to Ftp host OK" << std::endl;

        // TODO load sync data from file
        restartSync();

        break;
    }

    case QFtp::Login:
    {
        if( error )
        {
            std::cerr << "Can't login to Ftp host: "
                      << _ftp.errorString().toUtf8().data() << std::endl;
            break;
        }

        break;
    }

    case QFtp::List:
    {
        if( error )
        {
            std::cerr << "Can't list on Ftp: "
                      << _ftp.errorString().toUtf8().data() << std::endl;
            break;
        }

        // through all parents if needed
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
                        QString path_child = getFullPath( child );
                        int id_cd = _ftp.cd( path_child );
                        _commands[ id_cd ] = QFtp::Cd;
                        _current_node = child;

                        int id = _ftp.list();
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
                    // TODO save sync info to file
                    _block_update = false;
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
        if( !error )
        {
            auto iter = _put_get_files.find( id );
            if( iter != _put_get_files.end() )
            {
                auto file = (*iter).second;
                _put_get_files.erase( iter );
                onAddFile( file );
            }
        }
        else
        {
            std::cerr << "Can't put on Ftp: "
                      << _ftp.errorString().toUtf8().data() << std::endl;
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
                onSyncFile( file );
            }
        }
        else
        {
            std::cerr << "Can't get on Ftp: "
                      << _ftp.errorString().toUtf8().data() << std::endl;
        }

        // close input file
        _input_file.flush();
        _input_file.close();
        break;
    }

    case QFtp::Cd:
    case QFtp::None:
    case QFtp::SetTransferMode:
    case QFtp::SetProxy:
    case QFtp::RawCommand:
    {
        if( error )
        {
            std::cerr << "Some wrong with Ftp: "
                      << _ftp.errorString().toUtf8().data() << std::endl;
            break;
        }
    }

    case QFtp::Remove:
    case QFtp::Mkdir:
    case QFtp::Rmdir:
    case QFtp::Rename:
    {
        // TODO add this Ftp options
        break;
    }

    }

    // if somth wrong - reconnect
    if( error )
        closeConnection();
}

void FilesTranslatorFtp::addToList(QUrlInfo info)
{
    _current_node->appendUrlInfo( info, _current_node );
}

QFilePtr FilesTranslatorFtp::getFile( BaseFileKeeperPtr file_ptr )
{
    bool local( true );
    QString abs_path = getFullPath( file_ptr, local );
    QFileInfo file_info( abs_path );
    if( !file_info.exists() )
        return QFilePtr( new QFile() );
    else
    {
        QFile* file_star =  new QFile( abs_path );
        QFilePtr file_by_name = QFilePtr( file_star );
        return file_by_name;
    }
}

BaseFileKeeper::FileState FilesTranslatorFtp::getFileState( BaseFileKeeperPtr file )
{
    // find in tree
    bool local( true );
    QString ftp_pth = getFullPath( file, !local );
    QStringList node_names = ftp_pth.split( SEPARATOR,
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
            return BaseFileKeeper::FS_INVALID;
    }

    // check that file
    if( !cur_node->getInfo().isFile() )
       return BaseFileKeeper::FS_INVALID;

    // get file on local
    QString local_path = getFullPath( file, local );
    QFileInfo file_info( local_path );
    if( !file_info.exists() )
        return BaseFileKeeper::FS_UNSYNC;

    // get last modifed
    QDateTime last_modified_on_ftp = cur_node->getInfo().lastModified();
    QDateTime last_modified_local = file_info.lastModified();
    uint64_t size_on_ftp = cur_node->getInfo().size();
    uint64_t size_local = file_info.size();
    if( last_modified_on_ftp <= last_modified_local
            && size_local == size_on_ftp )
        return BaseFileKeeper::FS_SYNC;
    else
        return BaseFileKeeper::FS_UNSYNC;
}

BaseFileKeeper::FileState FilesTranslatorFtp::syncFile( BaseFileKeeperPtr file )
{
    auto state = getFileState( file );
    switch( state )
    {
    case BaseFileKeeper::FS_INVALID:
        break;
    case BaseFileKeeper::FS_SYNC:
    {
        onSyncFile( file );
        break;
    }

    case BaseFileKeeper::FS_UNSYNC:
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
        int id_cd = _ftp.cd( ftp_dir );
        _commands[ id_cd ] = QFtp::Cd;

        // create and open file
        _input_file.setFileName( local_path );
        _input_file.open( QFile::WriteOnly );

        // start geting
        int id = _ftp.get( ftp_file, &_input_file );
        _commands[ id ] = QFtp::Get;
        _put_get_files[ id ] = file;

        break;
    }

    }

    return state;
}

BaseFileKeeperPtr FilesTranslatorFtp::addFile( QString local_path, QString inner_path,
                                               BaseFileKeeper::FileType type, uint64_t entity_id )
{
    auto file = FileKeeperFabric::createFile( inner_path, entity_id, type );
    if( file )
    {
        //appendFile( file );

        bool local( true );
        QString inner_local_path = getFullPath( file, local );
        QFile local_file( local_path );
        QFileInfo inner_info( inner_local_path );
        if( !inner_info.dir().exists() )
            inner_info.dir().mkpath( "." );
        if( local_file.copy( inner_local_path ))
        {
            QFile inner_file( inner_local_path );
            inner_file.open( QFile::ReadOnly );
            QByteArray data = inner_file.readAll();

            // TODO use specific ftp wrapper
            QString category = getFileCategoryByType( type );
            int id_cd = _ftp.cd( SEPARATOR + category );
            _commands[ id_cd ] = QFtp::Cd;
            int make_id = _ftp.mkdir( file->getPath().split( "/", QString::SkipEmptyParts ).at( 0 ));
            _commands[ make_id ] = QFtp::Mkdir;

            QString outer_path = getFullPath( file, !local );
            int id = _ftp.put( data, outer_path );
            _commands[ id ] = QFtp::Put;
            _put_get_files[ id ] = file;
        }
    }
    return file;
}

QString FilesTranslatorFtp::getFullPath(FtpTreeNodePtr node)
{
    if( node == _root_node )
        return "/";

    QString path = node->getInfo().name();
    QString parent_path = getFullPath( node->getParent() );
    if( "/" == parent_path )
        parent_path = "";
    return parent_path + "/" + path;
}

void FilesTranslatorFtp::loginByUrl(QUrl url)
{
    int id;
    if (!url.userName().isEmpty())
        id = _ftp.login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
    else
        id = _ftp.login();
    _commands[ id ] = QFtp::Login;
}

void FilesTranslatorFtp::connectByUrl(QUrl url)
{
    int id_conn = _ftp.connectToHost( url.host(), url.port(21) );
    _commands[ id_conn ] = QFtp::ConnectToHost;
}

void FilesTranslatorFtp::closeConnection()
{
    // clear tree
    _root_node = nullptr;
    _current_node = nullptr;
    _block_update = false;

    // close connect
    int id = _ftp.close();
    _commands[ id ] = QFtp::Close;
}

QString FilesTranslatorFtp::getFileCategoryByType(BaseFileKeeper::FileType type)
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

QString FilesTranslatorFtp::getFileCategoryByType(BaseFileKeeperPtr file)
{
    return getFileCategoryByType( file->getType() );
}

QString FilesTranslatorFtp::getFullPath(BaseFileKeeperPtr file_ptr, bool local)
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

void FilesTranslatorFtp::restartSync()
{
    if( _block_update )
        return;

    _block_update = true;

    loginByUrl( _url );

    //_commands.clear();
    int id_cd = _ftp.cd( "/" );
    _commands[ id_cd ] = QFtp::Cd;

    // show root dir
    int id = _ftp.list( "/" );
    _commands[ id ] = QFtp::List;

    // init nodes
    if( !_root_node )
        _root_node = FtpTreeNodePtr( new FtpTreeNode );
    _current_node = _root_node;
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
