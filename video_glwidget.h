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
        Clarendon,
        Gingham,
        Valencia,
        Nashville,
        XPro2,
        Lomo,
        Hudson,
        Willow,
        Rise,
        Amaro,
        Tint,
        Brightness,
        Contrast,
        Vignette,
        FilmGrain,
        GridOverlay,
        CircleVignette,
        BokehDots,
        CornerFrame,
        // Add more filters here
    };
    explicit VideoGLWidget(QWidget *parent = nullptr);
    void setFrame(const QImage &frame);
    void setFilterType(FilterType type);
    FilterType currentFilterType() const { return filterType; }

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
