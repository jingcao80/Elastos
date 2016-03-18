
#ifndef __ELASTOS_DROID_SERVER_HDMI_HDMILOGGER_H__
#define __ELASTOS_DROID_SERVER_HDMI_HDMILOGGER_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

// using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Utility::IPair;
using Elastos::Utility::IHashMap;
using Elastos::Core::IThreadLocal;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * A logger that prevents spammy log. For the same log message, it logs once every 20seconds.
 * This class is not thread-safe.
 * <p>
 * For convenience, use single character prefix for all messages.
 * Here are common acronyms
 * <ul>
 *   <li>[T]: Timout
 *   <li>[R]: Received message
 *   <li>[S]: Sent message
 *   <li>[P]: Device polling result
 * </ul>
 */
class HdmiLogger
    : public Object
{
public:
    static CARAPI Warning(
        /* [in] */ const char* fmt, ...);

    static CARAPI Error(
        /* [in] */ const char* fmt, ...);

    static CARAPI Debug(
        /* [in] */ const char* fmt, ...);

private:
    HdmiLogger();

    CARAPI WarningInternal(
        /* [in] */ const String& logMessage);

    CARAPI ErrorInternal(
        /* [in] */ const String& logMessage);

    CARAPI DebugInternal(
        /* [in] */ const String& logMessage);

    static CARAPI_(String) ToLogString(
        /* [in] */ const char* fmt, ...);

    static CARAPI_(AutoPtr<HdmiLogger>) GetLogger();

    static CARAPI_(String) UpdateLog(
        /* [in] */ IHashMap* cache,
        /* [in] */ const String& logMessage);

    static CARAPI_(String) BuildMessage(
        /* [in] */ const String& message,
        /* [in] */ IPair* timing);

    static CARAPI IncreaseLogCount(
        /* [in] */ IHashMap* cache,
        /* [in] */ const String& message);

    static CARAPI_(Boolean) ShouldLogNow(
        /* [in] */ IPair* timing,
        /* [in] */ Int64 curTime);

    static CARAPI_(Boolean) InitDEBUG();

public:
    // private static final ThreadLocal<HdmiLogger> sLogger = new ThreadLocal<>();
    static pthread_key_t sKey;
    static pthread_once_t sKeyOnce;

private:
    static const String TAG;

    // Logging duration for same error message.
    static const Int64 ERROR_LOG_DURATTION_MILLIS;

    // 20s
    static const Boolean DEBUG;

    // Key (String): log message.
    // Value (Pair(Long, Integer)): a pair of last log time millis and the number of logMessage.
    // Cache for warning.
    AutoPtr<IHashMap> mWarningTimingCache;
    // Cache for error.
    AutoPtr<IHashMap> mErrorTimingCache;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_HDMILOGGER_H__
