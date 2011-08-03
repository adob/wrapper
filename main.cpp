#include <stdio.h>

#include <QSettings>
#include <QFileInfo>
#include <QCoreApplication>
#include <QTimer>


#include "print.h"
#include "log.h"
#include "Wrapper.h"


FILE *logfile;
int ourpid = QCoreApplication::applicationPid();

int main(int argc, char *argv[])
{    
    if (argc != 2) {
        our_log( (sprint "FATAL: Incorrect usage. Use: %s config.ini\n" % argv[0]) );
        return 1;
    }
    
    QSettings settings(argv[1], QSettings::IniFormat);
    switch (settings.status())
    {
      case QSettings::AccessError:
        our_log("FATAL: Cannot access configuration file\n");
        return 1;
        
      case QSettings::FormatError:
        our_log("FATAL: Format error while reading configuation file\n");
        return 1;
        
      default:
          ;
    }
    
    if (! settings.contains("command") ) {
        our_log("FATAL: Configuration file does not contain command property");
        return 1;
    }

    
    Wrapper wrapper;
    wrapper.command = settings.value("command").toString();
    wrapper.name = settings.value("name", QFileInfo(wrapper.command).baseName()).toString();
    wrapper.ourpid = ourpid;
    wrapper.timeout = settings.value("timeout", -1).toInt();
    if (wrapper.timeout != -1)
        wrapper.timeout *= 1000;
    
    QTimer::singleShot(0, &wrapper, SLOT(start()));

    QCoreApplication app(argc, argv);
    QCoreApplication::exec();
}
