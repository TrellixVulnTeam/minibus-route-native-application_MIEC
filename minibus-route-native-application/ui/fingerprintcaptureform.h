#ifndef FINGERPRINTCAPTUREFORM_H
#define FINGERPRINTCAPTUREFORM_H

#include <QImage>
#include <QMainWindow>
#include <QWidget>
#include <scanner.h>
#include <futronic_scanner.h>

namespace Ui {
class FingerprintCaptureForm;
}

class FingerprintCaptureForm : public QWidget
{
    Q_OBJECT

public:
    explicit FingerprintCaptureForm(QWidget *parent = nullptr);
    ~FingerprintCaptureForm();

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
    void home_button_clicked_signal();

private slots:
    void on_HomePushButton_clicked();

    void on_CapturePushButton_clicked();

    void on_FingerprintSavePushButton_clicked();

private:
    Ui::FingerprintCaptureForm *ui;

    Scanner* scanner_;

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
    bool is_streaming_;
    bool stop_streaming_;
    unsigned char* current_image_;
    int c_buffer_size_ ;
    int current_image_width_;
    int current_image_height_;
    int stored_image_counter_;
};

#endif // FINGERPRINTCAPTUREFORM_H
