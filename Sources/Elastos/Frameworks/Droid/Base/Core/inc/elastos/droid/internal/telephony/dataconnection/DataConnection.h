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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DATACONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DATACONNECTION_H__

#include "_Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.App.h>
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/internal/utility/StateMachine.h>
#include <elastos/droid/net/NetworkAgent.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Process.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IRetryManager;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkAgent;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::NetworkAgent;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Process;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * {@hide}
 *
 * DataConnection StateMachine.
 *
 * This a class for representing a single data connection, with instances of this
 * class representing a connection via the cellular network. There may be multiple
 * data connections and all of them are managed by the <code>DataConnectionTracker</code>.
 *
 * A recent change is to move retry handling into this class, with that change the
 * old retry manager is now used internally rather than exposed to the DCT. Also,
 * bringUp now has an initialRetry which is used limit the number of retries
 * during the initial bring up of the connection. After the connection becomes active
 * the current max retry is restored to the configured value.
 *
 * NOTE: All DataConnection objects must be running on the same looper, which is the default
 * as the coordinator has members which are used without synchronization.
 */
class DataConnection
    : public StateMachine
    , public IDataConnection
{
public:
    /**
     * Used internally for saving connecting parameters.
     */
    class ConnectionParams
        : public Object
    {
    public:
        CARAPI constructor(
            /* [in] */ IApnContext* apnContext,
            /* [in] */ Int32 initialMaxRetry,
            /* [in] */ Int32 profileId,
            /* [in] */ Int32 rilRadioTechnology,
            /* [in] */ Boolean retryWhenSSChange,
            /* [in] */ IMessage* onCompletedMsg);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

        Int32 mTag;

        AutoPtr<IApnContext> mApnContext;

        Int32 mInitialMaxRetry;

        Int32 mProfileId;

        Int32 mRilRat;

        Boolean mRetryWhenSSChange;

        AutoPtr<IMessage> mOnCompletedMsg;
    };

    /**
     * Used internally for saving disconnecting parameters.
     */
    class DisconnectParams
        : public Object
    {
    public:
        CARAPI constructor(
            /* [in] */ IApnContext* apnContext,
            /* [in] */ const String& reason,
            /* [in] */ IMessage* onCompletedMsg);

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

        Int32 mTag;

        AutoPtr<IApnContext> mApnContext;

        String mReason;

        AutoPtr<IMessage> mOnCompletedMsg;
    };

    class UpdateLinkPropertyResult
        : public Object
    {
    public:
        UpdateLinkPropertyResult(
            /* [in] */ ILinkProperties* curLp);

    public:
        AutoPtr<IDataCallResponseSetupResult> mSetupResult;

        AutoPtr<ILinkProperties> mOldLp;

        AutoPtr<ILinkProperties> mNewLp;
    };

private:
    /**
     * The parent state for all other states.
     */
    class DcDefaultState
        : public State
    {
    public:
        DcDefaultState(
            /* [in] */ DataConnection* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

         CARAPI_(String) GetName();

    private:
        DataConnection* mHost;
    };

    /**
     * The state machine is inactive and expects a EVENT_CONNECT.
     */
    class DcInactiveState
        : public State
    {
    public:
        DcInactiveState(
            /* [in] */ DataConnection* host);

        // Inform all contexts we've failed connecting
        CARAPI SetEnterNotificationParams(
            /* [in] */ ConnectionParams* cp,
            /* [in] */ IDcFailCause* cause);

        // Inform all contexts we've failed disconnected
        CARAPI SetEnterNotificationParams(
            /* [in] */ DisconnectParams* dp);

        // Inform all contexts of the failure cause
        CARAPI SetEnterNotificationParams(
            /* [in] */ IDcFailCause* cause);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DataConnection* mHost;
    };

    /**
     * The state machine is retrying and expects a EVENT_RETRY_CONNECTION.
     */
    class DcRetryingState
        : public State
    {
    public:
        DcRetryingState(
            /* [in] */ DataConnection* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DataConnection* mHost;
    };

    /**
     * The state machine is activating a connection.
     */
    class DcActivatingState
        : public State
    {
    public:
        DcActivatingState(
            /* [in] */ DataConnection* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DataConnection* mHost;
    };

    /**
     * The state machine is connected, expecting an EVENT_DISCONNECT.
     */
    class DcActiveState
        : public State
    {
    public:
        DcActiveState(
            /* [in] */ DataConnection* host);

        // @Override
        CARAPI Enter();

        // @Override
        CARAPI Exit();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DataConnection* mHost;
    };

    /**
     * The state machine is disconnecting.
     */
    class DcDisconnectingState
        : public State
    {
    public:
        DcDisconnectingState(
            /* [in] */ DataConnection* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DataConnection* mHost;
    };

    /**
     * The state machine is disconnecting after an creating a connection.
     */
    class DcDisconnectionErrorCreatingConnection
        : public State
    {
    public:
        DcDisconnectionErrorCreatingConnection(
            /* [in] */ DataConnection* host);

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        DataConnection* mHost;
    };

    class DcNetworkAgent
        : public NetworkAgent
    {
    public:
        TO_STRING_IMPL("DataConnection::DcNetworkAgent")

        DcNetworkAgent(
            /* [in] */ DataConnection* host);

        CARAPI constructor(
            /* [in] */ ILooper* l,
            /* [in] */ IContext* c,
            /* [in] */ const String& TAG,
            /* [in] */ INetworkInfo* ni,
            /* [in] */ INetworkCapabilities* nc,
            /* [in] */ ILinkProperties* lp,
            /* [in] */ Int32 score);

        CARAPI Unwanted();

    private:
        DataConnection* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    //***** Constructor (NOTE: uses dcc.getHandler() as its Handler)
    DataConnection();

    // Convert cmd to string or null if unknown
    static CARAPI CmdToString(
        /* [in] */ Int32 cmd,
        /* [out] */ String* result);

    /**
     * Create the connection object
     *
     * @param phone the Phone
     * @param id the connection id
     * @return DataConnection that was created.
     */
    static CARAPI MakeDataConnection(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ Int32 id,
        /* [in] */ IDcTrackerBase* dct,
        /* [in] */ IDcTesterFailBringUpAll* failBringUpAll,
        /* [in] */ IDcController* dcc,
        /* [out] */ IDataConnection** result);

    CARAPI Dispose();

    /* Getter functions */
    CARAPI GetCopyNetworkCapabilities(
        /* [out] */ INetworkCapabilities** result);

    CARAPI GetCopyLinkProperties(
        /* [out] */ ILinkProperties** result);

    CARAPI GetIsInactive(
        /* [out] */ Boolean* result);

    CARAPI GetCid(
        /* [out] */ Int32* result);

    CARAPI GetApnSetting(
        /* [out] */ IApnSetting** result);

    CARAPI SetLinkPropertiesHttpProxy(
        /* [in] */ IProxyInfo* proxy);

    CARAPI IsIpv4Connected(
        /* [out] */ Boolean* result);

    CARAPI IsIpv6Connected(
        /* [out] */ Boolean* result);

    CARAPI UpdateLinkProperty(
        /* [in] */ IDataCallResponse* newState,
        /* [out] */ UpdateLinkPropertyResult** result);

    /**
     * Get the DataConnection ID
     */
    CARAPI GetDataConnectionId(
        /* [out] */ Int32* result);

    /* package */
    CARAPI TearDownNow();

    /**
     * @return the string for msg.what as our info.
     */
    // @Override
    CARAPI_(String) GetWhatToString(
        /* [in] */ Int32 what);

    static CARAPI Slog(
        /* [in] */ const char *fmt, ...);

    /**
     * Log with debug
     *
     * @param s is string log
     */
    // @Override
    CARAPI Log(
        /* [in] */ const char *fmt, ...);

    /**
     * Log with debug attribute
     *
     * @param s is string log
     */
    // @Override
    CARAPI Logd(
        /* [in] */ const char *fmt, ...);

    /**
     * Log with verbose attribute
     *
     * @param s is string log
     */
    // @Override
    CARAPI Logv(
        /* [in] */ const char *fmt, ...);

    /**
     * Log with info attribute
     *
     * @param s is string log
     */
    // @Override
    CARAPI Logi(
        /* [in] */ const char *fmt, ...);

    /**
     * Log with warning attribute
     *
     * @param s is string log
     */
    // @Override
    CARAPI Logw(
        /* [in] */ const char *fmt, ...);

    /**
     * Log with error attribute
     *
     * @param s is string log
     */
    // @Override
    CARAPI Loge(
        /* [in] */ const char *fmt, ...);

    /**
     * Log with error attribute
     *
     * @param s is string log
     * @param e is a Throwable which logs additional information.
     */
    // @Override
    // CARAPI Loge(
    //     /* [in] */ const String& s,
    //     /* [in] */ IThrowable* e);

    /** Doesn't print mApnList of ApnContext's which would be recursive */
    CARAPI ToStringSimple(
        /* [out] */ String* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Dump the current state.
     *
     * @param fd
     * @param pw
     * @param args
     */
    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    /**
     * Read the MTU value from link properties where it can be set from network. In case
     * not set by the network, set it again using the mtu szie value defined in the APN
     * database for the connected APN
     */
    CARAPI CheckSetMtu(
        /* [in] */ IApnSetting* apn,
        /* [in] */ ILinkProperties* lp);

    CARAPI constructor(
        /* [in] */ IPhoneBase* phone,
        /* [in] */ const String& name,
        /* [in] */ Int32 id,
        /* [in] */ IDcTrackerBase* dct,
        /* [in] */ IDcTesterFailBringUpAll* failBringUpAll,
        /* [in] */ IDcController* dcc);

    CARAPI GetRetryConfig(
        /* [in] */ Boolean forDefault,
        /* [out] */ String* result);

    CARAPI ConfigureRetry(
        /* [in] */ Boolean forDefault);

    /**
     * Begin setting up a data connection, calls setupDataCall
     * and the ConnectionParams will be returned with the
     * EVENT_SETUP_DATA_CONNECTION_DONE AsyncResul.userObj.
     *
     * @param cp is the connection parameters
     */
    CARAPI OnConnect(
        /* [in] */ ConnectionParams* cp);

    /**
     * Get the technology that will be used to setup the data connection.
     *
     * @param radioTechnology is the ril radio technology.
     */
    CARAPI GetDataTechnology(
        /* [in] */ Int32 radioTechnology,
        /* [out] */ String* result);

    /**
     * TearDown the data connection when the deactivation is complete a Message with
     * msg.what == EVENT_DEACTIVATE_DONE and msg.obj == AsyncResult with AsyncResult.obj
     * containing the parameter o.
     *
     * @param o is the object returned in the AsyncResult.obj.
     */
    CARAPI TearDownData(
        /* [in] */ IInterface* o);

    CARAPI NotifyAllWithEvent(
        /* [in] */ IApnContext* alreadySent,
        /* [in] */ Int32 event,
        /* [in] */ const String& reason);

    CARAPI NotifyAllOfConnected(
        /* [in] */ const String& reason);

    CARAPI NotifyAllOfDisconnectDcRetrying(
        /* [in] */ const String& reason);

    CARAPI NotifyAllDisconnectCompleted(
        /* [in] */ IDcFailCause* cause);

    /**
     * Send the connectionCompletedMsg.
     *
     * @param cp is the ConnectionParams
     * @param cause and if no error the cause is DcFailCause.NONE
     * @param sendAll is true if all contexts are to be notified
     */
    CARAPI NotifyConnectCompleted(
        /* [in] */ ConnectionParams* cp,
        /* [in] */ IDcFailCause* cause,
        /* [in] */ Boolean sendAll);

    /**
     * Send ar.userObj if its a message, which is should be back to originator.
     *
     * @param dp is the DisconnectParams.
     */
    CARAPI NotifyDisconnectCompleted(
        /* [in] */ DisconnectParams* dp,
        /* [in] */ Boolean sendAll);

    /**
     * Clear all settings called when entering mInactiveState.
     */
    CARAPI ClearSettings();

    /**
     * Process setup completion.
     *
     * @param ar is the result
     * @return SetupResult.
     */
    CARAPI OnSetupConnectionCompleted(
        /* [in] */ IAsyncResult* ar,
        /* [out] */ IDataCallResponseSetupResult** result);

    CARAPI IsDnsOk(
        /* [in] */ ArrayOf<String>* domainNameServers,
        /* [out] */ Boolean* result);

    CARAPI UpdateTcpBufferSizes(
        /* [in] */ Int32 rilRat);

    CARAPI MakeNetworkCapabilities(
        /* [out] */ INetworkCapabilities** result);

    CARAPI IsIpAddress(
        /* [in] */ const String& address,
        /* [out] */ Boolean* result);

    CARAPI SetLinkProperties(
        /* [in] */ IDataCallResponse* response,
        /* [in] */ ILinkProperties* lp,
        /* [out] */ IDataCallResponseSetupResult** result);

    /**
     * Initialize connection, this will fail if the
     * apnSettings are not compatible.
     *
     * @param cp the Connection paramemters
     * @return true if initialization was successful.
     */
    CARAPI InitConnection(
        /* [in] */ ConnectionParams* cp,
        /* [out] */ Boolean* result);

    static CARAPI MsgToString(
        /* [in] */ IMessage* msg,
        /* [out] */ String* result);

    static CARAPI_(AutoPtr<IAtomicInteger32>) InitInstanceNumber();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitCmdToString();

public:
    AutoPtr<ArrayOf<String> > mPcscfAddr;

    //***** Package visible variables
    Int32 mTag;

    Int32 mCid;

    AutoPtr<IList> mApnContexts;

    AutoPtr<IPendingIntent> mReconnectIntent;

    AutoPtr<IRetryManager> mRetryManager;

    // ***** Event codes for driving the state machine, package visible for Dcc
    static const Int32 BASE;

    static const Int32 EVENT_CONNECT;

    static const Int32 EVENT_SETUP_DATA_CONNECTION_DONE;

    static const Int32 EVENT_GET_LAST_FAIL_DONE;

    static const Int32 EVENT_DEACTIVATE_DONE;

    static const Int32 EVENT_DISCONNECT;

    static const Int32 EVENT_RIL_CONNECTED;

    static const Int32 EVENT_DISCONNECT_ALL;

    static const Int32 EVENT_DATA_STATE_CHANGED;

    static const Int32 EVENT_TEAR_DOWN_NOW;

    static const Int32 EVENT_LOST_CONNECTION;

    static const Int32 EVENT_RETRY_CONNECTION;

    static const Int32 EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED;

    static const Int32 EVENT_DATA_CONNECTION_ROAM_ON;

    static const Int32 EVENT_DATA_CONNECTION_ROAM_OFF;

private:
    static const Boolean DBG;

    static const Boolean VDBG;

    /** Retry configuration: A doubling of retry times from 5secs to 30minutes */
    static const String DEFAULT_DATA_RETRY_CONFIG;

    /** Retry configuration for secondary networks: 4 tries in 20 sec */
    static const String SECONDARY_DATA_RETRY_CONFIG;

    static const String NETWORK_TYPE;

    // The data connection controller
    AutoPtr<IDcController> mDcController;

    // The Tester for failing all bringup's
    AutoPtr<IDcTesterFailBringUpAll> mDcTesterFailBringUpAll;

    static AutoPtr<IAtomicInteger32> mInstanceNumber;

    AutoPtr<IAsyncChannel> mAc;

    // Utilities for the DataConnection
    AutoPtr<IDcRetryAlarmController> mDcRetryAlarmController;

    // The DCT that's talking to us, we only support one!
    AutoPtr<IDcTrackerBase> mDct;

    AutoPtr<IApnSetting> mApnSetting;

    AutoPtr<ConnectionParams> mConnectionParams;

    AutoPtr<DisconnectParams> mDisconnectParams;

    AutoPtr<IDcFailCause> mDcFailCause;

    AutoPtr<IPhoneBase> mPhone;

    AutoPtr<ILinkProperties> mLinkProperties;

    Int64 mCreateTime;

    Int64 mLastFailTime;

    AutoPtr<IDcFailCause> mLastFailCause;

    static const String NULL_IP;

    AutoPtr<IObject> mUserData;

    Int32 mRilRat;

    Int32 mDataRegState;

    AutoPtr<INetworkInfo> mNetworkInfo;

    AutoPtr<INetworkAgent> mNetworkAgent;

    static const Int32 CMD_TO_STRING_COUNT;

    static AutoPtr<ArrayOf<String> > sCmdToString;

    /*
     * The id is owned by DataConnectionTracker.
     */
    Int32 mId;

    static const String TCP_BUFFER_SIZES_GPRS;

    static const String TCP_BUFFER_SIZES_EDGE;

    static const String TCP_BUFFER_SIZES_UMTS;

    static const String TCP_BUFFER_SIZES_1XRTT;

    static const String TCP_BUFFER_SIZES_EVDO;

    static const String TCP_BUFFER_SIZES_EHRPD;

    static const String TCP_BUFFER_SIZES_HSDPA;

    static const String TCP_BUFFER_SIZES_HSPA;

    static const String TCP_BUFFER_SIZES_LTE;

    static const String TCP_BUFFER_SIZES_HSPAP;

    AutoPtr<DcDefaultState> mDefaultState;

    AutoPtr<DcInactiveState> mInactiveState;

    AutoPtr<DcRetryingState> mRetryingState;

    AutoPtr<DcActivatingState> mActivatingState;

    AutoPtr<DcActiveState> mActiveState;

    AutoPtr<DcDisconnectingState> mDisconnectingState;

    AutoPtr<DcDisconnectionErrorCreatingConnection> mDisconnectingErrorCreatingConnection;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DATACONNECTION_H__
