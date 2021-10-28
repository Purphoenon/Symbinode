#include "floodfill.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>
#include "FreeImage.h"

FloodFillObject::FloodFillObject(QQuickItem *parent, QVector2D resolution):
    QQuickFramebufferObject (parent), m_resolution(resolution)
{

}

QQuickFramebufferObject::Renderer *FloodFillObject::createRenderer() const {
    return new FloodFillRenderer(m_resolution);
}

unsigned int &FloodFillObject::texture() {
    return m_texture;
}

void FloodFillObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int FloodFillObject::sourceTexture() {
    return m_sourceTexture;
}

void FloodFillObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    filledTex = true;
}

void FloodFillObject::saveTexture(QString fileName) {
    saveName = fileName;
    texSaving = true;
    update();
}

QVector2D FloodFillObject::resolution() {
    return m_resolution;
}

void FloodFillObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

FloodFillRenderer::FloodFillRenderer(QVector2D resolution): m_resolution(resolution)
{
    initializeOpenGLFunctions();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    preparationShader = new QOpenGLShaderProgram();
    preparationShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    preparationShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/prefloodfill.frag");
    preparationShader->link();

    fillShader = new QOpenGLShaderProgram();
    fillShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    fillShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fill.frag");
    fillShader->link();

    finalFillShader = new QOpenGLShaderProgram();
    finalFillShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    finalFillShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/finalfill.frag");
    finalFillShader->link();

    preparationShader->bind();
    preparationShader->setUniformValue(preparationShader->uniformLocation("grayscaleTexture"), 0);
    preparationShader->setUniformValue(preparationShader->uniformLocation("maskTexture"), 1);
    preparationShader->release();

    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->release();

    fillShader->bind();
    fillShader->setUniformValue(fillShader->uniformLocation("sourceTexture"), 0);
    fillShader->release();

    finalFillShader->bind();
    finalFillShader->setUniformValue(finalFillShader->uniformLocation("sourceTexture"), 0);
    finalFillShader->release();

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

    glGenFramebuffers(1, &resultFBO);
    glGenTextures(1, &m_floodFillTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, resultFBO);
    glBindTexture(GL_TEXTURE_2D, m_floodFillTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 2);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_floodFillTexture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &preFBO);
    glGenTextures(1, &m_prefloodFillTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, preFBO);
    glBindTexture(GL_TEXTURE_2D, m_prefloodFillTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_prefloodFillTexture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(2, fillFBO);
    glGenTextures(2, m_fillTexture);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fillFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_fillTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fillTexture[i], 0
        );
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FloodFillRenderer::~FloodFillRenderer() {
    delete textureShader;
    delete preparationShader;
    delete fillShader;
    delete finalFillShader;
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteTextures(1, &m_floodFillTexture);
    glDeleteTextures(2, &m_fillTexture[0]);
    glDeleteTextures(1, &m_prefloodFillTexture);
    glDeleteFramebuffers(1, &resultFBO);
    glDeleteFramebuffers(2, &fillFBO[0]);
    glDeleteFramebuffers(1, &preFBO);
}

QOpenGLFramebufferObject *FloodFillRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void FloodFillRenderer::synchronize(QQuickFramebufferObject *item) {
    FloodFillObject *floodFillItem = static_cast<FloodFillObject*>(item);
    if(floodFillItem->resUpdated) {
        floodFillItem->resUpdated = false;
        m_resolution = floodFillItem->resolution();
        updateTexResolution();
    }

    if(floodFillItem->filledTex) {
        floodFillItem->filledTex = false;
        m_sourceTexture = floodFillItem->sourceTexture();
        if(m_sourceTexture) {
            filling();
            floodFillItem->setTexture(m_floodFillTexture);
            floodFillItem->updatePreview(m_floodFillTexture);
        }
    }

    if(floodFillItem->texSaving) {
       floodFillItem->texSaving = false;
       saveTexture(floodFillItem->saveName);
    }
}

void FloodFillRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if(m_sourceTexture) {
        glBindVertexArray(textureVAO);
        textureShader->bind();
        textureShader->setUniformValue(textureShader->uniformLocation("lod"), 2.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_floodFillTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void FloodFillRenderer::filling() {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, preFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    preparationShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    preparationShader->setUniformValue(preparationShader->uniformLocation("res"), m_resolution);
    glBindVertexArray(textureVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    preparationShader->release();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    fillShader->bind();
    glBindVertexArray(textureVAO);
    glBindFramebuffer(GL_FRAMEBUFFER, fillFBO[0]);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_prefloodFillTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    fillShader->release();

    int amount = m_resolution.x()/128 + 1;
    QVector2D dir = QVector2D(0, 1);
    finalFillShader->bind();
    glBindVertexArray(textureVAO);
    for(int i = 1; i < amount; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, fillFBO[i%2]);
        glViewport(0, 0, m_resolution.x(), m_resolution.y());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        finalFillShader->setUniformValue(finalFillShader->uniformLocation("dir"), dir);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_fillTexture[1 - i%2]);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        dir = QVector2D(1, 1) - dir;

        glFlush();
    }
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    finalFillShader->release();

    glBindFramebuffer(GL_FRAMEBUFFER, resultFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("lod"), 0.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fillTexture[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_floodFillTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFinish();
}

void FloodFillRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_floodFillTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
    );

    glBindTexture(GL_TEXTURE_2D, m_prefloodFillTexture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr
    );

    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_fillTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FloodFillRenderer::saveTexture(QString fileName) {
    unsigned int fbo;
    unsigned int texture;
    glGenFramebuffers(1, &fbo);
    glGenTextures(1, &texture);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
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
    glBindVertexArray(textureVAO);
    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("lod"), 0.0f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_floodFillTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader->release();
    glBindVertexArray(0);

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

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteTextures(1, &texture);
    glDeleteFramebuffers(1, &fbo);
}
