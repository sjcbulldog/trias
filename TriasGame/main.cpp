#include "TriasGame.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TriasGame w;
    w.show();
    return a.exec();
}
