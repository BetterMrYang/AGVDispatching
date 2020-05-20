#ifndef INCLUDE_H
#define INCLUDE_H
#include <QMutex>
#include <QString>
#include <QSemaphore>
extern QMutex mutex_BUFFER; //串口缓冲区所

/*!
 * @breif:AGV数量锁
 * @function:slot_deleteAGVNo,slot_agvAccount,slot_timer_flushCall
 */
extern QSemaphore WRAGV;

struct DijkstraInfo //用于生成新地图的信息
{
    int start_num;
    int end_num;
    double length;
};


#endif // INCLUDE_H
