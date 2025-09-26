#include "cameracapture.h"
#include <QMediaDevices>
#include <QCameraDevice>
#include <QDebug>
#include <QMutex>

namespace {
QImage lastFrame;
QMutex frameMutex;
}

CameraCapture::CameraCapture(QObject *parent)
    : QObject(parent), camera(nullptr), videoSink(nullptr)
{
    setupCamera();
}

CameraCapture::~CameraCapture()
{
    if (camera) {
        camera->stop();
        delete camera;
    }
    if (videoSink) {
        delete videoSink;
    }
}

void CameraCapture::setupCamera()
{
    const auto cameras = QMediaDevices::videoInputs();
    if (cameras.isEmpty()) {
        qDebug() << "No camera detected!";
        return;
    }
    camera = new QCamera(cameras.first(), this);
    session.setCamera(camera);
    videoSink = new QVideoSink(this);
    session.setVideoOutput(videoSink);
    connect(videoSink, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &frame){
        if (!frame.isValid()) return;
        QImage img = frame.toImage();
        if (!img.isNull()) {
            QMutexLocker locker(&frameMutex);
            lastFrame = img;
            emit frameReady(img);
        }
    });
    camera->start();
}

void CameraCapture::captureImage()
{
    QMutexLocker locker(&frameMutex);
    if (!lastFrame.isNull()) {
        emit imageCaptured(lastFrame);
    }
}
