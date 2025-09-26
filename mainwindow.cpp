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
#include <QToolBar>
#include <QAction>
#include <QStyle>
#include <QStatusBar>
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    applyElegantStyle();

    setWindowTitle("QT Webcam");

    videoWidget = new VideoGLWidget(this);

    captureButton = findChild<QPushButton*>("captureButton");
    if (captureButton) {
        connect(captureButton, &QPushButton::clicked, this, &MainWindow::onCaptureButtonClicked);
    }
    filtersComboBox = findChild<QComboBox*>("filtersComboBox");
    if (filtersComboBox) {
        filtersComboBox->hide();
    }

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
    previewsScrollArea->setFixedHeight(130);
    previewsScrollArea->setWidget(previewsContainer);

    mainColumnLayout->addWidget(previewsScrollArea, 0);
    ui->videoLayout->addLayout(mainColumnLayout);

    setupToolBar();
    statusBar()->showMessage("Ready");

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
    if (statusBar()) statusBar()->showMessage("Captured", 1500);
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
    if (statusBar()) {
        statusBar()->showMessage(QStringLiteral("Filter: ") + filterNameByType(videoWidget->currentFilterType()), 2000);
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

void MainWindow::applyElegantStyle()
{
    const QString style = R"( 
        QMainWindow { background-color: #121212; }
        QWidget { color: #EDEDED; font-size: 11pt; }
        QStatusBar { background: #1E1E1E; color: #CFCFCF; }
        QToolBar { background: #1A1A1A; spacing: 6px; border: none; padding: 4px; }
        QToolButton { color: #EDEDED; padding: 6px 10px; border-radius: 6px; }
        QToolButton:hover { background: rgba(255,255,255,0.06); }
        QScrollArea { background: transparent; border: none; }
        QLabel { color: #CFCFCF; }
        QPushButton#captureButton { 
            border: 2px solid #FFFFFF; 
            border-radius: 25px; 
            min-width: 50px; min-height: 50px; 
            box-shadow: 0px 4px 12px rgba(0,0,0,0.6);
        }
        QPushButton#captureButton:hover { filter: brightness(1.1); }
        QPushButton#captureButton:pressed { transform: translateY(1px); }
    )";
    setStyleSheet(style);
}

void MainWindow::setupToolBar()
{
    if (!mainToolBar) {
        mainToolBar = new QToolBar(tr("Toolbar"), this);
        mainToolBar->setMovable(false);
        addToolBar(Qt::TopToolBarArea, mainToolBar);
    }

    // Capture action
    captureAction = new QAction(tr("Capture"), this);
    captureAction->setShortcut(QKeySequence(Qt::Key_Space));
    captureAction->setToolTip(tr("Capture snapshot (Space)"));
    connect(captureAction, &QAction::triggered, this, &MainWindow::onCaptureButtonClicked);

    // Quit action
    quitAction = new QAction(tr("Quit"), this);
    quitAction->setShortcut(QKeySequence::Quit);
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    mainToolBar->addAction(captureAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(quitAction);
}

QString MainWindow::filterNameByType(VideoGLWidget::FilterType type) const
{
    switch (type) {
    case VideoGLWidget::None: return QStringLiteral("None");
    case VideoGLWidget::Grayscale: return QStringLiteral("Grayscale");
    case VideoGLWidget::Sepia: return QStringLiteral("Sepia");
    case VideoGLWidget::Invert: return QStringLiteral("Invert");
    case VideoGLWidget::Blur: return QStringLiteral("Blur");
    case VideoGLWidget::Edge: return QStringLiteral("Edge");
    case VideoGLWidget::Move: return QStringLiteral("Move");
    case VideoGLWidget::Cool: return QStringLiteral("Cool");
    case VideoGLWidget::Emboss: return QStringLiteral("Emboss");
    case VideoGLWidget::Posterize: return QStringLiteral("Posterize");
    case VideoGLWidget::Warm: return QStringLiteral("Warm");
    case VideoGLWidget::Sharpen: return QStringLiteral("Sharpen");
    }
    return QStringLiteral("Unknown");
}

