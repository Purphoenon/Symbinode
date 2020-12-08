#include "splat.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>

SplatObject::SplatObject(QQuickItem *parent, QVector2D resolution, int size, int imagePerCell):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_size(size), m_imagePerSell(imagePerCell)
{
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *SplatObject::createRenderer() const {
    return new SplatRenderer(m_resolution);
}

unsigned int &SplatObject::texture() {
    return m_texture;
}

void SplatObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int SplatObject::maskTexture() {
    return m_maskTexture;
}

void SplatObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    bombedTex = true;
    update();
}

unsigned int SplatObject::sourceTexture() {
    return m_sourceTexture;
}

void SplatObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    bombedTex = true;
    update();
}

int SplatObject::size() {
    return m_size;
}

void SplatObject::setSize(int size) {
    m_size = size;
    bombedTex = true;
    update();
}

int SplatObject::imagePerCell() {
    return m_imagePerSell;
}

void SplatObject::setImagePerCell(int count) {
    m_imagePerSell = count;
    bombedTex = true;
    update();
}

QVector2D SplatObject::resolution() {
    return m_resolution;
}

void SplatObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

SplatRenderer::SplatRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();
    bombingShader = new QOpenGLShaderProgram();
    bombingShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    bombingShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/bombing.frag");
    bombingShader->link();
    randomShader = new QOpenGLShaderProgram();
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/random.frag");
    randomShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    bombingShader->bind();
    bombingShader->setUniformValue(bombingShader->uniformLocation("sourceTexture"), 0);
    bombingShader->setUniformValue(bombingShader->uniformLocation("randomTexture"), 1);
    bombingShader->setUniformValue(bombingShader->uniformLocation("maskTexture"), 2);
    bombingShader->release();
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

    glGenFramebuffers(1, &bombingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, bombingFBO);
    glGenTextures(1, &m_bombingTexture);
    glBindTexture(GL_TEXTURE_2D, m_bombingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_bombingTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &randomFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, randomFBO);
    glGenTextures(1, &m_randomTexture);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_randomTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, randomFBO);
    glViewport(0, 0, 512, 512);
    glBindVertexArray(textureVAO);
    randomShader->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    randomShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

SplatRenderer::~SplatRenderer() {
    delete bombingShader;
    delete randomShader;
    delete textureShader;
}

QOpenGLFramebufferObject *SplatRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void SplatRenderer::synchronize(QQuickFramebufferObject *item) {
    SplatObject *splatItem = static_cast<SplatObject*>(item);
    if(splatItem->resUpdated) {
        splatItem->resUpdated = false;
        m_resolution = splatItem->resolution();
        updateTexResolution();
        //bomb();
        //splatItem->setTexture(m_bombingTexture);
    }
    if(splatItem->bombedTex) {
        splatItem->bombedTex = false;
        m_sourceTexture = splatItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = splatItem->maskTexture();
            bombingShader->bind();
            bombingShader->setUniformValue(bombingShader->uniformLocation("scale"), splatItem->size());
            bombingShader->setUniformValue(bombingShader->uniformLocation("imagePerCell"), splatItem->imagePerCell());
            bombingShader->setUniformValue(bombingShader->uniformLocation("useMask"), maskTexture);
            bombingShader->release();
            bomb();
            if(splatItem->selectedItem) splatItem->updatePreview(m_bombingTexture, true);
            splatItem->setTexture(m_bombingTexture);
        }
    }    
}

void SplatRenderer::render() {
    std::cout << "update render" << std::endl;
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_sourceTexture) {
        textureShader->bind();
        glBindVertexArray(textureVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_bombingTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        textureShader->release();
    }
    std::cout << "updated render" << std::endl;
}

void SplatRenderer::bomb() {
    std::cout << "update bomb" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, bombingFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    bombingShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glActiveTexture(GL_TEXTURE0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    bombingShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::cout << "updated bomb" << std::endl;
}

void SplatRenderer::updateTexResolution() {
    std::cout << "update res" << std::endl;
    glBindTexture(GL_TEXTURE_2D, m_bombingTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    std::cout << "updated res" << std::endl;
}
