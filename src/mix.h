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
#include <string>
#include "FreeImage.h"

class MixObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    MixObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), GLint bpc = GL_RGBA8, float factor = 1.0f, int foregroundOpacity = 100, int backgroundOpacity = 100, int mode = 0, bool includingAlpha = true);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int firstTexture();
    void setFirstTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int secondTexture();
    void setSecondTexture(unsigned int texture);
    void saveTexture(QString fileName);
    QVariant factor();
    void setFactor(QVariant f);
    int mode();
    void setMode(int mode);
    bool includingAlpha();
    void setIncludingAlpha(bool including);
    int foregroundOpacity();
    void setForegroundOpacity(int opacity);
    int backgroundOpacity();
    void setBackgroundOpacity(int opacity);
    QVector2D resolution();
    void setResolution(QVector2D res);
    GLint bpc();
    void setBPC(GLint bpc);
    unsigned int &texture();
    void setTexture(unsigned int texture);    
    bool mixedTex = false;
    bool selectedItem = false;
    bool useFactorTexture = false;
    bool resUpdated = false;
    bool bpcUpdated = true;
    bool texSaving = false;
    QString saveName = "";
signals:
    void updatePreview(unsigned int previewData);
    void textureChanged();
private:
    unsigned int m_firstTexture = 0;
    unsigned int m_secondTexture = 0;
    QVariant m_factor = QVariant(1.0f);
    int m_fOpacity = 100;
    int m_bOpacity = 100;
    int m_mode = 0;
    bool m_includingAlpha = true;
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA8;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
};

class MixRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    MixRenderer(QVector2D resolution, GLint bpc);
    ~MixRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();   
private:
    void mix();
    void updateTextureRes();
    void saveTexture(QString fileName);
    QOpenGLShaderProgram *mixShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *renderTexture;
    unsigned int firstTexture = 0;
    unsigned int secondTexture = 0;
    unsigned int mixFBO;
    unsigned int mixTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int factorTexture = 0;
    float mixFactor = 1.0f;
    QVector2D m_resolution;
    GLint m_bpc = GL_RGBA8;
    unsigned int VAO;
    int currentMode = 0;
    QList<std::string> blendFunc {"normal", "mixMode", "overlay", "screen", "soft_light", "hard_light",
                               "lighten", "color_dodge", "color_burn", "darken", "add", "subtract",
                               "multiply", "divide", "difference", "exclusion"};
};

#endif // MIX_H
