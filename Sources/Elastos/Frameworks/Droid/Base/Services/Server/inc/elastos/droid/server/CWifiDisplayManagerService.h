#ifndef __ELASTOS_DROID_SERVER_CWIFIDISPLAYMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CWIFIDISPLAYMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CWifiDisplayManagerService.h"
#include "power/CPowerManagerService.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::WifiDisplay::IWifiDisplayThread;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Server::Power::CPowerManagerService;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Keep track of all those .apks everywhere.
 *
 * This is very central to the platform's security; please run the unit
 * tests whenever making modifications here:
 *
mmm frameworks/base/tests/AndroidTests
adb install -r -f out/target/product/passion/data/app/AndroidTests.apk
adb shell am instrument -w -e class com.android.unit_tests.PackageManagerTests com.android.unit_tests/android.test.InstrumentationTestRunner
 *
 */
CarClass(CWifiDisplayManagerService)
{
public:
    class ThreadDeathRecipient
        : public ElRefBase
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        ThreadDeathRecipient(
            /* [in] */ CWifiDisplayManagerService* host,
            /* [in] */ IWifiDisplayThread* client);

        CARAPI ProxyDied();

    private:
        CWifiDisplayManagerService*     mHost;
        AutoPtr<IBinder>                mClientBinder;
        AutoPtr<IWifiDisplayThread>    mClient;
    };

    class WifiDisplayClient : public ElRefBase {
    public:
        WifiDisplayClient(
            /* [in] */ CWifiDisplayManagerService* host,
            /* [in] */ IWifiDisplayThread* client);

        void SetWifiDisplayClientStatus(
            /* [in] */ Int32 status);

        Int32 GetWifiDisplayClientStatus();

        AutoPtr<IWifiDisplayThread> GetWifiDisplayClient();

    public:
        static const Int32 WIFIDISPLAY_CLIENT_NONE;
        static const Int32 WIFIDISPLAY_CLIENT_SENDING;
        static const Int32 WIFIDISPLAY_CLIENT_RECEIVEING;

    private:
        AutoPtr<IBinder>                mClientBinder;
        AutoPtr<IWifiDisplayThread>    mClient;
        Int32                           mStatus;
        AutoPtr<ThreadDeathRecipient>   mDeathRecipient;
        CWifiDisplayManagerService*     mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIPowerManager* powerManagerService);

    CARAPI AddWifiDisplayClient(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    CARAPI RemoveWifiDisplayClient(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    CARAPI StartWifiDisplaySend(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    CARAPI EndWifiDisplaySend(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    CARAPI StartWifiDisplayReceive(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    CARAPI EndWifiDisplayReceive(
        /* [in] */ IWifiDisplayThread* client,
        /* [out] */ Int32* result);

    CARAPI InjectMotionEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI ExitWifiDisplayReceive(
        /* [out] */ Boolean* result);

    CARAPI ExitWifiDisplaySend(
        /* [out] */ Boolean* result);

    CARAPI GetWifiDisplayStatus(
        /* [out] */ Int32* status);

private:
    CARAPI_(AutoPtr<WifiDisplayClient>) ContainsWifiDisplayThread(
        /* [in] */ IWifiDisplayThread* client);
private:
    static const String TAG;// = "WifiDisplayManagerService";

    static const Boolean LOCAL_LOGV;// = true;
    static const Boolean DEBUG_CLIENT;// = true;

    AutoPtr<IContext>                   mContext;
    AutoPtr<IIPowerManager>             mPM;
    AutoPtr<IIWindowManager>            mWindowManager;
    List<AutoPtr<WifiDisplayClient> >   mClients;
};

} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CWIFIDISPLAYMANAGERSERVICE_H__
