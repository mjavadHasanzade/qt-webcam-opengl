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
#include "video_glwidget.h"
#include <QMenu>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    videoWidget = new VideoGLWidget(this);

    captureButton = findChild<QPushButton*>("captureButton");
    if (captureButton) {
        connect(captureButton, &QPushButton::clicked, this, &MainWindow::onCaptureButtonClicked);
    }
    filtersComboBox = findChild<QComboBox*>("filtersComboBox");
    if (filtersComboBox) {
        filtersComboBox->addItem("None");
        filtersComboBox->addItem("Grayscale");
        filtersComboBox->addItem("Sepia");
        connect(filtersComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFilterChanged);
        filtersComboBox->setCurrentIndex(0);
    }

    ui->videoLayout->addWidget(videoWidget);

    cameraCapture = new CameraCapture(this);
    connect(cameraCapture, &CameraCapture::frameReady, videoWidget, &VideoGLWidget::setFrame);
    connect(cameraCapture, &CameraCapture::imageCaptured, this, &MainWindow::onImageCaptured);

    // Start with no filter
    videoWidget->setFilterType(VideoGLWidget::None);
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

void MainWindow::onFilterChanged(int index)
{
    switch (index) {
    case 0:
        videoWidget->setFilterType(VideoGLWidget::None);
        break;
    case 1:
        videoWidget->setFilterType(VideoGLWidget::Grayscale);
        break;
    case 2:
        videoWidget->setFilterType(VideoGLWidget::Sepia);
        break;
    }
}

