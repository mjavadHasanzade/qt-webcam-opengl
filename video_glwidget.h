#ifndef VIDEOGLWIDGET_H
#define VIDEOGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QImage>
#include <QOpenGLShaderProgram>

class VideoGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    enum FilterType {
        None,
        Grayscale,
        Sepia
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
};

#endif // VIDEOGLWIDGET_H
