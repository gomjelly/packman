#include "packman.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    packman w;
    w.show();
    return a.exec();
}
