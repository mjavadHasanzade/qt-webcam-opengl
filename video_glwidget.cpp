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
        if (glInitialized) {
            updateShader();
            update();
        }
    }
}

void VideoGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    glGenTextures(1, &textureId);
    glInitialized = true;
    updateShader();
}

void VideoGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    if (currentFrame.isNull() || !program || !program->isLinked()) return;

    program->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    program->setUniformValue("texture", 0);

    if (filterType == Move || filterType == FilmGrain) {
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
    // Ensure we have a current context for all GL and shader operations
    if (!isValid()) return;
    makeCurrent();
    if (program) {
        delete program;
        program = nullptr;
    }
    auto createProgramWithVertex = [this]() {
        QOpenGLShaderProgram *p = new QOpenGLShaderProgram(this);
        bool vOKLocal = p->addShaderFromSourceCode(QOpenGLShader::Vertex,
        "attribute vec2 position;\n"
        "attribute vec2 texCoord;\n"
        "varying vec2 vTexCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "    vTexCoord = texCoord;\n"
        "}"
        );
        if (!vOKLocal) {
            qWarning("Vertex shader compile error: %s", qPrintable(p->log()));
        }
        return p;
    };
    program = createProgramWithVertex();
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
    case Clarendon:
        fragPath = ":/assets/filters/clarendon.frag";
        break;
    case Gingham:
        fragPath = ":/assets/filters/gingham.frag";
        break;
    case Valencia:
        fragPath = ":/assets/filters/valencia.frag";
        break;
    case Nashville:
        fragPath = ":/assets/filters/nashville.frag";
        break;
    case XPro2:
        fragPath = ":/assets/filters/xpro2.frag";
        break;
    case Lomo:
        fragPath = ":/assets/filters/lomo.frag";
        break;
    case Hudson:
        fragPath = ":/assets/filters/hudson.frag";
        break;
    case Willow:
        fragPath = ":/assets/filters/willow.frag";
        break;
    case Rise:
        fragPath = ":/assets/filters/rise.frag";
        break;
    case Amaro:
        fragPath = ":/assets/filters/amaro.frag";
        break;
    case Tint:
        fragPath = ":/assets/filters/tint.frag";
        break;
    case Brightness:
        fragPath = ":/assets/filters/brightness.frag";
        break;
    case Contrast:
        fragPath = ":/assets/filters/contrast.frag";
        break;
    case Vignette:
        fragPath = ":/assets/filters/vignette.frag";
        break;
    case FilmGrain:
        fragPath = ":/assets/filters/filmgrain.frag";
        break;
    case GridOverlay:
        fragPath = ":/assets/filters/gridoverlay.frag";
        break;
    case CircleVignette:
        fragPath = ":/assets/filters/circlevignette.frag";
        break;
    case BokehDots:
        fragPath = ":/assets/filters/bokehdots.frag";
        break;
    case CornerFrame:
        fragPath = ":/assets/filters/cornerframe.frag";
        break;
    }
    auto loadAndLink = [this](const QString &path) -> bool {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
        const QString src = f.readAll();
        bool ok = program->addShaderFromSourceCode(QOpenGLShader::Fragment, src);
        if (!ok) {
            qWarning("Fragment shader compile error (%s): %s", qPrintable(path), qPrintable(program->log()));
            return false;
        }
        ok = program->link();
        if (!ok) {
            qWarning("Shader link error (%s): %s", qPrintable(path), qPrintable(program->log()));
        }
        return ok;
    };

    bool linked = loadAndLink(fragPath);
    if (!linked) {
        // Recreate and attach vertex shader before fallback fragment
        delete program;
        program = createProgramWithVertex();
        if (!loadAndLink(":/assets/filters/none.frag")) {
            delete program;
            program = nullptr;
        }
    }
    doneCurrent();
}
