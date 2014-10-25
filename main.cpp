#include "vpkcompare.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    VpkCompare w;
    w.show();
    return a.exec();
}
