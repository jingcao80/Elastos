
#include "elastos/droid/server/telecom/ThreadUtil.h"
#include "elastos/droid/server/telecom/Log.h"
#include <elastos/droid/os/Looper.h>

using Elastos::Droid::Os::Looper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String ThreadUtil::TAG("ThreadUtil");

ThreadUtil::ThreadUtil()
{
}

ECode ThreadUtil::IsOnMainThread(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Looper::GetMainLooper() == Looper::GetMyLooper();
    return NOERROR;
}

ECode ThreadUtil::CheckOnMainThread()
{
    Boolean isOnMainThread;
    IsOnMainThread(&isOnMainThread);
    if (!isOnMainThread) {
        Log::Wtf(TAG, "Must be on the main thread!");
    }
    return NOERROR;
}

ECode ThreadUtil::CheckNotOnMainThread()
{
    Boolean isOnMainThread;
    IsOnMainThread(&isOnMainThread);
    if (isOnMainThread) {
        Log::Wtf(TAG, "Must not be on the main thread!");
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
