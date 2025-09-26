#ifndef CAPTUREDIALOG_H
#define CAPTUREDIALOG_H

#include <QDialog>
#include <QImage>

class QLabel;
class QPushButton;

class CaptureDialog : public QDialog {
    Q_OBJECT
public:
    explicit CaptureDialog(const QImage &image, QWidget *parent = nullptr);
private slots:
    void onSaveClicked();
private:
    QLabel *imageLabel;
    QPushButton *saveButton;
    QImage capturedImage;
};

#endif // CAPTUREDIALOG_H
