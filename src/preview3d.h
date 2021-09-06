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
#include <QTime>
#include <QTimer>
#include <QLabel>
#include "FreeImage.h"

class Preview3DObject: public QQuickFramebufferObject
{
    Q_OBJECT
    Q_PROPERTY(int primitivesType READ primitivesType)
    Q_PROPERTY(int tilesSize READ tilesSize)
    Q_PROPERTY(float heightScale READ heightScale)
    Q_PROPERTY(float emissiveStrenght READ emissiveStrenght)
    Q_PROPERTY(float bloomRadius READ bloomRadius)
    Q_PROPERTY(float bloomIntensity READ bloomIntensity)
    Q_PROPERTY(float bloomThreshold READ bloomThreshold)
    Q_PROPERTY(bool bloom READ bloom)
public:
    Preview3DObject(QQuickItem *parent = nullptr);
    QQuickFramebufferObject::Renderer *createRenderer() const override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void hoverEnterEvent(QHoverEvent *event) override;
    void hoverLeaveEvent(QHoverEvent *event) override;
    QVector3D posCam();
    float zoomCam();
    QQuaternion rotQuat();
    int primitivesType();
    void setPrimitivesType(int type);
    int tilesSize();
    void setTilesSize(int id);
    float heightScale();
    void setHeightScale(float scale);
    float emissiveStrenght();
    void setEmissiveStrenght(float strenght);
    float bloomRadius();
    void setBloomRadius(float radius);
    float bloomIntensity();
    void setBloomIntensity(float intensity);
    float bloomThreshold();
    void setBloomThreshold(float intensity);
    bool bloom();
    void setBloom(bool enable);
    void transformed();
    QVariant albedo();
    QVariant metalness();
    QVariant roughness();
    unsigned int normal();
    unsigned int heightMap();
    unsigned int emission();
    QVector2D texResolution();
    void setTexResolution(QVector2D res);
    bool translationView = false;
    bool zoomView = false;
    bool rotationObject = false;
    bool resized = false;
    bool transformView = false;
    bool updateRes = false;
    bool useAlbedoTex = false;
    bool useMetalTex = false;
    bool useRoughTex = false;
    bool useNormTex = false;
    bool changedAlbedo = false;
    bool changedRough = false;
    bool changedMetal = false;
    bool changedNormal = false;
    bool changedHeight = false;
    bool changedEmission = false;
public slots:
    void updateAlbedo(QVariant albedo, bool useTexture);
    void updateMetal(QVariant metal, bool useTexture);
    void updateRough(QVariant rough, bool useTexture);
    void updateNormal(unsigned int normal);
    void updateHeight(unsigned int height);
    void updateEmission(unsigned int emission);
    void sizeUpdated(bool resize);
private:
    float lastX = 0.0f;
    float lastY = 0.0f;
    float theta = 0.0f;
    float phi = 0.0f;
    QVector3D lastWorldPos;
    QVector3D m_posCam = QVector3D(0.0f, 0.0f, 19.0f);
    float m_zoomCam = 35.0f;
    QQuaternion m_rotQuat;
    QVariant m_albedo = QVector3D(1.0f, 1.0f, 1.0f);
    QVariant m_metalness = 0.0f;
    QVariant m_roughness = 0.2f;
    unsigned int m_normal = 0;
    unsigned int m_height = 0;
    unsigned int m_emission = 0;
    QVector2D m_texResolution = QVector2D(1024, 1024);
    int m_primitive = 0;
    int m_tile = 1;
    float m_heightScale = 0.04f;
    float m_emissiveStrenght = 1.0f;
    float m_bloomRadius = 1.0f;
    float m_bloomIntensity = 0.25f;
    float m_bloomThreshold = 1.0f;
    bool m_bloom = false;
    QTimer m_timer;
};

class Preview3DRenderer: public QQuickFramebufferObject::Renderer, public QOpenGLFunctions_4_4_Core {
public:
    Preview3DRenderer();
    ~Preview3DRenderer();
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override;
    void synchronize(QQuickFramebufferObject *item) override;
    void render() override;
private:
    QOpenGLShaderProgram *pbrShader;
    //QOpenGLShaderProgram *pbrTessShader;
    QOpenGLShaderProgram *equirectangularShader;
    QOpenGLShaderProgram *irradianceShader;
    QOpenGLShaderProgram *prefilteredShader;
    QOpenGLShaderProgram *brdfShader;
    QOpenGLShaderProgram *backgroundShader;
    QOpenGLShaderProgram *textureShader;
    QOpenGLShaderProgram *blurShader;
    QOpenGLShaderProgram *bloomShader;
    QOpenGLShaderProgram *brightShader;
    QMatrix4x4 projection;
    QMatrix4x4 view;
    QMatrix4x4 model;
    QMatrix4x4 viewport;
    QQuaternion rotZ;
    QVector2D m_texResolution = QVector2D(1024, 1024);
    unsigned int hdrTexture = 0;
    unsigned int envCubemap = 0;
    unsigned int irradianceMap = 0;
    unsigned int prefilterMap = 0;
    unsigned int brdfLUTTexture = 0;
    unsigned int cubeMapVAO = 0;
    unsigned int sphereVAO = 0;
    unsigned int cubeVAO = 0;
    unsigned int quadVAO = 0;
    unsigned int planeVAO = 0;
    unsigned int indexCount;
    unsigned int wWidth = 0, wHeight = 0;
    QVector3D positionV = QVector3D(0.0f, 0.0f, 19.0f);
    float zoom = 35.0f;
    QQuaternion rotQuat;
    bool transformating = false;
    int samples = 8;
    int primitive = 0;
    float bloomRadius = 1.0f;
    bool bloom = false;
    int tilesSize = 1;
    float heightScale = 0.04f;
    float emissiveStrength = 1.0f;
    bool useAlbedoTex = false;
    bool useMetalTex = false;
    bool useRoughTex = false;
    bool useNormalTex = false;
    bool useHeightTex = false;
    bool useEmisTex = false;
    unsigned int hdrFBO = 0;
    unsigned int rboDepth = 0;
    unsigned int brightTexture = 0;
    unsigned int pingpongFBO[2];
    unsigned int pingpongBuffer[2];
    unsigned int screenFBO = 0;
    unsigned int screenTexture = 0;
    unsigned int multisampleFBO = 0;
    unsigned int multisampleTexture = 0;
    unsigned int combinedFBO = 0;
    unsigned int combinedTexture = 0;
    unsigned int outputFBO = 0;
    unsigned int albedoTexture = 0;
    unsigned int metalTexture = 0;
    unsigned int roughTexture = 0;
    unsigned int normalTexture = 0;
    unsigned int heightTexture = 0;
    unsigned int emissionTexture = 0;

    void renderCube();
    void renderQuad();
    void renderSphere();
    void renderPlane();
    void renderScene();
    void renderForBloom();
    void brightnessBlur();
    void updateMatrix();
    void updateTexResolution();
    void updateOutputsTexture(unsigned int &dst, const unsigned int &src);
};

#endif // PREVIEW3D_H
