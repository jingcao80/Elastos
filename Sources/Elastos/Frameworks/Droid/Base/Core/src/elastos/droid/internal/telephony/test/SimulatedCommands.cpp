/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.internal.telephony.test;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::IBaseCommands;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDataCallResponse;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDataProfile;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IUUSInfo;
using Elastos::Droid::Internal::Telephony::Gsm::ICallFailCause;
using Elastos::Droid::Internal::Telephony::Gsm::ISmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Gsm::ISuppServiceNotification;

using Elastos::Utility::IArrayList;

public class SimulatedCommands extends BaseCommands
        implements CommandsInterface, SimulatedRadioControl {
    private static const String LOG_TAG = "SimulatedCommands";

    private enum SimLockState {
        NONE,
        REQUIRE_PIN,
        REQUIRE_PUK,
        SIM_PERM_LOCKED
    }

    private enum SimFdnState {
        NONE,
        REQUIRE_PIN2,
        REQUIRE_PUK2,
        SIM_PERM_LOCKED
    }

    private static const SimLockState INITIAL_LOCK_STATE = SimLockState.NONE;
    private static const String DEFAULT_SIM_PIN_CODE = "1234";
    private static const String SIM_PUK_CODE = "12345678";
    private static const SimFdnState INITIAL_FDN_STATE = SimFdnState.NONE;
    private static const String DEFAULT_SIM_PIN2_CODE = "5678";
    private static const String SIM_PUK2_CODE = "87654321";

    //***** Instance Variables

    SimulatedGsmCallState simulatedCallState;
    HandlerThread mHandlerThread;
    SimLockState mSimLockedState;
    Boolean mSimLockEnabled;
    Int32 mPinUnlockAttempts;
    Int32 mPukUnlockAttempts;
    String mPinCode;
    SimFdnState mSimFdnEnabledState;
    Boolean mSimFdnEnabled;
    Int32 mPin2UnlockAttempts;
    Int32 mPuk2UnlockAttempts;
    Int32 mNetworkType;
    String mPin2Code;
    Boolean mSsnNotifyOn = FALSE;

    Int32 mPausedResponseCount;
    ArrayList<Message> mPausedResponses = new ArrayList<Message>();

    Int32 mNextCallFailCause = CallFailCause.NORMAL_CLEARING;

    //***** Constructor

    public
    SimulatedCommands() {
        Super(NULL);  // Don't log statistics
        mHandlerThread = new HandlerThread("SimulatedCommands");
        mHandlerThread->Start();
        Looper looper = mHandlerThread->GetLooper();

        simulatedCallState = new SimulatedGsmCallState(looper);

        SetRadioState(RadioState.RADIO_OFF);
        mSimLockedState = INITIAL_LOCK_STATE;
        mSimLockEnabled = (mSimLockedState != SimLockState.NONE);
        mPinCode = DEFAULT_SIM_PIN_CODE;
        mSimFdnEnabledState = INITIAL_FDN_STATE;
        mSimFdnEnabled = (mSimFdnEnabledState != SimFdnState.NONE);
        mPin2Code = DEFAULT_SIM_PIN2_CODE;
    }

    //***** CommandsInterface implementation

    //@Override
    CARAPI GetIccCardStatus(Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI SupplyIccPin(String pin, Message result)  {
        If (mSimLockedState != SimLockState.REQUIRE_PIN) {
            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPin: wrong state, state=" +
                    mSimLockedState);
            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
            return;
        }

        If (pin != NULL && pin->Equals(mPinCode)) {
            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPin: success!");
            mPinUnlockAttempts = 0;
            mSimLockedState = SimLockState.NONE;
            mIccStatusChangedRegistrants->NotifyRegistrants();

            If (result != NULL) {
                AsyncResult->ForMessage(result, NULL, NULL);
                result->SendToTarget();
            }

            return;
        }

        If (result != NULL) {
            mPinUnlockAttempts ++;

            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPin: failed! attempt=" +
                    mPinUnlockAttempts);
            If (mPinUnlockAttempts >= 3) {
                Rlog->I(LOG_TAG, "[SimCmd] supplyIccPin: set state to REQUIRE_PUK");
                mSimLockedState = SimLockState.REQUIRE_PUK;
            }

            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
        }
    }

    //@Override
    CARAPI SupplyIccPuk(String puk, String newPin, Message result)  {
        If (mSimLockedState != SimLockState.REQUIRE_PUK) {
            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPuk: wrong state, state=" +
                    mSimLockedState);
            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
            return;
        }

        If (puk != NULL && puk->Equals(SIM_PUK_CODE)) {
            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPuk: success!");
            mSimLockedState = SimLockState.NONE;
            mPukUnlockAttempts = 0;
            mIccStatusChangedRegistrants->NotifyRegistrants();

            If (result != NULL) {
                AsyncResult->ForMessage(result, NULL, NULL);
                result->SendToTarget();
            }

            return;
        }

        If (result != NULL) {
            mPukUnlockAttempts ++;

            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPuk: failed! attempt=" +
                    mPukUnlockAttempts);
            If (mPukUnlockAttempts >= 10) {
                Rlog->I(LOG_TAG, "[SimCmd] supplyIccPuk: set state to SIM_PERM_LOCKED");
                mSimLockedState = SimLockState.SIM_PERM_LOCKED;
            }

            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
        }
    }

    //@Override
    CARAPI SupplyIccPin2(String pin2, Message result)  {
        If (mSimFdnEnabledState != SimFdnState.REQUIRE_PIN2) {
            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPin2: wrong state, state=" +
                    mSimFdnEnabledState);
            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
            return;
        }

        If (pin2 != NULL && pin2->Equals(mPin2Code)) {
            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPin2: success!");
            mPin2UnlockAttempts = 0;
            mSimFdnEnabledState = SimFdnState.NONE;

            If (result != NULL) {
                AsyncResult->ForMessage(result, NULL, NULL);
                result->SendToTarget();
            }

            return;
        }

        If (result != NULL) {
            mPin2UnlockAttempts ++;

            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPin2: failed! attempt=" +
                    mPin2UnlockAttempts);
            If (mPin2UnlockAttempts >= 3) {
                Rlog->I(LOG_TAG, "[SimCmd] supplyIccPin2: set state to REQUIRE_PUK2");
                mSimFdnEnabledState = SimFdnState.REQUIRE_PUK2;
            }

            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
        }
    }

    //@Override
    CARAPI SupplyIccPuk2(String puk2, String newPin2, Message result)  {
        If (mSimFdnEnabledState != SimFdnState.REQUIRE_PUK2) {
            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPuk2: wrong state, state=" +
                    mSimLockedState);
            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
            return;
        }

        If (puk2 != NULL && puk2->Equals(SIM_PUK2_CODE)) {
            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPuk2: success!");
            mSimFdnEnabledState = SimFdnState.NONE;
            mPuk2UnlockAttempts = 0;

            If (result != NULL) {
                AsyncResult->ForMessage(result, NULL, NULL);
                result->SendToTarget();
            }

            return;
        }

        If (result != NULL) {
            mPuk2UnlockAttempts ++;

            Rlog->I(LOG_TAG, "[SimCmd] supplyIccPuk2: failed! attempt=" +
                    mPuk2UnlockAttempts);
            If (mPuk2UnlockAttempts >= 10) {
                Rlog->I(LOG_TAG, "[SimCmd] supplyIccPuk2: set state to SIM_PERM_LOCKED");
                mSimFdnEnabledState = SimFdnState.SIM_PERM_LOCKED;
            }

            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
        }
    }

    //@Override
    CARAPI ChangeIccPin(String oldPin, String newPin, Message result)  {
        If (oldPin != NULL && oldPin->Equals(mPinCode)) {
            mPinCode = newPin;
            If (result != NULL) {
                AsyncResult->ForMessage(result, NULL, NULL);
                result->SendToTarget();
            }

            return;
        }

        If (result != NULL) {
            Rlog->I(LOG_TAG, "[SimCmd] changeIccPin: pin failed!");

            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
        }
    }

    //@Override
    CARAPI ChangeIccPin2(String oldPin2, String newPin2, Message result)  {
        If (oldPin2 != NULL && oldPin2->Equals(mPin2Code)) {
            mPin2Code = newPin2;
            If (result != NULL) {
                AsyncResult->ForMessage(result, NULL, NULL);
                result->SendToTarget();
            }

            return;
        }

        If (result != NULL) {
            Rlog->I(LOG_TAG, "[SimCmd] changeIccPin2: pin2 failed!");

            CommandException ex = new CommandException(
                    CommandException.Error.PASSWORD_INCORRECT);
            AsyncResult->ForMessage(result, NULL, ex);
            result->SendToTarget();
        }
    }

    //@Override
    CARAPI
    ChangeBarringPassword(String facility, String oldPwd, String newPwd, Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI
    SetSuppServiceNotifications(Boolean enable, Message result) {
        ResultSuccess(result, NULL);

        If (enable && mSsnNotifyOn) {
            Rlog->W(LOG_TAG, "Supp Service Notifications already enabled!");
        }

        mSsnNotifyOn = enable;
    }

    //@Override
    CARAPI QueryFacilityLock(String facility, String pin,
                                   Int32 serviceClass, Message result) {
        QueryFacilityLockForApp(facility, pin, serviceClass, NULL, result);
    }

    //@Override
    CARAPI QueryFacilityLockForApp(String facility, String pin, Int32 serviceClass,
            String appId, Message result) {
        If (facility != NULL && facility->Equals(CommandsInterface.CB_FACILITY_BA_SIM)) {
            If (result != NULL) {
                Int32[] r = new Int32[1];
                r[0] = (mSimLockEnabled ? 1 : 0);
                Rlog->I(LOG_TAG, "[SimCmd] queryFacilityLock: SIM is "
                        + (r[0] == 0 ? "unlocked" : "locked"));
                AsyncResult->ForMessage(result, r, NULL);
                result->SendToTarget();
            }
            return;
        } else If (facility != NULL && facility->Equals(CommandsInterface.CB_FACILITY_BA_FD)) {
            If (result != NULL) {
                Int32[] r = new Int32[1];
                r[0] = (mSimFdnEnabled ? 1 : 0);
                Rlog->I(LOG_TAG, "[SimCmd] queryFacilityLock: FDN is "
                        + (r[0] == 0 ? "disabled" : "enabled"));
                AsyncResult->ForMessage(result, r, NULL);
                result->SendToTarget();
            }
            return;
        }

        Unimplemented(result);
    }

    //@Override
    CARAPI SetFacilityLock(String facility, Boolean lockEnabled, String pin, Int32 serviceClass,
            Message result) {
        SetFacilityLockForApp(facility, lockEnabled, pin, serviceClass, NULL, result);
    }

    //@Override
    CARAPI SetFacilityLockForApp(String facility, Boolean lockEnabled,
                                 String pin, Int32 serviceClass, String appId,
                                 Message result) {
        If (facility != NULL &&
                facility->Equals(CommandsInterface.CB_FACILITY_BA_SIM)) {
            If (pin != NULL && pin->Equals(mPinCode)) {
                Rlog->I(LOG_TAG, "[SimCmd] setFacilityLock: pin is valid");
                mSimLockEnabled = lockEnabled;

                If (result != NULL) {
                    AsyncResult->ForMessage(result, NULL, NULL);
                    result->SendToTarget();
                }

                return;
            }

            If (result != NULL) {
                Rlog->I(LOG_TAG, "[SimCmd] setFacilityLock: pin failed!");

                CommandException ex = new CommandException(
                        CommandException.Error.GENERIC_FAILURE);
                AsyncResult->ForMessage(result, NULL, ex);
                result->SendToTarget();
            }

            return;
        }  else If (facility != NULL &&
                facility->Equals(CommandsInterface.CB_FACILITY_BA_FD)) {
            If (pin != NULL && pin->Equals(mPin2Code)) {
                Rlog->I(LOG_TAG, "[SimCmd] setFacilityLock: pin2 is valid");
                mSimFdnEnabled = lockEnabled;

                If (result != NULL) {
                    AsyncResult->ForMessage(result, NULL, NULL);
                    result->SendToTarget();
                }

                return;
            }

            If (result != NULL) {
                Rlog->I(LOG_TAG, "[SimCmd] setFacilityLock: pin2 failed!");

                CommandException ex = new CommandException(
                        CommandException.Error.GENERIC_FAILURE);
                AsyncResult->ForMessage(result, NULL, ex);
                result->SendToTarget();
            }

            return;
        }

        Unimplemented(result);
    }

    //@Override
    CARAPI SupplyDepersonalization(String netpin, String type, Message result)  {
        Unimplemented(result);
    }

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result contains a List of DriverCall
     *      The ar.result List is sorted by DriverCall.index
     */
    //@Override
    CARAPI GetCurrentCalls (Message result) {
        If ((mState == RadioState.RADIO_ON) && !IsSimLocked()) {
            //Rlog->I("GSM", "[SimCmds] getCurrentCalls");
            ResultSuccess(result, simulatedCallState->GetDriverCalls());
        } else {
            //Rlog->I("GSM", "[SimCmds] getCurrentCalls: RADIO_OFF or SIM not ready!");
            ResultFail(result,
                new CommandException(
                    CommandException.Error.RADIO_NOT_AVAILABLE));
        }
    }

    /**
     *  @deprecated
     */
    //@Deprecated
    //@Override
    CARAPI GetPDPContextList(Message result) {
        GetDataCallList(result);
    }

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result contains a List of DataCallResponse
     */
    //@Override
    CARAPI GetDataCallList(Message result) {
        ResultSuccess(result, new ArrayList<DataCallResponse>(0));
    }

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     *
     * CLIR_DEFAULT     == on "use subscription default value"
     * CLIR_SUPPRESSION == on "CLIR suppression" (allow CLI presentation)
     * CLIR_INVOCATION  == on "CLIR invocation" (restrict CLI presentation)
     */
    //@Override
    CARAPI Dial (String address, Int32 clirMode, Message result) {
        simulatedCallState->OnDial(address);

        ResultSuccess(result, NULL);
    }

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     *
     * CLIR_DEFAULT     == on "use subscription default value"
     * CLIR_SUPPRESSION == on "CLIR suppression" (allow CLI presentation)
     * CLIR_INVOCATION  == on "CLIR invocation" (restrict CLI presentation)
     */
    //@Override
    CARAPI Dial(String address, Int32 clirMode, UUSInfo uusInfo, Message result) {
        simulatedCallState->OnDial(address);

        ResultSuccess(result, NULL);
    }

    //@Override
    CARAPI GetIMSI(Message result) {
        GetIMSIForApp(NULL, result);
    }
    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is String containing IMSI on success
     */
    //@Override
    CARAPI GetIMSIForApp(String aid, Message result) {
        ResultSuccess(result, "012345678901234");
    }

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is String containing IMEI on success
     */
    //@Override
    CARAPI GetIMEI(Message result) {
        ResultSuccess(result, "012345678901234");
    }

    /**
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is String containing IMEISV on success
     */
    //@Override
    CARAPI GetIMEISV(Message result) {
        ResultSuccess(result, "99");
    }

    /**
     * Hang up one individual connection.
     *  returned message
     *  retMsg.obj = AsyncResult ar
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     *
     *  3GPP 22.030 6.5.5
     *  "Releases a specific active call X"
     */
    //@Override
    CARAPI HangupConnection (Int32 gsmIndex, Message result) {
        Boolean success;

        success = simulatedCallState->OnChld('1', (Char32)('0'+gsmIndex));

        If (!success){
            Rlog->I("GSM", "[SimCmd] hangupConnection: resultFail");
            ResultFail(result, new RuntimeException("Hangup Error"));
        } else {
            Rlog->I("GSM", "[SimCmd] hangupConnection: resultSuccess");
            ResultSuccess(result, NULL);
        }
    }

    /**
     * 3GPP 22.030 6.5.5
     *  "Releases all held calls or sets User Determined User Busy (UDUB)
     *   for a waiting call."
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI HangupWaitingOrBackground (Message result) {
        Boolean success;

        success = simulatedCallState->OnChld('0', '\0');

        If (!success){
            ResultFail(result, new RuntimeException("Hangup Error"));
        } else {
            ResultSuccess(result, NULL);
        }
    }

    /**
     * 3GPP 22.030 6.5.5
     * "Releases all active Calls (if any exist) and accepts
     *  the Other (held or waiting) call."
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI HangupForegroundResumeBackground (Message result) {
        Boolean success;

        success = simulatedCallState->OnChld('1', '\0');

        If (!success){
            ResultFail(result, new RuntimeException("Hangup Error"));
        } else {
            ResultSuccess(result, NULL);
        }
    }

    /**
     * 3GPP 22.030 6.5.5
     * "Places all active Calls (if any exist) on hold and accepts
     *  the Other (held or waiting) call."
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI SwitchWaitingOrHoldingAndActive (Message result) {
        Boolean success;

        success = simulatedCallState->OnChld('2', '\0');

        If (!success){
            ResultFail(result, new RuntimeException("Hangup Error"));
        } else {
            ResultSuccess(result, NULL);
        }
    }

    /**
     * 3GPP 22.030 6.5.5
     * "Adds a held call to the conversation"
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI Conference (Message result) {
        Boolean success;

        success = simulatedCallState->OnChld('3', '\0');

        If (!success){
            ResultFail(result, new RuntimeException("Hangup Error"));
        } else {
            ResultSuccess(result, NULL);
        }
    }

    /**
     * 3GPP 22.030 6.5.5
     * "Connects the two calls and disconnects the subscriber from both calls"
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI ExplicitCallTransfer (Message result) {
        Boolean success;

        success = simulatedCallState->OnChld('4', '\0');

        If (!success){
            ResultFail(result, new RuntimeException("Hangup Error"));
        } else {
            ResultSuccess(result, NULL);
        }
    }

    /**
     * 3GPP 22.030 6.5.5
     * "Places all active calls on hold except call X with which
     *  communication shall be supported."
     */
    //@Override
    CARAPI SeparateConnection (Int32 gsmIndex, Message result) {
        Boolean success;

        Char32 ch = (Char32)(gsmIndex + '0');
        success = simulatedCallState->OnChld('2', ch);

        If (!success){
            ResultFail(result, new RuntimeException("Hangup Error"));
        } else {
            ResultSuccess(result, NULL);
        }
    }

    /**
     *
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI AcceptCall (Message result) {
        Boolean success;

        success = simulatedCallState->OnAnswer();

        If (!success){
            ResultFail(result, new RuntimeException("Hangup Error"));
        } else {
            ResultSuccess(result, NULL);
        }
    }

    /**
     *  also known as UDUB
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI RejectCall (Message result) {
        Boolean success;

        success = simulatedCallState->OnChld('0', '\0');

        If (!success){
            ResultFail(result, new RuntimeException("Hangup Error"));
        } else {
            ResultSuccess(result, NULL);
        }
    }

    /**
     * cause code returned as Integer in Message.obj.response
     * Returns integer cause code defined in TS 24.008
     * Annex H or closest approximation.
     * Most significant codes:
     * - Any defined in 22.001 F.4 (for generating busy/congestion)
     * - Cause 68: ACM >= ACMMax
     */
    //@Override
    CARAPI GetLastCallFailCause (Message result) {
        Int32[] ret = new Int32[1];

        ret[0] = mNextCallFailCause;
        ResultSuccess(result, ret);
    }

    /**
     * @deprecated
     */
    //@Deprecated
    //@Override
    CARAPI GetLastPdpFailCause (Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI GetLastDataCallFailCause(Message result) {
        //
        Unimplemented(result);
    }

    //@Override
    CARAPI SetMute (Boolean enableMute, Message result) {Unimplemented(result);}

    //@Override
    CARAPI GetMute (Message result) {Unimplemented(result);}

    /**
     * response.obj is an AsyncResult
     * response.obj.result is an Int32[2]
     * response.obj.result[0] is received signal Strength (0-31, 99)
     * response.obj.result[1] is  bit error Rate (0-7, 99)
     * as defined in TS 27.007 8.5
     */
    //@Override
    CARAPI GetSignalStrength (Message result) {
        Int32 ret[] = new Int32[2];

        ret[0] = 23;
        ret[1] = 0;

        ResultSuccess(result, ret);
    }

     /**
     * Assign a specified band for RF configuration.
     *
     * @param bandMode one of BM_*_BAND
     * @param result is callback message
     */
    //@Override
    CARAPI SetBandMode (Int32 bandMode, Message result) {
        ResultSuccess(result, NULL);
    }

    /**
     * Query the list of band mode supported by RF.
     *
     * @param result is callback message
     *        ((AsyncResult)response.obj).result  is an Int32[] where Int32[0] is
     *        the size of the array and the rest of each element representing
     *        one available BM_*_BAND
     */
    //@Override
    CARAPI QueryAvailableBandMode (Message result) {
        Int32 ret[] = new Int32 [4];

        ret[0] = 4;
        ret[1] = Phone.BM_US_BAND;
        ret[2] = Phone.BM_JPN_BAND;
        ret[3] = Phone.BM_AUS_BAND;

        ResultSuccess(result, ret);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SendTerminalResponse(String contents, Message response) {
        ResultSuccess(response, NULL);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SendEnvelope(String contents, Message response) {
        ResultSuccess(response, NULL);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SendEnvelopeWithStatus(String contents, Message response) {
        ResultSuccess(response, NULL);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI HandleCallSetupRequestFromSim(
            Boolean accept, Message response) {
        ResultSuccess(response, NULL);
    }

    /**
     * response.obj.result is an String[14]
     * See ril.h for details
     *
     * Please note that registration state 4 ("unknown") is treated
     * as "out of service" above
     */
    //@Override
    CARAPI GetVoiceRegistrationState (Message result) {
        String ret[] = new String[14];

        ret[0] = "5"; // registered roam
        ret[1] = NULL;
        ret[2] = NULL;
        ret[3] = NULL;
        ret[4] = NULL;
        ret[5] = NULL;
        ret[6] = NULL;
        ret[7] = NULL;
        ret[8] = NULL;
        ret[9] = NULL;
        ret[10] = NULL;
        ret[11] = NULL;
        ret[12] = NULL;
        ret[13] = NULL;

        ResultSuccess(result, ret);
    }

    /**
     * response.obj.result is an String[4]
     * response.obj.result[0] is registration state 0-5 from TS 27.007 7.2
     * response.obj.result[1] is LAC if registered or NULL if not
     * response.obj.result[2] is CID if registered or NULL if not
     * response.obj.result[3] indicates the available radio technology, where:
     *      0 == unknown
     *      1 == GPRS only
     *      2 == EDGE
     *      3 == UMTS
     *
     * valid LAC are 0x0000 - 0xffff
     * valid CID are 0x00000000 - 0xffffffff
     *
     * Please note that registration state 4 ("unknown") is treated
     * as "out of service" in the Android telephony system
     */
    //@Override
    CARAPI GetDataRegistrationState (Message result) {
        String ret[] = new String[4];

        ret[0] = "5"; // registered roam
        ret[1] = NULL;
        ret[2] = NULL;
        ret[3] = "2";

        ResultSuccess(result, ret);
    }

    /**
     * response.obj.result is a String[3]
     * response.obj.result[0] is Int64 alpha or NULL if unregistered
     * response.obj.result[1] is short alpha or NULL if unregistered
     * response.obj.result[2] is numeric or NULL if unregistered
     */
    //@Override
    CARAPI GetOperator(Message result) {
        String[] ret = new String[3];

        ret[0] = "El Telco Loco";
        ret[1] = "Telco Loco";
        ret[2] = "001001";

        ResultSuccess(result, ret);
    }

    /**
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI SendDtmf(Char32 c, Message result) {
        ResultSuccess(result, NULL);
    }

    /**
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI StartDtmf(Char32 c, Message result) {
        ResultSuccess(result, NULL);
    }

    /**
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI StopDtmf(Message result) {
        ResultSuccess(result, NULL);
    }

    /**
     *  ar.exception carries exception on failure
     *  ar.userObject contains the original value of result.obj
     *  ar.result is NULL on success and failure
     */
    //@Override
    CARAPI SendBurstDtmf(String dtmfString, Int32 on, Int32 off, Message result) {
        ResultSuccess(result, NULL);
    }

    /**
     * smscPDU is smsc address in PDU form GSM BCD format prefixed
     *      by a length Byte (as expected by TS 27.005) or NULL for default SMSC
     * pdu is SMS in PDU format as an ASCII hex string
     *      less the SMSC address
     */
    //@Override
    CARAPI SendSMS (String smscPDU, String pdu, Message result) {Unimplemented(result);}

    /**
     * Send an SMS message, Identical to sendSMS,
     * except that more messages are expected to be sent soon
     * smscPDU is smsc address in PDU form GSM BCD format prefixed
     *      by a length Byte (as expected by TS 27.005) or NULL for default SMSC
     * pdu is SMS in PDU format as an ASCII hex string
     *      less the SMSC address
     */
    //@Override
    CARAPI SendSMSExpectMore (String smscPDU, String pdu, Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI DeleteSmsOnSim(Int32 index, Message response) {
        Rlog->D(LOG_TAG, "Delete message at index " + index);
        Unimplemented(response);
    }

    //@Override
    CARAPI DeleteSmsOnRuim(Int32 index, Message response) {
        Rlog->D(LOG_TAG, "Delete RUIM message at index " + index);
        Unimplemented(response);
    }

    //@Override
    CARAPI WriteSmsToSim(Int32 status, String smsc, String pdu, Message response) {
        Rlog->D(LOG_TAG, "Write SMS to SIM with status " + status);
        Unimplemented(response);
    }

    //@Override
    CARAPI WriteSmsToRuim(Int32 status, String pdu, Message response) {
        Rlog->D(LOG_TAG, "Write SMS to RUIM with status " + status);
        Unimplemented(response);
    }

    //@Override
    CARAPI SetupDataCall(String radioTechnology, String profile,
            String apn, String user, String password, String authType,
            String protocol, Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI DeactivateDataCall(Int32 cid, Int32 reason, Message result) {Unimplemented(result);}

    //@Override
    CARAPI SetPreferredNetworkType(Int32 networkType , Message result) {
        mNetworkType = networkType;
        ResultSuccess(result, NULL);
    }

    //@Override
    CARAPI GetPreferredNetworkType(Message result) {
        Int32 ret[] = new Int32[1];

        ret[0] = mNetworkType;
        ResultSuccess(result, ret);
    }

    //@Override
    CARAPI GetNeighboringCids(Message result) {
        Int32 ret[] = new Int32[7];

        ret[0] = 6;
        For (Int32 i = 1; i<7; i++) {
            ret[i] = i;
        }
        ResultSuccess(result, ret);
    }

    //@Override
    CARAPI SetLocationUpdates(Boolean enable, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI GetSmscAddress(Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI SetSmscAddress(String address, Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI ReportSmsMemoryStatus(Boolean available, Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI ReportStkServiceIsRunning(Message result) {
        ResultSuccess(result, NULL);
    }

    //@Override
    CARAPI GetCdmaSubscriptionSource(Message result) {
        Unimplemented(result);
    }

    private Boolean IsSimLocked() {
        If (mSimLockedState != SimLockState.NONE) {
            return TRUE;
        }
        return FALSE;
    }

    //@Override
    CARAPI SetRadioPower(Boolean on, Message result) {
        If(on) {
            SetRadioState(RadioState.RADIO_ON);
        } else {
            SetRadioState(RadioState.RADIO_OFF);
        }
    }


    //@Override
    CARAPI AcknowledgeLastIncomingGsmSms(Boolean success, Int32 cause, Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI AcknowledgeLastIncomingCdmaSms(Boolean success, Int32 cause, Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI AcknowledgeIncomingGsmSmsWithPdu(Boolean success, String ackPdu,
            Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI IccIO(Int32 command, Int32 fileid, String path, Int32 p1, Int32 p2, Int32 p3, String data,
            String pin2, Message response) {
        IccIOForApp(command, fileid, path, p1, p2, p3, data,pin2, NULL, response);
    }

    /**
     * parameters equivalent to 27.007 AT+CRSM command
     * response.obj will be an AsyncResult
     * response.obj.userObj will be a SimIoResult on success
     */
    //@Override
    CARAPI IccIOForApp (Int32 command, Int32 fileid, String path, Int32 p1, Int32 p2,
                       Int32 p3, String data, String pin2, String aid, Message result) {
        Unimplemented(result);
    }

    /**
     * (AsyncResult)response.obj).result is an Int32[] with element [0] set to
     * 1 for "CLIP is provisioned", and 0 for "CLIP is not provisioned".
     *
     * @param response is callback message
     */
    //@Override
    CARAPI QueryCLIP(Message response) { Unimplemented(response); }


    /**
     * response.obj will be a an Int32[2]
     *
     * response.obj[0] will be TS 27.007 +CLIR parameter 'n'
     *  0 presentation indicator is used according to the subscription of the CLIR service
     *  1 CLIR invocation
     *  2 CLIR suppression
     *
     * response.obj[1] will be TS 27.007 +CLIR parameter 'm'
     *  0 CLIR not provisioned
     *  1 CLIR provisioned in permanent mode
     *  2 Unknown (e.g. no network, etc.)
     *  3 CLIR temporary mode presentation restricted
     *  4 CLIR temporary mode presentation allowed
     */

    //@Override
    CARAPI GetCLIR(Message result) {Unimplemented(result);}

    /**
     * clirMode is one of the CLIR_* constants above
     *
     * response.obj is NULL
     */

    //@Override
    CARAPI SetCLIR(Int32 clirMode, Message result) {Unimplemented(result);}

    /**
     * (AsyncResult)response.obj).result is an Int32[] with element [0] set to
     * 0 for disabled, 1 for enabled.
     *
     * @param serviceClass is a sum of SERVICE_CLASS_*
     * @param response is callback message
     */

    //@Override
    CARAPI QueryCallWaiting(Int32 serviceClass, Message response) {
        Unimplemented(response);
    }

    /**
     * @param enable is TRUE to enable, FALSE to disable
     * @param serviceClass is a sum of SERVICE_CLASS_*
     * @param response is callback message
     */

    //@Override
    CARAPI SetCallWaiting(Boolean enable, Int32 serviceClass,
            Message response) {
        Unimplemented(response);
    }

    /**
     * @param action is one of CF_ACTION_*
     * @param cfReason is one of CF_REASON_*
     * @param serviceClass is a sum of SERVICE_CLASSS_*
     */
    //@Override
    CARAPI SetCallForward(Int32 action, Int32 cfReason, Int32 serviceClass,
            String number, Int32 timeSeconds, Message result) {Unimplemented(result);}

    /**
     * cfReason is one of CF_REASON_*
     *
     * ((AsyncResult)response.obj).result will be an array of
     * CallForwardInfo's
     *
     * An array of length 0 means "disabled for all codes"
     */
    //@Override
    CARAPI QueryCallForwardStatus(Int32 cfReason, Int32 serviceClass,
            String number, Message result) {Unimplemented(result);}

    //@Override
    CARAPI SetNetworkSelectionModeAutomatic(Message result) {Unimplemented(result);}
    //@Override
    CARAPI ExitEmergencyCallbackMode(Message result) {Unimplemented(result);}
    //@Override
    CARAPI SetNetworkSelectionModeManual(
            String operatorNumeric, Message result) {Unimplemented(result);}

    /**
     * Queries whether the current network selection mode is automatic
     * or manual
     *
     * ((AsyncResult)response.obj).result  is an Int32[] with element [0] being
     * a 0 for automatic selection and a 1 for manual selection
     */

    //@Override
    CARAPI GetNetworkSelectionMode(Message result) {
        Int32 ret[] = new Int32[1];

        ret[0] = 0;
        ResultSuccess(result, ret);
    }

    /**
     * Queries the currently available networks
     *
     * ((AsyncResult)response.obj).result  is a List of NetworkInfo objects
     */
    //@Override
    CARAPI GetAvailableNetworks(Message result) {Unimplemented(result);}

    //@Override
    CARAPI GetBasebandVersion (Message result) {
        ResultSuccess(result, "SimulatedCommands");
    }

    /**
     * Simulates an Stk Call Control Alpha message
     * @param alphaString Alpha string to send.
     */
    CARAPI TriggerIncomingStkCcAlpha(String alphaString) {
        If (mCatCcAlphaRegistrant != NULL) {
            mCatCcAlphaRegistrant->NotifyResult(alphaString);
        }
    }

    CARAPI SendStkCcAplha(String alphaString) {
        TriggerIncomingStkCcAlpha(alphaString);
    }

    /**
     * Simulates an incoming USSD message
     * @param statusCode  Status code string. See <code>setOnUSSD</code>
     * in CommandsInterface.java
     * @param message Message text to send or NULL if none
     */
    //@Override
    CARAPI TriggerIncomingUssd(String statusCode, String message) {
        If (mUSSDRegistrant != NULL) {
            String[] result = {statusCode, message};
            mUSSDRegistrant->NotifyResult(result);
        }
    }


    //@Override
    CARAPI SendUSSD (String ussdString, Message result) {

        // We simulate this particular sequence
        If (ussdString->Equals("#646#")) {
            ResultSuccess(result, NULL);

            // 0 == USSD-Notify
            TriggerIncomingUssd("0", "You have NNN minutes remaining.");
        } else {
            ResultSuccess(result, NULL);

            TriggerIncomingUssd("0", "All Done");
        }
    }

    // inherited javadoc suffices
    //@Override
    CARAPI CancelPendingUssd (Message response) {
        ResultSuccess(response, NULL);
    }


    //@Override
    CARAPI ResetRadio(Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI InvokeOemRilRequestRaw(Byte[] data, Message response) {
        // Just echo back data
        If (response != NULL) {
            AsyncResult->ForMessage(response).result = data;
            response->SendToTarget();
        }
    }

    //@Override
    CARAPI InvokeOemRilRequestStrings(String[] strings, Message response) {
        // Just echo back data
        If (response != NULL) {
            AsyncResult->ForMessage(response).result = strings;
            response->SendToTarget();
        }
    }

    //***** SimulatedRadioControl


    /** Start the simulated phone ringing */
    //@Override
    CARAPI
    TriggerRing(String number) {
        simulatedCallState->TriggerRing(number);
        mCallStateRegistrants->NotifyRegistrants();
    }

    //@Override
    CARAPI
    ProgressConnectingCallState() {
        simulatedCallState->ProgressConnectingCallState();
        mCallStateRegistrants->NotifyRegistrants();
    }

    /** If a call is DIALING or ALERTING, progress it all the way to ACTIVE */
    //@Override
    CARAPI
    ProgressConnectingToActive() {
        simulatedCallState->ProgressConnectingToActive();
        mCallStateRegistrants->NotifyRegistrants();
    }

    /** automatically progress mobile originated calls to ACTIVE.
     *  default to TRUE
     */
    //@Override
    CARAPI
    SetAutoProgressConnectingCall(Boolean b) {
        simulatedCallState->SetAutoProgressConnectingCall(b);
    }

    //@Override
    CARAPI
    SetNextDialFailImmediately(Boolean b) {
        simulatedCallState->SetNextDialFailImmediately(b);
    }

    //@Override
    CARAPI
    SetNextCallFailCause(Int32 gsmCause) {
        mNextCallFailCause = gsmCause;
    }

    //@Override
    CARAPI
    TriggerHangupForeground() {
        simulatedCallState->TriggerHangupForeground();
        mCallStateRegistrants->NotifyRegistrants();
    }

    /** hangup holding calls */
    //@Override
    CARAPI
    TriggerHangupBackground() {
        simulatedCallState->TriggerHangupBackground();
        mCallStateRegistrants->NotifyRegistrants();
    }

    //@Override
    CARAPI TriggerSsn(Int32 type, Int32 code) {
        SuppServiceNotification not = new SuppServiceNotification();
        not.notificationType = type;
        not.code = code;
        mSsnRegistrant->NotifyRegistrant(new AsyncResult(NULL, not, NULL));
    }

    //@Override
    CARAPI
    Shutdown() {
        SetRadioState(RadioState.RADIO_UNAVAILABLE);
        Looper looper = mHandlerThread->GetLooper();
        If (looper != NULL) {
            looper->Quit();
        }
    }

    /** hangup all */

    //@Override
    CARAPI
    TriggerHangupAll() {
        simulatedCallState->TriggerHangupAll();
        mCallStateRegistrants->NotifyRegistrants();
    }

    //@Override
    CARAPI
    TriggerIncomingSMS(String message) {
        //TODO
    }

    //@Override
    CARAPI
    PauseResponses() {
        mPausedResponseCount++;
    }

    //@Override
    CARAPI
    ResumeResponses() {
        mPausedResponseCount--;

        If (mPausedResponseCount == 0) {
            For (Int32 i = 0, s = mPausedResponses->Size(); i < s ; i++) {
                mPausedResponses->Get(i).SendToTarget();
            }
            mPausedResponses->Clear();
        } else {
            Rlog->E("GSM", "SimulatedCommands.resumeResponses < 0");
        }
    }

    //***** Private Methods

    private void Unimplemented(Message result) {
        If (result != NULL) {
            AsyncResult->ForMessage(result).exception
                = new RuntimeException("Unimplemented");

            If (mPausedResponseCount > 0) {
                mPausedResponses->Add(result);
            } else {
                result->SendToTarget();
            }
        }
    }

    private void ResultSuccess(Message result, Object ret) {
        If (result != NULL) {
            AsyncResult->ForMessage(result).result = ret;
            If (mPausedResponseCount > 0) {
                mPausedResponses->Add(result);
            } else {
                result->SendToTarget();
            }
        }
    }

    private void ResultFail(Message result, Throwable tr) {
        If (result != NULL) {
            AsyncResult->ForMessage(result).exception = tr;
            If (mPausedResponseCount > 0) {
                mPausedResponses->Add(result);
            } else {
                result->SendToTarget();
            }
        }
    }

    // ***** Methods for CDMA support
    //@Override
    CARAPI
    GetDeviceIdentity(Message response) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
        Unimplemented(response);
    }

    //@Override
    CARAPI
    GetCDMASubscription(Message response) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
        Unimplemented(response);
    }

    //@Override
    CARAPI
    SetCdmaSubscriptionSource(Int32 cdmaSubscriptionType, Message response) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
        Unimplemented(response);
    }

    //@Override
    CARAPI QueryCdmaRoamingPreference(Message response) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
        Unimplemented(response);
    }

    //@Override
    CARAPI SetCdmaRoamingPreference(Int32 cdmaRoamingType, Message response) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
        Unimplemented(response);
    }

    //@Override
    CARAPI
    SetPhoneType(Int32 phoneType) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
    }

    //@Override
    CARAPI GetPreferredVoicePrivacy(Message result) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
        Unimplemented(result);
    }

    //@Override
    CARAPI SetPreferredVoicePrivacy(Boolean enable, Message result) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
        Unimplemented(result);
    }

    /**
     *  Set the TTY mode
     *
     * @param ttyMode is one of the following:
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_OFF}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_FULL}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_HCO}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_VCO}
     * @param response is callback message
     */
    //@Override
    CARAPI SetTTYMode(Int32 ttyMode, Message response) {
        Rlog->W(LOG_TAG, "Not implemented in SimulatedCommands");
        Unimplemented(response);
    }

    /**
     *  Query the TTY mode
     * (AsyncResult)response.obj).result is an Int32[] with element [0] set to
     * tty mode:
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_OFF}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_FULL}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_HCO}
     * - {@link com.android.internal.telephony.Phone#TTY_MODE_VCO}
     * @param response is callback message
     */
    //@Override
    CARAPI QueryTTYMode(Message response) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
        Unimplemented(response);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SendCDMAFeatureCode(String FeatureCode, Message response) {
        Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
        Unimplemented(response);
    }

    /**
     * {@inheritDoc}
     */
    //@Override
    CARAPI SendCdmaSms(Byte[] pdu, Message response){
       Rlog->W(LOG_TAG, "CDMA not implemented in SimulatedCommands");
    }

    //@Override
    CARAPI SetCdmaBroadcastActivation(Boolean activate, Message response) {
        Unimplemented(response);

    }

    //@Override
    CARAPI GetCdmaBroadcastConfig(Message response) {
        Unimplemented(response);

    }

    //@Override
    CARAPI SetCdmaBroadcastConfig(CdmaSmsBroadcastConfigInfo[] configs, Message response) {
        Unimplemented(response);
    }

    CARAPI ForceDataDormancy(Message response) {
        Unimplemented(response);
    }


    //@Override
    CARAPI SetGsmBroadcastActivation(Boolean activate, Message response) {
        Unimplemented(response);
    }


    //@Override
    CARAPI SetGsmBroadcastConfig(SmsBroadcastConfigInfo[] config, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI GetGsmBroadcastConfig(Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI SupplyIccPinForApp(String pin, String aid, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI SupplyIccPukForApp(String puk, String newPin, String aid, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI SupplyIccPin2ForApp(String pin2, String aid, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI SupplyIccPuk2ForApp(String puk2, String newPin2, String aid, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI ChangeIccPinForApp(String oldPin, String newPin, String aidPtr, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI ChangeIccPin2ForApp(String oldPin2, String newPin2, String aidPtr,
            Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI RequestIsimAuthentication(String nonce, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI RequestIccSimAuthentication(Int32 authContext, String data, String aid, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI GetVoiceRadioTechnology(Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI GetCellInfoList(Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI SetCellInfoListRate(Int32 rateInMillis, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI SetInitialAttachApn(String apn, String protocol, Int32 authType, String username,
            String password, Message result) {
    }

    //@Override
    CARAPI SetDataProfile(DataProfile[] dps, Message result) {
    }

    //@Override
    CARAPI GetImsRegistrationState(Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI SendImsCdmaSms(Byte[] pdu, Int32 retry, Int32 messageRef,
            Message response){
        Unimplemented(response);
    }

    //@Override
    CARAPI SendImsGsmSms(String smscPDU, String pdu,
            Int32 retry, Int32 messageRef, Message response){
        Unimplemented(response);
    }

    //@Override
    CARAPI IccOpenLogicalChannel(String AID, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI IccCloseLogicalChannel(Int32 channel, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI IccTransmitApduLogicalChannel(Int32 channel, Int32 cla, Int32 instruction,
            Int32 p1, Int32 p2, Int32 p3, String data, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI IccTransmitApduBasicChannel(Int32 cla, Int32 instruction, Int32 p1, Int32 p2,
            Int32 p3, String data, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI GetAtr(Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI NvReadItem(Int32 itemID, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI NvWriteItem(Int32 itemID, String itemValue, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI NvWriteCdmaPrl(Byte[] preferredRoamingList, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI NvResetConfig(Int32 resetType, Message response) {
        Unimplemented(response);
    }

    //@Override
    CARAPI GetHardwareConfig(Message result) {
        Unimplemented(result);
    }

    //@Override
    CARAPI RequestShutdown(Message result) {
        SetRadioState(RadioState.RADIO_UNAVAILABLE);
    }

    public Boolean NeedsOldRilFeature(String feature) {
        return FALSE;
    }
}
