
#ifndef __ELASTOS_DROID_SERVER_WIFI_RTTSERVICE_H__
#define __ELASTOS_DROID_SERVER_WIFI_RTTSERVICE_H__

#include "Elastos.Droid.Wifi.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/internal/utility/StateMachine.h"
#include "elastos/droid/internal/utility/State.h"
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/wifi/WifiNative.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Server::Wifi::WifiNative;
using Elastos::Droid::Wifi::IIRttManager;
using Elastos::Droid::Wifi::IRttManagerParcelableRttParams;
using Elastos::Droid::Wifi::IRttManagerRttParams;
using Elastos::Droid::Wifi::IRttManagerRttResult;
using Elastos::Core::IInteger32;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IQueue;
using Elastos::Utility::ILinkedList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class RttService
    : public SystemService
{
    friend class CRttServiceImpl;
public:
    class RttServiceImpl
        : public Object
        , public IIRttManager
        , public IBinder
    {
    public:
        class ClientInfo;

        class RttRequest : public Object
        {
        public:
            AutoPtr<IInteger32> mKey;
            ClientInfo* mCi;//avoid crossing-reference
            AutoPtr<ArrayOf<IRttManagerRttParams*> > mParams;
        };

        class ClientInfo : public Object
        {
        public:
            ClientInfo(
                /* [in] */ IAsyncChannel* c,
                /* [in] */ IMessenger* m,
                /* [in] */ RttServiceImpl* host);

            virtual CARAPI AddRttRequest(
                /* [in] */ Int32 key,
                /* [in] */ IRttManagerParcelableRttParams* parcelableParams,
                /* [out] */ Boolean* result);

            virtual CARAPI RemoveRttRequest(
                /* [in] */ Int32 key);

            virtual CARAPI ReportResult(
                /* [in] */ RttRequest* request,
                /* [in] */ ArrayOf<IRttManagerRttResult*>* results);

            virtual CARAPI ReportFailed(
                /* [in] */ RttRequest* request,
                /* [in] */ Int32 reason,
                /* [in] */ const String& description);

            virtual CARAPI ReportFailed(
                /* [in] */ Int32 key,
                /* [in] */ Int32 reason,
                /* [in] */ String description);

            virtual CARAPI ReportAborted(
                /* [in] */ Int32 key);

            virtual CARAPI Cleanup();

        public:
            AutoPtr<IHashMap> mRequests;//<Integer, RttRequest>
            RttServiceImpl* mHost;

        private:
            /*const*/ AutoPtr<IAsyncChannel> mChannel;
            /*const*/ AutoPtr<IMessenger> mMessenger;
        };

        class RttStateMachine
            : public StateMachine
        {
        public:
            class DefaultState
                : public State
            {
            public:
                DefaultState(
                    /* [in] */ RttStateMachine* host);

                // @Override
                CARAPI ProcessMessage(
                    /* [in] */ IMessage* msg,
                    /* [out] */ Boolean* result);

                CARAPI_(String) GetName();

            public:
                RttStateMachine* mHost;
            };

            class EnabledState
                : public State
            {
            public:
                EnabledState(
                    /* [in] */ RttStateMachine* host);

                // @Override
                CARAPI ProcessMessage(
                    /* [in] */ IMessage* msg,
                    /* [out] */ Boolean* result);

                CARAPI_(String) GetName();

            public:
                RttStateMachine* mHost;
            };

            class RequestPendingState
                : public State
            {
            public:
                RequestPendingState(
                    /* [in] */ RttStateMachine* host);

                // @Override
                CARAPI ProcessMessage(
                    /* [in] */ IMessage* msg,
                    /* [out] */ Boolean* result);

                CARAPI_(String) GetName();

            public:
                AutoPtr<RttRequest> mOutstandingRequest;
                RttStateMachine* mHost;
            };

        public:
            RttStateMachine(
                /* [in] */ ILooper* looper,
                /* [in] */ RttServiceImpl* host);

        public:
            AutoPtr<DefaultState> mDefaultState;
            AutoPtr<EnabledState> mEnabledState;
            AutoPtr<RequestPendingState> mRequestPendingState;
            RttServiceImpl* mHost;
        };

    private:
        class ClientHandler
            : public Handler
        {
        public:
            TO_STRING_IMPL("RttService::ClientHandler")

            ClientHandler(
                /* [in] */ ILooper* looper,
                /* [in] */ RttServiceImpl* host);

            // @Override
            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        public:
            RttServiceImpl* mHost;
        };

        class InnerBroadcastReceiver1
            : public BroadcastReceiver
        {
        public:
            InnerBroadcastReceiver1(
                /* [in] */ RttServiceImpl* owner);

            // @Override
            CARAPI OnReceive(
                /* [in] */ IContext* context,
                /* [in] */ IIntent* intent);

        private:
            RttServiceImpl* mOwner;
        };

    public:
        CAR_INTERFACE_DECL();

        RttServiceImpl();

        CARAPI constructor(
            /* [in] */ IContext* context);

        // @Override
        CARAPI GetMessenger(
            /* [out] */ IMessenger** messenger);

        virtual CARAPI StartService(
            /* [in] */ IContext* context);

        virtual CARAPI ReplySucceeded(
            /* [in] */ IMessage* msg,
            /* [in] */ IObject* obj);

        virtual CARAPI ReplyFailed(
            /* [in] */ IMessage* msg,
            /* [in] */ Int32 reason,
            /* [in] */ const String& description);

        virtual CARAPI IssueNextRequest(
            /* [out] */ RttRequest** result);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info)
                return Object::ToString(info);
        }

    public:
        AutoPtr<IContext> mContext;
        AutoPtr<RttStateMachine> mStateMachine;
        AutoPtr<ClientHandler> mClientHandler;
        AutoPtr<IQueue> mRequestQueue;//RttRequest
        AutoPtr<IHashMap> mClients;//<IMessenger, ClientInfo>
        static const Int32 BASE = IProtocol::BASE_WIFI_RTT_SERVICE;
        static const Int32 CMD_DRIVER_LOADED = BASE + 0;
        static const Int32 CMD_DRIVER_UNLOADED = BASE + 1;
        static const Int32 CMD_ISSUE_NEXT_REQUEST = BASE + 2;
        static const Int32 CMD_RTT_RESPONSE = BASE + 3;
        AutoPtr<IWifiNativeRttEventHandler> mEventHandler;
    };

private:
    class InnerWifiNativeRttEventHandler
        : public Object
        , public IWifiNativeRttEventHandler
    {
    public:
        CAR_INTERFACE_DECL();

        InnerWifiNativeRttEventHandler(
            /* [in] */ RttServiceImpl* owner);

        // @Override
        CARAPI OnRttResults(
            /* [in] */ ArrayOf<IRttManagerRttResult*>* result);

    private:
        RttServiceImpl* mOwner;
    };

public:
    RttService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

public:
    static const Boolean DBG;
    AutoPtr<RttServiceImpl> mImpl;

private:
    static const String TAG;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_RTTSERVICE_H__

