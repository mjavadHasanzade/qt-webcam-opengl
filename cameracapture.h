#ifndef CAMERACAPTURE_H
#define CAMERACAPTURE_H

#include <QObject>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QImage>

class CameraCapture : public QObject
{
    Q_OBJECT
public:
    explicit CameraCapture(QObject *parent = nullptr);
    ~CameraCapture();

public slots:
    void captureImage();

signals:
    void frameReady(const QImage &image);
    void imageCaptured(const QImage &image);

private:
    QCamera *camera;
    QMediaCaptureSession session;
    QVideoSink *videoSink;

    void setupCamera();
};

#endif // CAMERACAPTURE_H
