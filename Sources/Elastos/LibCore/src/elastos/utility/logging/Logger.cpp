
#ifdef _MSVC
#include "androidlog.h"
#else
#include <android/log.h>
#endif
#include <elastos/utility/logging/Logger.h>
#include <errno.h>

namespace Elastos {
namespace Utility {
namespace Logging {

#ifdef __cplusplus
extern "C" {
#endif
/*
 * Send a simple string to the log.
 */
int __android_log_buf_write(int bufID, int prio, const char *tag, const char *text);
int __android_log_buf_print(int bufID, int prio, const char *tag, const char *fmt, ...);
#ifdef __cplusplus
}
#endif


#ifdef _openkode
    #include <stdio.h>
#else // _openkode
    ECode ErrToECode(int err)
    {
        ECode ec = NOERROR;
        switch (err) {
        case 0:
            break;
        case EINTR:
            ec = E_INVALID_OPERATION;
            break;
        case EBADF:
            ec = E_FILE_NOT_FOUND;
            break;
        default:
            ec = E_FAIL;
            break;
        }
        return ec;
    }
#endif // _openkode

ECode __PrintlnImpl(Int32 bufID, Int32 priority, const char * tag, const char *msgBuf)
{
    if (bufID < Logger::LOG_ID_MAIN || bufID > Logger::LOG_ID_SYSTEM || tag == NULL
            || msgBuf == NULL || priority < Logger::VERBOSE || priority > Logger::ASSERT) {
        return E_INVALID_ARGUMENT;
    }

#ifdef _openkode
    char ch = '0';

    switch (priority) {
    case Logger::VERBOSE:
        ch = 'V';
        break;
    case Logger::___DEBUG:
        ch = 'D';
        break;
    case Logger::INFO:
        ch = 'I';
        break;
    case Logger::WARN:
        ch = 'W';
        break;
    case Logger::ERROR:
        ch = 'E';
        break;
    case Logger::ASSERT:
        ch = 'A';
        break;
    }

    printf("%c/%s: %s", ch, (const char *)tag, msgBuf);
    return NOERROR;
#else // _openkode
    return ErrToECode(
        __android_log_buf_write(bufID, priority, (const char *)tag, msgBuf));
#endif // _openkode
}

//=============================================================================
// Logger
//=============================================================================
const Int32 Logger::VERBOSE;
const Int32 Logger::___DEBUG;
const Int32 Logger::INFO;
const Int32 Logger::WARN;
const Int32 Logger::ERROR;
const Int32 Logger::ASSERT;
const Int32 Logger::LOG_ID_MAIN;
const Int32 Logger::LOG_ID_RADIO;
const Int32 Logger::LOG_ID_EVENTS;
const Int32 Logger::LOG_ID_SYSTEM;

#define MSG_BUF_SIZE    1024

ECode Logger::D(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(LOG_ID_MAIN, ANDROID_LOG_DEBUG, tag, msgBuf);
}

ECode Logger::E(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(LOG_ID_MAIN, ANDROID_LOG_ERROR, tag, msgBuf);
}

ECode Logger::I(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(LOG_ID_MAIN, ANDROID_LOG_INFO, tag, msgBuf);
}

ECode Logger::V(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(LOG_ID_MAIN, ANDROID_LOG_VERBOSE, tag, msgBuf);
}

ECode Logger::W(const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(LOG_ID_MAIN, ANDROID_LOG_WARN, tag, msgBuf);
}

Boolean Logger::IsLoggable(const char * tag, Int32 level)
{
    return TRUE;
}

ECode Logger::Println(Int32 priority, const char * tag, const char *fmt, ...)
{
    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(LOG_ID_MAIN, priority, tag, msgBuf);
}

ECode Logger::PrintlnImpl(Int32 bufID, Int32 priority, const char * tag, const char *fmt, ...)
{
    if (bufID < LOG_ID_MAIN || bufID > LOG_ID_SYSTEM || tag == NULL
            || fmt == NULL || priority < VERBOSE || priority > ASSERT) {
        return E_INVALID_ARGUMENT;
    }

    char msgBuf[MSG_BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msgBuf, MSG_BUF_SIZE, fmt, args);
    va_end(args);

    return __PrintlnImpl(bufID, priority, tag, msgBuf);
}

} // Logging
} // Utility
} // Elastos

