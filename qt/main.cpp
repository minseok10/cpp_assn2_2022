#include "GameController.h"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>

int main(int argc, char* argv[])
{
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");

    QGuiApplication app(argc, argv);

    GameController gameController;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("gameController", &gameController);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("ResistanceCoup", "Main");

    return app.exec();
}
