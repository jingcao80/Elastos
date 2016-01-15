#ifndef __ELASTOS_DROID_SERVER_CCOMMONTIMEMANAGEMENTSERVICE_H__
#define __ELASTOS_DROID_SERVER_CCOMMONTIMEMANAGEMENTSERVICE_H__

#include "_Elastos_Droid_Server_CCommonTimeManagementService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
//#include "elastos/droid/os/CommonTimeConfig.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Net::INetworkManagementEventObserver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
//using Elastos::Droid::Os::CommonTimeConfig;

namespace Elastos {
namespace Droid {
namespace Server {
/**
 * @hide
 * <p>CCommonTimeManagementService manages the configuration of the native Common Time service,
 * reconfiguring the native service as appropriate in response to changes in network configuration.
 */
CarClass(CCommonTimeManagementService)
{
private:
    class ReconnectRunnable : public Runnable
    {
    public:
        ReconnectRunnable(
            /* [in] */ CCommonTimeManagementService* owner)
            : mOwner(owner)
        {}

        CARAPI Run();

    private:
        CCommonTimeManagementService* mOwner;
    };

    class NoInterfaceRunnable : public Runnable
    {
    public:
        NoInterfaceRunnable(
            /* [in] */ CCommonTimeManagementService* owner)
            : mOwner(owner)
        {}

        CARAPI Run();

    private:
        CCommonTimeManagementService* mOwner;
    };


    /*
     * Callback handler implementations.
     */
    class IfaceObserver
        : public INetworkManagementEventObserver
        , public IBinder
        , public ElRefBase
    {
    public:
        IfaceObserver(
            /* [in] */ CCommonTimeManagementService* owner);

        CAR_INTERFACE_DECL();

        CARAPI InterfaceStatusChanged(
            /* [in] */ const String& iface,
            /* [in] */ Boolean up);

        CARAPI InterfaceLinkStateChanged(
            /* [in] */ const String& iface,
            /* [in] */ Boolean up);

        CARAPI InterfaceAdded(
            /* [in] */ const String& iface);

        CARAPI InterfaceRemoved(
            /* [in] */ const String& iface);

        CARAPI LimitReached(
            /* [in] */ const String& limitName,
            /* [in] */ const String& iface);

        CARAPI InterfaceClassDataActivityChanged(
            /* [in] */ const String& label,
            /* [in] */ Boolean active);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI ToString(
            /* [out] */ String* result);
    private:
        AutoPtr<CCommonTimeManagementService> mOwner;
    };

    class ConnectivityMangerObserver
        : public BroadcastReceiver
    {
    public:
        ConnectivityMangerObserver(
            /* [in] */ CCommonTimeManagementService* owner);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CCommonTimeManagementService::ConnectivityMangerObserver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<CCommonTimeManagementService> mOwner;
    };

/*
    private CommonTimeConfig.OnServerDiedListener mCTServerDiedListener =
        new CommonTimeConfig.OnServerDiedListener() {
            public void onServerDied() {
                scheduleTimeConfigReconnect();
            }
        };
*/
    class CTServerDiedListener
        : public ElRefBase
    {
    public:
        CTServerDiedListener(
            /* [in] */ CCommonTimeManagementService* owner);

        CARAPI OnServerDied();

    private:
        AutoPtr<CCommonTimeManagementService> mOwner;
    };

    /*
     * Inner helper classes
     */
    class InterfaceScoreRule
        : public ElRefBase
    {
        friend class CCommonTimeManagementService;
    public:
        InterfaceScoreRule(
            /* [in] */ const String& prefix,
            /* [in] */ Byte score);
    public:
        const String mPrefix;
        const Byte mScore;
    };

public:
    CCommonTimeManagementService();

    /*
     * Public interface (constructor, systemReady and dump)
     */
    CARAPI constructor(
        /* [in] */ IContext *context);

    static CARAPI_(Boolean) Initstatic();

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI SystemReady();

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor *fd,
        /* [in] */ IPrintWriter *pw,
        /* [in] */ ArrayOf<String> *args);

private:
    /*
     * Internal implementation
     */
    CARAPI_(void) CleanupTimeConfig();

    CARAPI_(void) ConnectToTimeConfig();

    CARAPI_(void) ScheduleTimeConfigReconnect();

    CARAPI_(void) HandleNoInterfaceTimeout();

    CARAPI_(void) ReevaluateServiceState();

private:
    /*
     * Constants and globals.
     */
    const static String TAG;
    const static Int32 NATIVE_SERVICE_RECONNECT_TIMEOUT = 5000;
    const static String AUTO_DISABLE_PROP;
    const static String ALLOW_WIFI_PROP;
    const static String SERVER_PRIO_PROP;
    const static String NO_INTERFACE_TIMEOUT_PROP;
    static Boolean AUTO_DISABLE;
    static Boolean ALLOW_WIFI;
    static Byte BASE_SERVER_PRIO;
    static Int32 NO_INTERFACE_TIMEOUT;
    static AutoPtr<ArrayOf<InterfaceScoreRule* > > IFACE_SCORE_RULES;

    const static Boolean staticParagraph;

    /*
     * Internal state
     */
    AutoPtr<IContext> mContext;
    AutoPtr<IINetworkManagementService> mNetMgr;
//TODO
//    AutoPtr<CommonTimeConfig> mCTConfig;
    String mCurIface;
    AutoPtr<IHandler> mReconnectHandler;
    AutoPtr<IHandler> mNoInterfaceHandler;
    Object mLock;
    Boolean mDetectedAtStartup;
    Byte mEffectivePrio;

    AutoPtr<IfaceObserver> mIfaceObserver;

    AutoPtr<ConnectivityMangerObserver> mConnectivityMangerObserver;

    AutoPtr<CTServerDiedListener> mCTServerDiedListener;

    AutoPtr<IRunnable> mReconnectRunnable;
    AutoPtr<IRunnable> mNoInterfaceRunnable;
};

}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CCOMMONTIMEMANAGEMENTSERVICE_H__
