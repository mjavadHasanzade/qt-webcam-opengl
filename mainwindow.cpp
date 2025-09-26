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
#include <QScrollArea>
#include <QEvent>
#include <QMouseEvent>

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
        filtersComboBox->hide();
    }

    // Build a vertical stack inside the left video area: main video on top, previews at bottom
    mainColumnLayout = new QVBoxLayout();
    mainColumnLayout->setContentsMargins(0, 0, 0, 0);
    mainColumnLayout->setSpacing(6);
    mainColumnLayout->addWidget(videoWidget, 1);

    previewsLayout = new QHBoxLayout();
    previewsLayout->setContentsMargins(6, 6, 6, 6);
    previewsLayout->setSpacing(6);

    QWidget *previewsContainer = new QWidget(this);
    previewsContainer->setLayout(previewsLayout);

    previewsScrollArea = new QScrollArea(this);
    previewsScrollArea->setWidgetResizable(true);
    previewsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    previewsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    previewsScrollArea->setFixedHeight(120);
    previewsScrollArea->setWidget(previewsContainer);

    mainColumnLayout->addWidget(previewsScrollArea, 0);
    ui->videoLayout->addLayout(mainColumnLayout);

    cameraCapture = new CameraCapture(this);
    connect(cameraCapture, &CameraCapture::frameReady, videoWidget, &VideoGLWidget::setFrame);
    connect(cameraCapture, &CameraCapture::imageCaptured, this, &MainWindow::onImageCaptured);

    // Start with no filter
    videoWidget->setFilterType(VideoGLWidget::None);

    struct FilterItem { VideoGLWidget::FilterType type; const char *name; };
    const FilterItem items[] = {
        { VideoGLWidget::None, "None" },
        { VideoGLWidget::Grayscale, "Grayscale" },
        { VideoGLWidget::Sepia, "Sepia" },
        { VideoGLWidget::Invert, "Invert" },
        { VideoGLWidget::Blur, "Blur" },
        { VideoGLWidget::Edge, "Edge" },
        { VideoGLWidget::Move, "Move" },
        { VideoGLWidget::Cool, "Cool" },
        { VideoGLWidget::Emboss, "Emboss" },
        { VideoGLWidget::Posterize, "Posterize" },
        { VideoGLWidget::Warm, "Warm" },
        { VideoGLWidget::Sharpen, "Sharpen" },
    };

    for (const FilterItem &fi : items) {
        QWidget *itemWrapper = new QWidget(this);
        QVBoxLayout *itemLayout = new QVBoxLayout(itemWrapper);
        itemLayout->setContentsMargins(2, 2, 2, 2);
        itemLayout->setSpacing(2);

        VideoGLWidget *preview = new VideoGLWidget(itemWrapper);
        preview->setFixedSize(140, 90);
        preview->setFilterType(fi.type);
        preview->installEventFilter(this);
        preview->setProperty("filterType", static_cast<int>(fi.type));

        QLabel *label = new QLabel(QString::fromUtf8(fi.name), itemWrapper);
        label->setAlignment(Qt::AlignHCenter);
        label->setMinimumHeight(16);

        itemLayout->addWidget(preview);
        itemLayout->addWidget(label);
        previewsLayout->addWidget(itemWrapper);
        previewWidgets.append(preview);
    }

    // Feed scaled frames to previews for performance
    if (cameraCapture) {
        for (VideoGLWidget *preview : previewWidgets) {
            connect(cameraCapture, &CameraCapture::frameReady, preview, [preview](const QImage &img){
                if (img.isNull()) return;
                QImage scaled = img.scaled(140, 90, Qt::KeepAspectRatio, Qt::FastTransformation);
                preview->setFrame(scaled);
            });
        }
    }
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
    QImage toShow = image;
    if (videoWidget && videoWidget->currentFilterType() != VideoGLWidget::None) {
        // Grab the filtered framebuffer to reflect current filter in the capture
        QImage grabbed = videoWidget->grabFramebuffer();
        if (!grabbed.isNull()) {
            toShow = grabbed.convertToFormat(QImage::Format_ARGB32);
        }
    }
    CaptureDialog *dialog = new CaptureDialog(toShow, this);
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
    case 3:
        videoWidget->setFilterType(VideoGLWidget::Invert);
        break;
    case 4:
        videoWidget->setFilterType(VideoGLWidget::Blur);
        break;
    case 5:
        videoWidget->setFilterType(VideoGLWidget::Edge);
        break;
    case 6:
        videoWidget->setFilterType(VideoGLWidget::Move);
        break;
    case 7:
        videoWidget->setFilterType(VideoGLWidget::Cool);
        break;
    case 8:
        videoWidget->setFilterType(VideoGLWidget::Emboss);
        break;
    case 9:
        videoWidget->setFilterType(VideoGLWidget::Posterize);
        break;
    case 10:
        videoWidget->setFilterType(VideoGLWidget::Warm);
        break;
    case 11:
        videoWidget->setFilterType(VideoGLWidget::Sharpen);
        break;
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        for (VideoGLWidget *preview : qAsConst(previewWidgets)) {
            if (watched == preview) {
                QVariant v = preview->property("filterType");
                if (v.isValid()) {
                    int t = v.toInt();
                    videoWidget->setFilterType(static_cast<VideoGLWidget::FilterType>(t));
                }
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

