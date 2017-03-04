#ifndef MATCHINGWIDGET_H
#define MATCHINGWIDGET_H

#include "matchingstate.h"
//#include <dpf/geo/data_types/geo_data.h>
#include <ctrcore/plugin/embifacenotifier.h>

#include <QWidget>
#include <QString>
#include <QSlider>
#include <QToolButton>
#include <QStack>

namespace visualize_system {

class TransformingState;

class MatchingWidget : public QWidget
{
    Q_OBJECT

public:
    MatchingWidget(uint rasterId, uint visualizerId, const QPolygonF& polygonInSceneCoords);
    ~MatchingWidget();
    uint getMatchingProviderId();
    quint64 getVisualizerWindowId();

protected slots:
    void slotSetTransparency(int value);
    void slotTransform(bool);
    void slotSetPoint(bool);
    void slotDeletPoint();
    void slotApply();
    void slotSave();
    void slotUndo();
    void widgetClose();
    void slot_transformFinished();
    void slot_matchingFinished();
    void slot_setTransparancy(int);
    void slot_enabledApplyButton(bool);
    void slot_enabledDeleteButton(bool);

private:
    struct StackData
    {
        StackData() : saver(0) {}
        qulonglong saver;
        QPolygonF vertexInSceneCoords;
        QList<MatchingState::MatchingPoint> matchingPoints;
//        dpf::geo::PolinomialGeoData polinomialGeoData;
    };

    const uint _rasterId;
    const uint _visualizerId;
    StackData _currentStackData;
    QStack<StackData> _stack;
    QSlider * _slider;
    QToolButton * _tbTransform, * _tbSetPoint,  * _tbDeletPoint, * _tbApply, * _tbSave, * _tbUndo, * _tbClose;
    QSharedPointer<TransformingState> _transformingState;
    QSharedPointer<MatchingState> _matchingState;
    EmbIFaceNotifier* _iface;
//    dpf::geo::GeoData* _savedGeoData;
    bool _matchingChanged;

signals:
    void signal_getRasterParams(uint rasterId, QPixmap& pixmap, int& zLevel, QPolygonF& vertexInSceneCoords);
    void signal_setVisibleForRaster(uint rasterShow, uint rasterHide);
    void signal_applyTransformation(uint rasterId, const QPolygonF& polygonInSceneCoords);
    void signal_getPolygonInSceneCoords(uint rasterId, QPolygonF &polygonInSceneCoords);
//    void signal_applyMatching(uint rasterId, QList<MatchingState::MatchingPoint> list, dpf::geo::PolinomialGeoData & data);
};

}

#endif









