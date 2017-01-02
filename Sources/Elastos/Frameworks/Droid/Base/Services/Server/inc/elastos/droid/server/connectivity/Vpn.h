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

#ifndef __ELASTOS_DROID_SERVER_CONNECTIVITY_VPN_H__
#define __ELASTOS_DROID_SERVER_CONNECTIVITY_VPN_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/net/NetworkAgent.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUidRange;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::INetworkAgent;
using Elastos::Droid::Net::NetworkAgent;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Net::NetworkInfoDetailedState;
using Elastos::Droid::KeyStore::Security::IKeyStore;
using Elastos::Droid::Internal::Net::IVpnConfig;
using Elastos::Droid::Internal::Net::IVpnProfile;
using Elastos::Droid::Internal::Net::ILegacyVpnInfo;
using Elastos::Core::Thread;
using Elastos::Utility::IList;
using Elastos::Utility::ISortedSet;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

class Vpn
    : public Object
{
private:
    class LegacyVpnRunner;

    class MyNetworkAgent
        : public NetworkAgent
    {
    public:
        TO_STRING_IMPL("Vpn::MyNetworkAgent")

        // @Override
        CARAPI Unwanted()
        {
            // We are user controlled, not driven by NetworkRequest.
            return NOERROR;
        }
    };

    class UserIntentReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("Vpn::UserIntentReceiver")

        UserIntentReceiver(
            /* [in] */ Vpn* host)
        : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        Vpn* mHost;
    };

    class Connection
        : public Object
        , public IServiceConnection
    {
    public:
        Connection(
            /* [in] */ Vpn* host)
        : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        Vpn* mHost;
        AutoPtr<IBinder> mService;
    };

    /**
     * Watch for the outer connection (passing in the constructor) going away.
     */
    class LegacyVpnRunnerReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("Vpn::LegacyVpnRunnerReceiver")

        LegacyVpnRunnerReceiver(
            /* [in] */ Vpn* host,
            /* [in] */ LegacyVpnRunner* runner)
        : mHost(host)
        , mLegacyVpnRunner(runner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        Vpn* mHost;
        LegacyVpnRunner* mLegacyVpnRunner;
    };

    class LegacyVpnRunner
        : public Thread
    {
    public:
        LegacyVpnRunner();

        CARAPI constructor(
            /* [in] */ IVpnConfig* config,
            /* [in] */ ArrayOf<String>* racoon,
            /* [in] */ ArrayOf<String>* mtpd,
            /* [in] */ Vpn* host);

        CARAPI Check(
            /* [in] */ const String& interfaze);

        CARAPI Exit();

        CARAPI Run();

    private:
        CARAPI Checkpoint(
                /* [in] */ Boolean yield);

        CARAPI Execute();

        CARAPI_(void) MonitorDaemons();

    private:
        friend class Vpn;

        static const String TAG;
        static Object sTAGLock;

        AutoPtr<ArrayOf<String> > mDaemons;
        AutoPtr<ArrayOf<StringArray> > mArguments;
        AutoPtr<ArrayOf<ILocalSocket*> > mSockets;
        String mOuterInterface;

        /**
         * Watch for the outer connection (passing in the constructor) going away.
         */
        AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
        AutoPtr<IAtomicInteger32> mOuterConnection;

        Int64 mTimer;
        Vpn* mHost;
    };

public:
    Vpn(
        /* [in] */ ILooper* looper,
        /* [in] */ IContext* context,
        /* [in] */ IINetworkManagementService* netService,
        /* [in] */ IIConnectivityManager* connMgr,
        /* [in] */ Int32 userHandle);

    /**
     * Set if this object is responsible for watching for {@link NetworkInfo}
     * teardown. When {@code false}, teardown is handled externally by someone
     * else.
     */
    CARAPI SetEnableTeardown(
        /* [in] */ Boolean enableTeardown);

    /**
     * Prepare for a VPN application. This method is designed to solve
     * race conditions. It first compares the current prepared package
     * with {@code oldPackage}. If they are the same, the prepared
     * package is revoked and replaced with {@code newPackage}. If
     * {@code oldPackage} is {@code null}, the comparison is omitted.
     * If {@code newPackage} is the same package or {@code null}, the
     * revocation is omitted. This method returns {@code true} if the
     * operation is succeeded.
     *
     * Legacy VPN is handled specially since it is not a real package.
     * It uses {@link VpnConfig#LEGACY_VPN} as its package name, and
     * it can be revoked by itself.
     *
     * @param oldPackage The package name of the old VPN application.
     * @param newPackage The package name of the new VPN application.
     * @return true if the operation is succeeded.
     */
    CARAPI Prepare(
        /* [in] */ const String& oldPackage,
        /* [in] */ const String& newPackage,
        /* [out] */ Boolean* result);

    /**
     * Set whether the current package has the ability to launch VPNs without user intervention.
     */
    CARAPI SetPackageAuthorization(
        /* [in] */ Boolean authorized);

    AutoPtr<INetworkInfo> GetNetworkInfo();

    /**
     * Return the configuration of the currently running VPN.
     */
    AutoPtr<IVpnConfig> GetVpnConfig();

    /**
     * Establish a VPN network and return the file descriptor of the VPN
     * interface. This methods returns {@code null} if the application is
     * revoked or not prepared.
     *
     * @param config The parameters to configure the network.
     * @return The file descriptor of the VPN interface.
     */
    CARAPI Establish(
        /* [in] */ IVpnConfig* config,
        /* [out] */ IParcelFileDescriptor** tun);

    CARAPI InterfaceStatusChanged(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    /**
     * Start legacy VPN, controlling native daemons as needed. Creates a
     * secondary thread to perform connection work, returning quickly.
     */
    CARAPI StartLegacyVpn(
        /* [in] */ IVpnProfile* socket,
        /* [in] */ IKeyStore* interfaze,
        /* [in] */ ILinkProperties* egress);

    CARAPI StopLegacyVpn();

    CARAPI GetLegacyVpnInfo(
        /* [out] */ ILegacyVpnInfo** info);

    CARAPI GetLegacyVpnConfig(
        /* [out] */ IVpnConfig** config);

    Boolean AddAddress(
        /* [in] */ const String& address,
        /* [in] */ Int32 prefixLength);

    Boolean RemoveAddress(
        /* [in] */ const String& address,
        /* [in] */ Int32 prefixLength);

private:
    /**
     * Update current state, dispaching event to listeners.
     */
    CARAPI_(void) UpdateState(
        /* [in] */ NetworkInfoDetailedState detailedState,
        /* [in] */ const String& reason);

    Boolean IsVpnUserPreConsented(
        /* [in] */ const String& packageName);

    Int32 GetAppUid(
        /* [in] */ const String& app,
        /* [in] */ Int32 userHandle);

    AutoPtr<ILinkProperties> MakeLinkProperties();

    CARAPI EnforceControlPermission();

    CARAPI AgentConnect();

    CARAPI AgentDisconnect(
        /* [in] */ INetworkInfo* networkInfo,
        /* [in] */ INetworkAgent* networkAgent);

    CARAPI AgentDisconnect(
        /* [in] */ INetworkAgent* networkAgent);

    CARAPI AgentDisconnect();

    Boolean IsRunningLocked();

    AutoPtr<ISortedSet> GetAppsUids(
        /* [in] */ IList* packageNames,
        /* [in] */ Int32 userHandle);

    // Note: This function adds to mVpnUsers but does not publish list to NetworkAgent.
    CARAPI AddVpnUserLocked(
        /* [in] */ Int32 userHandle);

    // Returns the subset of the full list of active UID ranges the VPN applies to (mVpnUsers) that
    // apply to userHandle.
    AutoPtr< ArrayOf<IUidRange*> > UidRangesForUser(
        /* [in] */ Int32 userHandle,
        /* [out] */ IList** list);

    CARAPI RemoveVpnUserLocked(
        /* [in] */ Int32 userHandle);

    CARAPI OnUserAdded(
        /* [in] */ Int32 userHandle);

    CARAPI OnUserRemoved(
        /* [in] */ Int32 userHandle);

    CARAPI PrepareStatusIntent();

    CARAPI NativeCreate(
        /* [in] */ Int32 mtu,
        /* [out] */ Int32* fd);

    CARAPI NativeGetName(
        /* [in] */ Int32 tun,
        /* [out] */ String* vpnName);

    CARAPI_(Int32) NativeSetAddresses(
        /* [in] */ const String& interfaze,
        /* [in] */ const String& addresses);

    CARAPI NativeReset(
        /* [in] */ const String& interfaze);

    CARAPI_(Int32) NativeCheck(
        /* [in] */ const String& interfaze);

    CARAPI_(Boolean) NativeAddAddress(
        /* [in] */ const String& interfaze,
        /* [in] */ const String& addresses,
        /* [in] */ Int32 prefixLength);

    CARAPI_(Boolean) NativeDelAddress(
        /* [in] */ const String& interfaze,
        /* [in] */ const String& addresses,
        /* [in] */ Int32 prefixLength);

    static CARAPI FindIPv4DefaultRoute(
        /* [in] */ ILinkProperties* prop,
        /* [out] */ IRouteInfo** routeInfo);

    CARAPI_(void) StartLegacyVpn(
        /* [in] */ IVpnConfig* config,
        /* [in] */ ArrayOf<String>* racoon,
        /* [in] */ ArrayOf<String>* mtpd);

private:
    friend class CVpnObserver;

    static const String NETWORKTYPE;
    static const String TAG;
    static const Boolean LOGD;

    AutoPtr<IContext> mContext;
    Int32 mOwnerUID;
    AutoPtr<INetworkInfo> mNetworkInfo;
    String mPackage;
    Int32 mHostUID;

    String mInterface;
    AutoPtr<Connection> mConnection;
    AutoPtr<LegacyVpnRunner> mLegacyVpnRunner;
    AutoPtr<IPendingIntent> mStatusIntent;

    volatile Boolean mEnableTeardown;
    AutoPtr<IIConnectivityManager> mConnService;
    AutoPtr<IINetworkManagementService> mNetd;
    AutoPtr<IVpnConfig> mConfig;
    AutoPtr<INetworkAgent> mNetworkAgent;
    AutoPtr<ILooper> mLooper;
    AutoPtr<INetworkCapabilities> mNetworkCapabilities;

    /* list of users using this VPN. */
    // @GuardedBy("this")
    AutoPtr<IList> mVpnUsers; //List<UidRange>
    AutoPtr<IBroadcastReceiver> mUserIntentReceiver;

    // Handle of user initiating VPN.
    Int32 mUserHandle;

    AutoPtr<IINetworkManagementEventObserver> mObserver;
    Object mLock;
};

} // Connectivity
} // Server
} // Droid
} // Elastos

#endif
