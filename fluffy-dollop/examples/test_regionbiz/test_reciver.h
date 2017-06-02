#ifndef TEST_RECIVER_H
#define TEST_RECIVER_H

#include <iostream>
#include <set>
#include <QObject>

class TestReciver: public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void onCurrentChange( uint64_t prev_id,
                      uint64_t new_id )
    {
        std::cout << "Current!!! prev: " << prev_id
                  << ", new: " << new_id << std::endl;
    }

    void onSelectedSet( std::vector< uint64_t > sel_ids, std::vector<uint64_t> desel_ids )
    {
        std::cout << "Selected " << sel_ids.size() << " elements, " <<
                  "desel " << desel_ids.size() << " elements" << std::endl;
    }

    void onCenterOn( uint64_t id )
    {
        std::cout << "Center on!!! id: " << id << std::endl;
    }
};

#endif // TEST_RECIVER_H
