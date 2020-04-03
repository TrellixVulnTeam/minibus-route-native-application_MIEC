#ifndef PORTRAITCAPTUREFORM_H
#define PORTRAITCAPTUREFORM_H

#include <QWidget>
#include <QVector>
#include <QSqlRecord>
#include <QSqlField>
#include <QCamera>
#include <QCameraImageCapture>

#include <api.h>

namespace Ui {
class PortraitCaptureForm;
}

class PortraitCaptureForm : public QWidget
{
    Q_OBJECT

public:
    explicit PortraitCaptureForm(QWidget *parent = nullptr);

    ~PortraitCaptureForm();

    void setMember(const QVector<QSqlRecord> &member);

signals:
    void home_button_clicked_signal(QVector<QSqlRecord> &member);

public slots:
    /*!
     * \brief processCapturedImage
     * \param requestId
     * \param img
     */
    void processCapturedImage(int requestId, const QImage &img);

    /*!
     * \brief deleteTempDir
     * \param id
     * \param path
     */
    void deleteTempDir(int id, QString path);

    /*!
     * \brief savePrints
     * \param directory
     */
    void savePrints(QString directory);

private slots:
    void on_HomePushButton_clicked();

    void on_CapturePushButton_clicked();

    void on_PortraitSavePushButton_clicked();

private:
    Ui::PortraitCaptureForm *ui;

    /* Member Details */
    QVector<QSqlRecord> member_;

    /* Camera */
    QImage captured_image_;
    QCamera *camera_;
    QCameraImageCapture *image_capture_;
    bool is_capturing_image_;

    /* helper functions */
    void startCamera();
    void stopCamera();
    int scanDevices();
    void setCamera(const QCameraInfo &cameraInfo);
    void updateCaptureMode();
    void reset();
};

#endif // PORTRAITCAPTUREFORM_H
