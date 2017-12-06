#ifndef UPLOAD_MANAGER_H
#define UPLOAD_MANAGER_H

#include <vector>
#include <QString>

class UploadManager
{
public:
    void addFileId( QString id );
    bool isFileOnUpload( QString id );
    void eraseFile( QString id );
    void clear();
    std::vector< QString > getFilesIds();

private:
    std::vector< QString > _ids;
};

#endif // UPLOAD_MANAGER_H
