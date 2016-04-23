#ifndef __ELASTOS_DROID_SERVER_ACTIVEADMIN_H__
#define __ELASTOS_DROID_SERVER_ACTIVEADMIN_H__

#include <elastos/droid/os/UEventObserver.h>
#include <elastos/droid/os/Handler.h>
#include "elastos/droid/server/SystemService.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::Os::IUEvent;
using Elastos::Droid::Os::IUEventObserver;
using Elastos::Droid::Os::IUserHandle;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IFileReader;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * DockObserver monitors for a docking station.
 */
class DockObserver
    : public SystemService
{
private:
    class BinderService
        : public Object
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

        CARAPI ToString(
            /* [out] */ String* str);
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("DockObserver::MyHandler")

        MyHandler(
            /* [in] */ DockObserver* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        DockObserver* mHost;
    };

    class MyUEventObserver
        : public UEventObserver
    {
    public:
        MyUEventObserver(
            /* [in] */ DockObserver* host);

        //@Override
        CARAPI OnUEvent(
            /* [in] */ IUEvent* event);
    private:
        DockObserver* mHost;
    };

public:
    DockObserver();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

private:
    CARAPI Init();

    CARAPI SetActualDockStateLocked(
        /* [in] */ Int32 newState);

    CARAPI SetDockStateLocked(
        /* [in] */ Int32 newState);

    CARAPI UpdateLocked();

    CARAPI HandleDockStateChange();

private:
    static const String TAG;

    static const String DOCK_UEVENT_MATCH;
    static const String DOCK_STATE_PATH;

    static const int MSG_DOCK_STATE_CHANGED;

    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    Object mLock;

    Boolean mSystemReady;

    Int32 mActualDockState;

    Int32 mReportedDockState;
    Int32 mPreviousDockState;

    Boolean mUpdatesStopped;

    AutoPtr<IHandler> mHandler;
    AutoPtr<IUEventObserver> mObserver;
};


}// Server
}// Droid
}// Elastos

#endif //__ELASTOS_DROID_SERVER_ACTIVEADMIN_H__