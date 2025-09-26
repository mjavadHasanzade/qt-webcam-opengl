#ifndef VIDEOGLWIDGET_H
#define VIDEOGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QImage>
#include <QOpenGLShaderProgram>
#include <QElapsedTimer>
#include <QTimer>

class VideoGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    enum FilterType {
        None,
        Grayscale,
        Sepia,
        Invert,
        Blur,
        Edge,
        Move,
        Cool,
        Emboss,
        Posterize,
        Warm,
        Sharpen,
        // Add more filters here
    };
    explicit VideoGLWidget(QWidget *parent = nullptr);
    void setFrame(const QImage &frame);
    void setFilterType(FilterType type);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    QImage currentFrame;
    GLuint textureId;
    QOpenGLShaderProgram *program;
    FilterType filterType = None;
    void updateTexture();
    void updateShader();
    QElapsedTimer timer;
    QTimer frameUpdateTimer;
    bool glInitialized = false;
};

#endif // VIDEOGLWIDGET_H
