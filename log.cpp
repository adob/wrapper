#include <windows.h>
#include "log.h"
#include "print.h"


void our_log(QString const& str)
{
    log_message("Wrapper", str);
    eprint str;
}


void log_message(QString const& name, QString const& msg)
{
    MessageType type;

    if (msg.startsWith("INFO"))
            type = Information;
    else if (msg.startsWith("WARN:"))
        type = Warning;
    else
        type = Information;

    win_log_message(name, msg, type);

}


void win_log_message(const QString & name, const QString &message, MessageType type,
                           int id, uint category, const QByteArray &data)
{
    WORD wType;
    switch (type) {
    case Error: wType = EVENTLOG_ERROR_TYPE; break;
    case Warning: wType = EVENTLOG_WARNING_TYPE; break;
    case Information: wType = EVENTLOG_INFORMATION_TYPE; break;
    default: wType = EVENTLOG_SUCCESS; break;
    }
    HANDLE h = RegisterEventSourceW(0, (wchar_t*)name.utf16());
    if (h) {
        const wchar_t *msg = (wchar_t*)message.utf16();
        const char *bindata = data.size() ? data.constData() : 0;
        ReportEventW(h, wType, category, id, 0, 1, data.size(),(const wchar_t **)&msg,
                     const_cast<char *>(bindata));
        DeregisterEventSource(h);
    }
}
