#ifndef RB_FILES_H
#define RB_FILES_H

#include <memory>
#include <QFile>

namespace regionbiz {

typedef std::shared_ptr< QFile > QFilePtr;
class BaseFileKeeper;
typedef std::shared_ptr< BaseFileKeeper > BaseFileKeeperPtr;
typedef std::vector< BaseFileKeeperPtr > BaseFileKeeperPtrs;
typedef std::map< uint64_t, BaseFileKeeperPtrs > FileKeepersById;

class BaseFileKeeper
{
    friend class RegionBizManager;
    friend class BaseDataTranslator;
    friend class BaseFilesTranslator;

public:
    enum FileType
    {
        FT_NONE,
        FT_PLAN,
        FT_IMAGE,
        FT_DOCUMENT
    };

    enum FileState
    {
        FS_SYNC,
        FS_UNSYNC,
        FS_INVALID
    };

    // base
    BaseFileKeeper( QString path, uint64_t entity_id );

    // params
    virtual FileType getType() = 0;
    QString getPath();
    uint64_t getEntityId();

    // convert
    template< typename Type >
    static std::shared_ptr< Type > convert( BaseFileKeeperPtr ptr )
    {
        return std::dynamic_pointer_cast< Type >( ptr );
    }

    // through tranlator
    QFilePtr getLocalFile();
    FileState getFileState();
    FileState syncFile();
    bool commit();

private:
    static FileKeepersById& getFiles();
    BaseFileKeeperPtr getItself();

    QString _path;
    uint64_t _entity_id;
};

//---------------------------------------------------------

class FileKeeperFabric
{
public:
    static BaseFileKeeperPtr createFile( QString path, uint64_t entity_id,
                                         BaseFileKeeper::FileType type );
};

//---------------------------------------------------------

class PlanFileKeeper: public BaseFileKeeper
{
public:
    struct PlanParams
    {
        double scale_w = 1;
        double scale_h = 1;
        double rotate = 0;
        double x = 0;
        double y = 0;
    };

    PlanFileKeeper(QString path , uint64_t entity_id);
    FileType getType() override;
    PlanParams getPlanParams();
    void setPlanParams( PlanParams params );

private:
    PlanParams _params;
};
typedef std::shared_ptr< PlanFileKeeper > PlanFileKeeperPtr;

//----------------------------------------------------------

class DocumentFileKeeper: public BaseFileKeeper
{
public:
    DocumentFileKeeper(QString path , uint64_t entity_id);
    FileType getType() override;
};
typedef std::shared_ptr< DocumentFileKeeper > DocumentFileKeeperPtr;

//----------------------------------------------------------

class ImageFileKeeper: public BaseFileKeeper
{
public:
    ImageFileKeeper(QString path , uint64_t entity_id);
    FileType getType() override;
};
typedef std::shared_ptr< DocumentFileKeeper > DocumentFileKeeperPtr;

}

#endif // RB_FILES_H
