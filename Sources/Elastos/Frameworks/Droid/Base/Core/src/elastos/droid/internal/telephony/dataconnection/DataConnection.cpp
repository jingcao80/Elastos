/*
 * Copyright (C) 2006 The Android Open Source Project
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * Not a Contribution.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony.dataconnection;


using Elastos::Droid::Internal::Telephony::Dataconnection::ApnSetting::IApnProfileType;
using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IRILConstants;
using Elastos::Droid::Internal::Telephony::IRetryManager;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::IStateMachine;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkAgent;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Utility::IPatterns;
using Elastos::Droid::Utility::ITimeUtils;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;

using Elastos::Net::IInetAddress;
using Elastos::Utility::ICollection;

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
public class DataConnection extends StateMachine {
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = TRUE;

    /** Retry configuration: A doubling of retry times from 5secs to 30minutes */
    private static const String DEFAULT_DATA_RETRY_CONFIG = "default_randomization=2000,"
        + "5000,10000,20000,40000,80000:5000,160000:5000,"
        + "320000:5000,640000:5000,1280000:5000,1800000:5000";

    /** Retry configuration for secondary networks: 4 tries in 20 sec */
    private static const String SECONDARY_DATA_RETRY_CONFIG =
            "max_retries=3, 5000, 5000, 5000";

    private static const String NETWORK_TYPE = "MOBILE";

    // The data connection controller
    private DcController mDcController;

    // The Tester for failing all bringup's
    private DcTesterFailBringUpAll mDcTesterFailBringUpAll;

    private static AtomicInteger mInstanceNumber = new AtomicInteger(0);
    private AsyncChannel mAc;

    // Utilities for the DataConnection
    private DcRetryAlarmController mDcRetryAlarmController;

    // The DCT that's talking to us, we only support one!
    private DcTrackerBase mDct = NULL;

    protected String[] mPcscfAddr;

    /**
     * Used internally for saving connecting parameters.
     */
    static class ConnectionParams {
        Int32 mTag;
        ApnContext mApnContext;
        Int32 mInitialMaxRetry;
        Int32 mProfileId;
        Int32 mRilRat;
        Boolean mRetryWhenSSChange;
        Message mOnCompletedMsg;

        ConnectionParams(ApnContext apnContext, Int32 initialMaxRetry, Int32 profileId,
                Int32 rilRadioTechnology, Boolean retryWhenSSChange, Message onCompletedMsg) {
            mApnContext = apnContext;
            mInitialMaxRetry = initialMaxRetry;
            mProfileId = profileId;
            mRilRat = rilRadioTechnology;
            mRetryWhenSSChange = retryWhenSSChange;
            mOnCompletedMsg = onCompletedMsg;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "{mTag=" + mTag + " mApnContext=" + mApnContext
                    + " mInitialMaxRetry=" + mInitialMaxRetry + " mProfileId=" + mProfileId
                    + " mRat=" + mRilRat
                    + " mOnCompletedMsg=" + MsgToString(mOnCompletedMsg) + "}";
        }
    }

    /**
     * Used internally for saving disconnecting parameters.
     */
    static class DisconnectParams {
        Int32 mTag;
        ApnContext mApnContext;
        String mReason;
        Message mOnCompletedMsg;

        DisconnectParams(ApnContext apnContext, String reason, Message onCompletedMsg) {
            mApnContext = apnContext;
            mReason = reason;
            mOnCompletedMsg = onCompletedMsg;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "{mTag=" + mTag + " mApnContext=" + mApnContext
                    + " mReason=" + mReason
                    + " mOnCompletedMsg=" + MsgToString(mOnCompletedMsg) + "}";
        }
    }

    private ApnSetting mApnSetting;
    private ConnectionParams mConnectionParams;
    private DisconnectParams mDisconnectParams;
    private DcFailCause mDcFailCause;

    private PhoneBase mPhone;
    private LinkProperties mLinkProperties = new LinkProperties();
    private Int64 mCreateTime;
    private Int64 mLastFailTime;
    private DcFailCause mLastFailCause;
    private static const String NULL_IP = "0.0.0.0";
    private Object mUserData;
    private Int32 mRilRat = Integer.MAX_VALUE;
    private Int32 mDataRegState = Integer.MAX_VALUE;
    private NetworkInfo mNetworkInfo;
    private NetworkAgent mNetworkAgent;

    //***** Package visible variables
    Int32 mTag;
    Int32 mCid;
    List<ApnContext> mApnContexts = NULL;
    PendingIntent mReconnectIntent = NULL;
    RetryManager mRetryManager = new RetryManager();


    // ***** Event codes for driving the state machine, package visible for Dcc
    static const Int32 BASE = Protocol.BASE_DATA_CONNECTION;
    static const Int32 EVENT_CONNECT = BASE + 0;
    static const Int32 EVENT_SETUP_DATA_CONNECTION_DONE = BASE + 1;
    static const Int32 EVENT_GET_LAST_FAIL_DONE = BASE + 2;
    static const Int32 EVENT_DEACTIVATE_DONE = BASE + 3;
    static const Int32 EVENT_DISCONNECT = BASE + 4;
    static const Int32 EVENT_RIL_CONNECTED = BASE + 5;
    static const Int32 EVENT_DISCONNECT_ALL = BASE + 6;
    static const Int32 EVENT_DATA_STATE_CHANGED = BASE + 7;
    static const Int32 EVENT_TEAR_DOWN_NOW = BASE + 8;
    static const Int32 EVENT_LOST_CONNECTION = BASE + 9;
    static const Int32 EVENT_RETRY_CONNECTION = BASE + 10;
    static const Int32 EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED = BASE + 11;
    static const Int32 EVENT_DATA_CONNECTION_ROAM_ON = BASE + 12;
    static const Int32 EVENT_DATA_CONNECTION_ROAM_OFF = BASE + 13;

    private static const Int32 CMD_TO_STRING_COUNT = EVENT_DATA_CONNECTION_ROAM_OFF - BASE + 1;
    private static String[] sCmdToString = new String[CMD_TO_STRING_COUNT];
    static {
        sCmdToString[EVENT_CONNECT - BASE] = "EVENT_CONNECT";
        sCmdToString[EVENT_SETUP_DATA_CONNECTION_DONE - BASE] =
                "EVENT_SETUP_DATA_CONNECTION_DONE";
        sCmdToString[EVENT_GET_LAST_FAIL_DONE - BASE] = "EVENT_GET_LAST_FAIL_DONE";
        sCmdToString[EVENT_DEACTIVATE_DONE - BASE] = "EVENT_DEACTIVATE_DONE";
        sCmdToString[EVENT_DISCONNECT - BASE] = "EVENT_DISCONNECT";
        sCmdToString[EVENT_RIL_CONNECTED - BASE] = "EVENT_RIL_CONNECTED";
        sCmdToString[EVENT_DISCONNECT_ALL - BASE] = "EVENT_DISCONNECT_ALL";
        sCmdToString[EVENT_DATA_STATE_CHANGED - BASE] = "EVENT_DATA_STATE_CHANGED";
        sCmdToString[EVENT_TEAR_DOWN_NOW - BASE] = "EVENT_TEAR_DOWN_NOW";
        sCmdToString[EVENT_LOST_CONNECTION - BASE] = "EVENT_LOST_CONNECTION";
        sCmdToString[EVENT_RETRY_CONNECTION - BASE] = "EVENT_RETRY_CONNECTION";
        sCmdToString[EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED - BASE] =
                "EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED";
        sCmdToString[EVENT_DATA_CONNECTION_ROAM_ON - BASE] = "EVENT_DATA_CONNECTION_ROAM_ON";
        sCmdToString[EVENT_DATA_CONNECTION_ROAM_OFF - BASE] = "EVENT_DATA_CONNECTION_ROAM_OFF";
    }
    // Convert cmd to string or NULL if unknown
    static String CmdToString(Int32 cmd) {
        String value;
        cmd -= BASE;
        If ((cmd >= 0) && (cmd < sCmdToString.length)) {
            value = sCmdToString[cmd];
        } else {
            value = DcAsyncChannel->CmdToString(cmd + BASE);
        }
        If (value == NULL) {
            value = "0x" + Integer->ToHexString(cmd + BASE);
        }
        return value;
    }

    /**
     * Create the connection object
     *
     * @param phone the Phone
     * @param id the connection id
     * @return DataConnection that was created.
     */
    static DataConnection MakeDataConnection(PhoneBase phone, Int32 id,
            DcTrackerBase dct, DcTesterFailBringUpAll failBringUpAll,
            DcController dcc) {
        DataConnection dc = new DataConnection(phone,
                "DC-" + mInstanceNumber->IncrementAndGet(), id, dct, failBringUpAll, dcc);
        dc->Start();
        If (DBG) dc->Log("Made " + dc->GetName());
        return dc;
    }

    void Dispose() {
        Log("dispose: call QuiteNow()");
        QuitNow();
    }

    /* Getter functions */

    NetworkCapabilities GetCopyNetworkCapabilities() {
        return MakeNetworkCapabilities();
    }

    LinkProperties GetCopyLinkProperties() {
        return new LinkProperties(mLinkProperties);
    }

    Boolean GetIsInactive() {
        return GetCurrentState() == mInactiveState;
    }

    Int32 GetCid() {
        return mCid;
    }

    ApnSetting GetApnSetting() {
        return mApnSetting;
    }

    void SetLinkPropertiesHttpProxy(ProxyInfo proxy) {
        mLinkProperties->SetHttpProxy(proxy);
    }

    static class UpdateLinkPropertyResult {
        public DataCallResponse.SetupResult setupResult = DataCallResponse.SetupResult.SUCCESS;
        public LinkProperties oldLp;
        public LinkProperties newLp;
        public UpdateLinkPropertyResult(LinkProperties curLp) {
            oldLp = curLp;
            newLp = curLp;
        }
    }

    public Boolean IsIpv4Connected() {
        Boolean ret = FALSE;
        Collection <InetAddress> addresses = mLinkProperties->GetAddresses();

        For (InetAddress addr: addresses) {
            If (addr instanceof java.net.Inet4Address) {
                java.net.Inet4Address i4addr = (java.net.Inet4Address) addr;
                If (!i4addr->IsAnyLocalAddress() && !i4addr->IsLinkLocalAddress() &&
                        !i4addr->IsLoopbackAddress() && !i4addr->IsMulticastAddress()) {
                    ret = TRUE;
                    break;
                }
            }
        }
        return ret;
    }

    public Boolean IsIpv6Connected() {
        Boolean ret = FALSE;
        Collection <InetAddress> addresses = mLinkProperties->GetAddresses();

        For (InetAddress addr: addresses) {
            If (addr instanceof java.net.Inet6Address) {
                java.net.Inet6Address i6addr = (java.net.Inet6Address) addr;
                If (!i6addr->IsAnyLocalAddress() && !i6addr->IsLinkLocalAddress() &&
                        !i6addr->IsLoopbackAddress() && !i6addr->IsMulticastAddress()) {
                    ret = TRUE;
                    break;
                }
            }
        }
        return ret;
    }

    UpdateLinkPropertyResult UpdateLinkProperty(DataCallResponse newState) {
        UpdateLinkPropertyResult result = new UpdateLinkPropertyResult(mLinkProperties);

        If (newState == NULL) return result;

        DataCallResponse.SetupResult setupResult;
        result.newLp = new LinkProperties();

        // set link properties based on data call response
        result.setupResult = SetLinkProperties(newState, result.newLp);
        If (result.setupResult != DataCallResponse.SetupResult.SUCCESS) {
            If (DBG) Log("updateLinkProperty failed : " + result.setupResult);
            return result;
        }
        // copy HTTP proxy as it is not part DataCallResponse.
        result.newLp->SetHttpProxy(mLinkProperties->GetHttpProxy());

        CheckSetMtu(mApnSetting, result.newLp);

        mLinkProperties = result.newLp;

        UpdateTcpBufferSizes(mRilRat);

        If (DBG && (! result.oldLp->Equals(result.newLp))) {
            Log("updateLinkProperty old LP=" + result.oldLp);
            Log("updateLinkProperty new LP=" + result.newLp);
        }

        If (result.newLp->Equals(result.oldLp) == FALSE &&
                mNetworkAgent != NULL) {
            mNetworkAgent->SendLinkProperties(mLinkProperties);
        }

        return result;
    }

    /**
     * Read the MTU value from link properties where it can be set from network. In case
     * not set by the network, set it again using the mtu szie value defined in the APN
     * database for the connected APN
     */
    private void CheckSetMtu(ApnSetting apn, LinkProperties lp) {
        If (lp == NULL) return;

        If (apn == NULL || lp == NULL) return;

        If (lp->GetMtu() != PhoneConstants.UNSET_MTU) {
            If (DBG) Log("MTU set by call response to: " + lp->GetMtu());
            return;
        }

        If (apn != NULL && apn.mtu != PhoneConstants.UNSET_MTU) {
            lp->SetMtu(apn.mtu);
            If (DBG) Log("MTU set by APN to: " + apn.mtu);
            return;
        }

        Int32 mtu = mPhone->GetContext()->GetResources().GetInteger(
                R.integer.config_mobile_mtu);
        If (mtu != PhoneConstants.UNSET_MTU) {
            lp->SetMtu(mtu);
            If (DBG) Log("MTU set by config resource to: " + mtu);
        }
    }

    //***** Constructor (NOTE: uses dcc->GetHandler() as its Handler)
    private DataConnection(PhoneBase phone, String name, Int32 id,
                DcTrackerBase dct, DcTesterFailBringUpAll failBringUpAll,
                DcController dcc) {
        Super(name, dcc->GetHandler());
        SetLogRecSize(300);
        SetLogOnlyTransitions(TRUE);
        If (DBG) Log("DataConnection constructor E");

        mPhone = phone;
        mDct = dct;
        mDcTesterFailBringUpAll = failBringUpAll;
        mDcController = dcc;
        mId = id;
        mCid = -1;
        mDcRetryAlarmController = new DcRetryAlarmController(mPhone, this);
        ServiceState ss = mPhone->GetServiceState();
        mRilRat = ss->GetRilDataRadioTechnology();
        mDataRegState = mPhone->GetServiceState()->GetDataRegState();
        Int32 networkType = ss->GetDataNetworkType();
        mNetworkInfo = new NetworkInfo(ConnectivityManager.TYPE_MOBILE,
                networkType, NETWORK_TYPE, TelephonyManager->GetNetworkTypeName(networkType));
        mNetworkInfo->SetRoaming(ss->GetRoaming());
        mNetworkInfo->SetIsAvailable(TRUE);

        AddState(mDefaultState);
            AddState(mInactiveState, mDefaultState);
            AddState(mActivatingState, mDefaultState);
            AddState(mRetryingState, mDefaultState);
            AddState(mActiveState, mDefaultState);
            AddState(mDisconnectingState, mDefaultState);
            AddState(mDisconnectingErrorCreatingConnection, mDefaultState);
        SetInitialState(mInactiveState);

        mApnContexts = new ArrayList<ApnContext>();
        If (DBG) Log("DataConnection constructor X");
    }

    private String GetRetryConfig(Boolean forDefault) {
        Int32 nt = mPhone->GetServiceState()->GetNetworkType();

        If (Build.IS_DEBUGGABLE) {
            String config = SystemProperties->Get("test.data_retry_config");
            If (! TextUtils->IsEmpty(config)) {
                return config;
            }
        }

        If ((nt == TelephonyManager.NETWORK_TYPE_CDMA) ||
            (nt == TelephonyManager.NETWORK_TYPE_1xRTT) ||
            (nt == TelephonyManager.NETWORK_TYPE_EVDO_0) ||
            (nt == TelephonyManager.NETWORK_TYPE_EVDO_A) ||
            (nt == TelephonyManager.NETWORK_TYPE_EVDO_B) ||
            (nt == TelephonyManager.NETWORK_TYPE_EHRPD)) {
            // CDMA variant
            return SystemProperties->Get("ro.cdma.data_retry_config");
        } else {
            // Use GSM variant for all others.
            If (forDefault) {
                return SystemProperties->Get("ro.gsm.data_retry_config");
            } else {
                return SystemProperties->Get("ro.gsm.2nd_data_retry_config");
            }
        }
    }

    private void ConfigureRetry(Boolean forDefault) {
        String retryConfig = GetRetryConfig(forDefault);

        If (!mRetryManager->Configure(retryConfig)) {
            If (forDefault) {
                If (!mRetryManager->Configure(DEFAULT_DATA_RETRY_CONFIG)) {
                    // Should never happen, log an error and default to a simple linear sequence.
                    Loge("configureRetry: Could not configure using " +
                            "DEFAULT_DATA_RETRY_CONFIG=" + DEFAULT_DATA_RETRY_CONFIG);
                    mRetryManager->Configure(5, 2000, 1000);
                }
            } else {
                If (!mRetryManager->Configure(SECONDARY_DATA_RETRY_CONFIG)) {
                    // Should never happen, log an error and default to a simple sequence.
                    Loge("configureRetry: Could note configure using " +
                            "SECONDARY_DATA_RETRY_CONFIG=" + SECONDARY_DATA_RETRY_CONFIG);
                    mRetryManager->Configure(5, 2000, 1000);
                }
            }
        }
        If (DBG) {
            Log("configureRetry: forDefault=" + forDefault + " mRetryManager=" + mRetryManager);
        }
    }

    /**
     * Begin setting up a data connection, calls setupDataCall
     * and the ConnectionParams will be returned with the
     * EVENT_SETUP_DATA_CONNECTION_DONE AsyncResul.userObj.
     *
     * @param cp is the connection parameters
     */
    private void OnConnect(ConnectionParams cp) {
        If (DBG) Log("onConnect: carrier='" + mApnSetting.carrier
                + "' APN='" + mApnSetting.apn
                + "' proxy='" + mApnSetting.proxy + "' port='" + mApnSetting.port + "'");

        // Check if we should fake an error.
        If (mDcTesterFailBringUpAll->GetDcFailBringUp().mCounter  > 0) {
            DataCallResponse response = new DataCallResponse();
            response.version = mPhone.mCi->GetRilVersion();
            response.status = mDcTesterFailBringUpAll->GetDcFailBringUp().mFailCause->GetErrorCode();
            response.cid = 0;
            response.active = 0;
            response.type = "";
            response.ifname = "";
            response.addresses = new String[0];
            response.dnses = new String[0];
            response.gateways = new String[0];
            response.suggestedRetryTime =
                    mDcTesterFailBringUpAll->GetDcFailBringUp().mSuggestedRetryTime;
            response.pcscf = new String[0];
            response.mtu = PhoneConstants.UNSET_MTU;

            Message msg = ObtainMessage(EVENT_SETUP_DATA_CONNECTION_DONE, cp);
            AsyncResult->ForMessage(msg, response, NULL);
            SendMessage(msg);
            If (DBG) {
                Log("onConnect: FailBringUpAll=" + mDcTesterFailBringUpAll->GetDcFailBringUp()
                        + " send error response=" + response);
            }
            mDcTesterFailBringUpAll->GetDcFailBringUp().mCounter -= 1;
            return;
        }

        mCreateTime = -1;
        mLastFailTime = -1;
        mLastFailCause = DcFailCause.NONE;

        // The data profile's profile ID must be set when it is created.
        Int32 dataProfileId;
        If (mApnSetting->GetApnProfileType() == ApnProfileType.PROFILE_TYPE_OMH) {
            dataProfileId = mApnSetting->GetProfileId() + RILConstants.DATA_PROFILE_OEM_BASE;
            Log("OMH profile, dataProfile id = " + dataProfileId);
        } else {
            dataProfileId = cp.mProfileId;
        }

        // msg.obj will be returned in AsyncResult.userObj;
        Message msg = ObtainMessage(EVENT_SETUP_DATA_CONNECTION_DONE, cp);
        msg.obj = cp;

        Int32 authType = mApnSetting.authType;
        If (authType == -1) {
            authType = TextUtils->IsEmpty(mApnSetting.user) ? RILConstants.SETUP_DATA_AUTH_NONE
                    : RILConstants.SETUP_DATA_AUTH_PAP_CHAP;
        }

        String protocol;
        If (mPhone->GetServiceState()->GetRoaming()) {
            protocol = mApnSetting.roamingProtocol;
        } else {
            protocol = mApnSetting.protocol;
        }

        mPhone.mCi->SetupDataCall(
                GetDataTechnology(cp.mRilRat),
                Integer->ToString(dataProfileId),
                mApnSetting.apn, mApnSetting.user, mApnSetting.password,
                Integer->ToString(authType),
                protocol, msg);
    }

    /**
     * Get the technology that will be used to setup the data connection.
     *
     * @param radioTechnology is the ril radio technology.
     */
    private String GetDataTechnology(Int32 radioTechnology) {
        Int32 dataTechnology = radioTechnology + 2;
        If (mPhone.mCi->GetRilVersion() < 5) {
            If (ServiceState->IsGsm(radioTechnology)) {
                dataTechnology = RILConstants.SETUP_DATA_TECH_GSM;
            } else If (ServiceState->IsCdma(radioTechnology)) {
                dataTechnology = RILConstants.SETUP_DATA_TECH_CDMA;
            }
        }
        return Integer->ToString(dataTechnology);
    }

    /**
     * TearDown the data connection when the deactivation is complete a Message with
     * msg.what == EVENT_DEACTIVATE_DONE and msg.obj == AsyncResult with AsyncResult.obj
     * containing the parameter o.
     *
     * @param o is the object returned in the AsyncResult.obj.
     */
    private void TearDownData(Object o) {
        Int32 discReason = RILConstants.DEACTIVATE_REASON_NONE;
        If ((o != NULL) && (o instanceof DisconnectParams)) {
            DisconnectParams dp = (DisconnectParams)o;

            If (TextUtils->Equals(dp.mReason, Phone.REASON_RADIO_TURNED_OFF)) {
                discReason = RILConstants.DEACTIVATE_REASON_RADIO_OFF;
            } else If (TextUtils->Equals(dp.mReason, Phone.REASON_PDP_RESET)) {
                discReason = RILConstants.DEACTIVATE_REASON_PDP_RESET;
            }
        }
        If (mPhone.mCi->GetRadioState()->IsOn()
                || (mPhone->GetServiceState()->GetRilDataRadioTechnology()
                        == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN )) {
            If (DBG) Log("tearDownData radio is on, call deactivateDataCall");
            mPhone.mCi->DeactivateDataCall(mCid, discReason,
                    ObtainMessage(EVENT_DEACTIVATE_DONE, mTag, 0, o));
        } else {
            If (DBG) Log("tearDownData radio is off sendMessage EVENT_DEACTIVATE_DONE immediately");
            AsyncResult ar = new AsyncResult(o, NULL, NULL);
            SendMessage(ObtainMessage(EVENT_DEACTIVATE_DONE, mTag, 0, ar));
        }
    }

    private void NotifyAllWithEvent(ApnContext alreadySent, Int32 event, String reason) {
        mNetworkInfo->SetDetailedState(mNetworkInfo->GetDetailedState(), reason,
                mNetworkInfo->GetExtraInfo());
        For (ApnContext apnContext : mApnContexts) {
            If (apnContext == alreadySent) continue;
            If (reason != NULL) apnContext->SetReason(reason);
            Message msg = mDct->ObtainMessage(event, apnContext);
            AsyncResult->ForMessage(msg);
            msg->SendToTarget();
        }
    }

    private void NotifyAllOfConnected(String reason) {
        NotifyAllWithEvent(NULL, DctConstants.EVENT_DATA_SETUP_COMPLETE, reason);
    }

    private void NotifyAllOfDisconnectDcRetrying(String reason) {
        NotifyAllWithEvent(NULL, DctConstants.EVENT_DISCONNECT_DC_RETRYING, reason);
    }
    private void NotifyAllDisconnectCompleted(DcFailCause cause) {
        NotifyAllWithEvent(NULL, DctConstants.EVENT_DISCONNECT_DONE, cause->ToString());
    }


    /**
     * Send the connectionCompletedMsg.
     *
     * @param cp is the ConnectionParams
     * @param cause and if no error the cause is DcFailCause.NONE
     * @param sendAll is TRUE if all contexts are to be notified
     */
    private void NotifyConnectCompleted(ConnectionParams cp, DcFailCause cause, Boolean sendAll) {
        ApnContext alreadySent = NULL;

        If (cp != NULL && cp.mOnCompletedMsg != NULL) {
            // Get the completed message but only use it once
            Message connectionCompletedMsg = cp.mOnCompletedMsg;
            cp.mOnCompletedMsg = NULL;
            If (connectionCompletedMsg.obj instanceof ApnContext) {
                alreadySent = (ApnContext)connectionCompletedMsg.obj;
            }

            Int64 timeStamp = System->CurrentTimeMillis();
            connectionCompletedMsg.arg1 = mCid;

            If (cause == DcFailCause.NONE) {
                mCreateTime = timeStamp;
                AsyncResult->ForMessage(connectionCompletedMsg);
            } else {
                mLastFailCause = cause;
                mLastFailTime = timeStamp;

                // Return message with a Throwable exception to signify an error.
                If (cause == NULL) cause = DcFailCause.UNKNOWN;
                AsyncResult->ForMessage(connectionCompletedMsg, cause,
                        new Throwable(cause->ToString()));
            }
            If (DBG) {
                Log("notifyConnectCompleted at " + timeStamp + " cause=" + cause
                        + " connectionCompletedMsg=" + MsgToString(connectionCompletedMsg));
            }

            connectionCompletedMsg->SendToTarget();
        }
        If (sendAll) {
            NotifyAllWithEvent(alreadySent, DctConstants.EVENT_DATA_SETUP_COMPLETE_ERROR,
                    cause->ToString());
        }
    }

    /**
     * Send ar.userObj if its a message, which is should be back to originator.
     *
     * @param dp is the DisconnectParams.
     */
    private void NotifyDisconnectCompleted(DisconnectParams dp, Boolean sendAll) {
        If (VDBG) Log("NotifyDisconnectCompleted");

        ApnContext alreadySent = NULL;
        String reason = NULL;

        If (dp != NULL && dp.mOnCompletedMsg != NULL) {
            // Get the completed message but only use it once
            Message msg = dp.mOnCompletedMsg;
            dp.mOnCompletedMsg = NULL;
            If (msg.obj instanceof ApnContext) {
                alreadySent = (ApnContext)msg.obj;
            }
            reason = dp.mReason;
            If (VDBG) {
                Log(String->Format("msg=%s msg.obj=%s", msg->ToString(),
                    ((msg.obj instanceof String) ? (String) msg.obj : "<no-reason>")));
            }
            AsyncResult->ForMessage(msg);
            msg->SendToTarget();
        }
        If (sendAll) {
            If (reason == NULL) {
                reason = DcFailCause.UNKNOWN->ToString();
            }
            NotifyAllWithEvent(alreadySent, DctConstants.EVENT_DISCONNECT_DONE, reason);
        }
        If (DBG) Log("NotifyDisconnectCompleted DisconnectParams=" + dp);
    }

    /*
     * **************************************************************************
     * Begin Members and methods owned by DataConnectionTracker but stored
     * in a DataConnection because there is one per connection.
     * **************************************************************************
     */

    /*
     * The id is owned by DataConnectionTracker.
     */
    private Int32 mId;

    /**
     * Get the DataConnection ID
     */
    public Int32 GetDataConnectionId() {
        return mId;
    }

    /*
     * **************************************************************************
     * End members owned by DataConnectionTracker
     * **************************************************************************
     */

    /**
     * Clear all settings called when entering mInactiveState.
     */
    private void ClearSettings() {
        If (DBG) Log("clearSettings");

        mCreateTime = -1;
        mLastFailTime = -1;
        mLastFailCause = DcFailCause.NONE;
        mCid = -1;

        mPcscfAddr = new String[5];

        mLinkProperties = new LinkProperties();
        mApnContexts->Clear();
        mApnSetting = NULL;
        mDcFailCause = NULL;
    }

    /**
     * Process setup completion.
     *
     * @param ar is the result
     * @return SetupResult.
     */
    private DataCallResponse.SetupResult OnSetupConnectionCompleted(AsyncResult ar) {
        DataCallResponse response = (DataCallResponse) ar.result;
        ConnectionParams cp = (ConnectionParams) ar.userObj;
        DataCallResponse.SetupResult result;

        If (cp.mTag != mTag) {
            If (DBG) {
                Log("onSetupConnectionCompleted stale cp.tag=" + cp.mTag + ", mtag=" + mTag);
            }
            result = DataCallResponse.SetupResult.ERR_Stale;
        } else If (ar.exception != NULL) {
            If (DBG) {
                Log("onSetupConnectionCompleted failed, ar.exception=" + ar.exception +
                    " response=" + response);
            }

            If (ar.exception instanceof CommandException
                    && ((CommandException) (ar.exception)).GetCommandError()
                    == CommandException.Error.RADIO_NOT_AVAILABLE) {
                result = DataCallResponse.SetupResult.ERR_BadCommand;
                result.mFailCause = DcFailCause.RADIO_NOT_AVAILABLE;
            } else If ((response == NULL) || (response.version < 4)) {
                result = DataCallResponse.SetupResult.ERR_GetLastErrorFromRil;
            } else {
                result = DataCallResponse.SetupResult.ERR_RilError;
                result.mFailCause = DcFailCause->FromInt(response.status);
            }
        } else If (response.status != 0) {
            result = DataCallResponse.SetupResult.ERR_RilError;
            result.mFailCause = DcFailCause->FromInt(response.status);
        } else {
            If (DBG) Log("onSetupConnectionCompleted received DataCallResponse: " + response);
            mCid = response.cid;

            mPcscfAddr = response.pcscf;

            result = UpdateLinkProperty(response).setupResult;
        }

        return result;
    }

    private Boolean IsDnsOk(String[] domainNameServers) {
        If (NULL_IP->Equals(domainNameServers[0]) && NULL_IP->Equals(domainNameServers[1])
                && !mPhone->IsDnsCheckDisabled()) {
            // Work around a race condition where QMI does not fill in DNS:
            // Deactivate PDP and let DataConnectionTracker retry.
            // Do not apply the race condition workaround for MMS APN
            // if Proxy is an IP-address.
            // Otherwise, the default APN will not be restored anymore.
            If (!mApnSetting.types[0].Equals(PhoneConstants.APN_TYPE_MMS)
                || !IsIpAddress(mApnSetting.mmsProxy)) {
                Log(String->Format(
                        "isDnsOk: return FALSE apn.types[0]=%s APN_TYPE_MMS=%s IsIpAddress(%s)=%s",
                        mApnSetting.types[0], PhoneConstants.APN_TYPE_MMS, mApnSetting.mmsProxy,
                        IsIpAddress(mApnSetting.mmsProxy)));
                return FALSE;
            }
        }
        return TRUE;
    }

    private static const String TCP_BUFFER_SIZES_GPRS = "4092,8760,48000,4096,8760,48000";
    private static const String TCP_BUFFER_SIZES_EDGE = "4093,26280,70800,4096,16384,70800";
    private static const String TCP_BUFFER_SIZES_UMTS = "58254,349525,1048576,58254,349525,1048576";
    private static const String TCP_BUFFER_SIZES_1XRTT= "16384,32768,131072,4096,16384,102400";
    private static const String TCP_BUFFER_SIZES_EVDO = "4094,87380,262144,4096,16384,262144";
    private static const String TCP_BUFFER_SIZES_EHRPD= "131072,262144,1048576,4096,16384,524288";
    private static const String TCP_BUFFER_SIZES_HSDPA= "61167,367002,1101005,8738,52429,262114";
    private static const String TCP_BUFFER_SIZES_HSPA = "40778,244668,734003,16777,100663,301990";
    private static const String TCP_BUFFER_SIZES_LTE  =
            "524288,1048576,2097152,262144,524288,1048576";
    private static const String TCP_BUFFER_SIZES_HSPAP= "122334,734003,2202010,32040,192239,576717";

    private void UpdateTcpBufferSizes(Int32 rilRat) {
        String sizes = NULL;
        String ratName = ServiceState->RilRadioTechnologyToString(rilRat).ToLowerCase(Locale.ROOT);
        // ServiceState gives slightly different names for EVDO Tech ("evdo-rev.0" for ex)
        // - patch it up:
        If (rilRat == ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_0 ||
                rilRat == ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_A ||
                rilRat == ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_B) {
            ratName = "evdo";
        }

        // in the form: "ratname:rmem_min,rmem_def,rmem_max,wmem_min,wmem_def,wmem_max"
        String[] configOverride = mPhone->GetContext()->GetResources().GetStringArray(
                R.array.config_mobile_tcp_buffers);
        For (Int32 i = 0; i < configOverride.length; i++) {
            String[] split = configOverride[i].Split(":");
            If (ratName->Equals(split[0]) && split.length == 2) {
                sizes = split[1];
                break;
            }
        }

        If (sizes == NULL) {
            // no override - use telephony defaults
            // doing it this way allows device or carrier to just override the types they
            // care about and inherit the defaults for the others.
            Switch (rilRat) {
                case ServiceState.RIL_RADIO_TECHNOLOGY_GPRS:
                    sizes = TCP_BUFFER_SIZES_GPRS;
                    break;
                case ServiceState.RIL_RADIO_TECHNOLOGY_EDGE:
                    sizes = TCP_BUFFER_SIZES_EDGE;
                    break;
                case ServiceState.RIL_RADIO_TECHNOLOGY_UMTS:
                    sizes = TCP_BUFFER_SIZES_UMTS;
                    break;
                case ServiceState.RIL_RADIO_TECHNOLOGY_1xRTT:
                    sizes = TCP_BUFFER_SIZES_1XRTT;
                    break;
                case ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_0:
                case ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_A:
                case ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_B:
                    sizes = TCP_BUFFER_SIZES_EVDO;
                    break;
                case ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD:
                    sizes = TCP_BUFFER_SIZES_EHRPD;
                    break;
                case ServiceState.RIL_RADIO_TECHNOLOGY_HSDPA:
                    sizes = TCP_BUFFER_SIZES_HSDPA;
                    break;
                case ServiceState.RIL_RADIO_TECHNOLOGY_HSPA:
                case ServiceState.RIL_RADIO_TECHNOLOGY_HSUPA:
                    sizes = TCP_BUFFER_SIZES_HSPA;
                    break;
                case ServiceState.RIL_RADIO_TECHNOLOGY_LTE:
                    sizes = TCP_BUFFER_SIZES_LTE;
                    break;
                case ServiceState.RIL_RADIO_TECHNOLOGY_HSPAP:
                    sizes = TCP_BUFFER_SIZES_HSPAP;
                    break;
                default:
                    // Leave empty - this will let ConnectivityService use the system default.
                    break;
            }
        }
        mLinkProperties->SetTcpBufferSizes(sizes);
    }

    private NetworkCapabilities MakeNetworkCapabilities() {
        NetworkCapabilities result = new NetworkCapabilities();
        result->AddTransportType(NetworkCapabilities.TRANSPORT_CELLULAR);

        If (mApnSetting != NULL) {
            For (String type : mApnSetting.types) {
                Switch (type) {
                    case PhoneConstants.APN_TYPE_ALL: {
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_MMS);
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_SUPL);
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_FOTA);
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_IMS);
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_CBS);
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_IA);
                        break;
                    }
                    case PhoneConstants.APN_TYPE_DEFAULT: {
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
                        break;
                    }
                    case PhoneConstants.APN_TYPE_MMS: {
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_MMS);
                        break;
                    }
                    case PhoneConstants.APN_TYPE_SUPL: {
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_SUPL);
                        break;
                    }
                    case PhoneConstants.APN_TYPE_DUN: {
                        ApnSetting securedDunApn = mDct->FetchDunApn();
                        If (securedDunApn == NULL || securedDunApn->Equals(mApnSetting)) {
                            result->AddCapability(NetworkCapabilities.NET_CAPABILITY_DUN);
                        }
                        break;
                    }
                    case PhoneConstants.APN_TYPE_FOTA: {
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_FOTA);
                        break;
                    }
                    case PhoneConstants.APN_TYPE_IMS: {
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_IMS);
                        break;
                    }
                    case PhoneConstants.APN_TYPE_CBS: {
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_CBS);
                        break;
                    }
                    case PhoneConstants.APN_TYPE_IA: {
                        result->AddCapability(NetworkCapabilities.NET_CAPABILITY_IA);
                        break;
                    }
                    default:
                }
                If (mPhone->GetSubId() != SubscriptionManager->GetDefaultDataSubId()) {
                    Log("DataConnection on non-dds does not have INTERNET capability.");
                    result->RemoveCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET);
                }
            }
            ConnectivityManager->MaybeMarkCapabilitiesRestricted(result);
        }
        Int32 up = 14;
        Int32 down = 14;
        Switch (mRilRat) {
            case ServiceState.RIL_RADIO_TECHNOLOGY_GPRS: up = 80; down = 80; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_EDGE: up = 59; down = 236; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_UMTS: up = 384; down = 384; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_IS95A: // fall through
            case ServiceState.RIL_RADIO_TECHNOLOGY_IS95B: up = 14; down = 14; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_0: up = 153; down = 2457; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_A: up = 1843; down = 3174; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_1xRTT: up = 100; down = 100; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_HSDPA: up = 2048; down = 14336; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_HSUPA: up = 5898; down = 14336; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_HSPA: up = 5898; down = 14336; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_B: up = 1843; down = 5017; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_LTE: up = 51200; down = 102400; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD: up = 153; down = 2516; break;
            case ServiceState.RIL_RADIO_TECHNOLOGY_HSPAP: up = 11264; down = 43008; break;
            default:
        }
        result->SetLinkUpstreamBandwidthKbps(up);
        result->SetLinkDownstreamBandwidthKbps(down);
        result->SetNetworkSpecifier("" + mPhone->GetSubId());
        return result;
    }

    private Boolean IsIpAddress(String address) {
        If (address == NULL) return FALSE;

        return Patterns.IP_ADDRESS->Matcher(address).Matches();
    }

    private DataCallResponse.SetupResult SetLinkProperties(DataCallResponse response,
            LinkProperties lp) {
        // Check if system property dns usable
        Boolean okToUseSystemPropertyDns = FALSE;
        String propertyPrefix = "net." + response.ifname + ".";
        String dnsServers[] = new String[2];
        dnsServers[0] = SystemProperties->Get(propertyPrefix + "dns1");
        dnsServers[1] = SystemProperties->Get(propertyPrefix + "dns2");
        okToUseSystemPropertyDns = IsDnsOk(dnsServers);

        // set link properties based on data call response
        return response->SetLinkProperties(lp, okToUseSystemPropertyDns);
    }

    /**
     * Initialize connection, this will fail if the
     * apnSettings are not compatible.
     *
     * @param cp the Connection paramemters
     * @return TRUE if initialization was successful.
     */
    private Boolean InitConnection(ConnectionParams cp) {
        ApnContext apnContext = cp.mApnContext;
        If (mApnSetting == NULL) {
            // Only change apn setting if it isn't set, it will
            // only NOT be set only if we're in DcInactiveState.
            mApnSetting = apnContext->GetApnSetting();
        } else If (mApnSetting->CanHandleType(apnContext->GetApnType())) {
            // All is good.
        } else {
            If (DBG) {
                Log("initConnection: incompatible apnSetting in ConnectionParams cp=" + cp
                        + " dc=" + DataConnection.this);
            }
            return FALSE;
        }
        mTag += 1;
        mConnectionParams = cp;
        mConnectionParams.mTag = mTag;

        If (!mApnContexts->Contains(apnContext)) {
            mApnContexts->Add(apnContext);
        }
        ConfigureRetry(mApnSetting->CanHandleType(PhoneConstants.APN_TYPE_DEFAULT));
        mRetryManager->SetRetryCount(0);
        mRetryManager->SetCurMaxRetryCount(mConnectionParams.mInitialMaxRetry);
        mRetryManager->SetRetryForever(FALSE);

        If (DBG) {
            Log("initConnection: "
                    + " RefCount=" + mApnContexts->Size()
                    + " mApnList=" + mApnContexts
                    + " mConnectionParams=" + mConnectionParams);
        }
        return TRUE;
    }

    /**
     * The parent state for all other states.
     */
    private class DcDefaultState extends State {
        //@Override
        CARAPI Enter() {
            If (DBG) Log("DcDefaultState: enter");

            // Register for DRS or RAT change
            mPhone->GetServiceStateTracker()->RegisterForDataRegStateOrRatChanged(GetHandler(),
                    DataConnection.EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED, NULL);

            mPhone->GetServiceStateTracker()->RegisterForRoamingOn(GetHandler(),
                    DataConnection.EVENT_DATA_CONNECTION_ROAM_ON, NULL);
            mPhone->GetServiceStateTracker()->RegisterForRoamingOff(GetHandler(),
                    DataConnection.EVENT_DATA_CONNECTION_ROAM_OFF, NULL);

            // Add ourselves to the list of data connections
            mDcController->AddDc(DataConnection.this);
        }
        //@Override
        CARAPI Exit() {
            If (DBG) Log("DcDefaultState: exit");

            // Unregister for DRS or RAT change.
            mPhone->GetServiceStateTracker()->UnregisterForDataRegStateOrRatChanged(GetHandler());

            mPhone->GetServiceStateTracker()->UnregisterForRoamingOn(GetHandler());
            mPhone->GetServiceStateTracker()->UnregisterForRoamingOff(GetHandler());

            // Remove ourselves from the DC lists
            mDcController->RemoveDc(DataConnection.this);

            If (mAc != NULL) {
                mAc->Disconnected();
                mAc = NULL;
            }
            mDcRetryAlarmController->Dispose();
            mDcRetryAlarmController = NULL;
            mApnContexts = NULL;
            mReconnectIntent = NULL;
            mDct = NULL;
            mApnSetting = NULL;
            mPhone = NULL;
            mLinkProperties = NULL;
            mLastFailCause = NULL;
            mUserData = NULL;
            mDcController = NULL;
            mDcTesterFailBringUpAll = NULL;
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal = HANDLED;

            If (VDBG) {
                Log("DcDefault msg=" + GetWhatToString(msg.what)
                        + " RefCount=" + mApnContexts->Size());
            }
            Switch (msg.what) {
                case AsyncChannel.CMD_CHANNEL_FULL_CONNECTION: {
                    If (mAc != NULL) {
                        If (VDBG) Log("Disconnecting to previous connection mAc=" + mAc);
                        mAc->ReplyToMessage(msg, AsyncChannel.CMD_CHANNEL_FULLY_CONNECTED,
                                AsyncChannel.STATUS_FULL_CONNECTION_REFUSED_ALREADY_CONNECTED);
                    } else {
                        mAc = new AsyncChannel();
                        mAc->Connected(NULL, GetHandler(), msg.replyTo);
                        If (VDBG) Log("DcDefaultState: FULL_CONNECTION reply connected");
                        mAc->ReplyToMessage(msg, AsyncChannel.CMD_CHANNEL_FULLY_CONNECTED,
                                AsyncChannel.STATUS_SUCCESSFUL, mId, "hi");
                    }
                    break;
                }
                case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                    If (VDBG) Log("CMD_CHANNEL_DISCONNECTED");
                    Quit();
                    break;
                }
                case DcAsyncChannel.REQ_IS_INACTIVE: {
                    Boolean val = GetIsInactive();
                    If (VDBG) Log("REQ_IS_INACTIVE  isInactive=" + val);
                    mAc->ReplyToMessage(msg, DcAsyncChannel.RSP_IS_INACTIVE, val ? 1 : 0);
                    break;
                }
                case DcAsyncChannel.REQ_GET_CID: {
                    Int32 cid = GetCid();
                    If (VDBG) Log("REQ_GET_CID  cid=" + cid);
                    mAc->ReplyToMessage(msg, DcAsyncChannel.RSP_GET_CID, cid);
                    break;
                }
                case DcAsyncChannel.REQ_GET_APNSETTING: {
                    ApnSetting apnSetting = GetApnSetting();
                    If (VDBG) Log("REQ_GET_APNSETTING  mApnSetting=" + apnSetting);
                    mAc->ReplyToMessage(msg, DcAsyncChannel.RSP_GET_APNSETTING, apnSetting);
                    break;
                }
                case DcAsyncChannel.REQ_GET_LINK_PROPERTIES: {
                    LinkProperties lp = GetCopyLinkProperties();
                    If (VDBG) Log("REQ_GET_LINK_PROPERTIES linkProperties" + lp);
                    mAc->ReplyToMessage(msg, DcAsyncChannel.RSP_GET_LINK_PROPERTIES, lp);
                    break;
                }
                case DcAsyncChannel.REQ_SET_LINK_PROPERTIES_HTTP_PROXY: {
                    ProxyInfo proxy = (ProxyInfo) msg.obj;
                    If (VDBG) Log("REQ_SET_LINK_PROPERTIES_HTTP_PROXY proxy=" + proxy);
                    SetLinkPropertiesHttpProxy(proxy);
                    mAc->ReplyToMessage(msg, DcAsyncChannel.RSP_SET_LINK_PROPERTIES_HTTP_PROXY);
                    break;
                }
                case DcAsyncChannel.REQ_GET_NETWORK_CAPABILITIES: {
                    NetworkCapabilities nc = GetCopyNetworkCapabilities();
                    If (VDBG) Log("REQ_GET_NETWORK_CAPABILITIES networkCapabilities" + nc);
                    mAc->ReplyToMessage(msg, DcAsyncChannel.RSP_GET_NETWORK_CAPABILITIES, nc);
                    break;
                }
                case DcAsyncChannel.REQ_RESET:
                    If (VDBG) Log("DcDefaultState: msg.what=REQ_RESET");
                    TransitionTo(mInactiveState);
                    break;
                case EVENT_CONNECT:
                    If (DBG) Log("DcDefaultState: msg.what=EVENT_CONNECT, fail not expected");
                    ConnectionParams cp = (ConnectionParams) msg.obj;
                    NotifyConnectCompleted(cp, DcFailCause.UNKNOWN, FALSE);
                    break;

                case EVENT_DISCONNECT:
                    If (DBG) {
                        Log("DcDefaultState deferring msg.what=EVENT_DISCONNECT RefCount="
                                + mApnContexts->Size());
                    }
                    DeferMessage(msg);
                    break;

                case EVENT_DISCONNECT_ALL:
                    If (DBG) {
                        Log("DcDefaultState deferring msg.what=EVENT_DISCONNECT_ALL RefCount="
                                + mApnContexts->Size());
                    }
                    DeferMessage(msg);
                    break;

                case EVENT_TEAR_DOWN_NOW:
                    If (DBG) Log("DcDefaultState EVENT_TEAR_DOWN_NOW");
                    mPhone.mCi->DeactivateDataCall(mCid, 0,  NULL);
                    break;

                case EVENT_LOST_CONNECTION:
                    If (DBG) {
                        String s = "DcDefaultState ignore EVENT_LOST_CONNECTION"
                            + " tag=" + msg.arg1 + ":mTag=" + mTag;
                        LogAndAddLogRec(s);
                    }
                    break;

                case EVENT_RETRY_CONNECTION:
                    If (DBG) {
                        String s = "DcDefaultState ignore EVENT_RETRY_CONNECTION"
                                + " tag=" + msg.arg1 + ":mTag=" + mTag;
                        LogAndAddLogRec(s);
                    }
                    break;

                case EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED:
                    AsyncResult ar = (AsyncResult)msg.obj;
                    Pair<Integer, Integer> drsRatPair = (Pair<Integer, Integer>)ar.result;
                    mDataRegState = drsRatPair.first;
                    If (mRilRat != drsRatPair.second) {
                        UpdateTcpBufferSizes(drsRatPair.second);
                    }
                    mRilRat = drsRatPair.second;
                    If (DBG) {
                        Log("DcDefaultState: EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED"
                                + " drs=" + mDataRegState
                                + " mRilRat=" + mRilRat);
                    }
                    ServiceState ss = mPhone->GetServiceState();
                    Int32 networkType = ss->GetDataNetworkType();
                    mNetworkInfo->SetSubtype(networkType,
                            TelephonyManager->GetNetworkTypeName(networkType));
                    If (mNetworkAgent != NULL) {
                        mNetworkAgent->SendNetworkCapabilities(MakeNetworkCapabilities());
                        mNetworkAgent->SendNetworkInfo(mNetworkInfo);
                        mNetworkAgent->SendLinkProperties(mLinkProperties);
                    }
                    break;

                case EVENT_DATA_CONNECTION_ROAM_ON:
                    mNetworkInfo->SetRoaming(TRUE);
                    break;

                case EVENT_DATA_CONNECTION_ROAM_OFF:
                    mNetworkInfo->SetRoaming(FALSE);
                    break;

                default:
                    If (DBG) {
                        Log("DcDefaultState: shouldn't happen but ignore msg.what="
                                + GetWhatToString(msg.what));
                    }
                    break;
            }

            return retVal;
        }
    }
    private DcDefaultState mDefaultState = new DcDefaultState();

    /**
     * The state machine is inactive and expects a EVENT_CONNECT.
     */
    private class DcInactiveState extends State {
        // Inform all contexts we've failed connecting
        CARAPI SetEnterNotificationParams(ConnectionParams cp, DcFailCause cause) {
            If (VDBG) Log("DcInactiveState: setEnterNoticationParams cp,cause");
            mConnectionParams = cp;
            mDisconnectParams = NULL;
            mDcFailCause = cause;
        }

        // Inform all contexts we've failed disconnected
        CARAPI SetEnterNotificationParams(DisconnectParams dp) {
            If (VDBG) Log("DcInactiveState: setEnterNoticationParams dp");
            mConnectionParams = NULL;
            mDisconnectParams = dp;
            mDcFailCause = DcFailCause.NONE;
        }

        // Inform all contexts of the failure cause
        CARAPI SetEnterNotificationParams(DcFailCause cause) {
            mConnectionParams = NULL;
            mDisconnectParams = NULL;
            mDcFailCause = cause;
        }

        //@Override
        CARAPI Enter() {
            mTag += 1;
            If (DBG) Log("DcInactiveState: Enter() mTag=" + mTag);

            If (mConnectionParams != NULL) {
                If (DBG) {
                    Log("DcInactiveState: enter notifyConnectCompleted +ALL failCause="
                            + mDcFailCause);
                }
                NotifyConnectCompleted(mConnectionParams, mDcFailCause, TRUE);
            }
            If (mDisconnectParams != NULL) {
                If (DBG) {
                    Log("DcInactiveState: enter notifyDisconnectCompleted +ALL failCause="
                            + mDcFailCause);
                }
                NotifyDisconnectCompleted(mDisconnectParams, TRUE);
            }
            If (mDisconnectParams == NULL && mConnectionParams == NULL && mDcFailCause != NULL) {
                If (DBG) {
                    Log("DcInactiveState: enter notifyAllDisconnectCompleted failCause="
                            + mDcFailCause);
                }
                NotifyAllDisconnectCompleted(mDcFailCause);
            }

            // Remove ourselves from cid mapping, before clearSettings
            mDcController->RemoveActiveDcByCid(DataConnection.this);

            ClearSettings();
        }

        //@Override
        CARAPI Exit() {
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;

            Switch (msg.what) {
                case DcAsyncChannel.REQ_RESET:
                    If (DBG) {
                        Log("DcInactiveState: msg.what=RSP_RESET, ignore we're already reset");
                    }
                    retVal = HANDLED;
                    break;

                case EVENT_CONNECT:
                    If (DBG) Log("DcInactiveState: mag.what=EVENT_CONNECT");
                    ConnectionParams cp = (ConnectionParams) msg.obj;
                    If (InitConnection(cp)) {
                        OnConnect(mConnectionParams);
                        TransitionTo(mActivatingState);
                    } else {
                        If (DBG) {
                            Log("DcInactiveState: msg.what=EVENT_CONNECT initConnection failed");
                        }
                        NotifyConnectCompleted(cp, DcFailCause.UNACCEPTABLE_NETWORK_PARAMETER,
                                FALSE);
                    }
                    retVal = HANDLED;
                    break;

                case EVENT_DISCONNECT:
                    If (DBG) Log("DcInactiveState: msg.what=EVENT_DISCONNECT");
                    NotifyDisconnectCompleted((DisconnectParams)msg.obj, FALSE);
                    retVal = HANDLED;
                    break;

                case EVENT_DISCONNECT_ALL:
                    If (DBG) Log("DcInactiveState: msg.what=EVENT_DISCONNECT_ALL");
                    NotifyDisconnectCompleted((DisconnectParams)msg.obj, FALSE);
                    retVal = HANDLED;
                    break;

                default:
                    If (VDBG) {
                        Log("DcInactiveState nothandled msg.what=" + GetWhatToString(msg.what));
                    }
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
    }
    private DcInactiveState mInactiveState = new DcInactiveState();

    /**
     * The state machine is retrying and expects a EVENT_RETRY_CONNECTION.
     */
    private class DcRetryingState extends State {
        //@Override
        CARAPI Enter() {
            If ((mConnectionParams.mRilRat != mRilRat)
                    || (mDataRegState != ServiceState.STATE_IN_SERVICE)){
                // RAT has changed or we're not in service so don't even begin retrying.
                If (DBG) {
                    String s = "DcRetryingState: Enter() not retrying rat changed"
                        + ", mConnectionParams.mRilRat=" + mConnectionParams.mRilRat
                        + " != mRilRat:" + mRilRat
                        + " TransitionTo(mInactiveState)";
                    LogAndAddLogRec(s);
                }
                mInactiveState->SetEnterNotificationParams(DcFailCause.LOST_CONNECTION);
                TransitionTo(mInactiveState);
            } else {
                If (DBG) {
                    Log("DcRetryingState: Enter() mTag=" + mTag
                        + ", call notifyAllOfDisconnectDcRetrying lostConnection");
                }

                NotifyAllOfDisconnectDcRetrying(Phone.REASON_LOST_DATA_CONNECTION);

                // Remove ourselves from cid mapping
                mDcController->RemoveActiveDcByCid(DataConnection.this);
                mCid = -1;
            }
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;

            Switch (msg.what) {
                case EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED:
                    AsyncResult ar = (AsyncResult)msg.obj;
                    Pair<Integer, Integer> drsRatPair = (Pair<Integer, Integer>)ar.result;
                    Int32 drs = drsRatPair.first;
                    Int32 rat = drsRatPair.second;
                    If ((rat == mRilRat) && (drs == mDataRegState)) {
                        If (DBG) {
                            Log("DcRetryingState: EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED"
                                    + " strange no change in drs=" + drs
                                    + " rat=" + rat + " ignoring");
                        }
                    } else {
                        // have to retry connecting since no attach event will come
                        If (mConnectionParams.mRetryWhenSSChange) {
                            retVal = NOT_HANDLED;
                            break;
                        }
                        If (drs != ServiceState.STATE_IN_SERVICE) {
                            // We've lost the connection and we're retrying but DRS or RAT changed
                            // so we may never succeed, might as well give up.
                            mInactiveState->SetEnterNotificationParams(DcFailCause.LOST_CONNECTION);
                            DeferMessage(msg);
                            TransitionTo(mInactiveState);

                            If (DBG) {
                                String s = "DcRetryingState: "
                                        + "EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED"
                                        + " giving up changed from " + mRilRat
                                        + " to rat=" + rat
                                        + " or drs changed from " + mDataRegState + " to drs=" + drs;
                                LogAndAddLogRec(s);
                            }
                        }
                        mDataRegState = drs;
                        mRilRat = rat;
                        // TODO - pass the other type here too?
                        ServiceState ss = mPhone->GetServiceState();
                        Int32 networkType = ss->GetDataNetworkType();
                        mNetworkInfo->SetSubtype(networkType,
                                TelephonyManager->GetNetworkTypeName(networkType));
                    }
                    retVal = HANDLED;
                    break;

                case EVENT_RETRY_CONNECTION: {
                    If (msg.arg1 == mTag) {
                        mRetryManager->IncreaseRetryCount();
                        If (DBG) {
                            Log("DcRetryingState EVENT_RETRY_CONNECTION"
                                    + " RetryCount=" +  mRetryManager->GetRetryCount()
                                    + " mConnectionParams=" + mConnectionParams);
                        }
                        OnConnect(mConnectionParams);
                        TransitionTo(mActivatingState);
                    } else {
                        If (DBG) {
                            Log("DcRetryingState stale EVENT_RETRY_CONNECTION"
                                    + " tag:" + msg.arg1 + " != mTag:" + mTag);
                        }
                    }
                    retVal = HANDLED;
                    break;
                }
                case DcAsyncChannel.REQ_RESET: {
                    If (DBG) {
                        Log("DcRetryingState: msg.what=RSP_RESET, ignore we're already reset");
                    }
                    mInactiveState->SetEnterNotificationParams(mConnectionParams,
                            DcFailCause.RESET_BY_FRAMEWORK);
                    TransitionTo(mInactiveState);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_CONNECT: {
                    ConnectionParams cp = (ConnectionParams) msg.obj;
                    If (DBG) {
                        Log("DcRetryingState: msg.what=EVENT_CONNECT"
                                + " RefCount=" + mApnContexts->Size() + " cp=" + cp
                                + " mConnectionParams=" + mConnectionParams);
                    }
                    If (InitConnection(cp)) {
                        OnConnect(mConnectionParams);
                        TransitionTo(mActivatingState);
                    } else {
                        If (DBG) {
                            Log("DcRetryingState: msg.what=EVENT_CONNECT initConnection failed");
                        }
                        NotifyConnectCompleted(cp, DcFailCause.UNACCEPTABLE_NETWORK_PARAMETER,
                                FALSE);
                    }
                    retVal = HANDLED;
                    break;
                }
                case EVENT_DISCONNECT: {
                    DisconnectParams dp = (DisconnectParams) msg.obj;

                    If (mApnContexts->Remove(dp.mApnContext) && mApnContexts->Size() == 0) {
                        If (DBG) {
                            Log("DcRetryingState msg.what=EVENT_DISCONNECT " + " RefCount="
                                    + mApnContexts->Size() + " dp=" + dp);
                        }
                        mInactiveState->SetEnterNotificationParams(dp);
                        TransitionTo(mInactiveState);
                    } else {
                        If (DBG) Log("DcRetryingState: msg.what=EVENT_DISCONNECT");
                        NotifyDisconnectCompleted(dp, FALSE);
                    }
                    retVal = HANDLED;
                    break;
                }
                case EVENT_DISCONNECT_ALL: {
                    If (DBG) {
                        Log("DcRetryingState msg.what=EVENT_DISCONNECT/DISCONNECT_ALL "
                                + "RefCount=" + mApnContexts->Size());
                    }
                    mInactiveState->SetEnterNotificationParams(DcFailCause.LOST_CONNECTION);
                    TransitionTo(mInactiveState);
                    retVal = HANDLED;
                    break;
                }
                default: {
                    If (VDBG) {
                        Log("DcRetryingState nothandled msg.what=" + GetWhatToString(msg.what));
                    }
                    retVal = NOT_HANDLED;
                    break;
                }
            }
            return retVal;
        }
    }
    private DcRetryingState mRetryingState = new DcRetryingState();

    /**
     * The state machine is activating a connection.
     */
    private class DcActivatingState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;
            AsyncResult ar;
            ConnectionParams cp;

            If (DBG) Log("DcActivatingState: msg=" + MsgToString(msg));
            Switch (msg.what) {
                case EVENT_DATA_CONNECTION_DRS_OR_RAT_CHANGED:
                case EVENT_CONNECT:
                    // Activating can't process until we're done.
                    DeferMessage(msg);
                    retVal = HANDLED;
                    break;

                case EVENT_SETUP_DATA_CONNECTION_DONE:
                    ar = (AsyncResult) msg.obj;
                    cp = (ConnectionParams) ar.userObj;

                    DataCallResponse.SetupResult result = OnSetupConnectionCompleted(ar);
                    If (result != DataCallResponse.SetupResult.ERR_Stale) {
                        If (mConnectionParams != cp) {
                            Loge("DcActivatingState: WEIRD mConnectionsParams:"+ mConnectionParams
                                    + " != cp:" + cp);
                        }
                    }
                    If (DBG) {
                        Log("DcActivatingState onSetupConnectionCompleted result=" + result
                                + " dc=" + DataConnection.this);
                    }
                    Switch (result) {
                        case SUCCESS:
                            // All is well
                            mDcFailCause = DcFailCause.NONE;
                            TransitionTo(mActiveState);
                            break;
                        case ERR_BadCommand:
                            // Vendor ril rejected the command and didn't connect.
                            // Transition to inactive but send notifications after
                            // we've entered the mInactive state.
                            mInactiveState->SetEnterNotificationParams(cp, result.mFailCause);
                            TransitionTo(mInactiveState);
                            break;
                        case ERR_UnacceptableParameter:
                            // The addresses given from the RIL are bad
                            TearDownData(cp);
                            TransitionTo(mDisconnectingErrorCreatingConnection);
                            break;
                        case ERR_GetLastErrorFromRil:
                            // Request failed and this is an old RIL
                            mPhone.mCi->GetLastDataCallFailCause(
                                    ObtainMessage(EVENT_GET_LAST_FAIL_DONE, cp));
                            break;
                        case ERR_RilError:
                            Int32 delay = mDcRetryAlarmController->GetSuggestedRetryTime(
                                                                    DataConnection.this, ar);
                            If (DBG) {
                                Log("DcActivatingState: ERR_RilError "
                                        + " delay=" + delay
                                        + " isRetryNeeded=" + mRetryManager->IsRetryNeeded()
                                        + " result=" + result
                                        + " result.isRestartRadioFail=" +
                                        result.mFailCause->IsRestartRadioFail()
                                        + " result.isPermanentFail=" +
                                        mDct->IsPermanentFail(result.mFailCause));
                            }
                            If (result.mFailCause->IsRestartRadioFail()) {
                                If (DBG) Log("DcActivatingState: ERR_RilError restart radio");
                                mDct->SendRestartRadio();
                                mInactiveState->SetEnterNotificationParams(cp, result.mFailCause);
                                TransitionTo(mInactiveState);
                            } else If (mDct->IsPermanentFail(result.mFailCause)) {
                                If (DBG) Log("DcActivatingState: ERR_RilError perm error");
                                mInactiveState->SetEnterNotificationParams(cp, result.mFailCause);
                                TransitionTo(mInactiveState);
                            } else If (delay >= 0) {
                                If (DBG) Log("DcActivatingState: ERR_RilError retry");
                                mDcRetryAlarmController->StartRetryAlarm(EVENT_RETRY_CONNECTION,
                                                            mTag, delay);
                                TransitionTo(mRetryingState);
                            } else {
                                If (DBG) Log("DcActivatingState: ERR_RilError no retry");
                                mInactiveState->SetEnterNotificationParams(cp, result.mFailCause);
                                TransitionTo(mInactiveState);
                            }
                            break;
                        case ERR_Stale:
                            Loge("DcActivatingState: stale EVENT_SETUP_DATA_CONNECTION_DONE"
                                    + " tag:" + cp.mTag + " != mTag:" + mTag);
                            break;
                        default:
                            throw new RuntimeException("Unknown SetupResult, should not happen");
                    }
                    retVal = HANDLED;
                    break;

                case EVENT_GET_LAST_FAIL_DONE:
                    ar = (AsyncResult) msg.obj;
                    cp = (ConnectionParams) ar.userObj;
                    If (cp.mTag == mTag) {
                        If (mConnectionParams != cp) {
                            Loge("DcActivatingState: WEIRD mConnectionsParams:" + mConnectionParams
                                    + " != cp:" + cp);
                        }

                        DcFailCause cause = DcFailCause.UNKNOWN;

                        If (ar.exception == NULL) {
                            Int32 rilFailCause = ((Int32[]) (ar.result))[0];
                            cause = DcFailCause->FromInt(rilFailCause);
                            If (cause == DcFailCause.NONE) {
                                If (DBG) {
                                    Log("DcActivatingState msg.what=EVENT_GET_LAST_FAIL_DONE"
                                            + " BAD: error was NONE, change to UNKNOWN");
                                }
                                cause = DcFailCause.UNKNOWN;
                            }
                        }
                        mDcFailCause = cause;

                        Int32 retryDelay = mRetryManager->GetRetryTimer();
                        If (DBG) {
                            Log("DcActivatingState msg.what=EVENT_GET_LAST_FAIL_DONE"
                                    + " cause=" + cause
                                    + " retryDelay=" + retryDelay
                                    + " isRetryNeeded=" + mRetryManager->IsRetryNeeded()
                                    + " dc=" + DataConnection.this);
                        }
                        If (cause->IsRestartRadioFail()) {
                            If (DBG) {
                                Log("DcActivatingState: EVENT_GET_LAST_FAIL_DONE"
                                        + " restart radio");
                            }
                            mDct->SendRestartRadio();
                            mInactiveState->SetEnterNotificationParams(cp, cause);
                            TransitionTo(mInactiveState);
                        } else If (mDct->IsPermanentFail(cause)) {
                            If (DBG) Log("DcActivatingState: EVENT_GET_LAST_FAIL_DONE perm er");
                            mInactiveState->SetEnterNotificationParams(cp, cause);
                            TransitionTo(mInactiveState);
                        } else If ((retryDelay >= 0) && (mRetryManager->IsRetryNeeded())) {
                            If (DBG) Log("DcActivatingState: EVENT_GET_LAST_FAIL_DONE retry");
                            mDcRetryAlarmController->StartRetryAlarm(EVENT_RETRY_CONNECTION, mTag,
                                                            retryDelay);
                            TransitionTo(mRetryingState);
                        } else {
                            If (DBG) Log("DcActivatingState: EVENT_GET_LAST_FAIL_DONE no retry");
                            mInactiveState->SetEnterNotificationParams(cp, cause);
                            TransitionTo(mInactiveState);
                        }
                    } else {
                        Loge("DcActivatingState: stale EVENT_GET_LAST_FAIL_DONE"
                                + " tag:" + cp.mTag + " != mTag:" + mTag);
                    }

                    retVal = HANDLED;
                    break;

                default:
                    If (VDBG) {
                        Log("DcActivatingState not handled msg.what=" +
                                GetWhatToString(msg.what) + " RefCount=" + mApnContexts->Size());
                    }
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
    }
    private DcActivatingState mActivatingState = new DcActivatingState();

    /**
     * The state machine is connected, expecting an EVENT_DISCONNECT.
     */
    private class DcActiveState extends State {
        //@Override CARAPI Enter() {
            If (DBG) Log("DcActiveState: enter dc=" + DataConnection.this);

            If (mRetryManager->GetRetryCount() != 0) {
                Log("DcActiveState: connected after retrying call notifyAllOfConnected");
                mRetryManager->SetRetryCount(0);
            }
            // If we were retrying there maybe more than one, otherwise they'll only be one.
            NotifyAllOfConnected(Phone.REASON_CONNECTED);

            // If the EVENT_CONNECT set the current max retry restore it here
            // if it didn't then this is effectively a NOP.
            mRetryManager->RestoreCurMaxRetryCount();
            mDcController->AddActiveDcByCid(DataConnection.this);

            mNetworkInfo->SetDetailedState(NetworkInfo.DetailedState.CONNECTED,
                    mNetworkInfo->GetReason(), NULL);
            mNetworkInfo->SetExtraInfo(mApnSetting.apn);
            UpdateTcpBufferSizes(mRilRat);
            mNetworkAgent = new DcNetworkAgent(GetHandler()->GetLooper(),
                    mPhone->GetContext(),
                    "DcNetworkAgent" + mPhone->GetSubId(), mNetworkInfo,
                    MakeNetworkCapabilities(), mLinkProperties, 50);
        }

        //@Override
        CARAPI Exit() {
            If (DBG) Log("DcActiveState: exit dc=" + this);
            mNetworkInfo->SetDetailedState(NetworkInfo.DetailedState.DISCONNECTED,
                    mNetworkInfo->GetReason(), mNetworkInfo->GetExtraInfo());
            mNetworkAgent->SendNetworkInfo(mNetworkInfo);
            mNetworkAgent = NULL;
        }

        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;

            Switch (msg.what) {
                case EVENT_CONNECT: {
                    ConnectionParams cp = (ConnectionParams) msg.obj;
                    If (DBG) {
                        Log("DcActiveState: EVENT_CONNECT cp=" + cp + " dc=" + DataConnection.this);
                    }
                    If (mApnContexts->Contains(cp.mApnContext)) {
                        Log("DcActiveState ERROR already added apnContext=" + cp.mApnContext);
                    } else {
                        mApnContexts->Add(cp.mApnContext);
                        If (DBG) {
                            Log("DcActiveState msg.what=EVENT_CONNECT RefCount="
                                    + mApnContexts->Size());
                        }
                    }
                    NotifyConnectCompleted(cp, DcFailCause.NONE, FALSE);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_DISCONNECT: {
                    DisconnectParams dp = (DisconnectParams) msg.obj;
                    If (DBG) {
                        Log("DcActiveState: EVENT_DISCONNECT dp=" + dp
                                + " dc=" + DataConnection.this);
                    }
                    If (mApnContexts->Contains(dp.mApnContext)) {
                        If (DBG) {
                            Log("DcActiveState msg.what=EVENT_DISCONNECT RefCount="
                                    + mApnContexts->Size());
                        }

                        If (mApnContexts->Size() == 1) {
                            mApnContexts->Clear();
                            mDisconnectParams = dp;
                            mConnectionParams = NULL;
                            dp.mTag = mTag;
                            TearDownData(dp);
                            TransitionTo(mDisconnectingState);
                        } else {
                            mApnContexts->Remove(dp.mApnContext);
                            NotifyDisconnectCompleted(dp, FALSE);
                        }
                    } else {
                        Log("DcActiveState ERROR no such apnContext=" + dp.mApnContext
                                + " in this dc=" + DataConnection.this);
                        NotifyDisconnectCompleted(dp, FALSE);
                    }
                    retVal = HANDLED;
                    break;
                }
                case EVENT_DISCONNECT_ALL: {
                    If (DBG) {
                        Log("DcActiveState EVENT_DISCONNECT clearing apn contexts,"
                                + " dc=" + DataConnection.this);
                    }
                    DisconnectParams dp = (DisconnectParams) msg.obj;
                    mDisconnectParams = dp;
                    mConnectionParams = NULL;
                    dp.mTag = mTag;
                    TearDownData(dp);
                    TransitionTo(mDisconnectingState);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_LOST_CONNECTION: {
                    If (DBG) {
                        Log("DcActiveState EVENT_LOST_CONNECTION dc=" + DataConnection.this);
                    }
                    If (mRetryManager->IsRetryNeeded()) {
                        // We're going to retry
                        Int32 delayMillis = mRetryManager->GetRetryTimer();
                        If (DBG) {
                            Log("DcActiveState EVENT_LOST_CONNECTION startRetryAlarm"
                                    + " mTag=" + mTag + " delay=" + delayMillis + "ms");
                        }
                        mDcRetryAlarmController->StartRetryAlarm(EVENT_RETRY_CONNECTION, mTag,
                                delayMillis);
                        TransitionTo(mRetryingState);
                    } else {
                        mInactiveState->SetEnterNotificationParams(DcFailCause.LOST_CONNECTION);
                        TransitionTo(mInactiveState);
                    }
                    retVal = HANDLED;
                    break;
                }
                case EVENT_DATA_CONNECTION_ROAM_ON: {
                    mNetworkInfo->SetRoaming(TRUE);
                    mNetworkAgent->SendNetworkInfo(mNetworkInfo);
                    retVal = HANDLED;
                    break;
                }
                case EVENT_DATA_CONNECTION_ROAM_OFF: {
                    mNetworkInfo->SetRoaming(FALSE);
                    mNetworkAgent->SendNetworkInfo(mNetworkInfo);
                    retVal = HANDLED;
                    break;
                }
                default:
                    If (VDBG) {
                        Log("DcActiveState not handled msg.what=" + GetWhatToString(msg.what));
                    }
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
    }
    private DcActiveState mActiveState = new DcActiveState();

    /**
     * The state machine is disconnecting.
     */
    private class DcDisconnectingState extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;

            Switch (msg.what) {
                case EVENT_CONNECT:
                    If (DBG) Log("DcDisconnectingState msg.what=EVENT_CONNECT. Defer. RefCount = "
                            + mApnContexts->Size());
                    DeferMessage(msg);
                    retVal = HANDLED;
                    break;

                case EVENT_DEACTIVATE_DONE:
                    If (DBG) Log("DcDisconnectingState msg.what=EVENT_DEACTIVATE_DONE RefCount="
                            + mApnContexts->Size());
                    AsyncResult ar = (AsyncResult) msg.obj;
                    DisconnectParams dp = (DisconnectParams) ar.userObj;
                    If (dp.mTag == mTag) {
                        // Transition to inactive but send notifications after
                        // we've entered the mInactive state.
                        mInactiveState->SetEnterNotificationParams((DisconnectParams) ar.userObj);
                        TransitionTo(mInactiveState);
                    } else {
                        If (DBG) Log("DcDisconnectState stale EVENT_DEACTIVATE_DONE"
                                + " dp.tag=" + dp.mTag + " mTag=" + mTag);
                    }
                    retVal = HANDLED;
                    break;

                default:
                    If (VDBG) {
                        Log("DcDisconnectingState not handled msg.what="
                                + GetWhatToString(msg.what));
                    }
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
    }
    private DcDisconnectingState mDisconnectingState = new DcDisconnectingState();

    /**
     * The state machine is disconnecting after an creating a connection.
     */
    private class DcDisconnectionErrorCreatingConnection extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            Boolean retVal;

            Switch (msg.what) {
                case EVENT_DEACTIVATE_DONE:
                    AsyncResult ar = (AsyncResult) msg.obj;
                    ConnectionParams cp = (ConnectionParams) ar.userObj;
                    If (cp.mTag == mTag) {
                        If (DBG) {
                            Log("DcDisconnectionErrorCreatingConnection" +
                                " msg.what=EVENT_DEACTIVATE_DONE");
                        }

                        // Transition to inactive but send notifications after
                        // we've entered the mInactive state.
                        mInactiveState->SetEnterNotificationParams(cp,
                                DcFailCause.UNACCEPTABLE_NETWORK_PARAMETER);
                        TransitionTo(mInactiveState);
                    } else {
                        If (DBG) {
                            Log("DcDisconnectionErrorCreatingConnection stale EVENT_DEACTIVATE_DONE"
                                    + " dp.tag=" + cp.mTag + ", mTag=" + mTag);
                        }
                    }
                    retVal = HANDLED;
                    break;

                default:
                    If (VDBG) {
                        Log("DcDisconnectionErrorCreatingConnection not handled msg.what="
                                + GetWhatToString(msg.what));
                    }
                    retVal = NOT_HANDLED;
                    break;
            }
            return retVal;
        }
    }
    private DcDisconnectionErrorCreatingConnection mDisconnectingErrorCreatingConnection =
                new DcDisconnectionErrorCreatingConnection();


    private class DcNetworkAgent extends NetworkAgent {
        public DcNetworkAgent(Looper l, Context c, String TAG, NetworkInfo ni,
                NetworkCapabilities nc, LinkProperties lp, Int32 score) {
            Super(l, c, TAG, ni, nc, lp, score);
        }

        protected void Unwanted() {
            If (mNetworkAgent != this) {
                Log("unwanted found mNetworkAgent=" + mNetworkAgent +
                        ", which isn't me.  Aborting unwanted");
                return;
            }
            // this can only happen if our exit has been called - we're already disconnected
            If (mApnContexts == NULL) return;
            For (ApnContext apnContext : mApnContexts) {
                Message msg = mDct->ObtainMessage(DctConstants.EVENT_DISCONNECT_DONE, apnContext);
                DisconnectParams dp = new DisconnectParams(apnContext, apnContext->GetReason(), msg);
                DataConnection.this->SendMessage(DataConnection.this.
                        ObtainMessage(EVENT_DISCONNECT, dp));
            }
        }
    }

    // ******* "public" interface

    /**
     * Used for testing purposes.
     */
    /* package */ void TearDownNow() {
        If (DBG) Log("TearDownNow()");
        SendMessage(ObtainMessage(EVENT_TEAR_DOWN_NOW));
    }

    /**
     * @return the string for msg.what as our info.
     */
    //@Override
    protected String GetWhatToString(Int32 what) {
        return CmdToString(what);
    }

    private static String MsgToString(Message msg) {
        String retVal;
        If (msg == NULL) {
            retVal = "NULL";
        } else {
            StringBuilder   b = new StringBuilder();

            b->Append("{what=");
            b->Append(CmdToString(msg.what));

            b->Append(" when=");
            TimeUtils->FormatDuration(msg->GetWhen() - SystemClock->UptimeMillis(), b);

            If (msg.arg1 != 0) {
                b->Append(" arg1=");
                b->Append(msg.arg1);
            }

            If (msg.arg2 != 0) {
                b->Append(" arg2=");
                b->Append(msg.arg2);
            }

            If (msg.obj != NULL) {
                b->Append(" obj=");
                b->Append(msg.obj);
            }

            b->Append(" target=");
            b->Append(msg->GetTarget());

            b->Append(" replyTo=");
            b->Append(msg.replyTo);

            b->Append("}");

            retVal = b->ToString();
        }
        return retVal;
    }

    static void Slog(String s) {
        Rlog->D("DC", s);
    }

    /**
     * Log with debug
     *
     * @param s is string log
     */
    //@Override
    protected void Log(String s) {
        Rlog->D(GetName(), s);
    }

    /**
     * Log with debug attribute
     *
     * @param s is string log
     */
    //@Override
    protected void Logd(String s) {
        Rlog->D(GetName(), s);
    }

    /**
     * Log with verbose attribute
     *
     * @param s is string log
     */
    //@Override
    protected void Logv(String s) {
        Rlog->V(GetName(), s);
    }

    /**
     * Log with info attribute
     *
     * @param s is string log
     */
    //@Override
    protected void Logi(String s) {
        Rlog->I(GetName(), s);
    }

    /**
     * Log with warning attribute
     *
     * @param s is string log
     */
    //@Override
    protected void Logw(String s) {
        Rlog->W(GetName(), s);
    }

    /**
     * Log with error attribute
     *
     * @param s is string log
     */
    //@Override
    protected void Loge(String s) {
        Rlog->E(GetName(), s);
    }

    /**
     * Log with error attribute
     *
     * @param s is string log
     * @param e is a Throwable which logs additional information.
     */
    //@Override
    protected void Loge(String s, Throwable e) {
        Rlog->E(GetName(), s, e);
    }

    /** Doesn't print mApnList of ApnContext's which would be recursive */
    public String ToStringSimple() {
        return GetName() + ": State=" + GetCurrentState()->GetName()
                + " mApnSetting=" + mApnSetting + " RefCount=" + mApnContexts->Size()
                + " mCid=" + mCid + " mCreateTime=" + mCreateTime
                + " mLastastFailTime=" + mLastFailTime
                + " mLastFailCause=" + mLastFailCause
                + " mTag=" + mTag
                + " mRetryManager=" + mRetryManager
                + " mLinkProperties=" + mLinkProperties
                + " linkCapabilities=" + MakeNetworkCapabilities();
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return "{" + ToStringSimple() + " mApnContexts=" + mApnContexts + "}";
    }

    /**
     * Dump the current state.
     *
     * @param fd
     * @param pw
     * @param args
     */
    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Print("DataConnection ");
        super->Dump(fd, pw, args);
        pw->Println(" mApnContexts.size=" + mApnContexts->Size());
        pw->Println(" mApnContexts=" + mApnContexts);
        pw->Flush();
        pw->Println(" mDataConnectionTracker=" + mDct);
        pw->Println(" mApnSetting=" + mApnSetting);
        pw->Println(" mTag=" + mTag);
        pw->Println(" mCid=" + mCid);
        pw->Println(" mRetryManager=" + mRetryManager);
        pw->Println(" mConnectionParams=" + mConnectionParams);
        pw->Println(" mDisconnectParams=" + mDisconnectParams);
        pw->Println(" mDcFailCause=" + mDcFailCause);
        pw->Flush();
        pw->Println(" mPhone=" + mPhone);
        pw->Flush();
        pw->Println(" mLinkProperties=" + mLinkProperties);
        pw->Flush();
        pw->Println(" mDataRegState=" + mDataRegState);
        pw->Println(" mRilRat=" + mRilRat);
        pw->Println(" mNetworkCapabilities=" + MakeNetworkCapabilities());
        pw->Println(" mCreateTime=" + TimeUtils->LogTimeOfDay(mCreateTime));
        pw->Println(" mLastFailTime=" + TimeUtils->LogTimeOfDay(mLastFailTime));
        pw->Println(" mLastFailCause=" + mLastFailCause);
        pw->Flush();
        pw->Println(" mUserData=" + mUserData);
        pw->Println(" mInstanceNumber=" + mInstanceNumber);
        pw->Println(" mAc=" + mAc);
        pw->Println(" mDcRetryAlarmController=" + mDcRetryAlarmController);
        pw->Flush();
    }
}
