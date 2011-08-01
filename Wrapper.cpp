#include <QCoreApplication>

#include "Wrapper.h"

#include "print.h"
#include "log.h"

Wrapper::Wrapper() : pump(proc)
{
    connect(&proc, SIGNAL(error(QProcess::ProcessError)), 
            this, SLOT(handle_error()));
    
    connect(&proc, SIGNAL(readyReadStandardError()), 
            this, SLOT(handle_stderr()));
    
    connect(&proc, SIGNAL(readyReadStandardOutput()), 
            this, SLOT(handle_stdout()));
    
    connect(&proc, SIGNAL(finished(int, QProcess::ExitStatus)), 
            this, SLOT(handle_finished(int, QProcess::ExitStatus)));
    
    connect(&proc, SIGNAL(started()),
            this, SLOT(handle_started()));
    
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(handle_timeout()));
    
    connect(&pump, SIGNAL(error(QString)),
            this, SLOT(handle_pump_error(QString)));

}

void
Wrapper::start()
{
    status = STARTING;
    
    proc.start(command);
    
    our_log( ( sprint "INFO: Starting %s[%d]\n" % name, pid ) );
    
    timer.start(30*1000);
    
}

void 
Wrapper::log(const char* msg)
{
    print "log:", msg;
}

void     
Wrapper::handle_finished(int exit_code, QProcess::ExitStatus status)
{
//     proc.setReadChannel(QProcess::StandardError);
//     proc.waitForReadyRead();
//     proc.readLine(buf, sizeof buf);
//     log(buf);
//     printf("<<<%s>>>\n", buf);
    
    handle_stdout();
    
    if (status == QProcess::CrashExit)
        our_log( (sprint "CRITICAL: %s[%d] crashed\n" % name, pid) );
    else
    {
        if (exit_code == 0)
            our_log( (sprint "INFO: %s[%d] finished with code %d\n" % name, pid, exit_code) );
        else
            our_log( (sprint "WARN: %s[%d] finished with code %d\n" % name, pid, exit_code) );
    }
    
    pump.terminate();
    pump.wait(5000);
    QCoreApplication::quit();
}

void
Wrapper::handle_stderr()
{
    proc.setReadChannel(QProcess::StandardError);
    while (proc.canReadLine())
    {
        int count = proc.readLine(buf, sizeof buf);
        log(buf);
    }
}

void
Wrapper::handle_stdout()
{
    proc.setReadChannel(QProcess::StandardOutput);
    
    int count; 
    
    while ( (count = proc.read(buf, sizeof buf)) > 0 )
    {
        fwrite(buf, 1, count, stdout);
    }
    
    fflush(stdout);
    
}


void
Wrapper::handle_error()
{
    if (status == STARTING)
        our_log( (sprint "FATAL: Could not start %s[%d]: %s\n" % 
            name, pid, proc.errorString()) );
    else
        our_log( (sprint "CRITICAL: %s[%d] encountered an error: %s\n" %
            name, pid, proc.errorString()) );
        
    quit();
}

void
Wrapper::handle_started()
{
    timer.stop();
    status = RUNNING;
    
    pump.start();
    
    if (timeout > 0)
        timer.start(timeout);
}

void
Wrapper::handle_timeout()
{
    if (status == STARTING)
        our_log( ( sprint "FATAL: Could not start %s[%d]: Timed out\n" % name, pid ) );
    
    else if (status == RUNNING)
    {
        status = KILLING;
        our_log( ( sprint "CRITICAL: %s[%d] timed out and will be killed.\n" % name, pid) );
    }
    
    quit();
}

void
Wrapper::handle_pump_error(QString const& error)
{
    our_log( (sprint "CRITICAL: %s[%d] encountered an error: %s\n" % name, pid, error) );
    
    quit();
}

void
Wrapper::quit()
{
    if (proc.state() != QProcess::NotRunning)
        proc.kill();
    else
        QCoreApplication::quit();
}
