/*
 * Copyright (C) 2007 The Android Open Source Project
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

package com.android.internal.telephony.cat;

using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Internal::Utility::IState;
using Elastos::Droid::Internal::Utility::IStateMachine;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::ITelephonyManager;

/**
 * Class used for queuing raw ril messages, decoding them into CommanParams
 * objects and sending the result back to the CAT Service.
 */
class RilMessageDecoder extends StateMachine {

    // constants
    private static const Int32 CMD_START = 1;
    private static const Int32 CMD_PARAMS_READY = 2;

    // members
    private CommandParamsFactory mCmdParamsFactory = NULL;
    private RilMessage mCurrentRilMessage = NULL;
    private Handler mCaller = NULL;
    private static Int32 mSimCount = 0;
    private static RilMessageDecoder[] mInstance = NULL;

    // States
    private StateStart mStateStart = new StateStart();
    private StateCmdParamsReady mStateCmdParamsReady = new StateCmdParamsReady();

    /**
     * Get the singleton instance, constructing if necessary.
     *
     * @param caller
     * @param fh
     * @return RilMesssageDecoder
     */
    public static synchronized RilMessageDecoder GetInstance(Handler caller, IccFileHandler fh,
            Int32 slotId) {
        If (NULL == mInstance) {
            mSimCount = TelephonyManager->GetDefault()->GetSimCount();
            mInstance = new RilMessageDecoder[mSimCount];
            For (Int32 i = 0; i < mSimCount; i++) {
                mInstance[i] = NULL;
            }
        }

        If (slotId != SubscriptionManager.INVALID_SLOT_ID && slotId < mSimCount) {
            If (NULL == mInstance[slotId]) {
                mInstance[slotId] = new RilMessageDecoder(caller, fh);
            }
        } else {
            CatLogger::D("RilMessageDecoder", "invaild slot id: " + slotId);
            return NULL;
        }

        return mInstance[slotId];
    }

    /**
     * Start decoding the message parameters,
     * when complete MSG_ID_RIL_MSG_DECODED will be returned to caller.
     *
     * @param rilMsg
     */
    CARAPI SendStartDecodingMessageParams(RilMessage rilMsg) {
        Message msg = ObtainMessage(CMD_START);
        msg.obj = rilMsg;
        SendMessage(msg);
    }

    /**
     * The command parameters have been decoded.
     *
     * @param resCode
     * @param cmdParams
     */
    CARAPI SendMsgParamsDecoded(ResultCode resCode, CommandParams cmdParams) {
        Message msg = ObtainMessage(RilMessageDecoder.CMD_PARAMS_READY);
        msg.arg1 = resCode->Value();
        msg.obj = cmdParams;
        SendMessage(msg);
    }

    private void SendCmdForExecution(RilMessage rilMsg) {
        Message msg = mCaller->ObtainMessage(CatService.MSG_ID_RIL_MSG_DECODED,
                new RilMessage(rilMsg));
        msg->SendToTarget();
    }

    private RilMessageDecoder(Handler caller, IccFileHandler fh) {
        Super("RilMessageDecoder");

        AddState(mStateStart);
        AddState(mStateCmdParamsReady);
        SetInitialState(mStateStart);

        mCaller = caller;
        mCmdParamsFactory = CommandParamsFactory->GetInstance(this, fh);
    }

    private RilMessageDecoder() {
        Super("RilMessageDecoder");
    }

    private class StateStart extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            If (msg.what == CMD_START) {
                If (DecodeMessageParams((RilMessage)msg.obj)) {
                    TransitionTo(mStateCmdParamsReady);
                }
            } else {
                CatLogger::D(this, "StateStart unexpected expecting START=" +
                         CMD_START + " got " + msg.what);
            }
            return TRUE;
        }
    }

    private class StateCmdParamsReady extends State {
        //@Override
        public Boolean ProcessMessage(Message msg) {
            If (msg.what == CMD_PARAMS_READY) {
                mCurrentRilMessage.mResCode = ResultCode->FromInt(msg.arg1);
                mCurrentRilMessage.mData = msg.obj;
                SendCmdForExecution(mCurrentRilMessage);
                TransitionTo(mStateStart);
            } else {
                CatLogger::D(this, "StateCmdParamsReady expecting CMD_PARAMS_READY="
                         + CMD_PARAMS_READY + " got " + msg.what);
                DeferMessage(msg);
            }
            return TRUE;
        }
    }

    private Boolean DecodeMessageParams(RilMessage rilMsg) {
        Boolean decodingStarted;

        mCurrentRilMessage = rilMsg;
        Switch(rilMsg.mId) {
        case CatService.MSG_ID_SESSION_END:
        case CatService.MSG_ID_CALL_SETUP:
            mCurrentRilMessage.mResCode = ResultCode.OK;
            SendCmdForExecution(mCurrentRilMessage);
            decodingStarted = FALSE;
            break;
        case CatService.MSG_ID_PROACTIVE_COMMAND:
        case CatService.MSG_ID_EVENT_NOTIFY:
        case CatService.MSG_ID_REFRESH:
            Byte[] rawData = NULL;
            try {
                rawData = IccUtils->HexStringToBytes((String) rilMsg.mData);
            } Catch (Exception e) {
                // zombie messages are dropped
                CatLogger::D(this, "decodeMessageParams dropping zombie messages");
                decodingStarted = FALSE;
                break;
            }
            try {
                // Start asynch parsing of the command parameters.
                mCmdParamsFactory->Make(BerTlv->Decode(rawData));
                decodingStarted = TRUE;
            } Catch (ResultException e) {
                // send to Service for proper RIL communication.
                CatLogger::D(this, "decodeMessageParams: caught ResultException e=" + e);
                mCurrentRilMessage.mResCode = e->Result();
                SendCmdForExecution(mCurrentRilMessage);
                decodingStarted = FALSE;
            }
            break;
        default:
            decodingStarted = FALSE;
            break;
        }
        return decodingStarted;
    }

    CARAPI Dispose(Int32 slotId) {
        QuitNow();
        mStateStart = NULL;
        mStateCmdParamsReady = NULL;
        mCmdParamsFactory->Dispose();
        mCmdParamsFactory = NULL;
        mCurrentRilMessage = NULL;
        mCaller = NULL;
        mInstance[slotId] = NULL;
    }
}
