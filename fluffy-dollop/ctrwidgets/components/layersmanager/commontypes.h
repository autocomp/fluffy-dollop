#ifndef COMMONTYPES_H
#define COMMONTYPES_H

namespace plugin_types
{

enum PixelVisualizerButtons
{
    SCALE_WIDGET,
    GRID,

};

}

namespace transform_state
{
enum class GlobalMode
{
    AllAction,
    MoveAndRotateOnly,
    MoveRotateAndCropOnly,
};

}

#endif // COMMONTYPES_H
