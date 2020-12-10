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

#ifndef BLUR_H
#define BLUR_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class BlurObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    BlurObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float intensity = 0.5f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    float intensity();
    void setIntensity(float intensity);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool bluredTex = false;
    bool resUpdated = false;
    bool selectedItem = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_maskTexture = 0;
    float m_intensity = 0.5f;
};

class BlurRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    BlurRenderer(QVector2D res);
    ~BlurRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createBlur();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    unsigned int m_sourceTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *blurShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // BLUR_H
