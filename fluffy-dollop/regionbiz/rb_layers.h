#ifndef RB_LAYERS_H
#define RB_LAYERS_H

#include <memory>
#include <set>
#include <QString>

namespace regionbiz
{

class Layer
{
public:
    uint64_t getId();
    QString getName();

    std::vector<QString> getMetadataNames();

private:
    Layer( uint64_t id, QString name );

    // params
    uint64_t _id;
    QString _name;

    // data
    std::set< QString > _files;
    std::set< QString > _metadata;
    std::set< uint64_t > _marks;
};
typedef std::shared_ptr< Layer > LayerPtr;

}

#endif // RB_LAYERS_H
