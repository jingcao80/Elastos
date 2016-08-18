
#ifndef __ELASTOS_DROID_SERVER_MEDIA_CKEYEVENTWAKELOCKRECEIVER_H__
#define __ELASTOS_DROID_SERVER_MEDIA_CKEYEVENTWAKELOCKRECEIVER_H__

#include "_Elastos_Droid_Server_Media_CKeyEventWakeLockReceiver.h"
#include "elastos/droid/os/ResultReceiver.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentOnFinished;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::ResultReceiver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

class MediaSessionService;
class CSessionManagerImpl;

CarClass(CKeyEventWakeLockReceiver)
    , public ResultReceiver
    , public IPendingIntentOnFinished
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IWeakReference* host);

        CARAPI Run();

    private:
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CKeyEventWakeLockReceiver()
        : mRefCount(0)
        , mLastTimeoutId(0)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IHandler* handler,
        /* [in] */ Handle64 host);

    CARAPI_(void) OnTimeout();

    CARAPI_(void) AquireWakeLockLocked();

    CARAPI Run();

    CARAPI_(void) OnReceiveResult(
        /* [in] */ Int32 resultCode,
        /* [in] */ IBundle* resultData);

    CARAPI OnSendFinished(
        /* [in] */ IPendingIntent* pendingIntent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* resultExtras);

private:
    CARAPI_(void) ReleaseWakeLockLocked();

private:
    AutoPtr<IHandler> mHandler;
    Int32 mRefCount;
    Int32 mLastTimeoutId;
    AutoPtr<IRunnable> mRunnable;

    MediaSessionService* mHost;

    friend class CSessionManagerImpl;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_CKEYEVENTWAKELOCKRECEIVER_H__
