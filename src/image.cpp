#include "image.h"
#include <QOpenGLFramebufferObjectFormat>
#include "FreeImage.h"
#include <iostream>

ImageObject::ImageObject(QQuickItem *parent, QVector2D resolution, float transX, float transY):
    QQuickFramebufferObject (parent), m_resolution(resolution), m_translateX(transX), m_translateY(transY)
{

}

QQuickFramebufferObject::Renderer *ImageObject::createRenderer() const {
    return new ImageRenderer(m_resolution);
}

unsigned int &ImageObject::texture() {
    return m_texture;
}

void ImageObject::setTexture(unsigned int texture) {
    m_texture = texture;
    textureChanged();
}

unsigned int ImageObject::maskTexture() {
    return m_maskTexture;
}

void ImageObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    layerUpdate = true;
}

void ImageObject::loadTexture(QString file) {
    loadTex = true;
    texName = file;
    update();
}

float ImageObject::translateX() {
    return m_translateX;
}

void ImageObject::setTranslateX(float x) {
    if(m_translateX == x) return;
    m_translateX = x;
    layerUpdate = true;
}

float ImageObject::translateY() {
    return m_translateY;
}

void ImageObject::setTranslateY(float y) {
    if(m_translateY == y) return;
    m_translateY = y;
    layerUpdate = true;
}

QVector2D ImageObject::resolution() {
    return m_resolution;
}

void ImageObject::setResolution(QVector2D res) {
    if(m_resolution == res) return;
    m_resolution = res;
    resUpdate = true;
    update();
}

ImageRenderer::ImageRenderer(QVector2D resolution): m_resolution(resolution) {
    initializeOpenGLFunctions();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    layerShader = new QOpenGLShaderProgram();
    layerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    layerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/layer.frag");
    layerShader->link();

    checkerShader = new QOpenGLShaderProgram();
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/checker.vert");
    checkerShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/checker.frag");
    checkerShader->link();

    textureShader->bind();
    textureShader->setUniformValue(textureShader->uniformLocation("textureSample"), 0);
    textureShader->setUniformValue(textureShader->uniformLocation("lod"), 0.0f);
    textureShader->release();

    layerShader->bind();
    layerShader->setUniformValue(layerShader->uniformLocation("textureSample"), 0);
    layerShader->setUniformValue(layerShader->uniformLocation("maskTexture"), 1);
    layerShader->setUniformValue(layerShader->uniformLocation("res"), m_resolution);
    layerShader->release();

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

    glGenTextures(1, &m_sourceTexture);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &m_texture);
    glGenFramebuffers(1, &FBO);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 2);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0
    );
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ImageRenderer::~ImageRenderer() {
    delete textureShader;
    delete checkerShader;
    delete layerShader;
    glDeleteVertexArrays(1, &textureVAO);
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &m_sourceTexture);
    glDeleteTextures(1, &m_texture);
}

QOpenGLFramebufferObject *ImageRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    return new QOpenGLFramebufferObject(size, format);
}

void ImageRenderer::synchronize(QQuickFramebufferObject *item) {
    ImageObject *imageItem = static_cast<ImageObject*>(item);

    if(imageItem->resUpdate) {
        imageItem->resUpdate = false;
        m_resolution = imageItem->resolution();
        updateTexResolution();
        layerShader->bind();
        layerShader->setUniformValue(layerShader->uniformLocation("res"), m_resolution);
        layerShader->release();
        if(!m_maskTexture) {
            createLayer();
            imageItem->setTexture(m_texture);
            imageItem->updatePreview(m_texture);
        }
    }
    if(imageItem->loadTex || imageItem->layerUpdate) {
        layerShader->bind();
        layerShader->setUniformValue(layerShader->uniformLocation("useMask"), m_maskTexture);
        layerShader->setUniformValue(layerShader->uniformLocation("transX"), imageItem->translateX());
        layerShader->setUniformValue(layerShader->uniformLocation("transY"), imageItem->translateY());
        if(imageItem->loadTex) {
            imageItem->loadTex = false;
            if(!imageItem->texName.isEmpty()) {
                loadTexture(imageItem->texName);
            }
            else {
                isTexture = false;
            }
        }
        if(imageItem->layerUpdate) {
            m_maskTexture = imageItem->maskTexture();            
            imageItem->layerUpdate = false;
            createLayer();
        }
        if(isTexture) {
            imageItem->setTexture(m_texture);
            imageItem->updatePreview(m_texture);
        }
        else {
            imageItem->setTexture(0);
            imageItem->updatePreview(0);
        }
    }
}

void ImageRenderer::render() {
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

    if(isTexture) {
        glBindVertexArray(textureVAO);
        textureShader->bind();
        textureShader->setUniformValue(textureShader->uniformLocation("lod"), 2.0f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        textureShader->release();
        glBindVertexArray(0);
    }
    glFlush();
}

void ImageRenderer::loadTexture(QString file) {
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    FREE_IMAGE_TYPE fit = FIT_UNKNOWN;
    FIBITMAP *dib(nullptr);
    BYTE* bits(nullptr);
    unsigned int width(0), height(0);
    uint bpp;
    GLenum format, type;
    GLint internalFormat;

    fif = FreeImage_GetFileType(file.toUtf8().constData(), 0);
    if (fif == FIF_UNKNOWN) {
        std::cout << "not get file type" << std::endl;
        return;
    }
    dib = FreeImage_Load(fif, file.toUtf8().constData());
    if (!dib) {
        std::cout << "not load image" << std::endl;
        return;
    }
    FreeImage_FlipVertical(dib);

    fit = FreeImage_GetImageType(dib);
    bits = FreeImage_GetBits(dib);
    width = FreeImage_GetWidth(dib);
    height = FreeImage_GetHeight(dib);
    bpp = FreeImage_GetBPP(dib);

    unsigned long long sizeType = 0;
    if(fit == FIT_BITMAP) {
        sizeType = sizeof (BYTE)*CHAR_BIT;
        type = GL_UNSIGNED_BYTE;
    }
    else if(fit == FIT_UINT16 ||fit == FIT_INT16 || fit == FIT_RGB16 || fit == FIT_RGBA16) {
        sizeType = sizeof(WORD) * CHAR_BIT;
        if(fit == FIT_UINT16) type = GL_UNSIGNED_INT;
        else if(fit == FIT_INT16) type = GL_INT;
        else type = GL_UNSIGNED_SHORT;
    }
    else if(fit == FIT_UINT32 || fit == FIT_INT32 || fit == FIT_FLOAT || fit == FIT_RGBF || fit == FIT_RGBAF) {
        sizeType = sizeof(FLOAT) * CHAR_BIT;
        if(fit == FIT_UINT32) type = GL_UNSIGNED_INT;
        else if(fit == FIT_INT32) type = GL_INT;
        else type = GL_FLOAT;
    }

    uint numChannels = bpp/sizeType;
    if(numChannels == 1) {
        format = GL_RED;
        internalFormat = GL_RED;
    }
    else if(numChannels == 3) {
        format = GL_BGR;
        internalFormat = GL_RGB;
    }
    else {
        format = GL_BGRA;
        internalFormat = GL_RGBA;
    }

    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, (void*)bits);
    glBindTexture(GL_TEXTURE_2D, 0);
    FreeImage_Unload(dib);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_resolution.x(), m_resolution.y(), 0, format, type, nullptr);

    createLayer();
    isTexture = true;
}

void ImageRenderer::createLayer() {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    layerShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_maskTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    layerShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFlush();
    glFinish();
}

void ImageRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
}
