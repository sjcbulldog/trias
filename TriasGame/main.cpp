#include "TriasGame.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("C&T Software");
    QCoreApplication::setOrganizationDomain("www.mewserver.org");
    QCoreApplication::setApplicationName("Trias");
    QCoreApplication::setApplicationVersion("1.0.0");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    TriasGame w;
    w.show();
    return a.exec();
}
