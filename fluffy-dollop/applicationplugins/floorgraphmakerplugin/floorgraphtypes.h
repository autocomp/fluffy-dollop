#ifndef FLOORGRAPHTYPES_H
#define FLOORGRAPHTYPES_H

namespace floor_graph_maker
{

enum class ElementType {None, Node, Edge};
enum class EdgeType {Wall, Door, Window};
enum class DoorState {Open, Close};

struct EdgeProperty
{
    EdgeType type = EdgeType::Wall;
    double wallWidth = 0.1;
    double wallHeight = 2.5;
    double windowHeightUnderFloor = 1;
    double windowHeight = 1.5;
    DoorState state = DoorState::Open;
};


}

#endif // FLOORGRAPHTYPES_H
