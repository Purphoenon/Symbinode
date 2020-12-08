#include "warp.h"
#include <iostream>
#include <QOpenGLFramebufferObjectFormat>

WarpObject::WarpObject(QQuickItem *parent, QVector2D resolution, float intensity):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_intensity(intensity)
{
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *WarpObject::createRenderer() const {
    return new WarpRenderer(m_resolution);
}

unsigned int &WarpObject::texture() {
    return m_texture;
}

void WarpObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

unsigned int WarpObject::maskTexture() {
    return m_maskTexture;
}

void WarpObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    warpedTex = true;
    update();
}

unsigned int WarpObject::sourceTexture() {
    return m_sourceTexture;
}

void WarpObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    warpedTex = true;
    update();
}

unsigned int WarpObject::warpTexture() {
    return m_warpTexture;
}

void WarpObject::setWarpTexture(unsigned int texture) {
    m_warpTexture = texture;
    warpedTex = true;
    update();
}

float WarpObject::intensity() {
    return m_intensity;
}

void WarpObject::setIntensity(float intensity) {
    m_intensity = intensity;
    warpedTex = true;
    update();
}

QVector2D WarpObject::resolution() {
    return m_resolution;
}

void WarpObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

WarpRenderer::WarpRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();
    warpShader = new QOpenGLShaderProgram();
    warpShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    warpShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/warp.frag");
    warpShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    warpShader->bind();
    warpShader->setUniformValue(warpShader->uniformLocation("sourceTexture"), 0);
    warpShader->setUniformValue(warpShader->uniformLocation("warpTexture"), 1);
    warpShader->setUniformValue(warpShader->uniformLocation("maskTexture"), 2);
    warpShader->release();
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->release();
    float vertQuadTex[] = {-1.0f, -1.0f, 0.0f, 0.0f,
                    -1.0f, 1.0f, 0.0f, 1.0f,
                    1.0f, -1.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &textureVAO);
    glBindVertexArray(textureVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuadTex), vertQuadTex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glGenFramebuffers(1, &warpFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, warpFBO);
    glGenTextures(1, &m_warpedTexture);
    glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_warpedTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

WarpRenderer::~WarpRenderer() {
    delete warpShader;
    delete textureShader;
}

QOpenGLFramebufferObject *WarpRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void WarpRenderer::synchronize(QQuickFramebufferObject *item) {
    WarpObject *warpItem = static_cast<WarpObject*>(item);
    if(warpItem->resUpdated) {
        warpItem->resUpdated = false;
        m_resolution = warpItem->resolution();
        updateTexResolution();
    }
    if(warpItem->warpedTex) {
        warpItem->warpedTex = false;
        m_sourceTexture = warpItem->sourceTexture();
        if(m_sourceTexture) {
            m_warpTexture = warpItem->warpTexture();
            maskTexture = warpItem->maskTexture();
            warpShader->bind();
            warpShader->setUniformValue(warpShader->uniformLocation("intensity"), warpItem->intensity());
            warpShader->setUniformValue(warpShader->uniformLocation("useMask"), maskTexture);
            warpShader->release();
            createWarp();
            warpItem->setTexture(m_warpedTexture);
            if(warpItem->selectedItem) {
                warpItem->updatePreview(m_warpedTexture, true);
            }
        }
    }      
}

void WarpRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_sourceTexture) {
        glBindVertexArray(textureVAO);
        textureShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
}

void WarpRenderer::createWarp() {
    glBindFramebuffer(GL_FRAMEBUFFER, warpFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    warpShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_warpTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    warpShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WarpRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_warpedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}
