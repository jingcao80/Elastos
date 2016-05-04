/*
 * Copyright (C) 2011 The Android Open Source Project
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

using Elastos::Droid::Internal::Telephony::Dataconnection::DataConnection::IConnectionParams;
using Elastos::Droid::Internal::Telephony::Dataconnection::DataConnection::IDisconnectParams;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;

using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Os::IMessage;

/**
 * AsyncChannel to a DataConnection
 */
public class DcAsyncChannel extends AsyncChannel {
    private static const Boolean DBG = FALSE;
    private String mLogTag;

    private DataConnection mDc;
    private Int64 mDcThreadId;

    public static const Int32 BASE = Protocol.BASE_DATA_CONNECTION_AC;

    public static const Int32 REQ_IS_INACTIVE = BASE + 0;
    public static const Int32 RSP_IS_INACTIVE = BASE + 1;

    public static const Int32 REQ_GET_CID = BASE + 2;
    public static const Int32 RSP_GET_CID = BASE + 3;

    public static const Int32 REQ_GET_APNSETTING = BASE + 4;
    public static const Int32 RSP_GET_APNSETTING = BASE + 5;

    public static const Int32 REQ_GET_LINK_PROPERTIES = BASE + 6;
    public static const Int32 RSP_GET_LINK_PROPERTIES = BASE + 7;

    public static const Int32 REQ_SET_LINK_PROPERTIES_HTTP_PROXY = BASE + 8;
    public static const Int32 RSP_SET_LINK_PROPERTIES_HTTP_PROXY = BASE + 9;

    public static const Int32 REQ_GET_NETWORK_CAPABILITIES = BASE + 10;
    public static const Int32 RSP_GET_NETWORK_CAPABILITIES = BASE + 11;

    public static const Int32 REQ_RESET = BASE + 12;
    public static const Int32 RSP_RESET = BASE + 13;

    private static const Int32 CMD_TO_STRING_COUNT = RSP_RESET - BASE + 1;
    private static String[] sCmdToString = new String[CMD_TO_STRING_COUNT];
    static {
        sCmdToString[REQ_IS_INACTIVE - BASE] = "REQ_IS_INACTIVE";
        sCmdToString[RSP_IS_INACTIVE - BASE] = "RSP_IS_INACTIVE";
        sCmdToString[REQ_GET_CID - BASE] = "REQ_GET_CID";
        sCmdToString[RSP_GET_CID - BASE] = "RSP_GET_CID";
        sCmdToString[REQ_GET_APNSETTING - BASE] = "REQ_GET_APNSETTING";
        sCmdToString[RSP_GET_APNSETTING - BASE] = "RSP_GET_APNSETTING";
        sCmdToString[REQ_GET_LINK_PROPERTIES - BASE] = "REQ_GET_LINK_PROPERTIES";
        sCmdToString[RSP_GET_LINK_PROPERTIES - BASE] = "RSP_GET_LINK_PROPERTIES";
        sCmdToString[REQ_SET_LINK_PROPERTIES_HTTP_PROXY - BASE] =
                "REQ_SET_LINK_PROPERTIES_HTTP_PROXY";
        sCmdToString[RSP_SET_LINK_PROPERTIES_HTTP_PROXY - BASE] =
                "RSP_SET_LINK_PROPERTIES_HTTP_PROXY";
        sCmdToString[REQ_GET_NETWORK_CAPABILITIES - BASE] = "REQ_GET_NETWORK_CAPABILITIES";
        sCmdToString[RSP_GET_NETWORK_CAPABILITIES - BASE] = "RSP_GET_NETWORK_CAPABILITIES";
        sCmdToString[REQ_RESET - BASE] = "REQ_RESET";
        sCmdToString[RSP_RESET - BASE] = "RSP_RESET";
    }

    // Convert cmd to string or NULL if unknown
    protected static String CmdToString(Int32 cmd) {
        cmd -= BASE;
        If ((cmd >= 0) && (cmd < sCmdToString.length)) {
            return sCmdToString[cmd];
        } else {
            return AsyncChannel->CmdToString(cmd + BASE);
        }
    }

    /**
     * enum used to notify action taken or necessary to be
     * taken after the link property is changed.
     */
    public enum LinkPropertyChangeAction {
        NONE, CHANGED, RESET;

        public static LinkPropertyChangeAction FromInt(Int32 value) {
            If (value == NONE->Ordinal()) {
                return NONE;
            } else If (value == CHANGED->Ordinal()) {
                return CHANGED;
            } else If (value == RESET->Ordinal()) {
                return RESET;
            } else {
                throw new RuntimeException("LinkPropertyChangeAction.fromInt: bad value=" + value);
            }
        }
    }

    public DcAsyncChannel(DataConnection dc, String logTag) {
        mDc = dc;
        mDcThreadId = mDc->GetHandler()->GetLooper().GetThread()->GetId();
        mLogTag = logTag;
    }

    /**
     * Request if the state machine is in the inactive state.
     * Response {@link #rspIsInactive}
     */
    CARAPI ReqIsInactive() {
        SendMessage(REQ_IS_INACTIVE);
        If (DBG) Log("reqIsInactive");
    }

    /**
     * Evaluate RSP_IS_INACTIVE.
     *
     * @return TRUE if the state machine is in the inactive state.
     */
    public Boolean RspIsInactive(Message response) {
        Boolean retVal = response.arg1 == 1;
        If (DBG) Log("rspIsInactive=" + retVal);
        return retVal;
    }

    /**
     * @return TRUE if the state machine is in the inactive state
     * and can be used for a new connection.
     */
    public Boolean IsInactiveSync() {
        Boolean value;
        If (IsCallerOnDifferentThread()) {
            Message response = SendMessageSynchronously(REQ_IS_INACTIVE);
            If ((response != NULL) && (response.what == RSP_IS_INACTIVE)) {
                value = RspIsInactive(response);
            } else {
                Log("rspIsInactive error response=" + response);
                value = FALSE;
            }
        } else {
            value = mDc->GetIsInactive();
        }
        return value;
    }

    /**
     * Request the Connection ID.
     * Response {@link #rspCid}
     */
    CARAPI ReqCid() {
        SendMessage(REQ_GET_CID);
        If (DBG) Log("reqCid");
    }

    /**
     * Evaluate a RSP_GET_CID message and return the cid.
     *
     * @param response Message
     * @return connection id or -1 if an error
     */
    public Int32 RspCid(Message response) {
        Int32 retVal = response.arg1;
        If (DBG) Log("rspCid=" + retVal);
        return retVal;
    }

    /**
     * @return connection id or -1 if an error
     */
    public Int32 GetCidSync() {
        Int32 value;
        If (IsCallerOnDifferentThread()) {
            Message response = SendMessageSynchronously(REQ_GET_CID);
            If ((response != NULL) && (response.what == RSP_GET_CID)) {
                value = RspCid(response);
            } else {
                Log("rspCid error response=" + response);
                value = -1;
            }
        } else {
            value = mDc->GetCid();
        }
        return value;
    }

    /**
     * Request the connections ApnSetting.
     * Response {@link #rspApnSetting}
     */
    CARAPI ReqApnSetting() {
        SendMessage(REQ_GET_APNSETTING);
        If (DBG) Log("reqApnSetting");
    }

    /**
     * Evaluate a RSP_APN_SETTING message and return the ApnSetting.
     *
     * @param response Message
     * @return ApnSetting, maybe NULL
     */
    public ApnSetting RspApnSetting(Message response) {
        ApnSetting retVal = (ApnSetting) response.obj;
        If (DBG) Log("rspApnSetting=" + retVal);
        return retVal;
    }

    /**
     * Get the connections ApnSetting.
     *
     * @return ApnSetting or NULL if an error
     */
    public ApnSetting GetApnSettingSync() {
        ApnSetting value;
        If (IsCallerOnDifferentThread()) {
            Message response = SendMessageSynchronously(REQ_GET_APNSETTING);
            If ((response != NULL) && (response.what == RSP_GET_APNSETTING)) {
                value = RspApnSetting(response);
            } else {
                Log("getApnSetting error response=" + response);
                value = NULL;
            }
        } else {
            value = mDc->GetApnSetting();
        }
        return value;
    }

    /**
     * Request the connections LinkProperties.
     * Response {@link #rspLinkProperties}
     */
    CARAPI ReqLinkProperties() {
        SendMessage(REQ_GET_LINK_PROPERTIES);
        If (DBG) Log("reqLinkProperties");
    }

    /**
     * Evaluate RSP_GET_LINK_PROPERTIES
     *
     * @param response
     * @return LinkProperties, maybe NULL.
     */
    public LinkProperties RspLinkProperties(Message response) {
        LinkProperties retVal = (LinkProperties) response.obj;
        If (DBG) Log("rspLinkProperties=" + retVal);
        return retVal;
    }

    /**
     * Get the connections LinkProperties.
     *
     * @return LinkProperties or NULL if an error
     */
    public LinkProperties GetLinkPropertiesSync() {
        LinkProperties value;
        If (IsCallerOnDifferentThread()) {
            Message response = SendMessageSynchronously(REQ_GET_LINK_PROPERTIES);
            If ((response != NULL) && (response.what == RSP_GET_LINK_PROPERTIES)) {
                value = RspLinkProperties(response);
            } else {
                Log("getLinkProperties error response=" + response);
                value = NULL;
            }
        } else {
            value = mDc->GetCopyLinkProperties();
        }
        return value;
    }

    /**
     * Request setting the connections LinkProperties.HttpProxy.
     * Response RSP_SET_LINK_PROPERTIES when complete.
     */
    CARAPI ReqSetLinkPropertiesHttpProxy(ProxyInfo proxy) {
        SendMessage(REQ_SET_LINK_PROPERTIES_HTTP_PROXY, proxy);
        If (DBG) Log("reqSetLinkPropertiesHttpProxy proxy=" + proxy);
    }

    /**
     * Set the connections LinkProperties.HttpProxy
     */
    CARAPI SetLinkPropertiesHttpProxySync(ProxyInfo proxy) {
        If (IsCallerOnDifferentThread()) {
            Message response =
                SendMessageSynchronously(REQ_SET_LINK_PROPERTIES_HTTP_PROXY, proxy);
            If ((response != NULL) && (response.what == RSP_SET_LINK_PROPERTIES_HTTP_PROXY)) {
                If (DBG) Log("setLinkPropertiesHttpPoxy ok");
            } else {
                Log("setLinkPropertiesHttpPoxy error response=" + response);
            }
        } else {
            mDc->SetLinkPropertiesHttpProxy(proxy);
        }
    }

    /**
     * Request the connections NetworkCapabilities.
     * Response {@link #rspNetworkCapabilities}
     */
    CARAPI ReqNetworkCapabilities() {
        SendMessage(REQ_GET_NETWORK_CAPABILITIES);
        If (DBG) Log("reqNetworkCapabilities");
    }

    /**
     * Evaluate RSP_GET_NETWORK_CAPABILITIES
     *
     * @param response
     * @return NetworkCapabilites, maybe NULL.
     */
    public NetworkCapabilities RspNetworkCapabilities(Message response) {
        NetworkCapabilities retVal = (NetworkCapabilities) response.obj;
        If (DBG) Log("rspNetworkCapabilities=" + retVal);
        return retVal;
    }

    /**
     * Get the connections NetworkCapabilities.
     *
     * @return NetworkCapabilities or NULL if an error
     */
    public NetworkCapabilities GetNetworkCapabilitiesSync() {
        NetworkCapabilities value;
        If (IsCallerOnDifferentThread()) {
            Message response = SendMessageSynchronously(REQ_GET_NETWORK_CAPABILITIES);
            If ((response != NULL) && (response.what == RSP_GET_NETWORK_CAPABILITIES)) {
                value = RspNetworkCapabilities(response);
            } else {
                value = NULL;
            }
        } else {
            value = mDc->GetCopyNetworkCapabilities();
        }
        return value;
    }

    /**
     * Response RSP_RESET when complete
     */
    CARAPI ReqReset() {
        SendMessage(REQ_RESET);
        If (DBG) Log("reqReset");
    }

    /**
     * Bring up a connection to the apn and return an AsyncResult in onCompletedMsg.
     * Used for cellular networks that use Acesss Point Names (APN) such
     * as GSM networks.
     *
     * @param apnContext is the Access Point Name to bring up a connection to
     * @param initialMaxRetry the number of retires for initial bringup.
     * @param profileId for the conneciton
     * @param onCompletedMsg is sent with its msg.obj as an AsyncResult object.
     *        With AsyncResult.userObj set to the original msg.obj,
     *        AsyncResult.result = FailCause and AsyncResult.exception = Exception().
     */
    CARAPI BringUp(ApnContext apnContext, Int32 initialMaxRetry, Int32 profileId,
            Int32 rilRadioTechnology, Boolean retryWhenSSChange, Message onCompletedMsg) {
        If (DBG) {
            Log("bringUp: apnContext=" + apnContext + " initialMaxRetry=" + initialMaxRetry
                + " onCompletedMsg=" + onCompletedMsg);
        }
        SendMessage(DataConnection.EVENT_CONNECT,
                    new ConnectionParams(apnContext, initialMaxRetry, profileId,
                            rilRadioTechnology, retryWhenSSChange, onCompletedMsg));
    }

    /**
     * Tear down the connection through the apn on the network.
     *
     * @param onCompletedMsg is sent with its msg.obj as an AsyncResult object.
     *        With AsyncResult.userObj set to the original msg.obj.
     */
    CARAPI TearDown(ApnContext apnContext, String reason, Message onCompletedMsg) {
        If (DBG) {
            Log("tearDown: apnContext=" + apnContext
                    + " reason=" + reason + " onCompletedMsg=" + onCompletedMsg);
        }
        SendMessage(DataConnection.EVENT_DISCONNECT,
                        new DisconnectParams(apnContext, reason, onCompletedMsg));
    }

    /**
     * Tear down the connection through the apn on the network.  Ignores refcount and
     * and always tears down.
     *
     * @param onCompletedMsg is sent with its msg.obj as an AsyncResult object.
     *        With AsyncResult.userObj set to the original msg.obj.
     */
    CARAPI TearDownAll(String reason, Message onCompletedMsg) {
        If (DBG) Log("tearDownAll: reason=" + reason + " onCompletedMsg=" + onCompletedMsg);
        SendMessage(DataConnection.EVENT_DISCONNECT_ALL,
                new DisconnectParams(NULL, reason, onCompletedMsg));
    }

    /**
     * @return connection id
     */
    public Int32 GetDataConnectionIdSync() {
        // Safe because this is owned by the caller.
        return mDc->GetDataConnectionId();
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return mDc->GetName();
    }

    private Boolean IsCallerOnDifferentThread() {
        Int64 curThreadId = Thread->CurrentThread()->GetId();
        Boolean value = mDcThreadId != curThreadId;
        If (DBG) Log("isCallerOnDifferentThread: " + value);
        return value;
    }

    private void Log(String s) {
        android.telephony.Rlog->D(mLogTag, "DataConnectionAc " + s);
    }

    public String[] GetPcscfAddr() {
        return mDc.mPcscfAddr;
    }
}
