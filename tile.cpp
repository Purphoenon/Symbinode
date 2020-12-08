#include "tile.h"
#include <QOpenGLFramebufferObjectFormat>
#include <iostream>

TileObject::TileObject(QQuickItem *parent, QVector2D resolution, float offsetX, float offsetY, int columns,
                       int rows, float scaleX, float scaleY, int rotation, float randPosition,
                       float randRotation, float randScale, float maskStrength, int inputsCount,
                       bool keepProportion): QQuickFramebufferObject (parent), m_resolution(resolution),
    m_offsetX(offsetX), m_offsetY(offsetY), m_columns(columns), m_rows(rows), m_scaleX(scaleX),
    m_scaleY(scaleY), m_rotationAngle(rotation), m_randPosition(randPosition), m_randRotation(randRotation),
    m_randScale(randScale), m_maskStrength(maskStrength), m_inputsCount(inputsCount),
    m_keepProportion(keepProportion)
{
    setMirrorVertically(true);
}

QQuickFramebufferObject::Renderer *TileObject::createRenderer() const {
    return new TileRenderer(m_resolution);
}

unsigned int &TileObject::texture() {
    return m_texture;
}

void TileObject::setTexture(unsigned int texture) {
    m_texture = texture;
    changedTexture();
}

unsigned int TileObject::maskTexture() {
    return m_maskTexture;
}

void TileObject::setMaskTexture(unsigned int texture) {
    m_maskTexture = texture;
    tiledTex = true;
    update();
}

unsigned int TileObject::sourceTexture() {
    return m_sourceTexture;
}

void TileObject::setSourceTexture(unsigned int texture) {
    m_sourceTexture = texture;
    tiledTex = true;
    update();
}

unsigned int TileObject::tile1() {
    return m_tile1;
}

void TileObject::setTile1(unsigned int texture) {
    m_tile1 = texture;
    tiledTex = true;
    update();
}

unsigned int TileObject::tile2() {
    return m_tile2;
}

void TileObject::setTile2(unsigned int texture) {
    m_tile2 = texture;
    tiledTex = true;
    update();
}

unsigned int TileObject::tile3() {
    return m_tile3;
}

void TileObject::setTile3(unsigned int texture) {
    m_tile3 = texture;
    tiledTex = true;
    update();
}

unsigned int TileObject::tile4() {
    return m_tile4;
}

void TileObject::setTile4(unsigned int texture) {
    m_tile4 = texture;
    tiledTex = true;
    update();
}

unsigned int TileObject::tile5() {
    return m_tile5;
}

void TileObject::setTile5(unsigned int texture) {
    m_tile5 = texture;
    tiledTex = true;
    update();
}

float TileObject::offsetX() {
    return m_offsetX;
}

void TileObject::setOffsetX(float offset) {
    m_offsetX = offset;
    tiledTex = true;
    update();
}

float TileObject::offsetY() {
    return m_offsetY;
}

void TileObject::setOffsetY(float offset) {
    m_offsetY = offset;
    tiledTex = true;
    update();
}

int TileObject::columns() {
    return m_columns;
}

void TileObject::setColumns(int columns) {
    m_columns = columns;
    tiledTex = true;
    update();
}

int TileObject::rows() {
    return m_rows;
}

void TileObject::setRows(int rows) {
    m_rows = rows;
    tiledTex = true;
    update();
}

float TileObject::scaleX() {
    return m_scaleX;
}

void TileObject::setScaleX(float scale) {
    m_scaleX = scale;
    tiledTex = true;
    update();
}

float TileObject::scaleY() {
    return m_scaleY;
}

void TileObject::setScaleY(float scale) {
    m_scaleY = scale;
    tiledTex = true;
    update();
}

int TileObject::rotationAngle() {
    return m_rotationAngle;
}

void TileObject::setRotationAngle(int angle) {
    m_rotationAngle = angle;
    tiledTex = true;
    update();
}

float TileObject::randPosition() {
    return m_randPosition;
}

void TileObject::setRandPosition(float rand) {
    m_randPosition = rand;
    tiledTex = true;
    update();
}

float TileObject::randRotation() {
    return m_randRotation;
}

void TileObject::setRandRotation(float rand) {
    m_randRotation = rand;
    tiledTex = true;
    update();
}

float TileObject::randScale() {
    return m_randScale;
}

void TileObject::setRandScale(float rand) {
    m_randScale = rand;
    tiledTex = true;
    update();
}

float TileObject::maskStrength() {
    return m_maskStrength;
}

void TileObject::setMaskStrength(float mask) {
    m_maskStrength = mask;
    tiledTex = true;
    update();
}

int TileObject::inputsCount() {
    return m_inputsCount;
}

void TileObject::setInputsCount(int count) {
    m_inputsCount = count;
    tiledTex = true;
    update();
}

bool TileObject::keepProportion() {
    return m_keepProportion;
}

void TileObject::setKeepProportion(bool keep) {
    m_keepProportion = keep;
    tiledTex = true;
    update();
}

QVector2D TileObject::resolution() {
    return m_resolution;
}

void TileObject::setResolution(QVector2D res) {
    m_resolution = res;
    resUpdated = true;
    update();
}

TileRenderer::TileRenderer(QVector2D res): m_resolution(res) {
    initializeOpenGLFunctions();

    tileShader = new QOpenGLShaderProgram();
    tileShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    tileShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/tile.frag");
    tileShader->link();

    randomShader = new QOpenGLShaderProgram();
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/noise.vert");
    randomShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/random.frag");
    randomShader->link();

    textureShader = new QOpenGLShaderProgram();
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/texture.vert");
    textureShader->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/texture.frag");
    textureShader->link();

    tileShader->bind();
    tileShader->setUniformValue(tileShader->uniformLocation("textureTile"), 0);
    tileShader->setUniformValue(tileShader->uniformLocation("maskTexture"), 1);
    tileShader->setUniformValue(tileShader->uniformLocation("randomTexture"), 2);
    tileShader->setUniformValue(tileShader->uniformLocation("tile1"), 3);
    tileShader->setUniformValue(tileShader->uniformLocation("tile2"), 4);
    tileShader->setUniformValue(tileShader->uniformLocation("tile3"), 5);
    tileShader->setUniformValue(tileShader->uniformLocation("tile4"), 6);
    tileShader->setUniformValue(tileShader->uniformLocation("tile5"), 7);
    tileShader->release();

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

    glGenFramebuffers(1, &tileFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, tileFBO);
    glGenTextures(1, &m_tiledTexture);
    glBindTexture(GL_TEXTURE_2D, m_tiledTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tiledTexture, 0);
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

TileRenderer::~TileRenderer() {
    delete tileShader;
    delete textureShader;
}

QOpenGLFramebufferObject *TileRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void TileRenderer::synchronize(QQuickFramebufferObject *item) {
    TileObject *tileItem = static_cast<TileObject*>(item);
    if(tileItem->resUpdated) {
        tileItem->resUpdated = false;
        m_resolution = tileItem->resolution();
        updateTexResolution();
    }
    if(tileItem->tiledTex) {
        tileItem->tiledTex = false;
        m_sourceTexture = tileItem->sourceTexture();
        m_tile1 = tileItem->tile1();
        m_tile2 = tileItem->tile2();
        m_tile3 = tileItem->tile3();
        m_tile4 = tileItem->tile4();
        m_tile5 = tileItem->tile5();
        if(m_sourceTexture || m_tile1 || m_tile2 || m_tile3 || m_tile4 || m_tile5) {
            maskTexture = tileItem->maskTexture();
            tileShader->bind();
            tileShader->setUniformValue(tileShader->uniformLocation("offsetX"), tileItem->offsetX());
            tileShader->setUniformValue(tileShader->uniformLocation("offsetY"), tileItem->offsetY());
            tileShader->setUniformValue(tileShader->uniformLocation("columns"), tileItem->columns());
            tileShader->setUniformValue(tileShader->uniformLocation("rows"), tileItem->rows());
            tileShader->setUniformValue(tileShader->uniformLocation("scaleX"), tileItem->scaleX());
            tileShader->setUniformValue(tileShader->uniformLocation("scaleY"), tileItem->scaleY());
            tileShader->setUniformValue(tileShader->uniformLocation("rotationAngle"), tileItem->rotationAngle());
            tileShader->setUniformValue(tileShader->uniformLocation("randPosition"), tileItem->randPosition());
            tileShader->setUniformValue(tileShader->uniformLocation("randRotation"), tileItem->randRotation());
            tileShader->setUniformValue(tileShader->uniformLocation("randScale"), tileItem->randScale());
            tileShader->setUniformValue(tileShader->uniformLocation("maskStrength"), tileItem->maskStrength());
            tileShader->setUniformValue(tileShader->uniformLocation("inputCount"), tileItem->inputsCount());
            tileShader->setUniformValue(tileShader->uniformLocation("keepProportion"), tileItem->keepProportion());
            tileShader->setUniformValue(tileShader->uniformLocation("useMask"), maskTexture);
            tileShader->release();
            createTile();
            tileItem->setTexture(m_tiledTexture);
            if(tileItem->selectedItem) {
                tileItem->updatePreview(m_tiledTexture, true);
            }
        }
        else {
            tileItem->setTexture(0);
        }
    }     
}

void TileRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if(m_sourceTexture  || m_tile1 || m_tile2 || m_tile3 || m_tile4 || m_tile5) {
        textureShader->bind();
        glBindVertexArray(textureVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_tiledTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        textureShader->release();
    }
}

void TileRenderer::createTile() {
    glBindFramebuffer(GL_FRAMEBUFFER, tileFBO);
    glViewport(0, 0, m_resolution.x(), m_resolution.y());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindVertexArray(textureVAO);
    tileShader->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_sourceTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, maskTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_randomTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_tile1);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_tile2);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_tile3);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_tile4);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, m_tile5);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    tileShader->release();
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TileRenderer::updateTexResolution() {
    glBindTexture(GL_TEXTURE_2D, m_tiledTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_resolution.x(), m_resolution.y(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}
