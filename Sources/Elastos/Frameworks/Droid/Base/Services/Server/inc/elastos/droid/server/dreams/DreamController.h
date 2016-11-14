
#ifndef __ELASTOS_DROID_SERVER_DREAMS_DREAMCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_DREAMS_DREAMCONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include <Elastos.Droid.Content.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Service::Dreams::IIDreamService;

using Elastos::IO::IPrintWriter;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Dreams {

class DreamController
    : public Object
{
class DreamRecord;

public:
    class Listener
        : public Object
    {
    public:
        virtual CARAPI OnDreamStopped(
            /* [in] */ IBinder* token) = 0;
    };

private:

    class ProxyDiedRunnable
        : public Runnable
    {
    public:
        ProxyDiedRunnable(
            /* [in] */ DreamRecord* host)
            : mDRHost(host)
        {}

        CARAPI Run();

    private:
        DreamRecord* mDRHost;
    };

    class ConnectedRunnable
        : public Runnable
    {
    public:
        ConnectedRunnable(
            /* [in] */ DreamRecord* host,
            /* [in] */ IBinder* service)
            : mDRHost(host)
            , mService(service)
        {}

        CARAPI Run();

    private:
        DreamRecord* mDRHost;
        AutoPtr<IBinder> mService;
    };

    class DisconnectedRunnable
        : public Runnable
    {
    public:
        DisconnectedRunnable(
            /* [in] */ DreamRecord* host)
            : mDRHost(host)
        {};

        CARAPI Run();

    private:
        DreamRecord* mDRHost;
    };

    class StopDreamRunnable
        : public Runnable
    {
    public:
        StopDreamRunnable(
            /* [in] */ DreamController* host,
            /* [in] */ DreamRecord* oldDream)
            : mDCHost(host)
            , mOldDream(oldDream)
        {}

        CARAPI Run();

    private:
        DreamController* mDCHost;
        DreamRecord* mOldDream;
    };

    class StopUnconnectedDreamRunnable
        : public Runnable
    {
    public:
        StopUnconnectedDreamRunnable(
            /* [in] */ DreamController* host)
            : mDCHost(host)
        {}

        CARAPI Run();

    private:
        DreamController* mDCHost;
    };

    class StopStubbornDreamRunnable
        : public Runnable
    {
    public:
        StopStubbornDreamRunnable(
            /* [in] */ DreamController* host)
            : mDCHost(host)
        {}

        CARAPI Run();

    private:
        DreamController* mDCHost;
    };

    class DreamRecord
        : public Object
        , public IServiceConnection
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        DreamRecord(
            /* [in] */ IBinder* token,
            /* [in] */ IComponentName* name,
            /* [in] */ Boolean isTest,
            /* [in] */ Boolean canDoze,
            /* [in] */ Int32 userId,
            /* [in] */ DreamController* host);

        CARAPI ProxyDied();

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    public:
        AutoPtr<IBinder> mToken;
        AutoPtr<IComponentName> mName;
        const Boolean mIsTest;
        const Boolean mCanDoze;
        const Int32 mUserId;

        Boolean mBound;
        Boolean mConnected;
        AutoPtr<IIDreamService> mService;
        Boolean mSentStartBroadcast;
        Boolean mWakingGently;

        DreamController* mHost;
    };

public:
    DreamController(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ Listener* listener);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw);

    CARAPI StartDream(
        /* [in] */ IBinder* token,
        /* [in] */ IComponentName* name,
        /* [in] */ Boolean isTest,
        /* [in] */ Boolean canDoze,
        /* [in] */ Int32 userId);

    CARAPI StopDream(
        /* [in] */ Boolean immediate);

private:
    CARAPI_(void) Attach(
        /* [in] */ IIDreamService* service);

private:
    static const String TAG;

    // How long we wait for a newly bound dream to create the service connection
    static const Int32 DREAM_CONNECTION_TIMEOUT = 5 * 1000;

    // Time to allow the dream to perform an exit transition when waking up.
    static const Int32 DREAM_FINISH_TIMEOUT = 5 * 1000;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    AutoPtr<Listener> mListener;
    AutoPtr<IIWindowManager> mIWindowManager;

    AutoPtr<IIntent> mDreamingStartedIntent; // = new Intent(Intent.ACTION_DREAMING_STARTED).addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY);
    AutoPtr<IIntent> mDreamingStoppedIntent; // = new Intent(Intent.ACTION_DREAMING_STOPPED).addFlags(Intent.FLAG_RECEIVER_REGISTERED_ONLY);

    AutoPtr<IIntent> mCloseNotificationShadeIntent; // = new Intent(Intent.ACTION_CLOSE_SYSTEM_DIALOGS);

    AutoPtr<DreamRecord> mCurrentDream;

    AutoPtr<IRunnable> mStopUnconnectedDreamRunnable;
    AutoPtr<IRunnable> mStopStubbornDreamRunnable;
};

} // namespace Dreams
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DREAMS_DREAMCONTROLLER_H__
