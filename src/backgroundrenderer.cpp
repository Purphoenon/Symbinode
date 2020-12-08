#include "backgroundrenderer.h"
#include <QOpenGLFramebufferObjectFormat>
#include "backgroundobject.h"
#include <iostream>

BackgroundRenderer::BackgroundRenderer()
{
    initializeOpenGLFunctions();
    backgroundGrid = new QOpenGLShaderProgram();
    backgroundGrid->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/grid.vert");
    backgroundGrid->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/grid.frag");
    backgroundGrid->link();

    float vert[] = {-1.0f, -1.0f,
                    -1.0f, 1.0f,
                    1.0f, -1.0f,
                    1.0f, 1.0f};
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vert), vert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

BackgroundRenderer::~BackgroundRenderer(){
    delete backgroundGrid;
}

QOpenGLFramebufferObject *BackgroundRenderer::createFramebufferObject(const QSize &size) {
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    format.setSamples(8);
    return new QOpenGLFramebufferObject(size, format);
}

void BackgroundRenderer::synchronize(QQuickFramebufferObject *item) {
    BackgroundObject *bckObj = static_cast<BackgroundObject*>(item);
    if(bckObj->isPan) {
        bckObj->isPan = false;
        panX = bckObj->viewPan().x();
        panY = bckObj->viewPan().y();
    }
    if(bckObj->isScaled) {
        bckObj->isScaled  = false;
        scale = bckObj->viewScale();
        panX = bckObj->viewPan().x();
        panY = bckObj->viewPan().y();
    }
}

void BackgroundRenderer::render() {
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.33f, 0.35f, 0.36f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    backgroundGrid->bind();
    backgroundGrid->setUniformValue(backgroundGrid->uniformLocation("offset"), QVector2D(panX, panY));
    backgroundGrid->setUniformValue(backgroundGrid->uniformLocation("scale"), scale);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    backgroundGrid->release();
}
