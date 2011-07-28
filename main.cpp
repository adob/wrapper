#include <stdio.h>

#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>
#include <QHostInfo>
#include <QTimer>


#include "print.h"
#include "Wrapper.h"


FILE *logfile;
QString hostname = QHostInfo::localHostName();
int ourpid = QCoreApplication::applicationPid();

void our_log(QString const& str)
{    
    
    auto t = time(NULL);
    char *timestr = asctime(localtime(&t));
    
    for (char *c=timestr; *c != '\0'; c++) if (*c=='\n') *c = '\0';
    
    if (logfile != stderr)
        eprint "%s" % str;
    
    fprint (logfile) "%s %s wrapper[%d]: %s" % timestr, hostname, ourpid, str;
    
    fflush(logfile);
}

int main(int argc, char *argv[])
{    
    
    if (argc != 2) {
        eprint "FATAL: Incorrect usage. Use: %s config.ini\n" % argv[0];
        return 1;
    }
    
    QSettings settings(argv[1], QSettings::IniFormat);
    switch (settings.status())
    {
      case QSettings::AccessError:
        eprint "FATAL: Cannot access configuration file\n";
        return 1;
        
      case QSettings::FormatError:
        eprint "FATAL: Format error while reading configuation file\n";
        return 1;
        
      default:
          ;
    }
    
    if (! settings.contains("command") ) {
        eprint "FATAL: Configuration file does not contain command property";
        return 1;
    }
    
    QString logfilename = settings.value("logfile", "-").toString();
    if (logfilename == "-")
        logfile = stderr;
    else
        logfile = fopen(logfilename.toLocal8Bit(), "a");
    
    if (! logfile) {
        perror("FATAL: Could not open log file");
        return 1;
    }
    
    Wrapper wrapper;
    wrapper.command = settings.value("command").toString();
    wrapper.name = settings.value("name", QFileInfo(wrapper.command).baseName()).toString();
    wrapper.logfile = logfile;
    wrapper.hostname = hostname;
    wrapper.ourpid = ourpid;
    wrapper.timeout = settings.value("timeout", -1).toInt();
    if (wrapper.timeout != -1)
        wrapper.timeout *= 1000;
    
    QTimer::singleShot(0, &wrapper, SLOT(start()));

    QCoreApplication app(argc, argv);
    QCoreApplication::exec();
}