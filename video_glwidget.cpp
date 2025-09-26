#include "video_glwidget.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>
#include <QFile>
#include <QTimer>
#include <QElapsedTimer>

VideoGLWidget::VideoGLWidget(QWidget *parent)
    : QOpenGLWidget(parent), textureId(0), program(nullptr)
{
    if (!timer.isValid()) timer.start();
    connect(&frameUpdateTimer, &QTimer::timeout, this, [this]{
        if (filterType == Move) update();
    });
    frameUpdateTimer.setInterval(16); // ~60 FPS
    frameUpdateTimer.start();
}

void VideoGLWidget::setFrame(const QImage &frame)
{
    currentFrame = frame.convertToFormat(QImage::Format_RGBA8888).flipped(Qt::Horizontal | Qt::Vertical);
    updateTexture();
    update();
}

void VideoGLWidget::setFilterType(FilterType type)
{
    if (filterType != type) {
        filterType = type;
        if (filterType == Move) {
            float t = timer.elapsed() / 1000.0f;
            program->setUniformValue("time", t);
        }
        updateShader();
        update();
    }
}

void VideoGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glGenTextures(1, &textureId);
    updateShader();
}

void VideoGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (currentFrame.isNull() || !program) return;

    program->bind();
    glBindTexture(GL_TEXTURE_2D, textureId);

    if (filterType == Move) {
        float t = timer.elapsed() / 1000.0f;
        program->setUniformValue("time", t);
    }

    // Set up vertices and texture coordinates for a full-screen quad
    GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };
    int posLoc = program->attributeLocation("position");
    int texLoc = program->attributeLocation("texCoord");
    program->enableAttributeArray(posLoc);
    program->enableAttributeArray(texLoc);
    program->setAttributeArray(posLoc, GL_FLOAT, vertices, 2, 4 * sizeof(GLfloat));
    program->setAttributeArray(texLoc, GL_FLOAT, vertices + 2, 2, 4 * sizeof(GLfloat));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    program->disableAttributeArray(posLoc);
    program->disableAttributeArray(texLoc);
    program->release();
}

void VideoGLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void VideoGLWidget::updateTexture()
{
    if (currentFrame.isNull()) return;
    makeCurrent();
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, currentFrame.width(), currentFrame.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, currentFrame.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    doneCurrent();
}

void VideoGLWidget::updateShader()
{
    if (program) {
        delete program;
        program = nullptr;
    }
    program = new QOpenGLShaderProgram(this);
    program->addShaderFromSourceCode(QOpenGLShader::Vertex,
        "attribute vec2 position;\n"
        "attribute vec2 texCoord;\n"
        "varying vec2 vTexCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "    vTexCoord = texCoord;\n"
        "}"
    );
    QString fragPath;
    switch (filterType) {
    case None:
        fragPath = ":/assets/filters/none.frag";
        break;
    case Grayscale:
        fragPath = ":/assets/filters/grayscale.frag";
        break;
    case Sepia:
        fragPath = ":/assets/filters/sepia.frag";
        break;
    case Invert:
        fragPath = ":/assets/filters/invert.frag";
        break;
    case Blur:
        fragPath = ":/assets/filters/blur.frag";
        break;
    case Edge:
        fragPath = ":/assets/filters/edge.frag";
        break;
    case Move:
        fragPath = ":/assets/filters/move.frag";
        break;
    case Cool:
        fragPath = ":/assets/filters/cool.frag";
        break;
    case Emboss:
        fragPath = ":/assets/filters/emboss.frag";
        break;
    case Posterize:
        fragPath = ":/assets/filters/posterize.frag";
        break;
    case Warm:
        fragPath = ":/assets/filters/warm.frag";
        break;
    case Sharpen:
        fragPath = ":/assets/filters/sharpen.frag";
        break;
    }
    QFile file(fragPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString frag = file.readAll();
        program->addShaderFromSourceCode(QOpenGLShader::Fragment, frag);
        program->link();
    }
}
