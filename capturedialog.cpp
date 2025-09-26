#include "capturedialog.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>

CaptureDialog::CaptureDialog(const QImage &image, QWidget *parent)
    : QDialog(parent), capturedImage(image)
{
    setWindowTitle("Captured Image");
    imageLabel = new QLabel(this);
    imageLabel->setPixmap(QPixmap::fromImage(image).scaled(320, 240, Qt::KeepAspectRatio));
    imageLabel->setAlignment(Qt::AlignCenter);

    saveButton = new QPushButton("Save", this);
    connect(saveButton, &QPushButton::clicked, this, &CaptureDialog::onSaveClicked);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(imageLabel);
    layout->addWidget(saveButton);
    setLayout(layout);
}

void CaptureDialog::onSaveClicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", "", "PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;BMP Files (*.bmp)");
    if (!fileName.isEmpty()) {
        if (!capturedImage.save(fileName)) {
            QMessageBox::warning(this, "Save Failed", "Failed to save the image.");
        }
    }
}
