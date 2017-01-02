//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_CCOMMONTIMEMANAGEMENTSERVICE_H__
#define __ELASTOS_DROID_SERVER_CCOMMONTIMEMANAGEMENTSERVICE_H__

#include "_Elastos_Droid_Server_CCommonTimeManagementService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/net/BaseNetworkObserver.h"
#include "elastos/droid/content/BroadcastReceiver.h"
//#include "elastos/droid/os/CommonTimeConfig.h"
#include <elastos/core/Runnable.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Net.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
//using Elastos::Droid::Os::CommonTimeConfig;
using Elastos::Droid::Server::Net::BaseNetworkObserver;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * @hide
 * <p>CCommonTimeManagementService manages the configuration of the native Common Time service,
 * reconfiguring the native service as appropriate in response to changes in network configuration.
 */
CarClass(CCommonTimeManagementService)
    , public Object
    , public IBinder
{
public:
    /*
     * Callback handler implementations.
     */
    class IfaceObserver
        : public BaseNetworkObserver
    {
    public:
        IfaceObserver();

        constructor(
            /* [in] */ IBinder* commonTimeManagementService);

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
    private:
        CCommonTimeManagementService* mHost;
    };

    /*
     * Inner helper classes
     */
    class InterfaceScoreRule
        : public Object
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
private:
    class ReconnectRunnable
        : public Runnable
    {
    public:
        ReconnectRunnable(
            /* [in] */ CCommonTimeManagementService* owner)
            : mHost(owner)
        {}

        CARAPI Run();

    private:
        CCommonTimeManagementService* mHost;
    };

    class NoInterfaceRunnable
        : public Runnable
    {
    public:
        NoInterfaceRunnable(
            /* [in] */ CCommonTimeManagementService* owner)
            : mHost(owner)
        {}

        CARAPI Run();

    private:
        CCommonTimeManagementService* mHost;
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

        TO_STRING_IMPL("CCommonTimeManagementService::ConnectivityMangerObserver: ")
    private:
        CCommonTimeManagementService* mHost;
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
        : public Object
    {
    public:
        CTServerDiedListener(
            /* [in] */ CCommonTimeManagementService* owner);

        CARAPI OnServerDied();

    private:
        CCommonTimeManagementService* mHost;
    };

    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCommonTimeManagementService();

    /*
     * Public interface (constructor, systemReady and dump)
     */
    CARAPI constructor(
        /* [in] */ IContext *context);

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
    static const String TAG;
    static const Int32 NATIVE_SERVICE_RECONNECT_TIMEOUT = 5000;
    static const String AUTO_DISABLE_PROP;
    static const String ALLOW_WIFI_PROP;
    static const String SERVER_PRIO_PROP;
    static const String NO_INTERFACE_TIMEOUT_PROP;
    static Boolean AUTO_DISABLE;
    static Boolean ALLOW_WIFI;
    static Byte BASE_SERVER_PRIO;
    static Int32 NO_INTERFACE_TIMEOUT;
    static AutoPtr<ArrayOf<InterfaceScoreRule* > > IFACE_SCORE_RULES;

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
    static const StaticInitializer sInitializer;
};

}//namespace Server
}//namespace Droid
}//namespace Elastos


DEFINE_CONVERSION_FOR(Elastos::Droid::Server::CCommonTimeManagementService::InterfaceScoreRule, IInterface)

#endif //__ELASTOS_DROID_SERVER_CCOMMONTIMEMANAGEMENTSERVICE_H__
