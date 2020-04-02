#include "verifymemberform.h"
#include "ui_verifymemberform.h"

#include <QMessageBox>

VerifyMemberForm::VerifyMemberForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VerifyMemberForm)
{
    ui->setupUi(this);
}

VerifyMemberForm::~VerifyMemberForm()
{
    delete ui;
}

void VerifyMemberForm::on_SearchPushButton_clicked()
{
    int step = 1;
    api service;
    QString id = "";
    QString id_type = "";

    if( ui->VehicleNumberRadioButton->isChecked() )
    {
        id_type = "license_number";
    }
    else if( ui->SAIDRadioButton->isChecked() )
    {
        id_type = "id_number";
    }
    else
    {
        /* Display notification */
        QMessageBox message_box;
        message_box.setWindowOpacity(50);
        message_box.setWindowTitle("Member Verification");
        message_box.setStyleSheet("QLabel{ font-weight: plain; font-size: 14px; } \
                                     QPushButton{ width:125px; height:10; font-size: 14px; }");
        message_box.setStandardButtons(QMessageBox::Ok);

        message_box.setIcon(QMessageBox::Critical);
        message_box.setText("Plase select verification methods");

        message_box.exec();

        return;
    }

    id = ui->MemberIDLineEdit->text();


    if( service.isMemberRegistered(id_type, id ) )
    {
        emit verification_success_signal(step);
    }
    else
    {
        /* Display notification */
        QMessageBox message_box;
        message_box.setWindowOpacity(50);
        message_box.setWindowTitle("Member Verification");
        message_box.setStyleSheet("QLabel{ font-weight: plain; font-size: 14px; } \
                                     QPushButton{ width:125px; height:10; font-size: 14px; }");
        message_box.setStandardButtons(QMessageBox::Ok);

        message_box.setIcon(QMessageBox::Warning);
        message_box.setText("Member does not exist!");

        message_box.exec();

        emit verification_failed_signal();
    }
}
