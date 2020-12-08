#include "polygon.h"
#include "QOpenGLFramebufferObjectFormat"
#include <iostream>

PolygonObject::PolygonObject(QQuickItem *parent, QVector2D resolution, int sides, float polygonScale, float smooth): QQuickFramebufferObject (parent),
    m_resolution(resolution), m_sides(sides), m_scale(polygonScale), m_smooth(smooth)
{

}

QQuickFramebufferObject::Renderer *PolygonObject::createRenderer() const {
    return new PolygonRenderer(m_resolution);
}

unsigned int PolygonObject::maskTexture() {
    return m_maskTexture;
}

void PolygonObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    generatedPolygon = true;
    update();
}

unsigned int &PolygonObject::texture() {
    return m_texture;
}

void PolygonObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

int PolygonObject::sides() {
    return m_sides;
}

void PolygonObject::setSides(int sides) {
    m_sides = sides;
    generatedPolygon = true;
    update();
}

float PolygonObject::polygonScale() {
    return m_scale;
}

void PolygonObject::setPolygonScale(float scale) {
    m_scale = scale;
    generatedPolygon = true;
    update();
}

float PolygonObject::smooth() {
    return m_smooth;
}

void PolygonObject::setSmooth(float smooth) {
    m_smooth = smooth;
    generatedPolygon = true;
    update();
}

QVector2D PolygonObject::resolution() {
    return m_resolution;
}

void PolygonObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

PolygonRenderer::PolygonRenderer(QVector2D resolution): m_resolution(resolution) {
    initializeOpenGLFunctions();
    generatePolygon = new QOpenGLShaderProgram();
    generatePolygon->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    generatePolygon->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/polygon.frag");
    generatePolygon->link();
    renderTexture = new QOpenGLShaderProgram();
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    renderTexture->link();
    generatePolygon->bind();
    generatePolygon->setUniformValue(generatePolygon->uniformLocation("maskTexture"), 0);
    generatePolygon->release();
    renderTexture->bind();
    renderTexture->setUniformValue(renderTexture->uniformLocation("texture"), 0);
    renderTexture->release();
    float vertQuad[] = {-1.0f, -1.0f,
                    -1.0f, 1.0f,
                    1.0f, -1.0f,
                    1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &polygonVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(polygonVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuad), vertQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float vertQuadTex[] = {-1.0f, -1.0f, 0.0f, 1.0f,
                    -1.0f, 1.0f, 0.0f, 0.0f,
                    1.0f, -1.0f, 1.0f, 1.0f,
                    1.0f, 1.0f, 1.0f, 0.0f};
    unsigned int VBO2;
    glGenVertexArrays(1, &textureVAO);
    glBindVertexArray(textureVAO);
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuadTex), vertQuadTex, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glGenFramebuffers(1, &polygonFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, polygonFBO);
    glGenTextures(1, &polygonTexture);
    glBindTexture(GL_TEXTURE_2D, polygonTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, polygonTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    createPolygon();
}

PolygonRenderer::~PolygonRenderer() {
    delete generatePolygon;
    delete renderTexture;
}

QOpenGLFramebufferObject *PolygonRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void PolygonRenderer::synchronize(QQuickFramebufferObject *item) {
    PolygonObject *polygonItem = static_cast<PolygonObject*>(item);
    if(polygonItem->resUpdated) {
        polygonItem->resUpdated = false;
        m_resolution = polygonItem->resolution();
        updateTexResolution();
        if(!maskTexture) {
            createPolygon();
            polygonItem->setTexture(polygonTexture);
        }
    }
    if(polygonItem->generatedPolygon) {
        polygonItem->generatedPolygon = false;
        maskTexture = polygonItem->maskTexture();
        generatePolygon->bind();
        generatePolygon->setUniformValue(generatePolygon->uniformLocation("sides"), polygonItem->sides());
        generatePolygon->setUniformValue(generatePolygon->uniformLocation("scale"), polygonItem->polygonScale());
        generatePolygon->setUniformValue(generatePolygon->uniformLocation("smoothValue"), polygonItem->smooth());
        generatePolygon->setUniformValue(generatePolygon->uniformLocation("useMask"), maskTexture);
        generatePolygon->release();
        createPolygon();
        if(polygonItem->selectedItem) polygonItem->updatePreview(polygonTexture, true);
        polygonItem->setTexture(polygonTexture);
    }   
}

void PolygonRenderer::render() {
    glDisable(GL_DEPTH_TEST);    
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderTexture->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, polygonTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    renderTexture->release();
}

void PolygonRenderer::createPolygon() {
    glBindFramebuffer(GL_FRAMEBUFFER, polygonFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(polygonVAO);
    generatePolygon->bind();
    generatePolygon->setUniformValue(generatePolygon->uniformLocation("res"), m_resolution);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    generatePolygon->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PolygonRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, polygonTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}
