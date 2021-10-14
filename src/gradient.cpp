#include "gradient.h"
#include <QOpenGLFramebufferObjectFormat>

GradientObject::GradientObject(QQuickItem *parent, QVector2D resolution, GLint bpc, QString type,
                               float startX, float startY, float endX, float endY, float centerWidth,
                               bool tiling): QQuickFramebufferObject (parent), m_gradientType(type),
    m_startX(startX), m_startY(startY), m_endX(endX), m_endY(endY), m_reflectedWidth(centerWidth),
    m_tiling(tiling), m_resolution(resolution), m_bpc(bpc)
{

}

QQuickFramebufferObject::Renderer *GradientObject::createRenderer() const {
    return new GradientRenderer(m_resolution, m_bpc);
}

QString GradientObject::gradientType() {
    return m_gradientType;
}

void GradientObject::setGradientType(QString type) {
    if(m_gradientType == type) return;
    m_gradientType = type;
    generatedGradient = true;
}

float GradientObject::startX() {
    return m_startX;
}

void GradientObject::setStartX(float x) {
    if(m_startX == x) return;
    m_startX = x;
    generatedGradient = true;
}

float GradientObject::startY() {
    return m_startY;
}

void GradientObject::setStartY(float y) {
    if(m_startY == y) return;
    m_startY = y;
    generatedGradient = true;
}

float GradientObject::endX() {
    return m_endX;
}

void GradientObject::setEndX(float x) {
    if(m_endX == x) return;
    m_endX = x;
    generatedGradient = true;
}

float GradientObject::endY() {
    return m_endY;
}

void GradientObject::setEndY(float y) {
    if(m_endY == y) return;
    m_endY = y;
    generatedGradient = true;
}

float GradientObject::reflectedWidth() {
    return m_reflectedWidth;
}

void GradientObject::setReflectedWidth(float width) {
    if(m_reflectedWidth == width) return;
    m_reflectedWidth = width;
    generatedGradient = true;
}

bool GradientObject::tiling() {
    return m_tiling;
}

void GradientObject::setTiling(bool tiling) {
    if(m_tiling == tiling) return;
    m_tiling = tiling;
    generatedGradient = true;
}

unsigned int GradientObject::maskTexture() {
    return m_maskTexture;
}

void GradientObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    generatedGradient = true;
    update();
};

unsigned int &GradientObject::texture() {
    return m_gradientTexture;
}

void GradientObject::setTexture(unsigned int texture) {
    m_gradientTexture = texture;
    changedTexture();
}

void GradientObject::saveTexture(QString fileName) {
    saveName = fileName;
    texSaving = true;
    update();
}

QVector2D GradientObject::resolution() {
    return m_resolution;
}

void GradientObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

GLint GradientObject::bpc() {
    return m_bpc;
}

void GradientObject::setBPC(GLint bpc) {
    if(m_bpc == bpc) return;
    m_bpc = bpc;
    bpcUpdated = true;
    update();
}

GradientRenderer::GradientRenderer(QVector2D res, GLint bpc): m_resolution(res), m_bpc(bpc){
    initializeOpenGLFunctions();
    gradientShader = new QOpenGLShaderProgram();
    gradientShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    gradientShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/gradient.frag");
    gradientShader->link();

    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();

    renderTexture = new QOpenGLShaderProgram();
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    renderTexture->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    renderTexture->link();

    renderTexture->bind();
    renderTexture->setUniformValue(renderTexture->uniformLocation("texture"), 0);
    renderTexture->setUniformValue(renderTexture->uniformLocation("lod"), 2.0f);
    renderTexture->release();

    gradientShader->bind();
    gradientShader->setUniformValue(gradientShader->uniformLocation("maskTexture"), 0);
    gradientShader->release();

    float vertQuad[] = {-1.0f, -1.0f,
                    -1.0f, 1.0f,
                    1.0f, -1.0f,
                    1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &gradientVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(gradientVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertQuad), vertQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    float vertQuadTex[] = {-1.0f, -1.0f, 0.0f, 0.0f,
                    -1.0f, 1.0f, 0.0f, 1.0f,
                    1.0f, -1.0f, 1.0f, 0.0f,
                    1.0f, 1.0f, 1.0f, 1.0f};
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

    glGenFramebuffers(1, &gradientFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gradientFBO);
    glGenTextures(1, &gradientTexture);
    glBindTexture(GL_TEXTURE_2D, gradientTexture);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 2);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gradientTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

GradientRenderer::~GradientRenderer() {
    delete gradientShader;
    delete checkerShader;
    delete renderTexture;
    glDeleteTextures(1, &gradientTexture);
    glDeleteFramebuffers(1, &gradientFBO);
    glDeleteVertexArrays(1, &gradientVAO);
    glDeleteVertexArrays(1, &textureVAO);
}

QOpenGLFramebufferObject *GradientRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void GradientRenderer::synchronize(QQuickFramebufferObject *item) {
    GradientObject *gradientItem = qobject_cast<GradientObject*>(item);

    if(gradientItem->resUpdated) {
        gradientItem->resUpdated = false;
        m_resolution = gradientItem->resolution();
        updateTexResolution();
        if(!m_maskTexture) {
            createGradient();
            gradientItem->setTexture(gradientTexture);
        }
    }
    if(gradientItem->generatedGradient || gradientItem->bpcUpdated) {
        if(gradientItem->bpcUpdated) {
            gradientItem->bpcUpdated = false;
            m_bpc = gradientItem->bpc();
            updateTexResolution();
        }
        if(gradientItem->generatedGradient) {
            gradientItem->generatedGradient = false;
            m_maskTexture = gradientItem->maskTexture();
            m_gradientType = gradientItem->gradientType();
            gradientShader->bind();
            gradientShader->setUniformValue(gradientShader->uniformLocation("startPos"), QVector2D(gradientItem->startX(), gradientItem->startY()));
            gradientShader->setUniformValue(gradientShader->uniformLocation("endPos"), QVector2D(gradientItem->endX(), gradientItem->endY()));
            gradientShader->setUniformValue(gradientShader->uniformLocation("whiteWidth"), gradientItem->reflectedWidth());
            gradientShader->setUniformValue(gradientShader->uniformLocation("tiling"), gradientItem->tiling());
            gradientShader->setUniformValue(gradientShader->uniformLocation("useMask"), m_maskTexture);
            gradientShader->release();
        }
        createGradient();
        gradientItem->setTexture(gradientTexture);
        gradientItem->updatePreview(gradientTexture);
    }

    if(gradientItem->texSaving) {
        gradientItem->texSaving = false;
        saveTexture(gradientItem->saveName);
    }
}

void GradientRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    checkerShader->bind();
    glBindVertexArray(textureVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    checkerShader->release();

    glBindVertexArray(textureVAO);
    renderTexture->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gradientTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderTexture->release();
    glBindVertexArray(0);
    glFlush();
}

void GradientRenderer::createGradient() {
    glBindFramebuffer(GL_FRAMEBUFFER, gradientFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(gradientVAO);
    gradientShader->bind();
    gradientShader->setUniformValue(gradientShader->uniformLocation("res"), m_resolution);
    GLuint index = glGetSubroutineIndex(gradientShader->programId(), GL_FRAGMENT_SHADER, m_gradientType.toStdString().c_str());
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &index);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);    
    gradientShader->release();
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, gradientTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFlush();
    glFinish();
}

void GradientRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, gradientTexture);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GradientRenderer::saveTexture(QString fileName) {
    qDebug("texture save");
    unsigned int fbo;
    unsigned int texture;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, texture);
    if(m_bpc == GL_RGBA16) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    else if(m_bpc == GL_RGBA8) {
        glTexImage2D(GL_TEXTURE_2D, 0, m_bpc, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    renderTexture->bind();
    glBindVertexArray(textureVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gradientTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    renderTexture->release();

    if(m_bpc == GL_RGBA16) {
        GLushort *pixels = (GLushort*)malloc(sizeof(GLushort)*4*m_resolution.x()*m_resolution.y());
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGRA, GL_UNSIGNED_SHORT, pixels);
        FIBITMAP *image16 = FreeImage_AllocateT(FIT_RGBA16, m_resolution.x(), m_resolution.y());
        int m_width = FreeImage_GetWidth(image16);
        int m_height = FreeImage_GetHeight(image16);
        for(int y = 0; y < m_height; ++y) {
            FIRGBA16 *bits = (FIRGBA16*)FreeImage_GetScanLine(image16, y);
            for(int x = 0; x < m_width; ++x) {
                bits[x].red = pixels[(m_width*(m_height - 1 - y) + x)*4 + 2];
                bits[x].green = pixels[(m_width*(m_height - 1 - y) + x)*4 + 1];
                bits[x].blue = pixels[(m_width*(m_height - 1 - y) + x)*4];
                bits[x].alpha = pixels[(m_width*(m_height - 1 - y) + x)*4 + 3];
            }
        }
        if (FreeImage_Save(FIF_PNG, image16, fileName.toUtf8().constData(), 0))
            printf("Successfully saved!\n");
        else
            printf("Failed saving!\n");
        FreeImage_Unload(image16);
        delete [] pixels;
    }
    else if(m_bpc == GL_RGBA8) {
        BYTE *pixels = (BYTE*)malloc(4*m_resolution.x()*m_resolution.y());
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glReadPixels(0, 0, m_resolution.x(), m_resolution.y(), GL_BGRA, GL_UNSIGNED_BYTE, pixels);
        FIBITMAP *image = FreeImage_ConvertFromRawBits(pixels, m_resolution.x(), m_resolution.y(), 4 * m_resolution.x(), 32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
        if (FreeImage_Save(FIF_PNG, image, fileName.toUtf8().constData(), 0))
            printf("Successfully saved!\n");
        else
            printf("Failed saving!\n");
        FreeImage_Unload(image);
        delete [] pixels;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &fbo);
}
