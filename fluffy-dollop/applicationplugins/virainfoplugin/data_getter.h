#ifndef TEST_DATA_GETTER_H
#define TEST_DATA_GETTER_H

#include <QMap>
#include <QSet>

#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

struct AreaData
{
    uint64_t id = 0;

    double area_rent = 0;
    double area_free = 0;
    double area_property = 0;

    int task_work = 0;
    int task_check = 0;
    int task_new = 0;

    int arendators_good = 0;
    int arendators_bad = 0;
    QSet< QString > arendators_good_set;
    QSet< QString > arendators_bad_set;

    double debt;
};

class DataGetter
{
public:
    static DataGetter& instance();
    static AreaData getData( uint64_t id );

private:
    DataGetter(){}
};

#endif // TEST_DATA_GETTER_H
