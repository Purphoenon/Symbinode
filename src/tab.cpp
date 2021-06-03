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

#include "tab.h"
#include <iostream>
#include <QRegularExpression>

Tab::Tab(QQuickItem *parent): QQuickItem (parent)
{
    m_scene = new Scene(parent);
    connect(m_scene, &Scene::fileNameUpdate, this, &Tab::setTitle);
    setAcceptedMouseButtons(Qt::AllButtons);
    setWidth(24);
    setHeight(25);
    view = new QQuickView();
    view->setSource(QUrl(QStringLiteral("qrc:/qml/TabRect.qml")));
    grTab = qobject_cast<QQuickItem *>(view->rootObject());
    grTab->setParentItem(this);
    connect(grTab, SIGNAL(activated()), this, SLOT(activate()));
    connect(grTab, SIGNAL(closed()), this, SLOT(close()));
}

Tab::~Tab() {
    delete m_scene;
    delete grTab;
    delete view;
}

bool Tab::save() {
    return m_scene->saveScene(m_scene->fileName());
}

Scene *Tab::scene() {
    return m_scene;
}

QString Tab::title() {
    QString fileName = m_scene->fileName();
    int idx = fileName.length() - fileName.lastIndexOf("/") - 1;
    QString title = fileName.right(idx);
    if (title.isEmpty()) title = "New";
    return title;
}

void Tab::setSelected(bool select) {
    selected = select;
    grTab->setProperty("selected", selected);
}

void Tab::activate() {
    setSelected(true);
    emit changeActiveTab(this);
}

void Tab::close() {
    emit closedTab(this);
}

void Tab::setTitle(QString fileName, bool modified) {
    int idx = fileName.length() - fileName.lastIndexOf(QRegularExpression("/|\\\\")) - 1;
    QString title = fileName.right(idx);
    if(modified) {
        title = grTab->property("fileName").toString();
        title.append(" *");
    }
    grTab->setProperty("fileName", title);
}
