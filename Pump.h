#pragma once

#include <QThread>
#include <QProcess>

class Pump : public QThread
{
    Q_OBJECT
    
    QProcess &proc;
    
public:
    Pump(QProcess &);
    void run();
    
signals:
    void error(QString);
};