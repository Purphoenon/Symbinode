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

#ifndef VORONOI_H
#define VORONOI_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class VoronoiObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    VoronoiObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), QString voronoiType = "crystals", int scale = 5, int scaleX = 1, int scaleY = 1, float jitter = 1.0f, bool inverse = false, float intensity = 1.0f, float bordersSize = 0.0f, int seed = 1);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int &texture();
    void setTexture(unsigned int texture);
    QString voronoiType();
    void setVoronoiType(QString type);
    int voronoiScale();
    void setVoronoiScale(int scale);
    int scaleX();
    void setScaleX(int scale);
    int scaleY();
    void setScaleY(int scale);
    float jitter();
    void setJitter(float jitter);
    bool inverse();
    void setInverse(bool inverse);
    float intensity();
    void setIntensity(float intensity);
    float bordersSize();
    void setBordersSize(float size);
    int seed();
    void setSeed(int seed);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool generatedVoronoi = true;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void changedTexture();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
    QString m_voronoiType = "crystals";
    int m_scale = 5;
    int m_scaleX = 1;
    int m_scaleY = 1;
    float m_jitter = 1.0f;
    bool m_inverse = false;
    float m_intensity = 1.0f;
    float m_borders = 0.0f;
    int m_seed = 1;
};

class VoronoiRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core
{
public:
    VoronoiRenderer(QVector2D resolution);
    ~VoronoiRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createVoronoi();
    void updateTexResolution();
    QOpenGLShaderProgram *generateVoronoi;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *renderTexture;
    unsigned int voronoiFBO;
    unsigned int voronoiVAO, textureVAO;
    unsigned int voronoiTexture;
    unsigned int maskTexture = 0;
    QVector2D m_resolution;
    QString m_voronoiType;
};

#endif // VORONOI_H
