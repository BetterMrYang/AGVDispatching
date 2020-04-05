#include "agvmonitor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AGVMonitor w;
    w.show();

    return a.exec();
}
