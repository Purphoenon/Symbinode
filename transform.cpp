#include "transform.h"
#include "QOpenGLFramebufferObjectFormat"

TransformObject::TransformObject(QQuickItem *parent, QVector2D resolution, float transX, float transY,
                                 float scaleX, float scaleY, int angle, bool clamp):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_translateX(transX), m_translateY(transY),
    m_scaleX(scaleX), m_scaleY(scaleY), m_angle(angle), m_clamp(clamp)
{
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *TransformObject::createRenderer() const {
    return new TransformRenderer(m_resolution);
}

unsigned int &TransformObject::texture() {
    return m_texture;
}

void TransformObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int TransformObject::maskTexture() {
    return m_maskTexture;
}

void TransformObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    transformedTex = true;
    update();
}

unsigned int TransformObject::sourceTexture() {
    return m_sourceTexture;
}

void TransformObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    transformedTex = true;
    update();
}

float TransformObject::translateX() {
    return m_translateX;
}

void TransformObject::setTranslateX(float transX) {
    m_translateX = transX;
    transformedTex = true;
    update();
}

float TransformObject::translateY() {
    return m_translateY;
}

void TransformObject::setTranslateY(float transY) {
    m_translateY = transY;
    transformedTex = true;
    update();
}

float TransformObject::scaleX() {
    return m_scaleX;
}

void TransformObject::setScaleX(float scaleX) {
    m_scaleX = scaleX;
    transformedTex = true;
    update();
}

float TransformObject::scaleY() {
    return m_scaleY;
}

void TransformObject::setScaleY(float scaleY) {
    m_scaleY = scaleY;
    transformedTex = true;
    update();
}

int TransformObject::rotation() {
    return m_angle;
}

void TransformObject::setRotation(int angle) {
    m_angle = angle;
    transformedTex = true;
    update();
}

bool TransformObject::clampCoords() {
    return m_clamp;
}

void TransformObject::setClampCoords(bool clamp) {
    m_clamp = clamp;
    transformedTex = true;
    update();
}

QVector2D TransformObject::resolution() {
    return m_resolution;
}

void TransformObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

TransformRenderer::TransformRenderer(QVector2D resolution): m_resolution(resolution) {
    initializeOpenGLFunctions();
    transformShader = new QOpenGLShaderProgram();
    transformShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    transformShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/transform.frag");
    transformShader->link();
    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();
    transformShader->bind();
    transformShader->setUniformValue(transformShader->uniformLocation("transTexture"), 0);
    transformShader->setUniformValue(transformShader->uniformLocation("maskTexture"), 1);
    transformShader->release();
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
    glGenFramebuffers(1, &transformFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, transformFBO);
    glGenTextures(1, &m_transformedTexture);
    glBindTexture(GL_TEXTURE_2D, m_transformedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_transformedTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TransformRenderer::~TransformRenderer() {
    delete transformShader;
    delete textureShader;
}

QOpenGLFramebufferObject *TransformRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void TransformRenderer::synchronize(QQuickFramebufferObject *item) {
    TransformObject *transformItem = static_cast<TransformObject*>(item);
    if(transformItem->resUpdated) {
        transformItem->resUpdated;
        m_resolution = transformItem->resolution();
        updateTexResolution();
    }

    if(transformItem->transformedTex) {
        transformItem->transformedTex = false;
        m_sourceTexture = transformItem->sourceTexture();
        if(m_sourceTexture) {
            maskTexture = transformItem->maskTexture();
            transformShader->bind();
            transformShader->setUniformValue(transformShader->uniformLocation("translate"), QVector2D(transformItem->translateX(), transformItem->translateY()));
            transformShader->setUniformValue(transformShader->uniformLocation("scale"), QVector2D(transformItem->scaleX(), transformItem->scaleY()));
            transformShader->setUniformValue(transformShader->uniformLocation("angle"), transformItem->rotation());
            transformShader->setUniformValue(transformShader->uniformLocation("clampTrans"), transformItem->clampCoords());
            transformShader->setUniformValue(transformShader->uniformLocation("useMask"), maskTexture);
            transformShader->release();
            transformateTexture();
            transformItem->setTexture(m_transformedTexture);
            if(transformItem->selectedItem) {
                transformItem->updatePreview(m_transformedTexture, true);
            }
        }
    }       
}

void TransformRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (m_sourceTexture) {
        glBindVertexArray(textureVAO);
        textureShader->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_transformedTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
}

void TransformRenderer::transformateTexture() {
    glBindFramebuffer(GL_FRAMEBUFFER, transformFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f ,0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    transformShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glBindVertexArray(textureVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    transformShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TransformRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_transformedTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}
