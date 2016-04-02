
#ifndef __ELASTOS_DROID_SERVER_WIFI_RTTSERVICE_H__
#define __ELASTOS_DROID_SERVER_WIFI_RTTSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"

// package com.android.server.wifi;
// import android.content.BroadcastReceiver;
// import android.content.Context;
// import android.content.Intent;
// import android.content.IntentFilter;
// import android.net.wifi.RttManager;
// import android.net.wifi.WifiManager;
// import android.os.Bundle;
// import android.os.Handler;
// import android.os.HandlerThread;
// import android.os.Looper;
// import android.os.Message;
// import android.os.Messenger;
// import android.os.Parcel;
// import android.os.RemoteException;
// import android.util.Log;
// import android.net.wifi.IRttManager;
// import android.util.Slog;
// import com.android.internal.util.AsyncChannel;
// import com.android.internal.util.Protocol;
// import com.android.internal.util.StateMachine;
// import com.android.internal.util.State;
// import com.android.server.SystemService;
// import java.util.HashMap;
// import java.util.Iterator;
// import java.util.LinkedList;
// import java.util.Queue;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Com.android.internal.Utility.AsyncChannel;
using Elastos::Droid::Os::IMessenger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

class RttService
    : public Object
    , public ISystemService
{
public:
    class RttServiceImpl
        : public Object
        , public IRttManager::Stub
    {
    public:
        class RttStateMachine
            : public Object
            , public IStateMachine
        {
        public:
            class DefaultState
                : public Object
                , public IState
            {
            public:
                // @Override
                CARAPI_(Boolean) ProcessMessage(
                    /* [in] */ IMessage* msg);
            };

            class EnabledState
                : public Object
                , public IState
            {
            public:
                // @Override
                CARAPI_(Boolean) ProcessMessage(
                    /* [in] */ IMessage* msg);
            };

            class RequestPendingState
                : public Object
                , public IState
            {
            public:
                // @Override
                CARAPI_(Boolean) ProcessMessage(
                    /* [in] */ IMessage* msg);

            public:
                AutoPtr<RttRequest> mOutstandingRequest;
            };

        public:
            RttStateMachine(
                /* [in] */ ILooper* looper);

        public:
            AutoPtr<DefaultState> mDefaultState;
            AutoPtr<EnabledState> mEnabledState;
            AutoPtr<RequestPendingState> mRequestPendingState;
        };

    private:
        class ClientHandler
            : public Object
            , public IHandler
        {
        public:
            ClientHandler(
                /* [in] */  android);

            // @Override
            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);
        };

        class InnerBroadcastReceiver1
            : public Object
            , public IBroadcastReceiver
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

        class RttRequest
        {
        public:
            AutoPtr<Integer> key;
            AutoPtr<ClientInfo> ci;
            AutoPtr<IRttManager> AutoPtr<::RttParams[]> params;
        };

        class ClientInfo
            : public Object
        {
        public:
            ClientInfo(
                /* [in] */ IAsyncChannel* c,
                /* [in] */ IMessenger* m);

            virtual CARAPI AddRttRequest(
                /* [in] */ Int32 key,
                /* [in] */  RttManager,
                /* [out] */ Boolean* result);

            virtual CARAPI RemoveRttRequest(
                /* [in] */ Int32 key);

            virtual CARAPI ReportResult(
                /* [in] */ RttRequest* request,
                /* [in] */  RttManager);

            virtual CARAPI ReportFailed(
                /* [in] */ RttRequest* request,
                /* [in] */ Int32 reason,
                /* [in] */ String description);

            virtual CARAPI ReportFailed(
                /* [in] */ Int32 key,
                /* [in] */ Int32 reason,
                /* [in] */ String description);

            virtual CARAPI ReportAborted(
                /* [in] */ Int32 key);

            virtual CARAPI Cleanup();

        public:
            AutoPtr< IHashMap<Integer, RttRequest> > mRequests;

        private:
            /*const*/ AutoPtr<IAsyncChannel> mChannel;
            /*const*/ AutoPtr<IMessenger> mMessenger;
        };

    public:
        RttServiceImpl();

        RttServiceImpl(
            /* [in] */ IContext* context);

        // @Override
        CARAPI_(AutoPtr<IMessenger>) GetMessenger();

        virtual CARAPI StartService(
            /* [in] */ IContext* context);

        virtual CARAPI ReplySucceeded(
            /* [in] */ IMessage* msg,
            /* [in] */ Object* obj);

        virtual CARAPI ReplyFailed(
            /* [in] */ IMessage* msg,
            /* [in] */ Int32 reason,
            /* [in] */ String description);

        virtual CARAPI IssueNextRequest(
            /* [out] */ RttRequest** result);

    private:
        AutoPtr<IContext> mContext;
        AutoPtr<RttStateMachine> mStateMachine;
        AutoPtr<ClientHandler> mClientHandler;
        AutoPtr< IQueue<RttRequest> > mRequestQueue;
        AutoPtr< IHashMap<IMessenger, ClientInfo> > mClients;
        static const Int32 BASE = Protocol.BASE_WIFI_RTT_SERVICE;
        static const Int32 CMD_DRIVER_LOADED = BASE + 0;
        static const Int32 CMD_DRIVER_UNLOADED = BASE + 1;
        static const Int32 CMD_ISSUE_NEXT_REQUEST = BASE + 2;
        static const Int32 CMD_RTT_RESPONSE = BASE + 3;
        AutoPtr<WifiNative::RttEventHandler> mEventHandler;
    };

private:
    class InnerWifiNativeRttEventHandler
        : public Object
        , public WifiNative::RttEventHandler
    {
    public:
        InnerWifiNativeRttEventHandler(
            /* [in] */ RttServiceImpl* owner);

        // @Override
        CARAPI OnRttResults(
            /* [in] */  RttManager);

    private:
        RttServiceImpl* mOwner;
    };

public:
    RttService(
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

