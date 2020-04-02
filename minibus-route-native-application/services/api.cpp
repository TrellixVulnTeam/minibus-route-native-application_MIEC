#include "api.h"

api::api(QObject *parent) : QObject(parent)
{

}

bool api::authenticateUser()
{
    return true;
}

bool api::isMemberRegistered(QString id_type, QString id)
{
    /* init database */
    Database db;

    /* Declare & sanitize db fields */
    QString table = "members";
    QVector<QString> select_columns = { "id_number", "license_number", "name", "surname" };
    QVector<QString> column_list;
    QVector<QString> value_list;
    QVector<QSqlRecord> result;

    if( id_type == "license_number" )
    {
        column_list = { "license_number" };
        value_list = { id };
    }
    else
    {
        column_list = { "id_number" };
        value_list = { id };
    }

    /* populate fields */
    QDateTime dt = QDateTime::currentDateTime();
    QString today = dt.toString("yyyy-MM-dd HH:mm:ss");
    qDebug() << "api::isMemberRegistered() - Date Today: " << today;

    if( db.connOpen() )
    {
        if( db.select(table, select_columns, column_list, value_list, result) )
        {
           qDebug() << "api::isMemberRegistered() - Member verification successful";
           qDebug() << "api::isMemberRegistered() - Result: " << result;
           return true;
        }
        else
        {
            qDebug() << "api::isMemberRegistered() - Member verification failed";
            return false;
        }
    }
    else
    {
        qDebug() << "api::isMemberRegistered() - DB Connection failed";
    }
}

bool api::postCapturedFingerprint(QString member_id, QByteArray image)
{
    /* init database */
    Database db;

    /* Declare & sanitize db fields */
    QString table = "fingerprints";

    /* populate fields */
    QDateTime dt = QDateTime::currentDateTime();
    QString today = dt.toString("yyyy-MM-dd HH:mm:ss");
    qDebug() << "api::postCapturedFingerprint() - Date Today: " << today;

    if( db.connOpen() )
    {
        if( db.insertTemplate(table, "fingerprint", member_id, today, today, image) )
        {
           qDebug() << "api::postCapturedFingerprint() - Fingerprint POST successful";
           return true;
        }
        else
        {
            qDebug() << "api::postCapturedFingerprint() - Fingerprint POST failed";
            return false;
        }
    }
    else
    {
        qDebug() << "api::postCapturedFingerprint() - DB Connection failed";
    }

    return false;
}

bool api::postCapturedPortrait(QString member_id, QByteArray image)
{
    /* init database */
    Database db;

    /* Declare & sanitize db fields */
    QString table = "portraits";

    /* populate fields */
    QDateTime dt = QDateTime::currentDateTime();
    QString today = dt.toString("yyyy-MM-dd HH:mm:ss");
    qDebug() << "api::postCapturedPortrait() - Date Today: " << today;

    if( db.connOpen() )
    {
        if( db.insertTemplate(table, "portrait", member_id, today, today, image) )
        {
           qDebug() << "api::postCapturedPortrait() - Potrait POST successful";
           return true;
        }
        else
        {
            qDebug() << "api::postCapturedPortrait() - Potrait POST failed";
            return false;
        }
    }
    else
    {
        qDebug() << "api::postCapturedPortrait() - DB Connection failed";
    }

    return false;
}

void api::getCapturedFingerprint(QByteArray &image)
{

}

void api::getCapturedPortrait(QByteArray &image)
{

}
