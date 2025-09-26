#include "video_glwidget.h"
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

VideoGLWidget::VideoGLWidget(QWidget *parent)
    : QOpenGLWidget(parent), textureId(0), program(nullptr)
{}

void VideoGLWidget::setFrame(const QImage &frame)
{
    currentFrame = frame.convertToFormat(QImage::Format_RGBA8888).mirrored(false, true);
    updateTexture();
    update();
}

void VideoGLWidget::setFilterType(FilterType type)
{
    if (filterType != type) {
        filterType = type;
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
    QString frag;
    switch (filterType) {
    case None:
        frag =
            "uniform sampler2D texture;\n"
            "varying vec2 vTexCoord;\n"
            "void main() {\n"
            "    gl_FragColor = texture2D(texture, vTexCoord);\n"
            "}";
        break;
    case Grayscale:
        frag =
            "uniform sampler2D texture;\n"
            "varying vec2 vTexCoord;\n"
            "void main() {\n"
            "    vec4 color = texture2D(texture, vTexCoord);\n"
            "    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));\n"
            "    gl_FragColor = vec4(gray, gray, gray, 1.0);\n"
            "}";
        break;
    case Sepia:
        frag =
            "uniform sampler2D texture;\n"
            "varying vec2 vTexCoord;\n"
            "void main() {\n"
            "    vec4 color = texture2D(texture, vTexCoord);\n"
            "    float r = dot(color.rgb, vec3(0.393, 0.769, 0.189));\n"
            "    float g = dot(color.rgb, vec3(0.349, 0.686, 0.168));\n"
            "    float b = dot(color.rgb, vec3(0.272, 0.534, 0.131));\n"
            "    gl_FragColor = vec4(r, g, b, 1.0);\n"
            "}";
        break;
    }
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, frag);
    program->link();
}
