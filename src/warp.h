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

#ifndef WARP_H
#define WARP_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class WarpObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    WarpObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float intensity = 0.1f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    unsigned int warpTexture();
    void setWarpTexture(unsigned int texture);
    float intensity();
    void setIntensity(float intensity);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool warpedTex = false;
    bool resUpdated = false;
    bool selectedItem = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void changedTexture();
private:
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int m_warpTexture = 0;
    unsigned int m_maskTexture = 0;
    float m_intensity = 0.1f;
};

class WarpRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    WarpRenderer(QVector2D res);
    ~WarpRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void createWarp();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int m_sourceTexture = 0;
    unsigned int m_warpTexture = 0;
    unsigned int m_warpedTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    unsigned int warpFBO = 0;
    QOpenGLShaderProgram *warpShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // WARP_H
