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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    videoWidget = new VideoGLWidget(this);


    connect(ui->captureButton, &QPushButton::clicked, this, &MainWindow::onCaptureButtonClicked);
    connect(ui->filtersButton, &QPushButton::clicked, this, &MainWindow::onFiltersButtonClicked);

    ui->videoLayout->addWidget(videoWidget);

    cameraCapture = new CameraCapture(this);
    connect(cameraCapture, &CameraCapture::frameReady, videoWidget, &VideoGLWidget::setFrame);
    connect(cameraCapture, &CameraCapture::imageCaptured, this, &MainWindow::onImageCaptured);

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

void MainWindow::onFiltersButtonClicked()
{
    QMenu menu(this);
    QAction *noneAction = menu.addAction("None");
    QAction *grayAction = menu.addAction("Grayscale");
    QAction *sepiaAction = menu.addAction("Sepia");
    QAction *chosen = menu.exec(filtersButton->mapToGlobal(QPoint(0, filtersButton->height())));
    if (chosen == noneAction) {
        videoWidget->setFilterType(VideoGLWidget::None);
    } else if (chosen == grayAction) {
        videoWidget->setFilterType(VideoGLWidget::Grayscale);
    } else if (chosen == sepiaAction) {
        videoWidget->setFilterType(VideoGLWidget::Sepia);
    }
}
