#include <QApplication>
#include <QObject>

#include <csignal>

#include "mainwindow.h"

static QApplication *appPtr = nullptr;
void signalHandler(int signal)
{
    std::cout << "\nsignalHandler(" << signal << ")" << std::endl;
    if (appPtr)
        appPtr->exit(128 + signal);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    appPtr = &a;
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    MainWindow window;
    window.show();

    return a.exec();
}
