/*
* Copyright (C) 2011-2014 MediaTek Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Utility::ILog;

public class DcSwitchAsyncChannel extends AsyncChannel {
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE;
    private static const String LOG_TAG = "DcSwitchAsyncChannel";

    private Int32 tagId = 0;
    private DcSwitchState mDcSwitchState;

    // ***** Event codes for driving the state machine
    private static const Int32 BASE = Protocol.BASE_DATA_CONNECTION_TRACKER + 0x00002000;
    static const Int32 REQ_CONNECT = BASE + 0;
    static const Int32 RSP_CONNECT = BASE + 1;
    static const Int32 REQ_DISCONNECT = BASE + 2;
    static const Int32 RSP_DISCONNECT = BASE + 3;
    static const Int32 REQ_IS_IDLE_STATE = BASE + 4;
    static const Int32 RSP_IS_IDLE_STATE = BASE + 5;
    static const Int32 REQ_IS_IDLE_OR_DEACTING_STATE = BASE + 6;
    static const Int32 RSP_IS_IDLE_OR_DEACTING_STATE = BASE + 7;

    private static const Int32 CMD_TO_STRING_COUNT = RSP_IS_IDLE_OR_DEACTING_STATE - BASE + 1;
    private static String[] sCmdToString = new String[CMD_TO_STRING_COUNT];
    static {
        sCmdToString[REQ_CONNECT - BASE] = "REQ_CONNECT";
        sCmdToString[RSP_CONNECT - BASE] = "RSP_CONNECT";
        sCmdToString[REQ_DISCONNECT - BASE] = "REQ_DISCONNECT";
        sCmdToString[RSP_DISCONNECT - BASE] = "RSP_DISCONNECT";
        sCmdToString[REQ_IS_IDLE_STATE - BASE] = "REQ_IS_IDLE_STATE";
        sCmdToString[RSP_IS_IDLE_STATE - BASE] = "RSP_IS_IDLE_STATE";
        sCmdToString[REQ_IS_IDLE_OR_DEACTING_STATE - BASE] = "REQ_IS_IDLE_OR_DEACTING_STATE";
        sCmdToString[RSP_IS_IDLE_OR_DEACTING_STATE - BASE] = "RSP_IS_IDLE_OR_DEACTING_STATE";
    }

    protected static String CmdToString(Int32 cmd) {
        cmd -= BASE;
        If ((cmd >= 0) && (cmd < sCmdToString.length)) {
            return sCmdToString[cmd];
        } else {
            return AsyncChannel->CmdToString(cmd + BASE);
        }
    }

    public DcSwitchAsyncChannel(DcSwitchState dcSwitchState, Int32 id) {
        mDcSwitchState = dcSwitchState;
        tagId = id;
    }

    CARAPI ReqConnect(String type) {
        SendMessage(REQ_CONNECT, type);
        If (DBG) Log("reqConnect");
    }

    public Int32 RspConnect(Message response) {
        Int32 retVal = response.arg1;
        If (DBG) Log("rspConnect=" + retVal);
        return retVal;
    }

    public Int32 ConnectSync(String type) {
        Message response = SendMessageSynchronously(REQ_CONNECT, type);
        If ((response != NULL) && (response.what == RSP_CONNECT)) {
            return RspConnect(response);
        } else {
            Log("rspConnect error response=" + response);
            return PhoneConstants.APN_REQUEST_FAILED;
        }
    }

    CARAPI ReqDisconnect(String type) {
        SendMessage(REQ_DISCONNECT, type);
        If (DBG) Log("reqDisconnect");
    }

    public Int32 RspDisconnect(Message response) {
        Int32 retVal = response.arg1;
        If (DBG) Log("rspDisconnect=" + retVal);
        return retVal;
    }

    public Int32 DisconnectSync(String type) {
        Message response = SendMessageSynchronously(REQ_DISCONNECT, type);
        If ((response != NULL) && (response.what == RSP_DISCONNECT)) {
            return RspDisconnect(response);
        } else {
            Log("rspDisconnect error response=" + response);
            return PhoneConstants.APN_REQUEST_FAILED;
        }
    }

    CARAPI ReqIsIdle() {
        SendMessage(REQ_IS_IDLE_STATE);
        If (DBG) Log("reqIsIdle");
    }

    public Boolean RspIsIdle(Message response) {
        Boolean retVal = response.arg1 == 1;
        If (DBG) Log("rspIsIdle=" + retVal);
        return retVal;
    }

    public Boolean IsIdleSync() {
        Message response = SendMessageSynchronously(REQ_IS_IDLE_STATE);
        If ((response != NULL) && (response.what == RSP_IS_IDLE_STATE)) {
            return RspIsIdle(response);
        } else {
            Log("rspIsIndle error response=" + response);
            return FALSE;
        }
    }

    CARAPI ReqIsIdleOrDeacting() {
        SendMessage(REQ_IS_IDLE_OR_DEACTING_STATE);
        If (DBG) Log("reqIsIdleOrDeacting");
    }

    public Boolean RspIsIdleOrDeacting(Message response) {
        Boolean retVal = response.arg1 == 1;
        If (DBG) Log("rspIsIdleOrDeacting=" + retVal);
        return retVal;
    }

    public Boolean IsIdleOrDeactingSync() {
        Message response = SendMessageSynchronously(REQ_IS_IDLE_OR_DEACTING_STATE);
        If ((response != NULL) && (response.what == RSP_IS_IDLE_OR_DEACTING_STATE)) {
            return RspIsIdleOrDeacting(response);
        } else {
            Log("rspIsIndleOrDeacting error response=" + response);
            return FALSE;
        }
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        return mDcSwitchState->GetName();
    }

    private void Log(String s) {
        Logger::D(LOG_TAG, "[DcSwitchAsyncChannel-" + tagId + "]: " + s);
    }
}
