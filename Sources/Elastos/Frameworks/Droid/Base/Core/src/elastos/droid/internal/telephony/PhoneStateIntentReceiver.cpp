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

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IRlog;

/**
 *
 *                            DO NOT USE THIS CLASS:
 *
 *      Use android.telephony.TelephonyManager and PhoneStateListener instead.
 *
 *
 */
@Deprecated
public class PhoneStateIntentReceiver extends BroadcastReceiver {
    private static const String LOG_TAG = "PhoneStatIntentReceiver";
    private static const Boolean DBG = FALSE;

    private static const Int32 NOTIF_PHONE    = 1 << 0;
    private static const Int32 NOTIF_SERVICE  = 1 << 1;
    private static const Int32 NOTIF_SIGNAL   = 1 << 2;

    PhoneConstants.State mPhoneState = PhoneConstants.State.IDLE;
    ServiceState mServiceState = new ServiceState();
    SignalStrength mSignalStrength = new SignalStrength();

    private Context mContext;
    private Handler mTarget;
    private IntentFilter mFilter;
    private Int32 mWants;
    private Int32 mPhoneStateEventWhat;
    private Int32 mServiceStateEventWhat;
    private Int32 mAsuEventWhat;

    public PhoneStateIntentReceiver() {
        Super();
        mFilter = new IntentFilter();
    }

    public PhoneStateIntentReceiver(Context context, Handler target) {
        This();
        SetContext(context);
        SetTarget(target);
    }

    CARAPI SetContext(Context c) {
        mContext = c;
    }

    CARAPI SetTarget(Handler h) {
        mTarget = h;
    }

    public PhoneConstants.State GetPhoneState() {
        If ((mWants & NOTIF_PHONE) == 0) {
            throw new RuntimeException
                ("client must call NotifyPhoneCallState(Int32)");
        }
        return mPhoneState;
    }

    public ServiceState GetServiceState() {
        If ((mWants & NOTIF_SERVICE) == 0) {
            throw new RuntimeException
                ("client must call NotifyServiceState(Int32)");
        }
        return mServiceState;
    }

    /**
     * Returns current signal strength in as an asu 0..31
     *
     * Throws RuntimeException if client has not called NotifySignalStrength()
     */
    public Int32 GetSignalStrengthLevelAsu() {
        // TODO: use new SignalStrength instead of asu
        If ((mWants & NOTIF_SIGNAL) == 0) {
            throw new RuntimeException
                ("client must call NotifySignalStrength(Int32)");
        }
        return mSignalStrength->GetAsuLevel();
    }

    /**
     * Return current signal strength in "dBm", ranging from -113 - -51dBm
     * or -1 if unknown
     *
     * @return signal strength in dBm, -1 if not yet updated
     * Throws RuntimeException if client has not called NotifySignalStrength()
     */
    public Int32 GetSignalStrengthDbm() {
        If ((mWants & NOTIF_SIGNAL) == 0) {
            throw new RuntimeException
                ("client must call NotifySignalStrength(Int32)");
        }
        return mSignalStrength->GetDbm();
    }

    CARAPI NotifyPhoneCallState(Int32 eventWhat) {
        mWants |= NOTIF_PHONE;
        mPhoneStateEventWhat = eventWhat;
        mFilter->AddAction(TelephonyManager.ACTION_PHONE_STATE_CHANGED);
    }

    public Boolean GetNotifyPhoneCallState() {
        Return ((mWants & NOTIF_PHONE) != 0);
    }

    CARAPI NotifyServiceState(Int32 eventWhat) {
        mWants |= NOTIF_SERVICE;
        mServiceStateEventWhat = eventWhat;
        mFilter->AddAction(TelephonyIntents.ACTION_SERVICE_STATE_CHANGED);
    }

    public Boolean GetNotifyServiceState() {
        Return ((mWants & NOTIF_SERVICE) != 0);
    }

    CARAPI NotifySignalStrength (Int32 eventWhat) {
        mWants |= NOTIF_SIGNAL;
        mAsuEventWhat = eventWhat;
        mFilter->AddAction(TelephonyIntents.ACTION_SIGNAL_STRENGTH_CHANGED);
    }

    public Boolean GetNotifySignalStrength() {
        Return ((mWants & NOTIF_SIGNAL) != 0);
    }

    CARAPI RegisterIntent() {
        mContext->RegisterReceiver(this, mFilter);
    }

    CARAPI UnregisterIntent() {
        mContext->UnregisterReceiver(this);
    }

    //@Override
    CARAPI OnReceive(Context context, Intent intent) {
        String action = intent->GetAction();

        try {
            If (TelephonyIntents.ACTION_SIGNAL_STRENGTH_CHANGED->Equals(action)) {
                mSignalStrength = SignalStrength->NewFromBundle(intent->GetExtras());

                If (mTarget != NULL && GetNotifySignalStrength()) {
                    Message message = Message->Obtain(mTarget, mAsuEventWhat);
                    mTarget->SendMessage(message);
                }
            } else If (TelephonyManager.ACTION_PHONE_STATE_CHANGED->Equals(action)) {
                If (DBG) Rlog->D(LOG_TAG, "onReceiveIntent: ACTION_PHONE_STATE_CHANGED, state="
                               + intent->GetStringExtra(PhoneConstants.STATE_KEY));
                String phoneState = intent->GetStringExtra(PhoneConstants.STATE_KEY);
                mPhoneState = Enum->ValueOf(
                        PhoneConstants.State.class, phoneState);

                If (mTarget != NULL && GetNotifyPhoneCallState()) {
                    Message message = Message->Obtain(mTarget,
                            mPhoneStateEventWhat);
                    mTarget->SendMessage(message);
                }
            } else If (TelephonyIntents.ACTION_SERVICE_STATE_CHANGED->Equals(action)) {
                mServiceState = ServiceState->NewFromBundle(intent->GetExtras());

                If (mTarget != NULL && GetNotifyServiceState()) {
                    Message message = Message->Obtain(mTarget,
                            mServiceStateEventWhat);
                    mTarget->SendMessage(message);
                }
            }
        } Catch (Exception ex) {
            Rlog->E(LOG_TAG, "[PhoneStateIntentRecv] caught " + ex);
            ex->PrintStackTrace();
        }
    }

}
