/*
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

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Internal::Telephony::IATParseEx;
using Elastos::Droid::Internal::Telephony::IDriverCall;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;

using Elastos::Droid::Telephony::IRlog;

class CallInfo {
    enum State {
        ACTIVE(0),
        HOLDING(1),
        DIALING(2),    // MO call only
        ALERTING(3),   // MO call only
        INCOMING(4),   // MT call only
        WAITING(5);    // MT call only

        State(Int32 value) {mValue = value;}

        private final Int32 mValue;
        public Int32 Value() {return mValue;}
    }

    Boolean mIsMT;
    State mState;
    Boolean mIsMpty;
    String mNumber;
    Int32 mTOA;

    CallInfo (Boolean isMT, State state, Boolean isMpty, String number) {
        mIsMT = isMT;
        mState = state;
        mIsMpty = isMpty;
        mNumber = number;

        If (number->Length() > 0 && number->CharAt(0) == '+') {
            mTOA = PhoneNumberUtils.TOA_International;
        } else {
            mTOA = PhoneNumberUtils.TOA_Unknown;
        }
    }

    static CallInfo
    CreateOutgoingCall(String number) {
        return new CallInfo (FALSE, State.DIALING, FALSE, number);
    }

    static CallInfo
    CreateIncomingCall(String number) {
        return new CallInfo (TRUE, State.INCOMING, FALSE, number);
    }

    String
    ToCLCCLine(Int32 index) {
        StringBuilder sb("+CLCC: "); sb += index; sb += ",";
        sb += (mIsMT ? "1" : "0"); sb += ","; sb += mState->Value();
        sb += ",0,"; sb += (mIsMpty ? "1" : "0"); sb += ",\"";
        sb += mNumber; sb += "\",", sb += mTOA;
        return sb.ToString();
    }

    DriverCall
    ToDriverCall(Int32 index) {
        DriverCall ret;

        ret = new DriverCall();

        ret.index = index;
        ret.isMT = mIsMT;

        try {
            ret.state = DriverCall->StateFromCLCC(mState->Value());
        } Catch (ATParseEx ex) {
            throw new RuntimeException("should never happen", ex);
        }

        ret.isMpty = mIsMpty;
        ret.number = mNumber;
        ret.TOA = mTOA;
        ret.isVoice = TRUE;
        ret.als = 0;

        return ret;
    }


    Boolean
    IsActiveOrHeld() {
        return mState == State.ACTIVE || mState == State.HOLDING;
    }

    Boolean
    IsConnecting() {
        return mState == State.DIALING || mState == State.ALERTING;
    }

    Boolean
    IsRinging() {
        return mState == State.INCOMING || mState == State.WAITING;
    }

}

class InvalidStateEx extends Exception {
    InvalidStateEx() {

    }
}


class SimulatedGsmCallState extends Handler {
    //***** Instance Variables

    CallInfo mCalls[] = new CallInfo[MAX_CALLS];

    private Boolean mAutoProgressConnecting = TRUE;
    private Boolean mNextDialFailImmediately;


    //***** Event Constants

    static const Int32 EVENT_PROGRESS_CALL_STATE = 1;

    //***** Constants

    static const Int32 MAX_CALLS = 7;
    /** number of msec between dialing -> alerting and alerting->active */
    static const Int32 CONNECTING_PAUSE_MSEC = 5 * 100;


    //***** Overridden from Handler

    public SimulatedGsmCallState(Looper looper) {
        Super(looper);
    }

    //@Override
    CARAPI
    HandleMessage(Message msg) {
        {    AutoLock syncLock(this) { Switch (msg.what);
            // PLEASE REMEMBER
            // calls may have hung up by the time delayed events happen

            case EVENT_PROGRESS_CALL_STATE:
                ProgressConnectingCallState();
            break;
        }}
    }

    //***** Public Methods

    /**
     * Start the simulated phone ringing
     * TRUE if succeeded, FALSE if failed
     */
    public Boolean
    TriggerRing(String number) {
        {    AutoLock syncLock(this);
            Int32 empty = -1;
            Boolean isCallWaiting = FALSE;

            // ensure there aren't already calls INCOMING or WAITING
            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo call = mCalls[i];

                If (call == NULL && empty < 0) {
                    empty = i;
                } else If (call != NULL
                    && (call.mState == CallInfo.State.INCOMING
                        || call.mState == CallInfo.State.WAITING)
                ) {
                    Rlog->W("ModelInterpreter",
                        "triggerRing failed; phone already ringing");
                    return FALSE;
                } else If (call != NULL) {
                    isCallWaiting = TRUE;
                }
            }

            If (empty < 0 ) {
                Rlog->W("ModelInterpreter", "triggerRing failed; all full");
                return FALSE;
            }

            mCalls[empty] = CallInfo->CreateIncomingCall(
                PhoneNumberUtils->ExtractNetworkPortion(number));

            If (isCallWaiting) {
                mCalls[empty].mState = CallInfo.State.WAITING;
            }

        }
        return TRUE;
    }

    /** If a call is DIALING or ALERTING, progress it to the next state */
    CARAPI
    ProgressConnectingCallState() {
        {    AutoLock syncLock(this);
            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo call = mCalls[i];

                If (call != NULL && call.mState == CallInfo.State.DIALING) {
                    call.mState = CallInfo.State.ALERTING;

                    If (mAutoProgressConnecting) {
                        SendMessageDelayed(
                                ObtainMessage(EVENT_PROGRESS_CALL_STATE, call),
                                CONNECTING_PAUSE_MSEC);
                    }
                    break;
                } else If (call != NULL
                        && call.mState == CallInfo.State.ALERTING
                ) {
                    call.mState = CallInfo.State.ACTIVE;
                    break;
                }
            }
        }
    }

    /** If a call is DIALING or ALERTING, progress it all the way to ACTIVE */
    CARAPI
    ProgressConnectingToActive() {
        {    AutoLock syncLock(this);
            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo call = mCalls[i];

                If (call != NULL && (call.mState == CallInfo.State.DIALING
                    || call.mState == CallInfo.State.ALERTING)
                ) {
                    call.mState = CallInfo.State.ACTIVE;
                    break;
                }
            }
        }
    }

    /** automatically progress mobile originated calls to ACTIVE.
     *  default to TRUE
     */
    CARAPI
    SetAutoProgressConnectingCall(Boolean b) {
        mAutoProgressConnecting = b;
    }

    CARAPI
    SetNextDialFailImmediately(Boolean b) {
        mNextDialFailImmediately = b;
    }

    /**
     * hangup ringing, dialing, or active calls
     * returns TRUE if call was hung up, FALSE if not
     */
    public Boolean
    TriggerHangupForeground() {
        {    AutoLock syncLock(this);
            Boolean found;

            found = FALSE;

            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo call = mCalls[i];

                If (call != NULL
                    && (call.mState == CallInfo.State.INCOMING
                        || call.mState == CallInfo.State.WAITING)
                ) {
                    mCalls[i] = NULL;
                    found = TRUE;
                }
            }

            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo call = mCalls[i];

                If (call != NULL
                    && (call.mState == CallInfo.State.DIALING
                        || call.mState == CallInfo.State.ACTIVE
                        || call.mState == CallInfo.State.ALERTING)
                ) {
                    mCalls[i] = NULL;
                    found = TRUE;
                }
            }
            return found;
        }
    }

    /**
     * hangup holding calls
     * returns TRUE if call was hung up, FALSE if not
     */
    public Boolean
    TriggerHangupBackground() {
        {    AutoLock syncLock(this);
            Boolean found = FALSE;

            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo call = mCalls[i];

                If (call != NULL && call.mState == CallInfo.State.HOLDING) {
                    mCalls[i] = NULL;
                    found = TRUE;
                }
            }

            return found;
        }
    }

    /**
     * hangup all
     * returns TRUE if call was hung up, FALSE if not
     */
    public Boolean
    TriggerHangupAll() {
        {    AutoLock syncLock(this);
            Boolean found = FALSE;

            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo call = mCalls[i];

                If (mCalls[i] != NULL) {
                    found = TRUE;
                }

                mCalls[i] = NULL;
            }

            return found;
        }
    }

    public Boolean
    OnAnswer() {
        {    AutoLock syncLock(this);
            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo call = mCalls[i];

                If (call != NULL
                    && (call.mState == CallInfo.State.INCOMING
                        || call.mState == CallInfo.State.WAITING)
                ) {
                    return SwitchActiveAndHeldOrWaiting();
                }
            }
        }

        return FALSE;
    }

    public Boolean
    OnHangup() {
        Boolean found = FALSE;

        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo call = mCalls[i];

            If (call != NULL && call.mState != CallInfo.State.WAITING) {
                mCalls[i] = NULL;
                found = TRUE;
            }
        }

        return found;
    }

    public Boolean
    OnChld(Char32 c0, Char32 c1) {
        Boolean ret;
        Int32 callIndex = 0;

        If (c1 != 0) {
            callIndex = c1 - '1';

            If (callIndex < 0 || callIndex >= mCalls.length) {
                return FALSE;
            }
        }

        Switch (c0) {
            case '0':
                ret = ReleaseHeldOrUDUB();
            break;
            case '1':
                If (c1 <= 0) {
                    ret = ReleaseActiveAcceptHeldOrWaiting();
                } else {
                    If (mCalls[callIndex] == NULL) {
                        ret = FALSE;
                    } else {
                        mCalls[callIndex] = NULL;
                        ret = TRUE;
                    }
                }
            break;
            case '2':
                If (c1 <= 0) {
                    ret = SwitchActiveAndHeldOrWaiting();
                } else {
                    ret = SeparateCall(callIndex);
                }
            break;
            case '3':
                ret = Conference();
            break;
            case '4':
                ret = ExplicitCallTransfer();
            break;
            case '5':
                If (TRUE) { //just so javac doesnt complain about break
                    //CCBS not impled
                    ret = FALSE;
                }
            break;
            default:
                ret = FALSE;

        }

        return ret;
    }

    public Boolean
    ReleaseHeldOrUDUB() {
        Boolean found = FALSE;

        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL && c->IsRinging()) {
                found = TRUE;
                mCalls[i] = NULL;
                break;
            }
        }

        If (!found) {
            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo c = mCalls[i];

                If (c != NULL && c.mState == CallInfo.State.HOLDING) {
                    found = TRUE;
                    mCalls[i] = NULL;
                    // don't stop...there may be more than one
                }
            }
        }

        return TRUE;
    }


    public Boolean
    ReleaseActiveAcceptHeldOrWaiting() {
        Boolean foundHeld = FALSE;
        Boolean foundActive = FALSE;

        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL && c.mState == CallInfo.State.ACTIVE) {
                mCalls[i] = NULL;
                foundActive = TRUE;
            }
        }

        If (!foundActive) {
            // FIXME this may not actually be how most basebands react
            // CHLD=1 may not hang up dialing/alerting calls
            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                CallInfo c = mCalls[i];

                If (c != NULL
                        && (c.mState == CallInfo.State.DIALING
                            || c.mState == CallInfo.State.ALERTING)
                ) {
                    mCalls[i] = NULL;
                    foundActive = TRUE;
                }
            }
        }

        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL && c.mState == CallInfo.State.HOLDING) {
                c.mState = CallInfo.State.ACTIVE;
                foundHeld = TRUE;
            }
        }

        If (foundHeld) {
            return TRUE;
        }

        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL && c->IsRinging()) {
                c.mState = CallInfo.State.ACTIVE;
                return TRUE;
            }
        }

        return TRUE;
    }

    public Boolean
    SwitchActiveAndHeldOrWaiting() {
        Boolean hasHeld = FALSE;

        // first, are there held calls?
        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL && c.mState == CallInfo.State.HOLDING) {
                hasHeld = TRUE;
                break;
            }
        }

        // Now, switch
        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL) {
                If (c.mState == CallInfo.State.ACTIVE) {
                    c.mState = CallInfo.State.HOLDING;
                } else If (c.mState == CallInfo.State.HOLDING) {
                    c.mState = CallInfo.State.ACTIVE;
                } else If (!hasHeld && c->IsRinging())  {
                    c.mState = CallInfo.State.ACTIVE;
                }
            }
        }

        return TRUE;
    }


    public Boolean
    SeparateCall(Int32 index) {
        try {
            CallInfo c;

            c = mCalls[index];

            If (c == NULL || c->IsConnecting() || CountActiveLines() != 1) {
                return FALSE;
            }

            c.mState = CallInfo.State.ACTIVE;
            c.mIsMpty = FALSE;

            For (Int32 i = 0 ; i < mCalls.length ; i++) {
                Int32 countHeld=0, lastHeld=0;

                If (i != index) {
                    CallInfo cb = mCalls[i];

                    If (cb != NULL && cb.mState == CallInfo.State.ACTIVE) {
                        cb.mState = CallInfo.State.HOLDING;
                        countHeld++;
                        lastHeld = i;
                    }
                }

                If (countHeld == 1) {
                    // if there's only one left, clear the MPTY flag
                    mCalls[lastHeld].mIsMpty = FALSE;
                }
            }

            return TRUE;
        } Catch (InvalidStateEx ex) {
            return FALSE;
        }
    }



    public Boolean
    Conference() {
        Int32 countCalls = 0;

        // if there's connecting calls, we can't do this yet
        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL) {
                countCalls++;

                If (c->IsConnecting()) {
                    return FALSE;
                }
            }
        }
        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL) {
                c.mState = CallInfo.State.ACTIVE;
                If (countCalls > 0) {
                    c.mIsMpty = TRUE;
                }
            }
        }

        return TRUE;
    }

    public Boolean
    ExplicitCallTransfer() {
        Int32 countCalls = 0;

        // if there's connecting calls, we can't do this yet
        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL) {
                countCalls++;

                If (c->IsConnecting()) {
                    return FALSE;
                }
            }
        }

        // disconnect the subscriber from both calls
        return TriggerHangupAll();
    }

    public Boolean
    OnDial(String address) {
        CallInfo call;
        Int32 freeSlot = -1;

        Rlog->D("GSM", "SC> dial '" + address + "'");

        If (mNextDialFailImmediately) {
            mNextDialFailImmediately = FALSE;

            Rlog->D("GSM", "SC< dial Fail (per request)");
            return FALSE;
        }

        String phNum = PhoneNumberUtils->ExtractNetworkPortion(address);

        If (phNum->Length() == 0) {
            Rlog->D("GSM", "SC< dial Fail (invalid ph num)");
            return FALSE;
        }

        // Ignore setting up GPRS
        If (phNum->StartsWith("*99") && phNum->EndsWith("#")) {
            Rlog->D("GSM", "SC< dial Ignored (gprs)");
            return TRUE;
        }

        // There can be at most 1 active "line" when we initiate
        // a new call
        try {
            If (CountActiveLines() > 1) {
                Rlog->D("GSM", "SC< dial Fail (invalid call state)");
                return FALSE;
            }
        } Catch (InvalidStateEx ex) {
            Rlog->D("GSM", "SC< dial Fail (invalid call state)");
            return FALSE;
        }

        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            If (freeSlot < 0 && mCalls[i] == NULL) {
                freeSlot = i;
            }

            If (mCalls[i] != NULL && !mCalls[i].IsActiveOrHeld()) {
                // Can't make outgoing calls when there is a ringing or
                // connecting outgoing call
                Rlog->D("GSM", "SC< dial Fail (invalid call state)");
                return FALSE;
            } else If (mCalls[i] != NULL && mCalls[i].mState == CallInfo.State.ACTIVE) {
                // All active calls behome held
                mCalls[i].mState = CallInfo.State.HOLDING;
            }
        }

        If (freeSlot < 0) {
            Rlog->D("GSM", "SC< dial Fail (invalid call state)");
            return FALSE;
        }

        mCalls[freeSlot] = CallInfo->CreateOutgoingCall(phNum);

        If (mAutoProgressConnecting) {
            SendMessageDelayed(
                    ObtainMessage(EVENT_PROGRESS_CALL_STATE, mCalls[freeSlot]),
                    CONNECTING_PAUSE_MSEC);
        }

        Rlog->D("GSM", "SC< Dial (slot = " + freeSlot + ")");

        return TRUE;
    }

    public List<DriverCall>
    GetDriverCalls() {
        ArrayList<DriverCall> ret = new ArrayList<DriverCall>(mCalls.length);

        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL) {
                DriverCall dc;

                dc = c->ToDriverCall(i + 1);
                ret->Add(dc);
            }
        }

        Rlog->D("GSM", "SC< getDriverCalls " + ret);

        return ret;
    }

    public List<String>
    GetClccLines() {
        ArrayList<String> ret = new ArrayList<String>(mCalls.length);

        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo c = mCalls[i];

            If (c != NULL) {
                ret->Add((c->ToCLCCLine(i + 1)));
            }
        }

        return ret;
    }

    private Int32
    CountActiveLines() throws InvalidStateEx {
        Boolean hasMpty = FALSE;
        Boolean hasHeld = FALSE;
        Boolean hasActive = FALSE;
        Boolean hasConnecting = FALSE;
        Boolean hasRinging = FALSE;
        Boolean mptyIsHeld = FALSE;

        For (Int32 i = 0 ; i < mCalls.length ; i++) {
            CallInfo call = mCalls[i];

            If (call != NULL) {
                If (!hasMpty && call.mIsMpty) {
                    mptyIsHeld = call.mState == CallInfo.State.HOLDING;
                } else If (call.mIsMpty && mptyIsHeld
                    && call.mState == CallInfo.State.ACTIVE
                ) {
                    Rlog->E("ModelInterpreter", "Invalid state");
                    throw new InvalidStateEx();
                } else If (!call.mIsMpty && hasMpty && mptyIsHeld
                    && call.mState == CallInfo.State.HOLDING
                ) {
                    Rlog->E("ModelInterpreter", "Invalid state");
                    throw new InvalidStateEx();
                }

                hasMpty |= call.mIsMpty;
                hasHeld |= call.mState == CallInfo.State.HOLDING;
                hasActive |= call.mState == CallInfo.State.ACTIVE;
                hasConnecting |= call->IsConnecting();
                hasRinging |= call->IsRinging();
            }
        }

        Int32 ret = 0;

        If (hasHeld) ret++;
        If (hasActive) ret++;
        If (hasConnecting) ret++;
        If (hasRinging) ret++;

        return ret;
    }

}
