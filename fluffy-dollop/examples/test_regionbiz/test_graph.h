#ifndef TEST_GRAPH_H
#define TEST_GRAPH_H

#include <QObject>

class TestGraph: public QObject
{
    Q_OBJECT
public:
    TestGraph();

private Q_SLOTS:
    void onChangeCurrent(uint64_t prev_id , uint64_t new_id);
    void onChangeEntity(uint64_t id);

private:
    void recalculate();

    uint64_t _current_facility_id = 0;
};

#endif // TEST_GRAPH_H
