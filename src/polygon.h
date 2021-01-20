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

#ifndef POLYGONT_H
#define POLYGONT_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class PolygonObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    PolygonObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int sides = 3, float polygonScale = 0.4f, float smooth = 0.0f, bool useAlpha = true);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int &texture();
    void setTexture(unsigned int texture);
    int sides();
    void setSides(int sides);
    float polygonScale();
    void setPolygonScale(float scale);
    float smooth();
    void setSmooth(float smooth);
    bool useAlpha();
    void setUseAlpha(bool use);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool generatedPolygon = true;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(unsigned int previewData);
    void changedTexture();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
    int m_sides = 3;
    float m_scale = 0.4f;
    float m_smooth = 0.0f;
    bool m_useAlpha = true;
};

class PolygonRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    PolygonRenderer(QVector2D resolution);
    ~PolygonRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createPolygon();
    void updateTexResolution();
    QOpenGLShaderProgram *generatePolygon;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *renderTexture;
    unsigned int polygonFBO;
    unsigned int polygonVAO, textureVAO;
    unsigned int polygonTexture;
    unsigned int maskTexture = 0;
    QVector2D m_resolution;
};

#endif // POLYGONT_H
