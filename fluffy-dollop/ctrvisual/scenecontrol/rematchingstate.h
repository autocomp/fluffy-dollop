#ifndef REMATCHINGSTATE_H
#define REMATCHINGSTATE_H

#include <ctrvisual/state//scrollbasestate.h>

namespace visualize_system {

class RematchingState : public ScrollBaseState
{
public:
    RematchingState(uint rasterId, QPolygonF sceneVertex);
    ~RematchingState();
    uint getRasterId();
    void setRasterScenePolygon(const QPolygonF& polygonInSceneCoords);
    bool isChanged();

protected:
    enum STATE
    {
        DONT_EDIT,
        MOVING,
        RESIZING,
        TURN,
        SET_ORIGINAL_SIZE,
        PROPORTIONAL_RESIZING,
        SET_POINT_ON_RASTER,
        SET_POINT_ON_MAP,
        MOVE_POINT_ON_RASTER,
        MOVE_POINT_ON_MAP,
        SCROLL_MAP
    };

    QPointF convertPolarToDecar(double const & r, double const & fi, const QPointF center)const;
    void convertDecartToPolar(QPointF const & p, double& r, double& fi)const;
    double getLenght(const QPointF& p1, const QPointF& p2)const;
    int normalizeDegree(int degree);
    bool check(double coef, QPointF p1, QPointF p2);

    const uint _rasterId;
    QCursor _cursor;
    STATE _state;
    QPolygonF _sceneVertex;
    QPointF _lastMousePos;
    bool _isChanged;
};

}

#endif // CHOICEOBJETCSTATE_H
