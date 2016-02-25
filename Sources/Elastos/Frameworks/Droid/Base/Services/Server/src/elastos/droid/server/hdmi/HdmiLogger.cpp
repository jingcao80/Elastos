
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
    /* [in] */ const String& logMessage,
    /* [in] */ ArrayOf<IObject>* objs)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        GetLogger()->WarningInternal(ToLogString(logMessage, objs));
#endif
}

ECode HdmiLogger::WarningInternal(
    /* [in] */ const String& logMessage)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String log = UpdateLog(mWarningTimingCache, logMessage);
        if (!log->IsEmpty()) {
            Slogger::W(TAG, log);
        }
#endif
}

ECode HdmiLogger::Error(
    /* [in] */ const String& logMessage,
    /* [in] */ ArrayOf<IObject>* objs)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        GetLogger()->ErrorInternal(ToLogString(logMessage, objs));
#endif
}

ECode HdmiLogger::ErrorInternal(
    /* [in] */ const String& logMessage)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        String log = UpdateLog(mErrorTimingCache, logMessage);
        if (!log->IsEmpty()) {
            Slogger::E(TAG, log);
        }
#endif
}

ECode HdmiLogger::Debug(
    /* [in] */ const String& logMessage,
    /* [in] */ ArrayOf<IObject>* objs)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        GetLogger()->DebugInternal(ToLogString(logMessage, objs));
#endif
}

ECode HdmiLogger::DebugInternal(
    /* [in] */ const String& logMessage)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (!DEBUG) {
            return;
        }
        Slogger::D(TAG, logMessage);
#endif
}

ECode HdmiLogger::ToLogString(
    /* [in] */ const String& logMessage,
    /* [in] */ ArrayOf<IObject*>* objs,
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (objs.length > 0) {
            return String->Format(logMessage, objs);
        } else {
            return logMessage;
        }
#endif
}

ECode HdmiLogger::GetLogger(
    /* [out] */ HdmiLogger** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        HdmiLogger logger = sLogger->Get();
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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Int64 curTime = SystemClock->UptimeMillis();
        Pair<Long, Integer> timing = cache->Get(logMessage);
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
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return new StringBuilder()
                .Append("[").Append(timing == NULL ? 1 : timing.second).Append("]:")
                .Append(message).ToString();
#endif
}

ECode HdmiLogger::IncreaseLogCount(
    /* [in] */ IHashMap* cache,
    /* [in] */ const String& message)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Pair<Long, Integer> timing = cache->Get(message);
        if (timing != NULL) {
            cache->Put(message, new Pair<>(timing.first, timing.second + 1));
        }
#endif
}

ECode HdmiLogger::ShouldLogNow(
    /* [in] */ IPair* timing,
    /* [in] */ Int64 curTime,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return timing == NULL || curTime - timing.first > ERROR_LOG_DURATTION_MILLIS;
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
