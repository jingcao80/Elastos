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

#ifndef __ELASTOS_DROID_SERVER_CNSDSERVICE_H__
#define __ELASTOS_DROID_SERVER_CNSDSERVICE_H__

#include "_Elastos_Droid_Server_CNsdService.h"
#include <elastos/droid/ext/frameworkext.h>
#define HASH_FOR_OS
#include <elastos/droid/ext/frameworkhash.h>
#include "elastos/droid/server/NativeDaemonConnector.h"
#include <Elastos.Droid.Utility.h>
#include <elastos/droid/R.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/internal/utility/AsyncChannel.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/internal/utility/StateMachine.h>
#include <elastos/droid/net/http/Request.h>
#include <elastos/droid/net/nsd/DnsSdTxtRecord.h>
#include <elastos/droid/net/nsd/NsdManager.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Process.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Net::Nsd::IDnsSdTxtRecord;
using Elastos::Droid::Net::Nsd::IINsdManager;
using Elastos::Droid::Net::Nsd::INsdManager;
using Elastos::Droid::Net::Nsd::INsdServiceInfo;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::Process;
using Elastos::Droid::R;
using Elastos::Droid::Server::INativeDaemonConnectorCallbacks;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Network Service Discovery Service handles remote service discovery operation requests by
 * implementing the INsdManager interface.
 *
 * @hide
 */
CarClass(CNsdService)
    , public Object
    , public IBinder
    , public IINsdManager
{
private:
    /* These should be in sync with system/netd/server/mDnsResponseCode.h */
    class NativeResponseCode
        : public Object
    {
    public:
        static const Int32 SERVICE_DISCOVERY_FAILED    =   602;
        static const Int32 SERVICE_FOUND               =   603;
        static const Int32 SERVICE_LOST                =   604;

        static const Int32 SERVICE_REGISTRATION_FAILED =   605;
        static const Int32 SERVICE_REGISTERED          =   606;

        static const Int32 SERVICE_RESOLUTION_FAILED   =   607;
        static const Int32 SERVICE_RESOLVED            =   608;

        static const Int32 SERVICE_UPDATED             =   609;
        static const Int32 SERVICE_UPDATE_FAILED       =   610;

        static const Int32 SERVICE_GET_ADDR_FAILED     =   611;
        static const Int32 SERVICE_GET_ADDR_SUCCESS    =   612;
    };

    class NativeEvent
        : public Object
    {
    public:
        NativeEvent(
            /* [in] */ Int32 code,
            /* [in] */ const String& raw,
            /* [in] */ ArrayOf<String>* cooked);

    public:
        Int32 mCode;
        String mRaw;
        AutoPtr<ArrayOf<String> > mCooked;
    };

    class NativeCallbackReceiver
        : public Object
        , public INativeDaemonConnectorCallbacks
    {
    public:
        NativeCallbackReceiver(
            /* [in] */ CNsdService* owner);

        CAR_INTERFACE_DECL()

        CARAPI OnDaemonConnected();

        CARAPI OnCheckHoldWakeLock(
            /* [in] */ Int32 code,
            /* [out] */ Boolean* result);

        CARAPI OnEvent(
            /* [in] */ Int32 code,
            /* [in] */ const String& raw,
            /* [in] */ ArrayOf<String>* cooked,
            /* [out] */ Boolean* result);
    public:
        CNsdService* mHost;
    };

    class ClientInfo;
    class NsdStateMachine
        : public StateMachine
    {
    public:
        class DefaultState
            : public State
        {
        public:
            DefaultState(
                /* [in] */ NsdStateMachine* owner);

            //@Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName();

        public:
            NsdStateMachine* mHost;
        };

        class DisabledState
            : public State
        {
        public:
            DisabledState(
                /* [in] */ NsdStateMachine* owner);

            //@Override
            CARAPI Enter();

            //@Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* result);

            CARAPI_(String) GetName();

        public:
            NsdStateMachine* mHost;
        };

        class EnabledState
            : public State
        {
        public:
            EnabledState(
                /* [in] */ NsdStateMachine* owner);

            //@Override
            CARAPI Enter();

            //@Override
            CARAPI Exit();

            //@Override
            CARAPI ProcessMessage(
                /* [in] */ IMessage* msg,
                /* [out] */ Boolean* value);

            CARAPI_(String) GetName();

        private:
            CARAPI_(Boolean) RequestLimitReached(
                /* [in] */ ClientInfo* clientInfo);

            CARAPI_(void) StoreRequestMap(
                /* [in] */ Int32 clientId,
                /* [in] */ Int32 globalId,
                /* [in] */ ClientInfo* clientInfo,
                /* [in] */ Int32 what);

            CARAPI_(void) RemoveRequestMap(
                /* [in] */ Int32 clientId,
                /* [in] */ Int32 globalId,
                /* [in] */ ClientInfo* clientInfo);

            CARAPI_(Boolean) HandleNativeEvent(
                /* [in] */ Int32 code,
                /* [in] */ const String& raw,
                /* [in] */ ArrayOf<String>* cooked);

        public:
            NsdStateMachine* mHost;
        };

        class MyContentObserver
            : public ContentObserver
        {
        public:
            TO_STRING_IMPL("NsdStateMachine::MyContentObserver")

            MyContentObserver();

            CARAPI constructor(
                /* [in] */ IHandler* handler,
                /* [in] */ NsdStateMachine* owner);

            CARAPI OnChange(
                /* [in] */ Boolean selfChange);
        public:
            NsdStateMachine* mHost;
        };

    protected:
        //@Override
        CARAPI_(String) GetWhatToString(
            /* [in] */ Int32 what);

    private:
        CARAPI_(void) RegisterForNsdSetting();

    public:
        NsdStateMachine(
            /* [in] */ const String& name,
            /* [in] */ CNsdService* owner);

    public:
        CNsdService* mHost;

    private:
        AutoPtr<DefaultState> mDefaultState;
        AutoPtr<DisabledState> mDisabledState;
        AutoPtr<EnabledState> mEnabledState;

    };

    class ClientInfo
        : public Object
    {
        friend class NsdStateMachine::DefaultState;
        friend class NsdStateMachine::EnabledState;

    public:
        ClientInfo(
            /* [in] */ CNsdService* host,
            /* [in] */ AsyncChannel* c,
            /* [in] */ IMessenger* m);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

    private:
        // Remove any pending requests from the global map when we get rid of a client,
        // and send cancellations to the daemon.
        CARAPI ExpungeAllRequests();

        // mClientIds is a sparse array of listener id -> mDnsClient id.  For a given mDnsClient id,
        // return the corresponding listener id.  mDnsClient id is also called a global id.
        CARAPI_(Int32) GetClientId(
            /* [in] */ Int32 globalId);

    public:
        static const Int32 MAX_LIMIT = 10;
        AutoPtr<AsyncChannel> mChannel;
        AutoPtr<IMessenger> mMessenger;
        /* Remembers a resolved service until getaddrinfo completes */
        AutoPtr<INsdServiceInfo> mResolvedService;

        /* A map from client id to unique id sent to mDns */
        AutoPtr<ISparseArray> mClientIds;

    private:
        /* A map from client id to the type of the request we had received */
        AutoPtr<ISparseArray> mClientRequests;

        CNsdService* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNsdService();

    ~CNsdService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    static CARAPI Create(
        /* [in] */ IContext* context,
        /* [out] */ CNsdService** nsdService);

    CARAPI GetMessenger(
        /* [out] */ IMessenger** messenger);

    CARAPI SetEnabled(
        /* [in] */ Boolean enable);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI_(String) CmdToString(
        /* [in] */ Int32 cmd);

    CARAPI_(void) SendNsdStateChangeBroadcast(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) IsNsdEnabled();

    CARAPI_(Int32) GetUniqueId();

    CARAPI_(Boolean) StartMDnsDaemon();

    CARAPI_(Boolean) StopMDnsDaemon();

    CARAPI_(Boolean) RegisterService(
        /* [in] */ Int32 regId,
        /* [in] */ INsdServiceInfo* service);

    CARAPI_(Boolean) UnregisterService(
        /* [in] */ Int32 regId);

    CARAPI_(Boolean) UpdateService(
        /* [in] */ Int32 regId,
        /* [in] */ IDnsSdTxtRecord* t);

    CARAPI_(Boolean) DiscoverServices(
        /* [in] */ Int32 discoveryId,
        /* [in] */ const String& serviceType);

    CARAPI_(Boolean) StopServiceDiscovery(
        /* [in] */ Int32 discoveryId);

    CARAPI_(Boolean) ResolveService(
        /* [in] */ Int32 resolveId,
        /* [in] */ INsdServiceInfo* service);

    CARAPI_(Boolean) StopResolveService(
        /* [in] */ Int32 resolveId);

    CARAPI_(Boolean) GetAddrInfo(
        /* [in] */ Int32 resolveId,
        /* [in] */ const String& hostname);

    CARAPI_(Boolean) StopGetAddrInfo(
        /* [in] */ Int32 resolveId);

    /* arg2 on the source message has an id that needs to be retained in replies
     * see NsdManager for details */
    CARAPI_(AutoPtr<IMessage>) ObtainMessage(
        /* [in] */ IMessage* srcMsg);

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

    CARAPI_(String) Unescape(
        /* [in] */ const String& s);

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitSCmdToString();

public:
    const static Int32 BASE = IProtocol::BASE_NSD_MANAGER;
    const static Int32 CMD_TO_STRING_COUNT = INsdManager::RESOLVE_SERVICE - BASE + 1;
    static AutoPtr<ArrayOf<String> > sCmdToString;

private:
    const static String TAG;
    const static String MDNS_TAG;
    const static Boolean DBG;

    AutoPtr<IContext> mContext;
    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<NsdStateMachine> mNsdStateMachine;

    /**
     * Clients receiving asynchronous messages
     */
    HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> > mClients;

    /* A map from unique id to client info */
    HashMap<Int32, AutoPtr<ClientInfo> > mIdToClientInfoMap;

    AutoPtr<AsyncChannel> mReplyChannel;

    Int32 INVALID_ID;
    Int32 mUniqueId;

    AutoPtr<NativeDaemonConnector> mNativeConnector;
    AutoPtr<ICountDownLatch> mNativeDaemonConnected;
};


} //Server
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_SERVER_CNSDSERVICE_H__
