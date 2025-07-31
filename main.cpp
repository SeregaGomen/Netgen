#include <QApplication>
#include <QSurfaceFormat>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QSurfaceFormat format;
    format.setVersion(3, 3); // Укажите нужную версию OpenGL
    format.setProfile(QSurfaceFormat::CoreProfile); // Или CompatibilityProfile
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSwapInterval(1); // Включить вертикальную синхронизацию
    QSurfaceFormat::setDefaultFormat(format);


    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
