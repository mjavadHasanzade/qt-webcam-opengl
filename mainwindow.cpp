#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QMediaDevices>
#include <QCameraDevice>
#include <QDebug>
#include <QVideoFrame>
#include <QPixmap>
#include "cameracapture.h"
#include <QPushButton>
#include <QMessageBox>
#include "capturedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);

    captureButton = findChild<QPushButton*>("captureButton");
    if (captureButton) {
        connect(captureButton, &QPushButton::clicked, this, &MainWindow::onCaptureButtonClicked);
    }

    ui->videoLayout->addWidget(label);

    cameraCapture = new CameraCapture(this);
    connect(cameraCapture, &CameraCapture::frameReady, this, [this](const QImage &img){
        if (!img.isNull())
            label->setPixmap(QPixmap::fromImage(img).scaled(label->size(), Qt::KeepAspectRatio));
    });
    connect(cameraCapture, &CameraCapture::imageCaptured, this, &MainWindow::onImageCaptured);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCaptureButtonClicked()
{
    if (cameraCapture)
        cameraCapture->captureImage();
}

void MainWindow::onImageCaptured(const QImage &image)
{
    if (image.isNull()) return;
    CaptureDialog *dialog = new CaptureDialog(image, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->show();
}
