/*
 * Copyright © 2020 Gukova Anastasiia
 * Copyright © 2020 Gukov Anton <fexcron@gmail.com>
 *
 *
 * This file is part of Symbinode.
 *
 * Symbinode is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Symbinode is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Symbinode.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QQmlApplicationEngine>
#include "backgroundobject.h"
#include "preview.h"
#include "preview3d.h"
#include "scene.h"
#include "node.h"
#include "mainwindow.h"
#include <iostream>
//#include "vld.h"

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

    if(QApplication::arguments().size() > 1) {
        const QString filename = QApplication::arguments().at(1);
        if(engine.rootObjects().size() > 0) {
            MainWindow *win = static_cast<MainWindow*>(engine.rootObjects().at(0));
            win->loadFile(filename);
        }
    }

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
