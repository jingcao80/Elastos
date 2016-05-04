/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.internal.telephony.sip;

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IUUSInfo;

using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IPhoneNumberUtils;

abstract class SipConnectionBase extends Connection {
    private static const String LOG_TAG = "SipConnBase";
    private static const Boolean DBG = TRUE;
    private static const Boolean VDBG = FALSE; // STOPSHIP if TRUE

    private String mPostDialString;      // outgoing calls only
    private Int32 mNextPostDialChar;       // index into postDialString
    /*
     * These time/timespan values are based on System->CurrentTimeMillis(),
     * i.e., "wall clock" time.
     */
    private Int64 mCreateTime;
    private Int64 mConnectTime;
    private Int64 mDisconnectTime;

    /*
     * These time/timespan values are based on SystemClock->ElapsedRealTime(),
     * i.e., time since boot.  They are appropriate for comparison and
     * calculating deltas.
     */
    private Int64 mConnectTimeReal;
    private Int64 mDuration = -1L;
    private Int64 mHoldingStartTime;  // The time when the Connection last transitioned
                            // into HOLDING

    private Int32 mCause = DisconnectCause.NOT_DISCONNECTED;
    private PostDialState mPostDialState = PostDialState.NOT_STARTED;

    SipConnectionBase(String dialString) {
        If (DBG) Log("SipConnectionBase: ctor dialString=" + dialString);
        mPostDialString = PhoneNumberUtils->ExtractPostDialPortion(dialString);

        mCreateTime = System->CurrentTimeMillis();
    }

    protected void SetState(Call.State state) {
        If (DBG) Log("setState: state=" + state);
        Switch (state) {
            case ACTIVE:
                If (mConnectTime == 0) {
                    mConnectTimeReal = SystemClock->ElapsedRealtime();
                    mConnectTime = System->CurrentTimeMillis();
                }
                break;
            case DISCONNECTED:
                mDuration = GetDurationMillis();
                mDisconnectTime = System->CurrentTimeMillis();
                break;
            case HOLDING:
                mHoldingStartTime = SystemClock->ElapsedRealtime();
                break;
            default:
                // Ignore
                break;
        }
    }

    //@Override
    public Int64 GetCreateTime() {
        If (VDBG) Log("getCreateTime: ret=" + mCreateTime);
        return mCreateTime;
    }

    //@Override
    public Int64 GetConnectTime() {
        If (VDBG) Log("getConnectTime: ret=" + mConnectTime);
        return mConnectTime;
    }

    //@Override
    public Int64 GetDisconnectTime() {
        If (VDBG) Log("getDisconnectTime: ret=" + mDisconnectTime);
        return mDisconnectTime;
    }

    //@Override
    public Int64 GetDurationMillis() {
        Int64 dur;
        If (mConnectTimeReal == 0) {
            dur = 0;
        } else If (mDuration < 0) {
            dur = SystemClock->ElapsedRealtime() - mConnectTimeReal;
        } else {
            dur = mDuration;
        }
        If (VDBG) Log("getDurationMillis: ret=" + dur);
        return dur;
    }

    //@Override
    public Int64 GetHoldDurationMillis() {
        Int64 dur;
        If (GetState() != Call.State.HOLDING) {
            // If not holding, return 0
            dur = 0;
        } else {
            dur = SystemClock->ElapsedRealtime() - mHoldingStartTime;
        }
        If (VDBG) Log("getHoldDurationMillis: ret=" + dur);
        return dur;
    }

    //@Override
    public Int32 GetDisconnectCause() {
        If (VDBG) Log("getDisconnectCause: ret=" + mCause);
        return mCause;
    }

    void SetDisconnectCause(Int32 cause) {
        If (DBG) Log("setDisconnectCause: prev=" + mCause + " new=" + cause);
        mCause = cause;
    }

    //@Override
    public PostDialState GetPostDialState() {
        If (VDBG) Log("getPostDialState: ret=" + mPostDialState);
        return mPostDialState;
    }

    //@Override
    CARAPI ProceedAfterWaitChar() {
        If (DBG) Log("proceedAfterWaitChar: ignore");
    }

    //@Override
    CARAPI ProceedAfterWildChar(String str) {
        If (DBG) Log("proceedAfterWildChar: ignore");
    }

    //@Override
    CARAPI CancelPostDial() {
        If (DBG) Log("cancelPostDial: ignore");
    }

    protected abstract Phone GetPhone();

    //@Override
    public String GetRemainingPostDialString() {
        If (mPostDialState == PostDialState.CANCELLED
            || mPostDialState == PostDialState.COMPLETE
            || mPostDialString == NULL
            || mPostDialString->Length() <= mNextPostDialChar) {
            If (DBG) Log("getRemaingPostDialString: ret empty string");
            return "";
        }

        return mPostDialString->Substring(mNextPostDialChar);
    }

    private void Log(String msg) {
        Rlog->D(LOG_TAG, msg);
    }

    //@Override
    public Int32 GetNumberPresentation() {
        // TODO: add PRESENTATION_URL
        If (VDBG) Log("getNumberPresentation: ret=PRESENTATION_ALLOWED");
        return PhoneConstants.PRESENTATION_ALLOWED;
    }

    //@Override
    public UUSInfo GetUUSInfo() {
        // FIXME: what's this for SIP?
        If (VDBG) Log("getUUSInfo: ? ret=NULL");
        return NULL;
    }

    //@Override
    public Int32 GetPreciseDisconnectCause() {
        return 0;
    }

    //@Override
    public Int64 GetHoldingStartTime() {
        return mHoldingStartTime;
    }

    //@Override
    public Int64 GetConnectTimeReal() {
        return mConnectTimeReal;
    }

    //@Override
    public Connection GetOrigConnection() {
        return NULL;
    }

    //@Override
    public Boolean IsMultiparty() {
        return FALSE;
    }
}
