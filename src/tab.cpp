#include "tab.h"
#include <iostream>

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

Scene *Tab::scene() {
    return m_scene;
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
    int idx = fileName.length() - fileName.lastIndexOf("/") - 1;
    QString title = fileName.right(idx);
    if(modified) {
        title = grTab->property("fileName").toString();
        title.append(" *");
    }
    grTab->setProperty("fileName", title);
}
