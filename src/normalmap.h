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

#ifndef NORMALMAP_H
#define NORMALMAP_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class NormalMapObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    NormalMapObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float strenght = 6.0f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int grayscaleTexture();
    void setGrayscaleTexture(unsigned int texture);
    float strenght();
    void setStrenght(float strenght);
    QVector2D resolution();
    void setResolution(QVector2D res);
    unsigned int &normalTexture();
    void setNormalTexture(unsigned int texture);
    bool selectedItem = false;
    bool normalGenerated = false;
    bool resUpdated = false;
signals:
    void updatePreview(unsigned int previewData);
    void textureChanged();
private:
    unsigned int m_grayscaleTexture = 0;
    float m_strenght = 6.0f;
    QVector2D m_resolution;
    unsigned int m_normalTexture = 0;
};

class NormalMapRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    NormalMapRenderer(QVector2D resolution);
    ~NormalMapRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    unsigned int m_grayscaleTexture = 0;
    unsigned int m_normalTexture = 0;
    float strenght = 3.0f;
    QVector2D m_resolution;
    unsigned int VAO = 0;
    unsigned int textureVAO = 0;
    unsigned int normalMapFBO = 0;
    QOpenGLShaderProgram *normalMap;
    QOpenGLShaderProgram *textureShader;

    void createNormalMap();
    void updateTexResolution();
};

#endif // NORMALMAP_H
