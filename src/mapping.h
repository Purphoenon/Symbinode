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

#ifndef MAPPING_H
#define MAPPING_H

#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>

class MappingObject: public QQuickFramebufferObject
{
    Q_OBJECT
public:
    MappingObject(QQuickItem *parent = nullptr, QVector2D resolution = QVector2D(1024, 1024), float inputMin = 0.0f, float inputMax = 1.0f, float outputMin = 0.0f, float outputMax = 1.0f);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    unsigned int &texture();
    void setTexture(unsigned int texture);
    unsigned int maskTexture();
    void setMaskTexture(unsigned int texture);
    unsigned int sourceTexture();
    void setSourceTexture(unsigned int texture);
    float inputMin();
    void setInputMin(float value);
    float inputMax();
    void setInputMax(float value);
    float outputMin();
    void setOutputMin(float value);
    float outputMax();
    void setOutputMax(float value);
    QVector2D resolution();
    void setResolution(QVector2D res);
    bool mappedTex = false;
    bool selectedItem = false;
    bool resUpdated = false;
signals:
    void updatePreview(QVariant previewData, bool useTexture);
    void textureChanged();
private:
    QVector2D m_resolution;
    unsigned int m_sourceTexture = 0;
    unsigned int m_texture = 0;
    unsigned int m_maskTexture = 0;
    float m_inputMin = 0.0f;
    float m_inputMax = 1.0f;
    float m_outputMin = 0.0f;
    float m_outputMax = 1.0f;
};

class MappingRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    MappingRenderer(QVector2D res);
    ~MappingRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    void map();
    void updateTexResolution();
    QVector2D m_resolution;
    unsigned int mappingFBO;
    unsigned int m_mappingTexture = 0;
    unsigned int m_sourceTexture = 0;
    unsigned int maskTexture = 0;
    unsigned int textureVAO = 0;
    QOpenGLShaderProgram *mappingShader;
    QOpenGLShaderProgram *checkerShader;
    QOpenGLShaderProgram *textureShader;
};

#endif // MAPPING_H
