#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QMediaDevices>
#include <QCameraDevice>
#include <QDebug>
#include <QVideoFrame>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    ui->centralwidget->setLayout(layout);

    // List available cameras
    qDebug() << "Available cameras:";
    const auto cameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cam : cameras)
        qDebug() << " -" << cam.description();

    if (cameras.isEmpty()) {
        qDebug() << "No camera detected!";
        return;
    }

    camera = new QCamera(cameras.first(), this);

    session.setCamera(camera);

    videoSink = new QVideoSink(this);
    session.setVideoOutput(videoSink);

    connect(videoSink, &QVideoSink::videoFrameChanged, this, [=](const QVideoFrame &frame){
        if (!frame.isValid()) return;
        QImage img = frame.toImage();
        if (!img.isNull())
            label->setPixmap(QPixmap::fromImage(img).scaled(label->size(), Qt::KeepAspectRatio));
    });

    // Start camera
    camera->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}
