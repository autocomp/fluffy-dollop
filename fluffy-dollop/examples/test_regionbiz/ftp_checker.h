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

    void onTimeout()
    {
        using namespace regionbiz;

        auto mngr = RegionBizManager::instance();

        mngr->subscribeFileAdded( this, SLOT( onAdd() ));
        mngr->subscribeFileSynced( this, SLOT( onSync( regionbiz::BaseFileKeeperPtr )));

        auto region = mngr->getBaseEntity( 1 );
        auto files = region->getFiles();
        if( files.empty() )
            return;

        BaseFileKeeperPtr file = files.at( 0 );
        auto state = file->getFileState();

        if( state == BaseFileKeeper::FS_UNSYNC )
            file->syncFile();
        else
        {
            QFilePtr file_ptr = file->getLocalFile();
            bool open = file_ptr->open(QFile::ReadOnly);
            QByteArray data = file_ptr->readAll();
            std::cerr << "Synced file: " << data.data() << std::endl;
        }
    }
};

#endif // FTP_CHECKER_H
