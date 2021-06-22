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

#ifndef COLORING_H
#define COLORING_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>
#include "FreeImage.h"

class ColoringObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    ColoringObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8, QVector3D color = QVector3D(1, 1, 1));
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    void saveTexture(QString fileName);
    QVector3D color();
    void setColor(QVector3D color);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    bool colorizedTex = false;
    bool selectedItem = false;
    bool resUpdated = false;
    bool bpcUpdated = false;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void textureChanged();
private:
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA8;
    unsigned int m_sourceTexture = 0;
    unsigned int m_texture = 0;
    QVector3D m_color = QVector3D(1, 1, 1);
};

class ColoringRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    ColoringRenderer(QVector2D res, GLint bpc);
    ~ColoringRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void colorize();
    void updateTexResolution();
    void saveTexture(QString fileName);
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA8;
    unsigned int colorFBO;
    unsigned int m_colorTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *coloringShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // COLORING_H
