#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QLabel>
#include "cameracapture.h"
#include <QPushButton>
#include <QComboBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CameraCapture;
class VideoGLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    VideoGLWidget *videoWidget;
    QPushButton *captureButton;
    QComboBox *filtersComboBox;
    CameraCapture *cameraCapture;

private slots:
    void onCaptureButtonClicked();
    void onImageCaptured(const QImage &image);
    void onFilterChanged(int index);
};

#endif // MAINWINDOW_H
