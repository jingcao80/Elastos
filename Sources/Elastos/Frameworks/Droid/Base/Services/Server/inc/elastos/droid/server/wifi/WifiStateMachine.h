
#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFISTATEMACHINE_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFISTATEMACHINE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/DhcpInfoInternal.h"
#include "elastos/droid/net/DhcpStateMachine.h"
#include "elastos/droid/net/wifi/SupplicantStateTracker.h"
#include "elastos/droid/net/wifi/WifiMonitor.h"
#include "elastos/droid/net/wifi/WifiNative.h"
#include "elastos/droid/net/wifi/WifiConfigStore.h"
#include "elastos/droid/utility/AsyncChannel.h"
#include "elastos/droid/utility/State.h"
#include "elastos/droid/utility/StateMachine.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/utility/etl/List.h>

using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Regex::IPattern;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Net::DhcpInfoInternal;
using Elastos::Droid::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

extern "C" const InterfaceID EIID_TetherStateChange;

/**
 * Track the state of Wifi connectivity. All event handling is done here,
 * and all changes in connectivity state are initiated here.
 *
 * Wi-Fi now supports three modes of operation: Client, SoftAp and p2p
 * In the current implementation, we support concurrent wifi p2p and wifi operation.
 * The WifiStateMachine handles SoftAp and Client operations while WifiP2pService
 * handles p2p operation.
 *
 * @hide
 */
class WifiStateMachine : public StateMachine
{
public:

    /********************************************************
     * BroadcastReceivers
     *******************************************************/

    class TetherReceiver : public BroadcastReceiver
    {
    public:
        TetherReceiver(
            /* [in] */ WifiStateMachine* parent);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("WifiStateMachine::TetherReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        WifiStateMachine* mOwner;
    };

    class ScanReceiver : public BroadcastReceiver
    {
    public:
        ScanReceiver(
            /* [in] */ WifiStateMachine* parent);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("WifiStateMachine::ScanReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        WifiStateMachine* mOwner;
    };

    class ScreenReceiver : public BroadcastReceiver
    {
    public:
        ScreenReceiver(
            /* [in] */ WifiStateMachine* parent);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("WifiStateMachine::ScreenReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        WifiStateMachine* mOwner;
    };

    class StopCounterReceiver : public BroadcastReceiver
    {
    public:
        StopCounterReceiver(
            /* [in] */ WifiStateMachine* parent);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("WifiStateMachine::StopCounterReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        WifiStateMachine* mOwner;
    };

    /********************************************************
     * ContentObserver
     *******************************************************/
    class WifiContentObserver : public ContentObserver
    {
    public:
        WifiContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ WifiStateMachine* owner);

        virtual CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        WifiStateMachine* mOwner;
    };

    /********************************************************
     * HSM states
     *******************************************************/
    class DefaultState : public State
    {
    public:
        DefaultState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DefaultState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class InitialState : public State
    {
    public:
        InitialState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        //TODO: could move logging into a common class
        CARAPI Enter();

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "InitialState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class DriverLoadingState : public State
    {
    private:
        class Runnable
            : public ElRefBase
            , public IRunnable
        {
        public:
            Runnable(
                /* [in] */ Int32 arg,
                /* [in] */ WifiStateMachine* owner)
                : mArg(arg)
                , mOwner(owner)
            {}

            CAR_INTERFACE_DECL();

            CARAPI Run();

        private:
            Int32 mArg;
            WifiStateMachine* mOwner;
        };

    public:
        DriverLoadingState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DriverLoadingState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class DriverLoadedState : public State
    {
    public:
        DriverLoadedState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DriverLoadedState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class DriverUnloadingState : public State
    {
    private:
        class Runnable
            : public ElRefBase
            , public IRunnable
        {
        public:
            Runnable(
                /* [in] */ Int32 arg,
                /* [in] */ WifiStateMachine* owner)
                : mArg(arg)
                , mOwner(owner)
            {}

            CAR_INTERFACE_DECL();

            CARAPI Run();

        private:
            Int32 mArg;
            WifiStateMachine* mOwner;
        };

    public:
        DriverUnloadingState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DriverUnloadingState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class DriverUnloadedState : public State
    {
    public:
        DriverUnloadedState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DriverUnloadedState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class DriverFailedState : public State
    {
    public:
        DriverFailedState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DriverFailedState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class SupplicantStartingState : public State
    {
    public:
        SupplicantStartingState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "SupplicantStartingState";
            return NOERROR;
        }

    private:
        CARAPI_(void) InitializeWpsDetails();

    private:
        WifiStateMachine* mOwner;
    };

    class SupplicantStartedState : public State
    {
    public:
        SupplicantStartedState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Exit();

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "SupplicantStartedState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class SupplicantStoppingState : public State
    {
    public:
        SupplicantStoppingState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "SupplicantStoppingState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class DriverStartingState : public State
    {
    public:
        DriverStartingState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DriverStartingState";
            return NOERROR;
        }

    private:
        Int32 mTries;
        WifiStateMachine* mOwner;
    };

    class DriverStartedState : public State
    {
    public:
        DriverStartedState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Exit();

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DriverStartedState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class WaitForP2pDisableState : public State
    {
    public:
        WaitForP2pDisableState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "WaitForP2pDisableState";
            return NOERROR;
        }

    private:
        AutoPtr<State> mTransitionToState;
        WifiStateMachine* mOwner;
    };

    class DriverStoppingState : public State
    {
    public:
        DriverStoppingState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DriverStoppingState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class DriverStoppedState : public State
    {
    public:
        DriverStoppedState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DriverStoppedState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class ScanModeState : public State
    {
    public:
        ScanModeState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "ScanModeState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class ConnectModeState : public State
    {
    public:
        ConnectModeState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "ConnectModeState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class L2ConnectedState : public State
    {
    public:
        L2ConnectedState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Exit();

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "L2ConnectedState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class ObtainingIpState : public State
    {
    public:
        ObtainingIpState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "ObtainingIpState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class VerifyingLinkState : public State
    {
    public:
        VerifyingLinkState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "VerifyingLinkState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class CaptivePortalCheckState : public State
    {
    public:
        CaptivePortalCheckState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "CaptivePortalCheckState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class ConnectedState : public State
    {
    public:
        ConnectedState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Exit();

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "ConnectedState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class DisconnectingState : public State
    {
    public:
        DisconnectingState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DisconnectingState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class DisconnectedState : public State
    {
    public:
        DisconnectedState(
            /* [in] */ WifiStateMachine* owner)
            : mAlarmEnabled(FALSE)
            , mFrameworkScanIntervalMs(0)
            , mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Exit();

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DisconnectedState";
            return NOERROR;
        }

    private:
        CARAPI_(void) SetScanAlarm(
            /* [in] */ Boolean enabled);

    private:
        Boolean mAlarmEnabled;
        /* This is set from the overlay config file or from a secure setting.
         * A value of 0 disables scanning in the framework.
         */
        Int64 mFrameworkScanIntervalMs;
        WifiStateMachine* mOwner;
    };

    class WpsRunningState : public State
    {
    public:
        WpsRunningState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Exit();

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "WpsRunningState";
            return NOERROR;
        }

    private:
        //Tracks the source to provide a reply
        AutoPtr<IMessage> mSourceMessage;
        WifiStateMachine* mOwner;
    };

    class SoftApStartingState : public State
    {
    public:
        SoftApStartingState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "SoftApStartingState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class SoftApStartedState : public State
    {
    public:
        SoftApStartedState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "SoftApStartedState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class TetheringState : public State
    {
    public:
        TetheringState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "TetheringState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class TetheredState : public State
    {
    public:
        TetheredState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "TetheredState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

    class SoftApStoppingState : public State
    {
    public:
        SoftApStoppingState(
            /* [in] */ WifiStateMachine* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "SoftApStoppingState";
            return NOERROR;
        }

    private:
        WifiStateMachine* mOwner;
    };

private:
    class TetherStateChange
        : public ElRefBase
        , public IInterface
    {
    public:
        CAR_INTERFACE_DECL()

        TetherStateChange(
            /* [in] */ ArrayOf<String>* av,
            /* [in] */ ArrayOf<String>* ac)
            : mAvailable(av)
            , mActive(ac)
        {}

    public:
        AutoPtr< ArrayOf<String> > mAvailable;
        AutoPtr< ArrayOf<String> > mActive;
    };

    class Runnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        Runnable(
            /* [in] */ IWifiConfiguration* config,
            /* [in] */ WifiStateMachine* owner)
            : mOwner(owner)
        {}

        CAR_INTERFACE_DECL();

        CARAPI Run();

    private:
        AutoPtr<IWifiConfiguration> mConfig;
        WifiStateMachine* mOwner;
    };

public:
    WifiStateMachine(
        /* [in] */ IContext* context,
        /* [in] */ const String& wlanInterface);

    ~WifiStateMachine();

    /*********************************************************
     * Methods exposed for public use
     ********************************************************/

    CARAPI_(AutoPtr<IMessenger>) GetMessenger();

    /**
     * TODO: doc
     */
    CARAPI_(Boolean) SyncPingSupplicant(
        /* [in] */ AsyncChannel* channel);

    /**
     * TODO: doc
     */
    CARAPI_(void) StartScan(
        /* [in] */ Boolean forceActive);

    /**
     * TODO: doc
     */
    CARAPI_(void) SetWifiEnabled(
        /* [in] */ Boolean enable);

    /**
     * TODO: doc
     */
    CARAPI_(void) SetWifiApEnabled(
        /* [in] */ IWifiConfiguration* wifiConfig,
        /* [in] */ Boolean enable);

    CARAPI_(void) SetWifiApConfiguration(
        /* [in] */ IWifiConfiguration* config);

    CARAPI_(AutoPtr<IWifiConfiguration>) SyncGetWifiApConfiguration();

    /**
     * TODO: doc
     */
    CARAPI_(Int32) SyncGetWifiState();

    /**
     * TODO: doc
     */
    CARAPI_(String) SyncGetWifiStateByName();

    /**
     * TODO: doc
     */
    CARAPI_(Int32) SyncGetWifiApState();

    /**
     * TODO: doc
     */
    CARAPI_(String) SyncGetWifiApStateByName();

    /**
     * Get status information for the current connection, if any.
     * @return a {@link WifiInfo} object containing information about the current connection
     *
     */
    CARAPI_(AutoPtr<IWifiInfo>) SyncRequestConnectionInfo();

    CARAPI_(AutoPtr<IDhcpInfo>) SyncGetDhcpInfo();

    /**
     * TODO: doc
     */
    CARAPI_(void) SetDriverStart(
        /* [in] */ Boolean enable,
        /* [in] */ Boolean ecm);

    CARAPI_(void) CaptivePortalCheckComplete();

    /**
     * TODO: doc
     */
    CARAPI_(void) SetScanOnlyMode(
        /* [in] */ Boolean enable);

    /**
     * TODO: doc
     */
    CARAPI_(void) SetScanType(
        /* [in] */ Boolean active);

    /**
     * TODO: doc
     */
    CARAPI_(AutoPtr<ArrayOf<IScanResult*> >) SyncGetScanResultsList();

    /**
     * Disconnect from Access Point
     */
    CARAPI_(void) DisconnectCommand();

    /**
     * Initiate a reconnection to AP
     */
    CARAPI_(void) ReconnectCommand();

    /**
     * Initiate a re-association to AP
     */
    CARAPI_(void) ReassociateCommand();

    /**
     * Add a network synchronously
     *
     * @return network id of the new network
     */
    CARAPI_(Int32) SyncAddOrUpdateNetwork(
        /* [in] */ AsyncChannel* channel,
        /* [in] */ IWifiConfiguration* config);

    CARAPI_(AutoPtr<IObjectContainer>) SyncGetConfiguredNetworks(
        /* [in] */ AsyncChannel* channel);

    /**
     * Delete a network
     *
     * @param networkId id of the network to be removed
     */
    CARAPI_(Boolean) SyncRemoveNetwork(
        /* [in] */ AsyncChannel* channel,
        /* [in] */ Int32 networkId);

    /**
     * Enable a network
     *
     * @param netId network id of the network
     * @param disableOthers true, if all other networks have to be disabled
     * @return {@code true} if the operation succeeds, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) SyncEnableNetwork(
        /* [in] */ AsyncChannel* channel,
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers);

    /**
     * Disable a network
     *
     * @param netId network id of the network
     * @return {@code true} if the operation succeeds, {@code FALSE} otherwise
     */
    CARAPI_(Boolean) SyncDisableNetwork(
        /* [in] */ AsyncChannel* channel,
        /* [in] */ Int32 netId);

    /**
     * Blacklist a BSSID. This will avoid the AP if there are
     * alternate APs to connect
     *
     * @param bssid BSSID of the network
     */
    CARAPI_(void) AddToBlacklist(
        /* [in] */ const String& bssid);

    /**
     * Clear the blacklist list
     *
     */
    CARAPI_(void) ClearBlacklist();

    CARAPI_(void) EnableRssiPolling(
        /* [in] */ Boolean enabled);

    CARAPI_(void) EnableBackgroundScanCommand(
        /* [in] */ Boolean enabled);

    CARAPI_(void) EnableAllNetworks();

    /**
     * Start filtering Multicast v4 packets
     */
    CARAPI_(void) StartFilteringMulticastV4Packets();

    /**
     * Stop filtering Multicast v4 packets
     */
    CARAPI_(void) StopFilteringMulticastV4Packets();

    /**
     * Start filtering Multicast v4 packets
     */
    CARAPI_(void) StartFilteringMulticastV6Packets();

    /**
     * Stop filtering Multicast v4 packets
     */
    CARAPI_(void) StopFilteringMulticastV6Packets();

    /**
     * Set high performance mode of operation.
     * Enabling would set active power mode and disable suspend optimizations;
     * disabling would set auto power mode and enable suspend optimizations
     * @param enable true if enable, FALSE otherwise
     */
    CARAPI_(void) SetHighPerfModeEnabled(
        /* [in] */ Boolean enable);

    /**
     * Set the country code
     * @param countryCode following ISO 3166 format
     * @param persist {@code true} if the setting should be remembered.
     */
    CARAPI_(void) SetCountryCode(
        /* [in] */ const String& countryCode,
        /* [in] */ Boolean persist);

    /**
     * Set the operational frequency band
     * @param band
     * @param persist {@code true} if the setting should be remembered.
     */
    CARAPI_(void) SetFrequencyBand(
        /* [in] */ Int32 band,
        /* [in] */ Boolean persist);

    /**
     * Returns the operational frequency band
     */
    CARAPI_(Int32) GetFrequencyBand();

    /**
     * Returns the wifi configuration file
     */
    CARAPI_(String) GetConfigFile();

    /**
     * Send a message indicating bluetooth adapter connection state changed
     */
    CARAPI_(void) SendBluetoothAdapterStateChange(
        /* [in] */ Int32 state);

    /**
     * Save configuration on supplicant
     *
     * @return {@code true} if the operation succeeds, {@code FALSE} otherwise
     *
     * TODO: deprecate this
     */
    CARAPI_(Boolean) SyncSaveConfig(
        /* [in] */ AsyncChannel* channel);

    CARAPI_(void) UpdateBatteryWorkSource(
        /* [in] */ IWorkSource* newSource);

    // @Override
    CARAPI_(String) ToString();

    CARAPI_(void) HandlePreDhcpSetup();

    CARAPI_(void) HandlePostDhcpSetup();

protected:
    // @Override
    CARAPI_(Boolean) RecordLogRec(
        /* [in] */ IMessage* msg);

private:
    /*********************************************************
     * Internal private functions
     ********************************************************/
    CARAPI_(void) HandleScreenStateChanged(
        /* [in] */ Boolean screenOn);

    CARAPI_(void) CheckAndSetConnectivityInstance();

    CARAPI_(Boolean) StartTethering(
        /* [in] */ ArrayOf<String>* available);

    CARAPI_(void) StopTethering();

    CARAPI_(Boolean) IsWifiTethered(
        /* [in] */ ArrayOf<String>* active);

    /**
     * Set the country code from the system setting value, if any.
     */
    CARAPI_(void) SetCountryCode();

    /**
     * Set the frequency band from the system setting value, if any.
     */
    CARAPI_(void) SetFrequencyBand();

    CARAPI_(void) SetSuspendOptimizationsNative(
        /* [in] */ Int32 reason,
        /* [in] */ Boolean enabled);

    CARAPI_(void) SetSuspendOptimizations(
        /* [in] */ Int32 reason,
        /* [in] */ Boolean enabled);

    CARAPI_(void) SetWifiState(
        /* [in] */ Int32 wifiState);

    CARAPI_(void) SetWifiApState(
        /* [in] */ Int32 wifiApState);

    /**
     * Format:
     * bssid=68:7f:76:d7:1a:6e
     * freq=2412
     * level=-44
     * tsf=1344626243700342
     * flags=[WPA2-PSK-CCMP][WPS][ESS]
     * ssid=zfdy
     * ====
     * bssid=68:5f:74:d7:1a:6f
     * freq=5180
     * level=-73
     * tsf=1344626243700373
     * flags=[WPA2-PSK-CCMP][WPS][ESS]
     * ssid=zuby
     * ====
     */
    CARAPI_(void) SetScanResults(
        /* [in] */ const String& scanResults);

    /*
     * Fetch RSSI and linkspeed on current connection
     */
    CARAPI_(void) FetchRssiAndLinkSpeedNative();

    /*
     * Fetch TX packet counters on current connection
     */
    CARAPI_(void) FetchPktcntNative(
        /* [in] */ IRssiPacketCountInfo* info);

    CARAPI_(void) ConfigureLinkProperties();

    CARAPI_(Int32) GetMaxDhcpRetries();

    CARAPI_(void) SendScanResultsAvailableBroadcast();

    CARAPI_(void) SendRssiChangeBroadcast(
        /* [in] */ Int32 newRssi);

    CARAPI_(void) SendNetworkStateChangeBroadcast(
        /* [in] */ const String& bssid);

    CARAPI_(void) SendLinkConfigurationChangedBroadcast();

    CARAPI_(void) SendSupplicantConnectionChangedBroadcast(
        /* [in] */ Boolean connected);

    /**
     * Record the detailed state of a network.
     * @param state the new {@code DetailedState}
     */
    CARAPI_(void) SetNetworkDetailedState(
        /* [in] */ NetworkInfoDetailedState state);

    CARAPI_(NetworkInfoDetailedState) GetNetworkDetailedState();


    CARAPI_(SupplicantState) HandleSupplicantStateChange(
        /* [in] */ IMessage* message);

    /**
     * Resets the Wi-Fi Connections by clearing any state, resetting any sockets
     * using the interface, stopping DHCP & disabling interface
     */
    CARAPI_(void) HandleNetworkDisconnect();

    CARAPI_(void) HandleSuccessfulIpConfiguration(
        /* [in] */ DhcpInfoInternal* dhcpInfoInternal);

    CARAPI_(void) HandleFailedIpConfiguration();

    /* Current design is to not set the config on a running hostapd but instead
     * stop and start tethering when user changes config on a running access point
     *
     * TODO: Add control channel setup through hostapd that allows changing config
     * on a running daemon
     */
    CARAPI_(void) StartSoftApWithConfig(
        /* [in] */ IWifiConfiguration* config);

    //State machine initiated requests can have replyTo set to null indicating
    //there are no recepients, we ignore those reply actions
    CARAPI_(void) ReplyToMessage(
        /* [in] */ IMessage* msg,
        /* [in] */ Int32 what);

    CARAPI_(void) ReplyToMessage(
        /* [in] */ IMessage* msg,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1);

    CARAPI_(void) ReplyToMessage(
        /* [in] */ IMessage* msg,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    /**
     * arg2 on the source message has a unique id that needs to be retained in replies
     * to match the request
     *
     * see WifiManager for details
     */
    CARAPI_(AutoPtr<IMessage>) ObtainMessageWithArg2(
        /* [in] */ IMessage* srcMsg);

    CARAPI_(void) Log(
        /* [in] */ const String& s);

    CARAPI_(void) Loge(
        /* [in] */ const String& s);

public:
    /* The base for wifi message types */
    static const Int32 BASE = IProtocol::BASE_WIFI;
    /* Load the driver */
    static const Int32 CMD_LOAD_DRIVER = BASE + 1;
    /* Unload the driver */
    static const Int32 CMD_UNLOAD_DRIVER = BASE + 2;
    /* Indicates driver load succeeded */
    static const Int32 CMD_LOAD_DRIVER_SUCCESS = BASE + 3;
    /* Indicates driver load failed */
    static const Int32 CMD_LOAD_DRIVER_FAILURE = BASE + 4;
    /* Indicates driver unload succeeded */
    static const Int32 CMD_UNLOAD_DRIVER_SUCCESS = BASE + 5;
    /* Indicates driver unload failed */
    static const Int32 CMD_UNLOAD_DRIVER_FAILURE = BASE + 6;

    /* Start the supplicant */
    static const Int32 CMD_START_SUPPLICANT = BASE + 11;
    /* Stop the supplicant */
    static const Int32 CMD_STOP_SUPPLICANT = BASE + 12;
    /* Start the driver */
    static const Int32 CMD_START_DRIVER = BASE + 13;
    /* Stop the driver */
    static const Int32 CMD_STOP_DRIVER = BASE + 14;
    /* Indicates Static IP succeded */
    static const Int32 CMD_STATIC_IP_SUCCESS = BASE + 15;
    /* Indicates Static IP failed */
    static const Int32 CMD_STATIC_IP_FAILURE = BASE + 16;
    /* Indicates supplicant stop failed */
    static const Int32 CMD_STOP_SUPPLICANT_FAILED = BASE + 17;
    /* Delayed stop to avoid shutting down driver too quick*/
    static const Int32 CMD_DELAYED_STOP_DRIVER = BASE + 18;
    /* A delayed message sent to start driver when it fail to come up */
    static const Int32 CMD_DRIVER_START_TIMED_OUT = BASE + 19;
    /* Ready to switch to network as default */
    static const Int32 CMD_CAPTIVE_CHECK_COMPLETE = BASE + 20;

    /* Start the soft access point */
    static const Int32 CMD_START_AP = BASE + 21;
    /* Indicates soft ap start succeded */
    static const Int32 CMD_START_AP_SUCCESS = BASE + 22;
    /* Indicates soft ap start failed */
    static const Int32 CMD_START_AP_FAILURE = BASE + 23;
    /* Stop the soft access point */
    static const Int32 CMD_STOP_AP = BASE + 24;
    /* Set the soft access point configuration */
    static const Int32 CMD_SET_AP_CONFIG = BASE + 25;
    /* Soft access point configuration set completed */
    static const Int32 CMD_SET_AP_CONFIG_COMPLETED = BASE + 26;
    /* Request the soft access point configuration */
    static const Int32 CMD_REQUEST_AP_CONFIG = BASE + 27;
    /* Response to access point configuration request */
    static const Int32 CMD_RESPONSE_AP_CONFIG = BASE + 28;
    /* Invoked when getting a tether state change notification */
    static const Int32 CMD_TETHER_STATE_CHANGE = BASE + 29;
    /* A delayed message sent to indicate tether state change failed to arrive */
    static const Int32 CMD_TETHER_NOTIFICATION_TIMED_OUT = BASE + 30;

    static const Int32 CMD_BLUETOOTH_ADAPTER_STATE_CHANGE = BASE + 31;

    /* Supplicant commands */
    /* Is supplicant alive ? */
    static const Int32 CMD_PING_SUPPLICANT = BASE + 51;
    /* Add/update a network configuration */
    static const Int32 CMD_ADD_OR_UPDATE_NETWORK = BASE + 52;
    /* Delete a network */
    static const Int32 CMD_REMOVE_NETWORK = BASE + 53;
    /* Enable a network. The device will attempt a connection to the given network. */
    static const Int32 CMD_ENABLE_NETWORK = BASE + 54;
    /* Enable all networks */
    static const Int32 CMD_ENABLE_ALL_NETWORKS = BASE + 55;
    /* Blacklist network. De-prioritizes the given BSSID for connection. */
    static const Int32 CMD_BLACKLIST_NETWORK = BASE + 56;
    /* Clear the blacklist network list */
    static const Int32 CMD_CLEAR_BLACKLIST = BASE + 57;
    /* Save configuration */
    static const Int32 CMD_SAVE_CONFIG = BASE + 58;
    /* Get configured networks*/
    static const Int32 CMD_GET_CONFIGURED_NETWORKS = BASE + 59;

    /* Supplicant commands after driver start*/
    /* Initiate a scan */
    static const Int32 CMD_START_SCAN = BASE + 71;
    /* Set scan mode. CONNECT_MODE or SCAN_ONLY_MODE */
    static const Int32 CMD_SET_SCAN_MODE = BASE + 72;
    /* Set scan type. SCAN_ACTIVE or SCAN_PASSIVE */
    static const Int32 CMD_SET_SCAN_TYPE = BASE + 73;
    /* Disconnect from a network */
    static const Int32 CMD_DISCONNECT = BASE + 74;
    /* Reconnect to a network */
    static const Int32 CMD_RECONNECT = BASE + 75;
    /* Reassociate to a network */
    static const Int32 CMD_REASSOCIATE = BASE + 76;
    /* Controls suspend mode optimizations
     *
     * When high perf mode is enabled, suspend mode optimizations are disabled
     *
     * When high perf mode is disabled, suspend mode optimizations are enabled
     *
     * Suspend mode optimizations include:
     * - packet filtering
     * - turn off roaming
     * - DTIM wake up settings
     */
    static const Int32 CMD_SET_HIGH_PERF_MODE = BASE + 77;
    /* Set the country code */
    static const Int32 CMD_SET_COUNTRY_CODE = BASE + 80;
    /* Enables RSSI poll */
    static const Int32 CMD_ENABLE_RSSI_POLL = BASE + 82;
    /* RSSI poll */
    static const Int32 CMD_RSSI_POLL = BASE + 83;
    /* Set up packet filtering */
    static const Int32 CMD_START_PACKET_FILTERING = BASE + 84;
    /* Clear packet filter */
    static const Int32 CMD_STOP_PACKET_FILTERING = BASE + 85;
    /* Enable suspend mode optimizations in the driver */
    static const Int32 CMD_SET_SUSPEND_OPT_ENABLED = BASE + 86;
    /* When there are no saved networks, we do a periodic scan to notify user of
     * an open network */
    static const Int32 CMD_NO_NETWORKS_PERIODIC_SCAN = BASE + 88;

    /* arg1 values to CMD_STOP_PACKET_FILTERING and CMD_START_PACKET_FILTERING */
    static const Int32 MULTICAST_V6 = 1;
    static const Int32 MULTICAST_V4 = 0;

   /* Set the frequency band */
    static const Int32 CMD_SET_FREQUENCY_BAND = BASE + 90;
    /* Enable background scan for configured networks */
    static const Int32 CMD_ENABLE_BACKGROUND_SCAN = BASE + 91;

    /* Commands from/to the SupplicantStateTracker */
    /* Reset the supplicant state tracker */
    static const Int32 CMD_RESET_SUPPLICANT_STATE = BASE + 111;

    /* P2p commands */
    /* We are ok with no response here since we wont do much with it anyway */
    static const Int32 CMD_ENABLE_P2P = BASE + 131;
    /* In order to shut down supplicant cleanly, we wait till p2p has
     * been disabled */
    static const Int32 CMD_DISABLE_P2P_REQ = BASE + 132;
    static const Int32 CMD_DISABLE_P2P_RSP = BASE + 133;

    static const Boolean DBG;

private:
    static const String TAG;
    static const String NETWORKTYPE;

    AutoPtr<WifiMonitor> mWifiMonitor;
    AutoPtr<WifiNative> mWifiNative;
    AutoPtr<WifiConfigStore> mWifiConfigStore;
    AutoPtr<IINetworkManagementService> mNwService;
    AutoPtr<IConnectivityManager> mConnectivityManager;

    Boolean mP2pSupported;
    AutoPtr<IAtomicBoolean> mP2pConnected;
    Boolean mTemporarilyDisconnectWifi;
    String mPrimaryDeviceType;

    /* Scan results handling */
    List< AutoPtr<IScanResult> > mScanResults;
    static const Int32 SCAN_RESULT_CACHE_SIZE = 80;
    HashMap<String, AutoPtr<IScanResult> > mScanResultCache;
    Object mScanResultCacheLock;

    /* Chipset supports background scan */
    Boolean mBackgroundScanSupported;

    String mInterfaceName;
    /* Tethering interface could be seperate from wlan interface */
    String mTetherInterfaceName;

    Int32 mLastSignalLevel;
    String mLastBssid;
    Int32 mLastNetworkId;
    Boolean mEnableRssiPolling;
    Boolean mEnableBackgroundScan;
    Int32 mRssiPollToken;
    Int32 mReconnectCount;
    Boolean mIsScanMode;
    Boolean mScanResultIsPending;
    /* Tracks if the current scan settings are active */
    Boolean mSetScanActive;
    /* Tracks if state machine has received any screen state change broadcast yet.
     * We can miss one of these at boot.
     */
    AutoPtr<IAtomicBoolean> mScreenBroadcastReceived;

    Boolean mBluetoothConnectionActive;

    AutoPtr<IPowerManagerWakeLock> mSuspendWakeLock;

    /**
     * Interval in milliseconds between polling for RSSI
     * and linkspeed information
     */
    static const Int32 POLL_RSSI_INTERVAL_MSECS = 3000;

    /**
     * Delay between supplicant restarts upon failure to establish connection
     */
    static const Int32 SUPPLICANT_RESTART_INTERVAL_MSECS = 5000;

    /**
     * Number of times we attempt to restart supplicant
     */
    static const Int32 SUPPLICANT_RESTART_TRIES = 5;

    Int32 mSupplicantRestartCount;
    /* Tracks sequence number on stop failure message */
    Int32 mSupplicantStopFailureToken;

    /**
     * Tether state change notification time out
     */
    static const Int32 TETHER_NOTIFICATION_TIME_OUT_MSECS = 5000;

    /* Tracks sequence number on a tether notification time out */
    Int32 mTetherToken;

    /**
     * Driver start time out.
     */
    static const Int32 DRIVER_START_TIME_OUT_MSECS = 10000;

    /* Tracks sequence number on a driver time out */
    Int32 mDriverStartToken;

    AutoPtr<ILinkProperties> mLinkProperties;

    /* Tracks sequence number on a periodic scan message */
    Int32 mPeriodicScanToken;

    // Wakelock held during wifi start/stop and driver load/unload
    AutoPtr<IPowerManagerWakeLock> mWakeLock;

    AutoPtr<IContext> mContext;

    AutoPtr<DhcpInfoInternal> mDhcpInfoInternal;
    Object mDhcpInfoInternalLock;
    AutoPtr<IWifiInfo> mWifiInfo;
    AutoPtr<INetworkInfo> mNetworkInfo;
    AutoPtr<SupplicantStateTracker> mSupplicantStateTracker;
    AutoPtr<DhcpStateMachine> mDhcpStateMachine;

    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IPendingIntent> mScanIntent;
    AutoPtr<IPendingIntent> mDriverStopIntent;

    /* Tracks current frequency mode */
    AutoPtr<IAtomicInteger32> mFrequencyBand;

    /* Tracks if we are filtering Multicast v4 packets. Default is to filter. */
    AutoPtr<IAtomicBoolean> mFilteringMulticastV4Packets;

    // Channel for sending replies.
    AutoPtr<AsyncChannel> mReplyChannel;

    AutoPtr<IWifiP2pManager> mWifiP2pManager;
    //Used to initiate a connection with WifiP2pService
    AutoPtr<AsyncChannel> mWifiP2pChannel;
    AutoPtr<AsyncChannel> mWifiApConfigChannel;

    // Event log tags (must be in sync with event-log-tags)
    static const Int32 EVENTLOG_WIFI_STATE_CHANGED = 50021;
    static const Int32 EVENTLOG_WIFI_EVENT_HANDLED = 50022;
    static const Int32 EVENTLOG_SUPPLICANT_STATE_CHANGED = 50023;

    static const Int32 CONNECT_MODE = 1;
    static const Int32 SCAN_ONLY_MODE = 2;

    static const Int32 SCAN_ACTIVE = 1;
    static const Int32 SCAN_PASSIVE = 2;

    static const Int32 SUCCESS = 1;
    static const Int32 FAILURE = -1;

    /* Phone in emergency call back mode */
    static const Int32 IN_ECM_STATE = 1;
    static const Int32 NOT_IN_ECM_STATE = 0;

    /**
     * The maximum number of times we will retry a connection to an access point
     * for which we have failed in acquiring an IP address from DHCP. A value of
     * N means that we will make N+1 connection attempts in all.
     * <p>
     * See {@link Settings.Secure#WIFI_MAX_DHCP_RETRY_COUNT}. This is the default
     * value if a Settings value is not present.
     */
    static const Int32 DEFAULT_MAX_DHCP_RETRIES = 9;

    /* Tracks if suspend optimizations need to be disabled by DHCP,
     * screen or due to high perf mode.
     * When any of them needs to disable it, we keep the suspend optimizations
     * disabled
     */
    Int32 mSuspendOptNeedsDisabled;

    static const Int32 SUSPEND_DUE_TO_DHCP = 1;
    static const Int32 SUSPEND_DUE_TO_HIGH_PERF = 1 << 1;
    static const Int32 SUSPEND_DUE_TO_SCREEN = 1 << 2;

    /* Tracks if user has enabled suspend optimizations through settings */
    AutoPtr<IAtomicBoolean> mUserWantsSuspendOpt;

    /**
     * Default framework scan interval in milliseconds. This is used in the scenario in which
     * wifi chipset does not support background scanning to set up a
     * periodic wake up scan so that the device can connect to a new access
     * point on the move. {@link Settings.Global#WIFI_FRAMEWORK_SCAN_INTERVAL_MS} can
     * override this.
     */
    Int32 mDefaultFrameworkScanIntervalMs;

    /**
     * Supplicant scan interval in milliseconds.
     * Comes from {@link Settings.Global#WIFI_SUPPLICANT_SCAN_INTERVAL_MS} or
     * from the default config if the setting is not set
     */
    Int64 mSupplicantScanIntervalMs;

    /**
     * Minimum time interval between enabling all networks.
     * A device can end up repeatedly connecting to a bad network on screen on/off toggle
     * due to enabling every time. We add a threshold to avoid this.
     */
    static const Int32 MIN_INTERVAL_ENABLE_ALL_NETWORKS_MS = 10 * 60 * 1000; /* 10 minutes */
    Int64 mLastEnableAllNetworksTime;

    /**
     * Starting and shutting down driver too quick causes problems leading to driver
     * being in a bad state. Delay driver stop.
     */
    Int32 mDriverStopDelayMs;
    Int32 mDelayedStopCounter;
    Boolean mInDelayedStop;

    static const Int32 MIN_RSSI = -200;
    static const Int32 MAX_RSSI = 256;

    /* Default parent state */
    AutoPtr<State> mDefaultState;
    /* Temporary initial state */
    AutoPtr<State> mInitialState;
    /* Unloading the driver */
    AutoPtr<State> mDriverUnloadingState;
    /* Loading the driver */
    AutoPtr<State> mDriverUnloadedState;
    /* Driver load/unload failed */
    AutoPtr<State> mDriverFailedState;
    /* Driver loading */
    AutoPtr<State> mDriverLoadingState;
    /* Driver loaded */
    AutoPtr<State> mDriverLoadedState;
    /* Driver loaded, waiting for supplicant to start */
    AutoPtr<State> mSupplicantStartingState;
    /* Driver loaded and supplicant ready */
    AutoPtr<State> mSupplicantStartedState;
    /* Waiting for supplicant to stop and monitor to exit */
    AutoPtr<State> mSupplicantStoppingState;
    /* Driver start issued, waiting for completed event */
    AutoPtr<State> mDriverStartingState;
    /* Driver started */
    AutoPtr<State> mDriverStartedState;
    /* Wait until p2p is disabled
     * This is a special state which is entered right after we exit out of DriverStartedState
     * before transitioning to another state.
     */
    AutoPtr<State> mWaitForP2pDisableState;
    /* Driver stopping */
    AutoPtr<State> mDriverStoppingState;
    /* Driver stopped */
    AutoPtr<State> mDriverStoppedState;
    /* Scan for networks, no connection will be established */
    AutoPtr<State> mScanModeState;
    /* Connecting to an access point */
    AutoPtr<State> mConnectModeState;
    /* Connected at 802.11 (L2) level */
    AutoPtr<State> mL2ConnectedState;
    /* fetching IP after connection to access point (assoc+auth complete) */
    AutoPtr<State> mObtainingIpState;
    /* Waiting for link quality verification to be complete */
    AutoPtr<State> mVerifyingLinkState;
    /* Waiting for captive portal check to be complete */
    AutoPtr<State> mCaptivePortalCheckState;
    /* Connected with IP addr */
    AutoPtr<State> mConnectedState;
    /* disconnect issued, waiting for network disconnect confirmation */
    AutoPtr<State> mDisconnectingState;
    /* Network is not connected, supplicant assoc+auth is not complete */
    AutoPtr<State> mDisconnectedState;
    /* Waiting for WPS to be completed*/
    AutoPtr<State> mWpsRunningState;

    /* Soft ap is starting up */
    AutoPtr<State> mSoftApStartingState;
    /* Soft ap is running */
    AutoPtr<State> mSoftApStartedState;
    /* Soft ap is running and we are waiting for tether notification */
    AutoPtr<State> mTetheringState;
    /* Soft ap is running and we are tethered through connectivity service */
    AutoPtr<State> mTetheredState;
    /* Waiting for untether confirmation to stop soft Ap */
    AutoPtr<State> mSoftApStoppingState;

    /**
     * One of  {@link WifiManager#WIFI_STATE_DISABLED},
     *         {@link WifiManager#WIFI_STATE_DISABLING},
     *         {@link WifiManager#WIFI_STATE_ENABLED},
     *         {@link WifiManager#WIFI_STATE_ENABLING},
     *         {@link WifiManager#WIFI_STATE_UNKNOWN}
     *
     */
    AutoPtr<IAtomicInteger32> mWifiState;

    /**
     * One of  {@link WifiManager#WIFI_AP_STATE_DISABLED},
     *         {@link WifiManager#WIFI_AP_STATE_DISABLING},
     *         {@link WifiManager#WIFI_AP_STATE_ENABLED},
     *         {@link WifiManager#WIFI_AP_STATE_ENABLING},
     *         {@link WifiManager#WIFI_AP_STATE_FAILED}
     *
     */
    AutoPtr<IAtomicInteger32> mWifiApState;

    AutoPtr<IAtomicInteger32> mLastEnableUid;
    AutoPtr<IAtomicInteger32> mLastApEnableUid;

    static const Int32 SCAN_REQUEST = 0;
    static String ACTION_START_SCAN;

    static String DELAYED_STOP_COUNTER;
    static const Int32 DRIVER_STOP_REQUEST = 0;
    static String ACTION_DELAYED_DRIVER_STOP;

    /**
     * Keep track of whether WIFI is running.
     */
    Boolean mIsRunning;

    /**
     * Keep track of whether we last told the battery stats we had started.
     */
    Boolean mReportedRunning;

    /**
     * Most recently set source of starting WIFI.
     */
    AutoPtr<IWorkSource> mRunningWifiUids;
    Object mRunningWifiUidsLock;

    /**
     * The last reported UIDs that were responsible for starting WIFI.
     */
    AutoPtr<IWorkSource> mLastRunningWifiUids;

    AutoPtr<IIBatteryStats> mBatteryStats;
    AutoPtr<IUserHandle> USERHANDLE_ALL;

    static const String BSSID_STR;
    static const String FREQ_STR;
    static const String LEVEL_STR;
    static const String TSF_STR;
    static const String FLAGS_STR;
    static const String SSID_STR;
    static const String DELIMITER_STR;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_SERVER_WIFI_WIFISTATEMACHINE_H__
