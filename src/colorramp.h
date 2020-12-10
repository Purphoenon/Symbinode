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

#ifndef COLORRAMP_H
#define COLORRAMP_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>
#include <vector>
#include <QJsonArray>

class ColorRampObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    ColorRampObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QJsonArray stops = {QJsonArray{1, 1, 1, 1}, QJsonArray{0, 0, 0, 0}});
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    std::vector<QVector4D> &stops();
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    void setGradientsStops(QJsonArray stops);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool rampedTex = false;
    bool selectedItem = false;
    bool resUpdated = false;
public slots:
    void gradientAdd(QVector3D color, qreal pos);
    void positionUpdate(qreal pos, int index);
    void colorUpdate(QVector3D color, int index);
    void gradientDelete(int index);
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;    
    unsigned int m_maskTexture = 0;
    std::vector<QVector4D> m_stops;
};

class ColorRampRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core{
public:
    ColorRampRenderer(QVector2D res);
    ~ColorRampRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void colorRamp(const std::vector<QVector4D> &stops);
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int colorFBO;
    unsigned int m_colorTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    unsigned int gradientsSSBO;
    QOpenGLShaderProgram *colorRampShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // COLORRAMP_H
