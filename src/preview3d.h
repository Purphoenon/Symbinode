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

#ifndef PREVIEW3D_H
#define PREVIEW3D_H
#include <QQuickFramebufferObject>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLShaderProgram>
#include "FreeImage.h"

class Preview3DObject: public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(int primitivesType READ primitivesType)
public:
    Preview3DObject(QQuickItem *parent = nullptr);
    QQuickFramebufferObject::Renderer *createRenderer() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    QVector3D posCam();
    float zoomCam();
    QQuaternion rotQuat();
    int primitivesType();
    void setPrimitivesType(int type);
    int tilesSize();
    void setTilesSize(int id);
    QVariant albedo();
    QVariant metalness();
    QVariant roughness();
    unsigned int normal();
    QVector2D texResolution();
    void setTexResolution(QVector2D res);
    bool translationView = false;
    bool zoomView = false;
    bool rotationObject = false;
    bool updateRes = false;
    bool useAlbedoTex = false;
    bool useMetalTex = false;
    bool useRoughTex = false;
    bool changedAlbedo = false;
    bool changedRough = false;
    bool changedMetal = false;
    bool changedNormal = false;
public slots:
    void updateAlbedo(QVariant albedo, bool useTexture);
    void updateMetal(QVariant metal, bool useTexture);
    void updateRough(QVariant rough, bool useTexture);
    void updateNormal(unsigned int normal);
private:
    float lastX = 0.0f;
    float lastY = 0.0f;
    float theta = 0.0f;
    float phi = 0.0f;
    QVector3D lastWorldPos;
    QVector3D m_posCam = QVector3D(0.0f, 0.0f, -19.0f);
    float m_zoomCam = 35.0f;
    QQuaternion m_rotQuat;
    QVariant m_albedo = QVector3D(1.0f, 1.0f, 1.0f);
    QVariant m_metalness = 0.0f;
    QVariant m_roughness = 0.2f;
    unsigned int m_normal = 0;
    QVector2D m_texResolution = QVector2D(1024, 1024);
    int m_primitive = 0;
    int m_tile = 1;
};

class Preview3DRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    Preview3DRenderer();
    ~Preview3DRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);
    void synchronize(QQuickFramebufferObject *item);
    void render();
private:
    QOpenGLShaderProgram *pbrShader;
    QOpenGLShaderProgram *equirectangularShader;
    QOpenGLShaderProgram *irradianceShader;
    QOpenGLShaderProgram *prefilteredShader;
    QOpenGLShaderProgram *brdfShader;
    QOpenGLShaderProgram *backgroundShader;
    QOpenGLShaderProgram *textureShader;
    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 model;
    QVector2D m_texResolution = QVector2D(1024, 1024);
    unsigned int hdrTexture = 0;
    unsigned int envCubemap = 0;
    unsigned int irradianceMap = 0;
    unsigned int prefilterMap = 0;
    unsigned int brdfLUTTexture = 0;
    unsigned int sphereVAO = 0;
    unsigned int cubeVAO = 0;
    unsigned int quadVAO = 0;
    unsigned int planeVAO = 0;
    unsigned int indexCount;
    unsigned int wWidth, wHeight;
    QVector3D positionV = QVector3D(0.0f, 0.0f, -19.0f);
    float zoom = 35.0f;
    QQuaternion rotQuat;
    int primitive = 0;
    unsigned int albedoTexture = 0;
    unsigned int metalTexture = 0;
    unsigned int roughTexture = 0;
    unsigned int normalTexture = 0;

    void renderCube();
    void renderQuad();
    void renderSphere();
    void renderPlane();
    void updateMatrix();
    void updateTexResolution();
    void updateOutputsTexture(unsigned int &dst, const unsigned int &src);
};

#endif // PREVIEW3D_H
