#ifndef FILE_LOAD_MANAGER_H
#define FILE_LOAD_MANAGER_H

#include <list>
#include <QDir>

#include <regionbiz/rb_files.h>

namespace regionbiz {

class FileLoadManager
{
public:
    void init( QString file_list, QString cache_dir );

    void addFileOnLoad( BaseFileKeeperPtr file);
    bool isFileOnLoad( BaseFileKeeperPtr file );
    void stopFileLoad( BaseFileKeeperPtr file );

    std::vector< BaseFileKeeperPtr > getFilesForLoad();

private:
    void loadFileList();
    void saveFileList();

    std::list< QString > _files_on_load;
    std::map< QString, BaseFileKeeper::FileType > _types;
    QDir _cache_dir;
    QString _load_file;
};

}

#endif // FILE_LOAD_MANAGER_H
