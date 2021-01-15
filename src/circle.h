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

#ifndef CIRCLE_H
#define CIRCLE_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class CircleObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    CircleObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), int interpolation = 1, float radius = 0.5f, float smooth = 0.01f, bool useAlpha = true);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int &texture();
    void setTexture(unsigned int texture);
    int interpolation();
    void setInterpolation(int interpolation);
    float radius();
    void setRadius(float radius);
    float smooth();
    void setSmooth(float smooth);
    bool useAlpha();
    void setUseAlpha(bool use);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool generatedCircle = true;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void changedTexture();
private:
    QVector2D m_resolution;
    unsigned int m_texture;
    unsigned int m_maskTexture = 0;
    int m_interpolation = 1;
    float m_radius = 0.5f;
    float m_smooth = 0.01f;
    bool m_useAlpha = true;
};

class CircleRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    CircleRenderer(QVector2D resolution);
    ~CircleRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createCircle();
    void updateTexResolution();
    QOpenGLShaderProgram *generateCircle;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *renderTexture;
    unsigned int circleFBO;
    unsigned int circleVAO, textureVAO;
    unsigned int circleTexture;
    unsigned int maskTexture = 0;
    QVector2D m_resolution;
};

#endif // CIRCLE_H
