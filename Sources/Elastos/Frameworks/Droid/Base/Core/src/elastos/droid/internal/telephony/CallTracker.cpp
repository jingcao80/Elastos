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

package com.android.internal.telephony;

using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Text::ITextUtils;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;


/**
 * {@hide}
 */
public abstract class CallTracker extends Handler {

    private static const Boolean DBG_POLL = FALSE;

    //***** Constants

    static const Int32 POLL_DELAY_MSEC = 250;

    protected Int32 mPendingOperations;
    protected Boolean mNeedsPoll;
    protected Message mLastRelevantPoll;
    protected ArrayList<Connection> mHandoverConnections = new ArrayList<Connection>();

    public CommandsInterface mCi;

    protected Boolean mNumberConverted = FALSE;
    private final Int32 VALID_COMPARE_LENGTH   = 3;

    //***** Events

    protected static const Int32 EVENT_POLL_CALLS_RESULT             = 1;
    protected static const Int32 EVENT_CALL_STATE_CHANGE             = 2;
    protected static const Int32 EVENT_REPOLL_AFTER_DELAY            = 3;
    protected static const Int32 EVENT_OPERATION_COMPLETE            = 4;
    protected static const Int32 EVENT_GET_LAST_CALL_FAIL_CAUSE      = 5;

    protected static const Int32 EVENT_SWITCH_RESULT                 = 8;
    protected static const Int32 EVENT_RADIO_AVAILABLE               = 9;
    protected static const Int32 EVENT_RADIO_NOT_AVAILABLE           = 10;
    protected static const Int32 EVENT_CONFERENCE_RESULT             = 11;
    protected static const Int32 EVENT_SEPARATE_RESULT               = 12;
    protected static const Int32 EVENT_ECT_RESULT                    = 13;
    protected static const Int32 EVENT_EXIT_ECM_RESPONSE_CDMA        = 14;
    protected static const Int32 EVENT_CALL_WAITING_INFO_CDMA        = 15;
    protected static const Int32 EVENT_THREE_WAY_DIAL_L2_RESULT_CDMA = 16;
    protected static const Int32 EVENT_THREE_WAY_DIAL_BLANK_FLASH    = 20;

    protected void PollCallsWhenSafe() {
        mNeedsPoll = TRUE;

        If (CheckNoOperationsPending()) {
            mLastRelevantPoll = ObtainMessage(EVENT_POLL_CALLS_RESULT);
            mCi->GetCurrentCalls(mLastRelevantPoll);
        }
    }

    protected void
    PollCallsAfterDelay() {
        Message msg = ObtainMessage();

        msg.what = EVENT_REPOLL_AFTER_DELAY;
        SendMessageDelayed(msg, POLL_DELAY_MSEC);
    }

    protected Boolean
    IsCommandExceptionRadioNotAvailable(Throwable e) {
        return e != NULL && e instanceof CommandException
                && ((CommandException)e).GetCommandError()
                        == CommandException.Error.RADIO_NOT_AVAILABLE;
    }

    protected abstract void HandlePollCalls(AsyncResult ar);

    protected Connection GetHoConnection(DriverCall dc) {
        For (Connection hoConn : mHandoverConnections) {
            Log("getHoConnection - compare number: hoConn= " + hoConn->ToString());
            If (hoConn->GetAddress() != NULL && hoConn->GetAddress()->Contains(dc.number)) {
                Log("getHoConnection: Handover connection match found = " + hoConn->ToString());
                return hoConn;
            }
        }
        For (Connection hoConn : mHandoverConnections) {
            Log("getHoConnection: compare state hoConn= " + hoConn->ToString());
            If (hoConn->GetStateBeforeHandover() == Call->StateFromDCState(dc.state)) {
                Log("getHoConnection: Handover connection match found = " + hoConn->ToString());
                return hoConn;
            }
        }
        return NULL;
    }

    protected void NotifySrvccState(Call.SrvccState state, ArrayList<Connection> c) {
        If (state == Call.SrvccState.STARTED && c != NULL) {
            // SRVCC started. Prepare handover connections list
            mHandoverConnections->AddAll(c);
        } else If (state != Call.SrvccState.COMPLETED) {
            // SRVCC FAILED/CANCELED. Clear the handover connections list
            // Individual connections will be removed from the list in HandlePollCalls()
            mHandoverConnections->Clear();
        }
        Log("notifySrvccState: mHandoverConnections= " + mHandoverConnections->ToString());
    }

    protected void HandleRadioAvailable() {
        PollCallsWhenSafe();
    }

    /**
     * Obtain a complete message that indicates that this operation
     * does not require polling of GetCurrentCalls(). However, if other
     * operations that do need GetCurrentCalls() are pending or are
     * scheduled while this operation is pending, the invocation
     * of GetCurrentCalls() will be postponed until this
     * operation is also complete.
     */
    protected Message
    ObtainNoPollCompleteMessage(Int32 what) {
        mPendingOperations++;
        mLastRelevantPoll = NULL;
        return ObtainMessage(what);
    }

    /**
     * @return TRUE if we're idle or there's a call to GetCurrentCalls() pending
     * but nothing else
     */
    private Boolean
    CheckNoOperationsPending() {
        If (DBG_POLL) Log("checkNoOperationsPending: pendingOperations=" +
                mPendingOperations);
        return mPendingOperations == 0;
    }

    /**
     * Routine called from dial to check if the number is a test Emergency number
     * and if so remap the number. This allows a short emergency number to be remapped
     * to a regular number for testing how the frameworks handles emergency numbers
     * without actually calling an emergency number.
     *
     * This is not a full test and is not a substitute for testing real emergency
     * numbers but can be useful.
     *
     * To use this feature set a system property ril.test.emergencynumber to a pair of
     * numbers separated by a colon. If the first number matches the number parameter
     * this routine returns the second number. Example:
     *
     * ril.test.emergencynumber=112:1-123-123-45678
     *
     * To test Dial 112 take call then hang up on MO device to enter ECM
     * see RIL#processSolicited RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND
     *
     * @param dialString to test if it should be remapped
     * @return the same number or the remapped number.
     */
    protected String CheckForTestEmergencyNumber(String dialString) {
        String testEn = SystemProperties->Get("ril.test.emergencynumber");
        If (DBG_POLL) {
            Log("checkForTestEmergencyNumber: dialString=" + dialString +
                " testEn=" + testEn);
        }
        If (!TextUtils->IsEmpty(testEn)) {
            String values[] = testEn->Split(":");
            Log("checkForTestEmergencyNumber: values.length=" + values.length);
            If (values.length == 2) {
                If (values[0].Equals(
                        android.telephony.PhoneNumberUtils->StripSeparators(dialString))) {
                    mCi->TestingEmergencyCall();
                    Log("checkForTestEmergencyNumber: remap " +
                            dialString + " to " + values[1]);
                    dialString = values[1];
                }
            }
        }
        return dialString;
    }

    protected String ConvertNumberIfNecessary(PhoneBase phoneBase, String dialNumber) {
        If (dialNumber == NULL) {
            return dialNumber;
        }
        String[] convertMaps = phoneBase->GetContext()->GetResources().GetStringArray(
                R.array.dial_string_replace);
        Log("convertNumberIfNecessary Roaming"
            + " convertMaps.length " + convertMaps.length
            + " dialNumber->Length() " + dialNumber->Length());

        If (convertMaps.length < 1 || dialNumber->Length() < VALID_COMPARE_LENGTH) {
            return dialNumber;
        }

        String[] entry;
        String[] tmpArray;
        String outNumber = "";
        For(String convertMap : convertMaps) {
            Log("convertNumberIfNecessary: " + convertMap);
            entry = convertMap->Split(":");
            If (entry.length > 1) {
                tmpArray = entry[1].Split(",");
                If (!TextUtils->IsEmpty(entry[0]) && dialNumber->Equals(entry[0])) {
                    If (tmpArray.length >= 2 && !TextUtils->IsEmpty(tmpArray[1])) {
                        If (CompareGid1(phoneBase, tmpArray[1])) {
                            mNumberConverted = TRUE;
                        }
                    } else If (outNumber->IsEmpty()) {
                        mNumberConverted = TRUE;
                    }
                    If (mNumberConverted) {
                        If(!TextUtils->IsEmpty(tmpArray[0]) && tmpArray[0].EndsWith("MDN")) {
                            String prefix = tmpArray[0].Substring(0, tmpArray[0].Length() -3);
                            outNumber = prefix + phoneBase->GetLine1Number();
                        } else {
                            outNumber = tmpArray[0];
                        }
                    }
                }
            }
        }

        If (mNumberConverted) {
            Log("convertNumberIfNecessary: convert service number");
            return outNumber;
        }

        return dialNumber;

    }

    private Boolean CompareGid1(PhoneBase phoneBase, String serviceGid1) {
        String gid1 = phoneBase->GetGroupIdLevel1();
        Int32 gid_length = serviceGid1->Length();
        Boolean ret = TRUE;

        If (serviceGid1 == NULL || serviceGid1->Equals("")) {
            Log("compareGid1 serviceGid is empty, return " + ret);
            return ret;
        }
        // Check if gid1 match service GID1
        If (!((gid1 != NULL) && (gid1->Length() >= gid_length) &&
                gid1->Substring(0, gid_length).EqualsIgnoreCase(serviceGid1))) {
            Log(" gid1 " + gid1 + " serviceGid1 " + serviceGid1);
            ret = FALSE;
        }
        Log("compareGid1 is " + (ret?"Same":"Different"));
        return ret;
    }

    //***** Overridden from Handler
    //@Override
    public abstract void HandleMessage (Message msg);
    public abstract void RegisterForVoiceCallStarted(Handler h, Int32 what, Object obj);
    public abstract void UnregisterForVoiceCallStarted(Handler h);
    public abstract void RegisterForVoiceCallEnded(Handler h, Int32 what, Object obj);
    public abstract void UnregisterForVoiceCallEnded(Handler h);

    protected abstract void Log(String msg);

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("CallTracker:");
        pw->Println(" mPendingOperations=" + mPendingOperations);
        pw->Println(" mNeedsPoll=" + mNeedsPoll);
        pw->Println(" mLastRelevantPoll=" + mLastRelevantPoll);
    }
}
