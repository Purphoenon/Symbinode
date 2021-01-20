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

#ifndef ONECHANEL_H
#define ONECHANEL_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class OneChanelObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    OneChanelObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024));
    QQuickFramebufferObject::Renderer *createRenderer() const;
    QVariant value();
    void setValue(QVariant val);
    void setColorTexture(unsigned int texture);
    void setSourceTexture(unsigned int texture);
    unsigned int &texture();
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool useTex = false;
    bool selectedItem = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void updateValue(QVariant data, bool useTexture);
private:
    QVariant m_value;
    unsigned int m_colorTexture = 0;
    unsigned int m_sourceTexture = 0;
    QVector2D m_resolution;
};

class OneChanelRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    OneChanelRenderer(QVector2D resolution);
    ~OneChanelRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createColor();
    void saveTexture(QString dir);
    QOpenGLShaderProgram *renderChanel;
    float val = 0.0f;
    QVector2D m_resolution;
    unsigned int m_colorFBO = 0;
    unsigned int m_colorTexture = 0;
    unsigned int texture = 0;    
    unsigned int VAO = 0;
};

#endif // ONECHANEL_H
