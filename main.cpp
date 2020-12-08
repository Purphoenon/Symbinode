#include <QApplication>
#include <QQmlApplicationEngine>
#include "backgroundobject.h"
#include "preview.h"
#include "preview3d.h"
#include "scene.h"
#include "node.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    QSurfaceFormat format;
    format.setSamples(16);
    QSurfaceFormat::setDefaultFormat(format);

    qmlRegisterType<BackgroundObject>("backgroundobject", 1, 0, "BackgroundObject");
    qmlRegisterType<PreviewObject>("preview", 1, 0, "PreviewObject");
    qmlRegisterType<Preview3DObject>("preview3d", 1, 0, "Preview3DObject");
    qmlRegisterType<Scene>("scene", 1, 0, "Scene");
    qmlRegisterType<Node>("node", 1, 0, "NodeItem");
    qmlRegisterType<MainWindow>("mainwindow", 1, 0, "MainWindow");
    qRegisterMetaType<Node*>("Node*");
    qRegisterMetaType<QList<Node*>>("QList<Node*>");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
