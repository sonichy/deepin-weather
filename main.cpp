#include "mainwindow.h"
#include <DApplication>
#include <DWidgetUtil>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    qSetMessagePattern("[ %{file}: %{line} ] %{message}");
    MainWindow w;
    w.resize(220,330);
    w.show();
    //w.move(0,100);
    moveToCenter(&w);
    return a.exec();
}
