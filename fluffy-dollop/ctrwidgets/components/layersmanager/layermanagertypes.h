#ifndef LAYERMANAGERTYPES_H
#define LAYERMANAGERTYPES_H

namespace layers_manager_form
{

const int DepthScenePixelInMeter = 100;

enum class LayerTypes
{
    Base = 1,
    Etalon,
    Other
};

enum class ItemTypes
{
    Invalide = 0,
    Layer = 1,
    Rasters, // [0..N] Raster
    Raster,
    Vectors, // [0..N] Vector
    Vector,
    Marks, // [0..N] Defect and\or Photo and\orPhoto3d
    Defect,
    Photo,
    Photo3d,
    FacilityEtalonRaster,
    FacilityPolygonOnPlan
};

enum class ZoomState
{
    TenCentimeters,
    OneMeter,
    TenMeter,
    HundredMeter
};

}

#endif // LAYERMANAGERTYPES_H
