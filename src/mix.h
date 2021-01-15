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

#ifndef MIX_H
#define MIX_H
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class MixObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    MixObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float factor = 0.5f, int mode = 0, bool includingAlpha = true);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int firstTexture();
    void setFirstTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int secondTexture();
    void setSecondTexture(unsigned int texture);
    QVariant factor();
    void setFactor(QVariant f);
    int mode();
    void setMode(int mode);
    bool includingAlpha();
    void setIncludingAlpha(bool including);
    QVector2D resolution();
    void setResolution(QVector2D res);
    unsigned int &texture();
    void setTexture(unsigned int texture);    
    bool mixedTex = false;
    bool selectedItem = false;
    bool useFactorTexture = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    unsigned int m_firstTexture = 0;
    unsigned int m_secondTexture = 0;
    QVariant m_factor = QVariant(0.5f);
    int m_mode = 0;
    bool m_includingAlpha = true;
    QVector2D m_resolution;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
};

class MixRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    MixRenderer(QVector2D resolution);
    ~MixRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();   
private:
    void mix();
    void updateTextureRes();
    QOpenGLShaderProgram *mixShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *renderTexture;
    unsigned int firstTexture = 0;
    unsigned int secondTexture = 0;
    unsigned int mixFBO;
    unsigned int mixTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int factorTexture = 0;
    float mixFactor = 0.5f;
    QVector2D m_resolution;
    unsigned int VAO;
};

#endif // MIX_H
