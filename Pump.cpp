
#include <QFile>

#include "Pump.h"
#include "print.h"


Pump::Pump(QProcess & p) : proc(p)
{
    
}

void
Pump::run()
{
    char buf[1024*8];
    int count;
    
    QFile file;
    file.open(stdin, QIODevice::ReadOnly | QIODevice::Unbuffered);
    
    while ( (count = file.read(buf, sizeof buf) ) > 0)
    {
        int written = proc.write(buf, count);
        
        if (written != count) 
        {
            emit error("Error writing to process");
            return;
        }
    }
    //fprintf(stderr, "done writing");
    proc.closeWriteChannel();
}