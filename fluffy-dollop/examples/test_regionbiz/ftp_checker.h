#ifndef FTP_CHECKER_H
#define FTP_CHECKER_H

#include <QTimer>

#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_entity_filter.h>
#include <regionbiz/rb_files_translator.h>

class FtpChecker: public QObject
{
    Q_OBJECT
public:
    FtpChecker()
    {
        QTimer* timer = new QTimer( this );
        timer->singleShot( 5000, this, SLOT( onTimeout() ));
    }

private Q_SLOTS:
    void onAdd()
    {
        std::cerr << "File added!" << std::endl;
    }

    void onSync( regionbiz::BaseFileKeeperPtr file )
    {
        std::cerr << "File sync!!!" << std::endl;

        regionbiz::QFilePtr file_ptr = file->getLocalFile();
        bool open = file_ptr->open(QFile::ReadOnly);
        QByteArray data = file_ptr->readAll();
        std::cerr << "Geted file: " << data.data() << std::endl;
    }
    void onDelete( regionbiz::BaseFileKeeperPtr file )
    {
        std::cerr << "File deleted! " << file->getPath().toUtf8().data() << std::endl;
    }

    void onTimeout()
    {
        using namespace regionbiz;

        auto mngr = RegionBizManager::instance();

        mngr->subscribeFileAdded( this, SLOT( onAdd() ));
        mngr->subscribeFileSynced( this, SLOT( onSync( regionbiz::BaseFileKeeperPtr )));
        mngr->subscribeFileDeleted( this, SLOT( onDelete( regionbiz::BaseFileKeeperPtr )));

        auto region = mngr->getBaseEntity( 1 );
        auto files = region->getFiles();
        if( files.empty() )
        {
            BaseFileKeeperPtr file =
                    mngr->addFile( "images/photo.jpg", BaseFileKeeper::FT_PLAN, region->getId() );
            file->setName( "Test" );

            std::cerr << "State of file: " << file->getFileState() << std::endl;
            file->commit();
            return;
        }

        BaseFileKeeperPtr file = files.at( 0 );
        auto state = file->getFileState();
        std::cerr << "State of file after: " << state << std::endl;

        if( state == BaseFileKeeper::FS_UNSYNC )
            file->syncFile();
        else if ( state == BaseFileKeeper::FS_SYNC )
        {
            QFilePtr file_ptr = file->getLocalFile();
            bool open = file_ptr->open( QFile::ReadOnly );
            QByteArray data = file_ptr->readAll();
            std::cerr << "Synced file: " << data.data() << std::endl;

            std::cerr << "Delete file: "
                      << mngr->deleteFile( file ) << std::endl;
            std::cerr << "Commit: " << region->commit() << std::endl;
        }
        else
        {
            std::cerr << "Delete file: "
                      << mngr->deleteFile( file ) << std::endl;
            std::cerr << "Commit: " << region->commit() << std::endl;
        }
    }
};

#endif // FTP_CHECKER_H
