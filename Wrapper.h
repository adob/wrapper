#pragma once
#include <stdio.h>
#include <QObject>
#include <QProcess>
#include <QFile>
#include <QTimer>
#include "Pump.h"

class Wrapper : public QObject
{
    Q_OBJECT
    
    enum Status {
        STARTING,
        RUNNING,
        KILLING
    };
    
public:
    QString command;
    QString name;
    QString hostname;
    int ourpid;
    FILE *logfile;
    int timeout;

    
    Wrapper();
    
public slots:
    void start();
    
    
    
private slots:
   // void handle_stdin();
    void handle_stderr();
    void handle_stdout();
    void handle_finished(int, QProcess::ExitStatus);
    void handle_error();
    void handle_started();
    void handle_timeout();
    void handle_pump_error(QString const&);
    
private:
    QProcess proc;
    QTimer timer;
    
    char buf[1024*8];
    int pid;
    
    void log(const char*);
    void quit();
    Status status;
    Pump pump;
    
};