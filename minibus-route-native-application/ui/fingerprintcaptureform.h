#ifndef FINGERPRINTCAPTUREFORM_H
#define FINGERPRINTCAPTUREFORM_H

#include <QImage>
#include <QMainWindow>
#include <QWidget>
#include <QJsonObject>
#include <QVector>
#include <QSqlField>
#include <QSqlRecord>
#include <api.h>
#include <scanner.h>
#include <futronic_scanner.h>

#include <person.h>

namespace Ui {
class FingerprintCaptureForm;
}

class FingerprintCaptureForm : public QWidget
{
    Q_OBJECT

public:
    explicit FingerprintCaptureForm(QWidget *parent = nullptr);
    ~FingerprintCaptureForm();

    void setPerson(QJsonObject &member, AdminMode mode);

    void setMember(const QString memberDbID);

    void setEmployee(QString employeeDbID);

    void setMilitaryVeteran(QString militaryVeteranDbID);
public slots:

    /*!
    * \brief on_update_screen_during_streaming
    * \param Flag
    */
    void on_update_screen_during_streaming(int flag);

    /*!
     * \brief on_image_receive
     * \param Image
     * \param quality
     */
    void on_image_receive(QByteArray Image, int quality);

signals:
    void home_button_clicked_signal(AdminMode mode);

private slots:
    void on_HomePushButton_clicked();

    void on_CapturePushButton_clicked();

    void on_FingerprintSavePushButton_clicked();

    void on_FingerprintRetrievalSuccessful(QJsonObject &member);
    void on_FingerprintRetrievalFailure();

    void on_FingerprintPostSuccessful();
    void on_FingerprintPostFailure();

private:
    Ui::FingerprintCaptureForm *ui;

    Scanner* scanner_;

    /* Person Details */
    QString employee_db_id_;
    QString military_veteran_db_id_;
    QString member_db_id_;
    QVector<QSqlRecord> member_;
    AdminMode mode_;

    /*!
    * \brief The fingerData struct
    */
    struct fingerData
    {
      unsigned char *image;
      int quality_score;
      int image_width;
      int image_height;

      // assign memory
      /*!
       * \brief initialiseMemory
       * \param size
       */
      void initialiseMemory(int size)
      {
          this->image = new (nothrow) unsigned char[size];
      }

      // assign values during streaming
      /*!
       * \brief setFingerDataValues
       * \param image
       * \param quality_score
       * \param image_width
       * \param image_height
       */
      void setFingerDataValues( unsigned char *image, int quality_score,\
                           int image_width, int image_height )
      {

          // copying
          for(int i = 0 ; i < image_width * image_height; i++)
            this->image[i] = image[i];

          // set values
          this->quality_score = quality_score;
          this->image_width   = image_width;
          this->image_height  = image_height;
      }


    };

    QImage captured_image_;
    QImage qleft_thumb_image_;
    QImage qright_thumb_image_;
    fingerData left_thumb_image_;
    fingerData right_thumb_image_;
    fingerData current_finger_;
    bool is_left_thumb_captured_;
    bool is_right_thumb_captured_;
    bool is_streaming_;
    bool is_fingerprint_captured_;
    bool stop_streaming_;
    unsigned char* current_image_;
    int c_buffer_size_ ;
    int current_image_width_;
    int current_image_height_;
    int stored_image_counter_;
};

#endif // FINGERPRINTCAPTUREFORM_H
