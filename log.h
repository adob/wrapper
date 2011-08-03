#pragma once

#include <QString>
#include <QByteArray>


void our_log(QString const& str);
void log_message(QString const& name, QString const& msg);


enum MessageType
{
    Success = 0, Error, Warning, Information
};

void win_log_message(const QString & name, const QString &message, MessageType type = Success,
            int id = 0, uint category = 0, const QByteArray &data = QByteArray());
