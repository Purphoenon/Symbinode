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

#ifndef TAB_H
#define TAB_H
#include <QQuickItem>
#include <QQuickView>
#include "scene.h"

class Tab: public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(Scene *scene READ scene)
public:
    Tab(QQuickItem *parent = nullptr);
    ~Tab();
    Scene *scene();    
    void setTitle(QString fileName, bool modified);
    void setSelected(bool select);
public slots:
    void activate();
    void close();
signals:
    void changeActiveTab(Tab *tab);
    void closedTab(Tab *tab);
    void movedTab(Tab *tab, int index);
private:
    Scene *m_scene = nullptr;
    QQuickView *view;
    QQuickItem *grTab;
    float dragX = 0.0f;
    float dragY = 0.0f;
    bool selected = true;
};

#endif // TAB_H
