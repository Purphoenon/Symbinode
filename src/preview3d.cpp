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

#include "preview3d.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>

Preview3DObject::Preview3DObject(QQuickItem *parent): QQuickFramebufferObject (parent)
{
    setAcceptedMouseButtons(Qt::AllButtons);
    //setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *Preview3DObject::createRenderer() const {
    return new Preview3DRenderer();
}

void Preview3DObject::mousePressEvent(QMouseEvent *event) {
    if(event->button() == Qt::MidButton) {
        lastX = event->pos().x();
        lastY = event->pos().y();
        if(event->modifiers() == Qt::ShiftModifier) {
            QMatrix4x4 projection = QMatrix4x4();
            projection.perspective(m_zoomCam, (float)width()/(float)height(), 0.1f, 38.0f);
            QMatrix4x4 view = QMatrix4x4();
            view.translate(0, 0, 19);
            QMatrix4x4 model = QMatrix4x4();
            model.translate(0.0f, 0.0f, 0.0f);
            QVector3D z = QVector3D(0, 0, 0).project(model*view, projection, QRect(0, 0, width(), height()));
            QVector3D viewPos = QVector3D(lastX, lastY, z.z());
            lastWorldPos = viewPos.unproject(model*view, projection, QRect(0, 0, width(), height()));
        }
    }
    else if(event->button() == Qt::RightButton) {
        lastX = event->pos().x();
        lastY = event->pos().y();
    }
    else {
        event->ignore();
    }
}

void Preview3DObject::mouseMoveEvent(QMouseEvent *event) {
    if(event->buttons() == Qt::MidButton && event->modifiers() == Qt::ShiftModifier) {
        QMatrix4x4 projection = QMatrix4x4();
        projection.perspective(m_zoomCam, (float)width()/(float)height(), 0.1f, 38.0f);
        QMatrix4x4 view = QMatrix4x4();
        view.translate(0, 0, 19);
        QMatrix4x4 model = QMatrix4x4();
        model.translate(0.0f, 0.0f, 0.0f);
        QVector3D z = QVector3D(0, 0, 0).project(model*view, projection, QRect(0, 0, width(), height()));
        QVector3D viewPosNew = QVector3D(event->pos().x(), event->pos().y(), z.z());
        QVector3D worldPosNew = viewPosNew.unproject(model*view, projection, QRect(0, 0, width(), height()));
        QVector3D offset = QVector3D(worldPosNew.x() - lastWorldPos.x(), worldPosNew.y() - lastWorldPos.y(), 0);
        m_posCam += offset;
        lastX = event->pos().x();
        lastY = event->pos().y();
        lastWorldPos = worldPosNew;
        translationView = true;
        update();
    }
    else if(event->buttons() == Qt::MidButton) {
        theta += 0.01f*(lastX - event->pos().x());
        phi += 0.01f*(event->pos().y() - lastY);

        float x = 0;
        float y = sin(theta/2);
        float z = 0;
        float w = cos(theta/2);

        QQuaternion rotY = QQuaternion(w, x, y, z);

        x = sin(phi/2);
        y = 0;
        z = 0;
        w = cos(phi/2);

        QQuaternion rotX = QQuaternion(w, x, y, z);

        m_rotQuat = rotX * rotY;
        lastX = event->pos().x();
        lastY = event->pos().y();
        rotationObject = true;
        update();
    }
    else if(event->buttons() == Qt::RightButton && event->modifiers() == Qt::AltModifier) {
        float stepZoom = 0.0f;
        if(event->pos().x() - lastX > 0) {
           if((m_zoomCam - 0.05f*(event->pos().x() - lastX)) < 1.0f) stepZoom = m_zoomCam - 1.0f;
           else stepZoom = 0.05f*(event->pos().x() - lastX);
        }
        else {
            if((m_zoomCam - 0.05f*(event->pos().x() - lastX)) > 45.0f) stepZoom = m_zoomCam - 45.0f;
            else stepZoom = 0.05f*(event->pos().x() - lastX);
        }
        if(stepZoom != 0.0f) {
            m_zoomCam -= stepZoom;
            lastX = event->pos().x();
            lastY = event->pos().y();
            zoomView = true;
            update();
        }
    }
}

void Preview3DObject::wheelEvent(QWheelEvent *event) {
    float stepZoom = 0.0f;
    if(event->angleDelta().y() > 0) {
       if((m_zoomCam - 0.01f*event->angleDelta().y()) < 1.0f) return;
       stepZoom = 0.01f*event->angleDelta().y();
    }
    else {
        if((m_zoomCam - 0.01f*event->angleDelta().y()) > 45.0f) return;
        stepZoom = 0.01f*event->angleDelta().y();
    }
    m_zoomCam -= stepZoom;
    zoomView = true;
    update();
}

QVector3D Preview3DObject::posCam() {
    return m_posCam;
}

float Preview3DObject::zoomCam() {
    return m_zoomCam;
}

QQuaternion Preview3DObject::rotQuat() {
    return m_rotQuat;
}

int Preview3DObject::primitivesType() {
    return m_primitive;
}

void Preview3DObject::setPrimitivesType(int type) {
    m_primitive = type;
    theta = 0;
    phi = 0;
    m_rotQuat = QQuaternion();
    m_zoomCam = 35.0f;
    m_posCam = QVector3D(0, 0, 19);
    update();
}

int Preview3DObject::tilesSize() {
    return m_tile;
}

void Preview3DObject::setTilesSize(int id) {
    m_tile = id + 1;
    update();
}

bool Preview3DObject::isSelfShadow() {
    return m_selfShadow;
}

void Preview3DObject::setSelfShadow(bool enable) {
    m_selfShadow = enable;
    update();
}

float Preview3DObject::heightScale() {
    return m_heightScale;
}

void Preview3DObject::setHeightScale(float scale) {
    m_heightScale = scale;
    update();
}

QVariant Preview3DObject::albedo() {
    return m_albedo;
}

QVariant Preview3DObject::metalness() {
    return m_metalness;
}

QVariant Preview3DObject::roughness() {
    return m_roughness;
}

unsigned int Preview3DObject::normal() {
    return m_normal;
}

unsigned int Preview3DObject::heightMap() {
    return m_height;
}

QVector2D Preview3DObject::texResolution() {
    return m_texResolution;
}

void Preview3DObject::setTexResolution(QVector2D res) {
    m_texResolution = res;
    updateRes = true;
    update();
}

void Preview3DObject::updateAlbedo(QVariant albedo, bool useTexture) {
    useAlbedoTex = useTexture;    
    m_albedo = albedo;
    changedAlbedo = useTexture;
    update();
}

void Preview3DObject::updateMetal(QVariant metal, bool useTexture) {
    useMetalTex = useTexture;
    m_metalness = metal;
    changedMetal = useTexture;
    update();
}

void Preview3DObject::updateRough(QVariant rough, bool useTexture) {
    useRoughTex = useTexture;
    m_roughness = rough;
    changedRough = useTexture;
    update();
}

void Preview3DObject::updateNormal(unsigned int normal) {
    m_normal = normal;
    changedNormal = true;
    update();
}

void Preview3DObject::updateHeight(unsigned int height) {
    m_height = height;
    changedHeight = true;
    update();
}

Preview3DRenderer::Preview3DRenderer() {
    initializeOpenGLFunctions();

    pbrShader = new QOpenGLShaderProgram();
    pbrShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/pbr.vert");
    pbrShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/pbr.frag");
    pbrShader->link();

    equirectangularShader = new QOpenGLShaderProgram();
    equirectangularShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/cubemap.vert");
    equirectangularShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/equirectangular.frag");
    equirectangularShader->link();

    irradianceShader = new QOpenGLShaderProgram();
    irradianceShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/cubemap.vert");
    irradianceShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/irradiance.frag");
    irradianceShader->link();

    prefilteredShader = new QOpenGLShaderProgram();
    prefilteredShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/cubemap.vert");
    prefilteredShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/prefiltered.frag");
    prefilteredShader->link();

    brdfShader = new QOpenGLShaderProgram();
    brdfShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/brdf.vert");
    brdfShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/brdf.frag");
    brdfShader->link();

    backgroundShader = new QOpenGLShaderProgram();
    backgroundShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/background.vert");
    backgroundShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/background.frag");
    backgroundShader->link();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/brdf.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    pbrShader->bind();
    pbrShader->setUniformValue(pbrShader->uniformLocation("irradianceMap"), 0);
    pbrShader->setUniformValue(pbrShader->uniformLocation("prefilterMap"), 1);
    pbrShader->setUniformValue(pbrShader->uniformLocation("brdfLUT"), 2);
    pbrShader->setUniformValue(pbrShader->uniformLocation("albedoMap"), 3);
    pbrShader->setUniformValue(pbrShader->uniformLocation("normalMap"), 4);
    pbrShader->setUniformValue(pbrShader->uniformLocation("metallicMap"), 5);
    pbrShader->setUniformValue(pbrShader->uniformLocation("roughnessMap"), 6);
    pbrShader->setUniformValue(pbrShader->uniformLocation("heightMap"), 7);
    pbrShader->setUniformValue(pbrShader->uniformLocation("albedoVal"), QVector3D(0.8f, 0.4f, 0.0f));
    pbrShader->setUniformValue(pbrShader->uniformLocation("metallicVal"), 0.0f);
    pbrShader->setUniformValue(pbrShader->uniformLocation("roughnessVal"), 0.2f);
    pbrShader->release();

    backgroundShader->bind();
    backgroundShader->setUniformValue(backgroundShader->uniformLocation("environmentMap"), 0);
    backgroundShader->release();

    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->release();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    unsigned int captureFBO;
    unsigned int captureRBO;
    glGenFramebuffers(1, &captureFBO);
    glGenRenderbuffers(1, &captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FIBITMAP *dib(nullptr);
    BYTE* bits(nullptr);
    unsigned int width(0), height(0);
    fif = FreeImage_GetFileType("hdr/Newport_Loft_Ref.hdr", 0);
    if (fif == FIF_UNKNOWN) {
        std::cout << "not get file type" << std::endl;
    }
    dib = FreeImage_Load(fif, "hdr/Newport_Loft_Ref.hdr");
    if (!dib) {
        std::cout << "not load image" << std::endl;
    }
    bits = FreeImage_GetBits(dib);
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, (void*)bits);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    FreeImage_Unload(dib);

    QMatrix4x4 captureProjection;
    captureProjection.perspective(90.0f, 1.0f, 0.1f, 10.0f);
    QMatrix4x4 captureView[6];
    captureView[0].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f));
    captureView[1].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(-1.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f));
    captureView[2].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f));
    captureView[3].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, -1.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f));
    captureView[4].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, 1.0f), QVector3D(0.0f, -1.0f, 0.0f));
    captureView[5].lookAt(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 0.0f, -1.0f), QVector3D(0.0f, -1.0f, 0.0f));

    glGenTextures(1, &envCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    equirectangularShader->bind();
    equirectangularShader->setUniformValue(equirectangularShader->uniformLocation("equirectangularMap"), 0);
    equirectangularShader->setUniformValue(equirectangularShader->uniformLocation("projection"), captureProjection);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    glViewport(0, 0, 512, 512);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for(unsigned int i = 0; i < 6; ++i) {
        equirectangularShader->setUniformValue(equirectangularShader->uniformLocation("view"), captureView[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    equirectangularShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    //irradiance cubemap
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
    glGenTextures(1, &irradianceMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    irradianceShader->bind();
    irradianceShader->setUniformValue(irradianceShader->uniformLocation("projection"), captureProjection);
    glActiveTexture(GL_TEXTURE0);
    irradianceShader->setUniformValue(irradianceShader->uniformLocation("environmentMap"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    glViewport(0, 0, 32, 32);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    for(unsigned int i = 0; i < 6; ++i) {
        irradianceShader->setUniformValue(irradianceShader->uniformLocation("view"), captureView[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderCube();
    }
    irradianceShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &prefilterMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 4);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LOD, 4);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    unsigned int maxMipLevels = 5;
    for(unsigned int mip = 0; mip < maxMipLevels; ++mip) {
        unsigned int mipWidth = 128 * std::pow(0.5, mip);
        unsigned int mipHeight = 128 * std::pow(0.5, mip);
        prefilteredShader->bind();
        prefilteredShader->setUniformValue(prefilteredShader->uniformLocation("environmentMap"), 0);
        prefilteredShader->setUniformValue(prefilteredShader->uniformLocation("projection"), captureProjection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
        glViewport(0, 0, mipWidth, mipHeight);

        float roughness = (float)mip / (float)(maxMipLevels - 1);
        prefilteredShader->setUniformValue(prefilteredShader->uniformLocation("roughness"), roughness);
        for(unsigned int i = 0; i < 6; ++i) {
            prefilteredShader->setUniformValue(prefilteredShader->uniformLocation("view"), captureView[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap, mip);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderCube();
        }
        prefilteredShader->release();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glDisable(GL_BLEND);
    glGenTextures(1, &brdfLUTTexture);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfLUTTexture, 0);
    glViewport(0, 0, 512, 512);
    brdfShader->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    renderQuad();
    brdfShader->release();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &albedoTexture);
    glBindTexture(GL_TEXTURE_2D, albedoTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &metalTexture);
    glBindTexture(GL_TEXTURE_2D, metalTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &roughTexture);
    glBindTexture(GL_TEXTURE_2D, roughTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Preview3DRenderer::~Preview3DRenderer() {
    delete pbrShader;
    delete irradianceShader;
    delete backgroundShader;
    delete equirectangularShader;
    delete prefilteredShader;
    delete textureShader;
    delete brdfShader;
}

QOpenGLFramebufferObject *Preview3DRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(16);
    updateMatrix();
    return new QOpenGLFramebufferObject(size, format);
}

void Preview3DRenderer::synchronize(QQuickFramebufferObject *item) {
    Preview3DObject *previewItem = static_cast<Preview3DObject*>(item);
    wWidth = previewItem->width();
    wHeight = previewItem->height();
    if(primitive != previewItem->primitivesType()) {
        primitive = previewItem->primitivesType();
        positionV = previewItem->posCam();
        zoom = previewItem->zoomCam();
        rotQuat = previewItem->rotQuat();
        updateMatrix();
    }
    if(previewItem->translationView) {
        previewItem->translationView = false;
        positionV = previewItem->posCam();
        updateMatrix();
    }
    if(previewItem->zoomView) {
        previewItem->zoomView = false;
        zoom = previewItem->zoomCam();
        updateMatrix();
    }
    if(previewItem->rotationObject) {
        previewItem->rotationObject = false;
        rotQuat = previewItem->rotQuat();
        updateMatrix();
    }
    if(previewItem->updateRes) {
        previewItem->updateRes = false;
        m_texResolution = previewItem->texResolution();
        updateTexResolution();
    }
    pbrShader->bind();
    pbrShader->setUniformValue(pbrShader->uniformLocation("tilesSize"), previewItem->tilesSize());
    pbrShader->setUniformValue(pbrShader->uniformLocation("enableSelfShadow"), previewItem->isSelfShadow());
    pbrShader->setUniformValue(pbrShader->uniformLocation("heightScale"), previewItem->heightScale());
    pbrShader->setUniformValue(pbrShader->uniformLocation("useAlbMap"), previewItem->useAlbedoTex);
    if(previewItem->useAlbedoTex) {
        if(previewItem->changedAlbedo) {
            previewItem->changedAlbedo = false;
            updateOutputsTexture(albedoTexture, previewItem->albedo().toUInt());
        }
    }
    else {
        pbrShader->setUniformValue(pbrShader->uniformLocation("albedoVal"), qvariant_cast<QVector3D>(previewItem->albedo()));
    }
    pbrShader->setUniformValue(pbrShader->uniformLocation("useMetalMap"), previewItem->useMetalTex);
    if(previewItem->useMetalTex) {
        if(previewItem->changedMetal) {
            previewItem->changedMetal = false;
            updateOutputsTexture(metalTexture, previewItem->metalness().toUInt());
        }
    }
    else {
        pbrShader->setUniformValue(pbrShader->uniformLocation("metallicVal"), previewItem->metalness().toFloat());
    }
    pbrShader->setUniformValue(pbrShader->uniformLocation("useRoughMap"), previewItem->useRoughTex);
    if(previewItem->useRoughTex) {
        if(previewItem->changedRough) {
            previewItem->changedRough = false;
            updateOutputsTexture(roughTexture, previewItem->roughness().toUInt());
        }
    }
    else {
        pbrShader->setUniformValue(pbrShader->uniformLocation("roughnessVal"), previewItem->roughness().toFloat());
    }

    if(previewItem->changedNormal) {
        previewItem->changedNormal = false;
        updateOutputsTexture(normalTexture, previewItem->normal());
    }
    bool useNorm = previewItem->normal() ? true : false;
    pbrShader->setUniformValue(pbrShader->uniformLocation("useNormMap"), useNorm);

    if(previewItem->changedHeight) {
        previewItem->changedHeight = false;
        updateOutputsTexture(heightTexture, previewItem->heightMap());
    }
    bool useHeight = previewItem->heightMap();
    pbrShader->setUniformValue(pbrShader->uniformLocation("useHeightMap"), useHeight);
    pbrShader->release();
}

void Preview3DRenderer::render() {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glClearColor(0.227f, 0.235f, 0.243f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    pbrShader->bind();
    pbrShader->setUniformValue(pbrShader->uniformLocation("projection"), projection);
    pbrShader->setUniformValue(pbrShader->uniformLocation("view"), view);
    pbrShader->setUniformValue(pbrShader->uniformLocation("model"), model);
    pbrShader->setUniformValue(pbrShader->uniformLocation("camPos"), positionV);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, albedoTexture);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, metalTexture);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, roughTexture);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, heightTexture);
    switch (primitive) {
        case 0: default:
            renderSphere();
            break;
        case 1:
            renderCube();
            break;
        case 2:
            renderPlane();
            break;
    }

    pbrShader->release();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    /*glDepthFunc(GL_LEQUAL);
    backgroundShader->bind();
    backgroundShader->setUniformValue(backgroundShader->uniformLocation("projection"), projection);
    backgroundShader->setUniformValue(backgroundShader->uniformLocation("view"), view);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
    renderCube();
    backgroundShader->release();
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);*/

    glDisable(GL_DEPTH_TEST);
}

void Preview3DRenderer::renderCube() {
    if (!cubeVAO)
    {
        unsigned int cubeVBO;

        float vertices[] = {
           -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,
           -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f,
           -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,

           -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f,
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
           -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // top-left
           -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // bottom-left
           // left face
           -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-right
           -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
           -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // bottom-left
           -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // bottom-left
           -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
           -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-right
           // right face
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // top-right
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // bottom-left
           // bottom face
           -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // bottom-left
           -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // bottom-right
           -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // top-right
           // top face
           -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // bottom-right
           -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // top-left
           -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Preview3DRenderer::renderQuad() {
    if (!quadVAO)
    {
        unsigned int quadVBO;

        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Preview3DRenderer::renderSphere() {
    if (!sphereVAO) {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<QVector3D> positions;
        std::vector<QVector2D> uv;
        std::vector<QVector3D> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
        {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(QVector3D(xPos, yPos, zPos));
                uv.push_back(QVector2D(2*xSegment, ySegment));
                normals.push_back(QVector3D(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y       * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        std::vector<float> data;
        for (int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x());
            data.push_back(positions[i].y());
            data.push_back(positions[i].z());
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x());
                data.push_back(normals[i].y());
                data.push_back(normals[i].z());
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x());
                data.push_back(uv[i].y());
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        float stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
   }
    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Preview3DRenderer::renderPlane() {
    if (!planeVAO)
    {
        unsigned int quadVBO;

        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
             1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        };

        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(planeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Preview3DRenderer::updateMatrix() {
    float x = 0;
    float y = 0;
    float z = sin(3.14f/2);
    float w = cos(3.14f/2);

    rotZ = QQuaternion(w, x, y, z);
    projection = QMatrix4x4();
    projection.perspective(zoom, (float)wWidth/(float)wHeight, 0.1f, 38.0f);
    view = QMatrix4x4();
    view.lookAt(positionV, positionV + QVector3D(0, 0, -1), QVector3D(0, 1, 0));
    //view.translate(positionV);
    view.rotate(rotZ);
    model = QMatrix4x4();
    model.translate(0.0f, 0.0f, 0.0f);
    model.rotate(rotQuat);
}

void Preview3DRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, albedoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texResolution.x(), m_texResolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, roughTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texResolution.x(), m_texResolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, metalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texResolution.x(), m_texResolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texResolution.x(), m_texResolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Preview3DRenderer::updateOutputsTexture(unsigned int &dst, const unsigned int &src) {
    glBindTexture(GL_TEXTURE_2D, src);
    GLubyte *pixels = new GLubyte[m_texResolution.x()*m_texResolution.y()*4];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, dst);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texResolution.x(), m_texResolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    delete [] pixels;
}
