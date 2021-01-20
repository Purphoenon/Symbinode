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

#ifndef COLOR_H
#define COLOR_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class ColorObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    ColorObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QVector3D color = QVector3D(1, 1, 1));
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    QVector3D color();
    void setColor(QVector3D color);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool createdTexture = true;
    bool resUpdated = false;
signals:
    void updatePreview(unsigned int previewData);
    void textureChanged();
private:
    QVector2D m_resolution;
    QVector3D m_color = QVector3D(1, 1, 1);
    unsigned int m_texture = 0;
};

class ColorRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    ColorRenderer(QVector2D res);
    ~ColorRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createColor();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int colorFBO;
    unsigned int m_colorTexture = 0;
    unsigned int colorVAO = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *colorShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // COLOR_H
