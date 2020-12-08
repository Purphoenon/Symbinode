#include "mirror.h"
#include <QOpenGLFramebufferObjectFormat>

MirrorObject::MirrorObject(QQuickItem *parent, QVector2D resolution, int dir):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_direction(dir)
{
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *MirrorObject::createRenderer() const {
    return new MirrorRenderer(m_resolution);
}

unsigned int &MirrorObject::texture() {
    return m_texture;
}

void MirrorObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int MirrorObject::maskTexture() {
    return m_maskTexture;
}

void MirrorObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    mirroredTex = true;
    update();
}

unsigned int MirrorObject::sourceTexture() {
    return m_sourceTexture;
}

void MirrorObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    mirroredTex = true;
    update();
}

int MirrorObject::direction() {
    return m_direction;
}

void MirrorObject::setDirection(int dir) {
    m_direction = dir;
    mirroredTex = true;
    update();
}

QVector2D MirrorObject::resolution() {
    return m_resolution;
}

void MirrorObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

MirrorRenderer::MirrorRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();
    mirrorShader = new QOpenGLShaderProgram();
    mirrorShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    mirrorShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/mirror.frag");
    mirrorShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    mirrorShader->bind();
    mirrorShader->setUniformValue(mirrorShader->uniformLocation("sourceTexture"), 0);
    mirrorShader->setUniformValue(mirrorShader->uniformLocation("maskTexture"), 1);
    mirrorShader->release();
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

    glGenFramebuffers(1, &mirrorFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);
    glGenTextures(1, &m_mirrorTexture);
    glBindTexture(GL_TEXTURE_2D, m_mirrorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_mirrorTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

MirrorRenderer::~MirrorRenderer() {
    delete mirrorShader;
    delete textureShader;
}

QOpenGLFramebufferObject *MirrorRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void MirrorRenderer::synchronize(QQuickFramebufferObject *item) {
    MirrorObject *mirrorItem = static_cast<MirrorObject*>(item);
    if(mirrorItem->resUpdated) {
        mirrorItem->resUpdated = false;
        m_resolution = mirrorItem->resolution();
        updateTexResolution();
    }
    if(mirrorItem->mirroredTex) {
        mirrorItem->mirroredTex = false;
        m_sourceTexture = mirrorItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = mirrorItem->maskTexture();
            mirrorShader->bind();
            mirrorShader->setUniformValue(mirrorShader->uniformLocation("dir"), mirrorItem->direction());
            mirrorShader->setUniformValue(mirrorShader->uniformLocation("useMask"), maskTexture);
            mirrorShader->release();
            mirror();
            if(mirrorItem->selectedItem) mirrorItem->updatePreview(m_mirrorTexture, true);
            mirrorItem->setTexture(m_mirrorTexture);
        }
    }    
}

void MirrorRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_sourceTexture) {
        textureShader->bind();
        glBindVertexArray(textureVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_mirrorTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        textureShader->release();
    }
}

void MirrorRenderer::mirror() {
    glBindFramebuffer(GL_FRAMEBUFFER, mirrorFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    mirrorShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    mirrorShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MirrorRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_mirrorTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );
    glBindTexture(GL_TEXTURE_2D, 0);
}
