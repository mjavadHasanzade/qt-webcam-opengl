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
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QVector>

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
    QVBoxLayout *mainColumnLayout = nullptr;
    QHBoxLayout *previewsLayout = nullptr;
    QScrollArea *previewsScrollArea = nullptr;
    QVector<VideoGLWidget*> previewWidgets;

private slots:
    void onCaptureButtonClicked();
    void onImageCaptured(const QImage &image);
    void onFilterChanged(int index);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // MAINWINDOW_H
