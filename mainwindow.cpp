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
#include <QGraphicsDropShadowEffect>

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
        // Add shadow using effect rather than unsupported CSS properties
        auto *shadow = new QGraphicsDropShadowEffect(this);
        shadow->setBlurRadius(16);
        shadow->setOffset(0, 3);
        shadow->setColor(QColor(0,0,0,160));
        captureButton->setGraphicsEffect(shadow);
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
        { VideoGLWidget::Clarendon, "Clarendon" },
        { VideoGLWidget::Gingham, "Gingham" },
        { VideoGLWidget::Valencia, "Valencia" },
        { VideoGLWidget::Nashville, "Nashville" },
        { VideoGLWidget::XPro2, "X-Pro II" },
        { VideoGLWidget::Lomo, "Lomo" },
        { VideoGLWidget::Hudson, "Hudson" },
        { VideoGLWidget::Willow, "Willow" },
        { VideoGLWidget::Rise, "Rise" },
        { VideoGLWidget::Amaro, "Amaro" },
        { VideoGLWidget::Tint, "Tint" },
        { VideoGLWidget::Brightness, "Brightness" },
        { VideoGLWidget::Contrast, "Contrast" },
        { VideoGLWidget::Vignette, "Vignette" },
        { VideoGLWidget::FilmGrain, "Film Grain" },
        { VideoGLWidget::GridOverlay, "Grid Overlay" },
        { VideoGLWidget::CircleVignette, "Circle Vignette" },
        { VideoGLWidget::BokehDots, "Bokeh Dots" },
        { VideoGLWidget::CornerFrame, "Corner Frame" },
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
        }
        QPushButton#captureButton:hover { background: rgba(255,255,255,0.06); }
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
    case VideoGLWidget::Clarendon: return QStringLiteral("Clarendon");
    case VideoGLWidget::Gingham: return QStringLiteral("Gingham");
    case VideoGLWidget::Valencia: return QStringLiteral("Valencia");
    case VideoGLWidget::Nashville: return QStringLiteral("Nashville");
    case VideoGLWidget::XPro2: return QStringLiteral("X-Pro II");
    case VideoGLWidget::Lomo: return QStringLiteral("Lomo");
    case VideoGLWidget::Hudson: return QStringLiteral("Hudson");
    case VideoGLWidget::Willow: return QStringLiteral("Willow");
    case VideoGLWidget::Rise: return QStringLiteral("Rise");
    case VideoGLWidget::Amaro: return QStringLiteral("Amaro");
    case VideoGLWidget::GridOverlay: return QStringLiteral("Grid Overlay");
    case VideoGLWidget::CircleVignette: return QStringLiteral("Circle Vignette");
    case VideoGLWidget::BokehDots: return QStringLiteral("Bokeh Dots");
    case VideoGLWidget::CornerFrame: return QStringLiteral("Corner Frame");
    case VideoGLWidget::Tint: return QStringLiteral("Tint");
    case VideoGLWidget::Brightness: return QStringLiteral("Brightness");
    case VideoGLWidget::Contrast: return QStringLiteral("Contrast");
    case VideoGLWidget::Vignette: return QStringLiteral("Vignette");
    case VideoGLWidget::FilmGrain: return QStringLiteral("Film Grain");
    }
    return QStringLiteral("Unknown");
}

