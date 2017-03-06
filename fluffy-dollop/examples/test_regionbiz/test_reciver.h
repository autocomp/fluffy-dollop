#ifndef TEST_RECIVER_H
#define TEST_RECIVER_H

#include <iostream>
#include <QObject>

class TestReciver: public QObject
{
    Q_OBJECT
public Q_SLOTS:
    void onSelection( uint64_t prev_id,
                      uint64_t new_id )
    {
        std::cout << "Selection!!! prev: " << prev_id
                  << ", new: " << new_id << std::endl;
    }

    void onCenterOn( uint64_t id )
    {
        std::cout << "Center on!!! id: " << id << std::endl;
    }
};

#endif // TEST_RECIVER_H
