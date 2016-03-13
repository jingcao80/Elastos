
#include "elastos/droid/server/hdmi/HdmiLogger.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Utility::ISlog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

const String HdmiLogger::TAG("HDMI");
const Int64 HdmiLogger::ERROR_LOG_DURATTION_MILLIS = 20 * 1000;
const Boolean HdmiLogger::DEBUG = FALSE;
const AutoPtr<IThreadLocal> HdmiLogger::sLogger = InitLogger();

HdmiLogger::HdmiLogger()
{
#if 0 // TODO: Translate codes below
    mWarningTimingCache = new HashMap<>();
    mErrorTimingCache = new HashMap<>();
#endif
}

ECode HdmiLogger::Warning(
    /* [in] */ const char* fmt, ...)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        va_list ap;
        va_start(ap, fmt);
        GetLogger()->WarningInternal(ToLogString(fmt, ap));
        va_end(ap);
#endif
}

ECode HdmiLogger::WarningInternal(
    /* [in] */ const String& logMessage)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String log = UpdateLog(mWarningTimingCache, logMessage);
        Boolean isEmpty;
        log->IsEmpty(&isEmpty);
        if (!isEmpty) {
            Slogger::W(TAG, log);
        }
#endif
}

ECode HdmiLogger::Error(
    /* [in] */ const char* fmt, ...)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        va_list ap;
        va_start(ap, fmt);
        GetLogger()->ErrorInternal(ToLogString(fmt, ap));
        va_end(ap);
#endif
}

ECode HdmiLogger::ErrorInternal(
    /* [in] */ const String& logMessage)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String log = UpdateLog(mErrorTimingCache, logMessage);
        Boolean isEmpty;
        log->IsEmpty(&isEmpty);
        if (!isEmpty) {
            Slogger::E(TAG, log);
        }
#endif
}

ECode HdmiLogger::Debug(
    /* [in] */ const char* fmt, ...)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
    va_list ap;
    va_start(ap, fmt);
    GetLogger()->DebugInternal(ToLogString(fmt, ap));
    va_end(ap);
#endif
}

ECode HdmiLogger::DebugInternal(
    /* [in] */ const String& logMessage)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!DEBUG) {
            return NOERROR;
        }
        Slogger::D(TAG, logMessage);
#endif
}

String HdmiLogger::ToLogString(
    /* [in] */ const char* fmt, ...)
{
    String rev;
#if 0 // TODO: Translate codes below
    va_list ap;
    va_start(ap, fmt);
    rev.AppendFormat(fmt, ap);
    va_end(ap);
#endif
    return rev;
}

ECode HdmiLogger::GetLogger(
    /* [out] */ HdmiLogger** result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IInterface> obj;
        sLogger->Get(, (IInterface**)&obj);
        HdmiLogger logger = I::Probe(obj);
        if (logger == NULL) {
            logger = new HdmiLogger();
            sLogger->Set(logger);
        }
        return logger;
#endif
}

ECode HdmiLogger::UpdateLog(
    /* [in] */ IHashMap* cache,
    /* [in] */ const String& logMessage,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int64 curTime = SystemClock->UptimeMillis();
        AutoPtr<IInterface> obj;
        cache->Get(logMessage, (IInterface**)&obj);
        Pair<Long, Integer> timing = I::Probe(obj);
        if (ShouldLogNow(timing, curTime)) {
            String log = BuildMessage(logMessage, timing);
            cache->Put(logMessage, new Pair<>(curTime, 1));
            return log;
        } else {
            IncreaseLogCount(cache, logMessage);
        }
        return "";
#endif
}

ECode HdmiLogger::BuildMessage(
    /* [in] */ const String& message,
    /* [in] */ IPair* timing,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return new StringBuilder()
                .Append("[").Append(timing == NULL ? 1 : timing->mSecond).Append("]:")
                .Append(message).ToString();
#endif
}

ECode HdmiLogger::IncreaseLogCount(
    /* [in] */ IHashMap* cache,
    /* [in] */ const String& message)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        AutoPtr<IInterface> obj;
        cache->Get(message, (IInterface**)&obj);
        Pair<Long, Integer> timing = I::Probe(obj);
        if (timing != NULL) {
            cache->Put(message, new Pair<>(timing->mFirst, timing->mSecond + 1));
        }
#endif
}

ECode HdmiLogger::ShouldLogNow(
    /* [in] */ IPair* timing,
    /* [in] */ Int64 curTime,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return timing == NULL || curTime - timing->mFirst > ERROR_LOG_DURATTION_MILLIS;
#endif
}

AutoPtr<IThreadLocal> HdmiLogger::InitLogger()
{
    AutoPtr<IThreadLocal> rev;
#if 0 // TODO: Translate codes below
    = new ThreadLocal<>();
#endif
    return rev;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
