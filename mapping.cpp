#include "mapping.h"
#include <QOpenGLFramebufferObjectFormat>

MappingObject::MappingObject(QQuickItem *parent, QVector2D resolution, float inputMin, float inputMax,
                             float outputMin, float outputMax): QQuickFramebufferObject (parent),
    m_resolution(resolution), m_inputMin(inputMin), m_inputMax(inputMax), m_outputMin(outputMin),
    m_outputMax(outputMax)
{
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *MappingObject::createRenderer() const {
    return new MappingRenderer(m_resolution);
}

unsigned int &MappingObject::texture() {
    return m_texture;
}

void MappingObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int MappingObject::maskTexture() {
    return m_maskTexture;
}

void MappingObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    mappedTex = true;
    update();
}

unsigned int MappingObject::sourceTexture() {
    return m_sourceTexture;
}

void MappingObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    mappedTex = true;
    update();
}

float MappingObject::inputMin() {
    return m_inputMin;
}

void MappingObject::setInputMin(float value) {
    m_inputMin = value;
    mappedTex = true;
    update();
}

float MappingObject::inputMax() {
    return m_inputMax;
}

void MappingObject::setInputMax(float value) {
    m_inputMax = value;
    mappedTex = true;
    update();
}

float MappingObject::outputMin() {
    return m_outputMin;
}

void MappingObject::setOutputMin(float value) {
    m_outputMin = value;
    mappedTex = true;
    update();
}

float MappingObject::outputMax() {
    return  m_outputMax;
}

void MappingObject::setOutputMax(float value) {
    m_outputMax = value;
    mappedTex = true;
    update();
}

QVector2D MappingObject::resolution() {
    return m_resolution;
}

void MappingObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

MappingRenderer::~MappingRenderer() {
    delete mappingShader;
    delete textureShader;
}

MappingRenderer::MappingRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();
    mappingShader = new QOpenGLShaderProgram();
    mappingShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    mappingShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/mapping.frag");
    mappingShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    mappingShader->bind();
    mappingShader->setUniformValue(mappingShader->uniformLocation("sourceTexture"), 0);
    mappingShader->setUniformValue(mappingShader->uniformLocation("maskTexture"), 1);
    mappingShader->release();
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

    glGenFramebuffers(1, &mappingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mappingFBO);
    glGenTextures(1, &m_mappingTexture);
    glBindTexture(GL_TEXTURE_2D, m_mappingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_mappingTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

QOpenGLFramebufferObject *MappingRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void MappingRenderer::synchronize(QQuickFramebufferObject *item) {
    MappingObject *mappingItem = static_cast<MappingObject*>(item);
    if(mappingItem->resUpdated) {
        mappingItem->resUpdated = false;
        m_resolution = mappingItem->resolution();
        updateTexResolution();
    }
    if(mappingItem->mappedTex) {
        mappingItem->mappedTex = false;
        m_sourceTexture = mappingItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = mappingItem->maskTexture();
            mappingShader->bind();
            mappingShader->setUniformValue(mappingShader->uniformLocation("inputMin"), mappingItem->inputMin());
            mappingShader->setUniformValue(mappingShader->uniformLocation("inputMax"), mappingItem->inputMax());
            mappingShader->setUniformValue(mappingShader->uniformLocation("outputMin"), mappingItem->outputMin());
            mappingShader->setUniformValue(mappingShader->uniformLocation("outputMax"), mappingItem->outputMax());
            mappingShader->setUniformValue(mappingShader->uniformLocation("useMask"), maskTexture);
            mappingShader->release();
            map();
            if(mappingItem->selectedItem) mappingItem->updatePreview(m_mappingTexture, true);
            mappingItem->setTexture(m_mappingTexture);
        }
    }    
}

void MappingRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_sourceTexture) {
        glBindVertexArray(textureVAO);
        textureShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_mappingTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
}

void MappingRenderer::map() {
    glBindFramebuffer(GL_FRAMEBUFFER, mappingFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    mappingShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    mappingShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MappingRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_mappingTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glBindTexture(GL_TEXTURE_2D, 0);
}
