#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // 环境
    qputenv("QT_IM_MODULE", QByteArray("Qt5Input"));
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
