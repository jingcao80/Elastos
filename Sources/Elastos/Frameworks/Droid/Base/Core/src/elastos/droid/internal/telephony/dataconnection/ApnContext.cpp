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

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::INetworkConfig;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Droid::Internal::IR;
using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Internal::Telephony::IPhone;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger;

/**
 * Maintain the Apn context
 */
public class ApnContext {

    public final String LOG_TAG;

    protected static const Boolean DBG = FALSE;

    private final Context mContext;

    private final String mApnType;

    private DctConstants.State mState;

    private ArrayList<ApnSetting> mWaitingApns = NULL;

    public final Int32 priority;

    /** A zero indicates that all waiting APNs had a permanent error */
    private AtomicInteger mWaitingApnsPermanentFailureCountDown;

    private ApnSetting mApnSetting;

    DcAsyncChannel mDcAc;

    String mReason;

    PendingIntent mReconnectAlarmIntent;

    /**
     * user/app requested connection on this APN
     */
    AtomicBoolean mDataEnabled;

    private final Object mRefCountLock = new Object();
    private Int32 mRefCount = 0;

    /**
     * carrier requirements met
     */
    AtomicBoolean mDependencyMet;

    private final DcTrackerBase mDcTracker;

    public ApnContext(Context context, String apnType, String logTag, NetworkConfig config,
            DcTrackerBase tracker) {
        mContext = context;
        mApnType = apnType;
        mState = DctConstants.State.IDLE;
        SetReason(Phone.REASON_DATA_ENABLED);
        mDataEnabled = new AtomicBoolean(FALSE);
        mDependencyMet = new AtomicBoolean(config.dependencyMet);
        mWaitingApnsPermanentFailureCountDown = new AtomicInteger(0);
        priority = config.priority;
        LOG_TAG = logTag;
        mDcTracker = tracker;
    }

    public String GetApnType() {
        return mApnType;
    }

    public synchronized DcAsyncChannel GetDcAc() {
        return mDcAc;
    }

    public synchronized void SetDataConnectionAc(DcAsyncChannel dcac) {
        If (DBG) {
            Log("setDataConnectionAc: old dcac=" + mDcAc + " new dcac=" + dcac
                    + " this=" + this);
        }
        mDcAc = dcac;
    }

    public synchronized PendingIntent GetReconnectIntent() {
        return mReconnectAlarmIntent;
    }

    public synchronized void SetReconnectIntent(PendingIntent intent) {
        mReconnectAlarmIntent = intent;
    }

    public synchronized ApnSetting GetApnSetting() {
        Log("getApnSetting: apnSetting=" + mApnSetting);
        return mApnSetting;
    }

    public synchronized void SetApnSetting(ApnSetting apnSetting) {
        Log("setApnSetting: apnSetting=" + apnSetting);
        mApnSetting = apnSetting;
    }

    public synchronized void SetWaitingApns(ArrayList<ApnSetting> waitingApns) {
        mWaitingApns = waitingApns;
        mWaitingApnsPermanentFailureCountDown->Set(mWaitingApns->Size());
    }

    public Int32 GetWaitingApnsPermFailCount() {
        return mWaitingApnsPermanentFailureCountDown->Get();
    }

    CARAPI DecWaitingApnsPermFailCount() {
        mWaitingApnsPermanentFailureCountDown->DecrementAndGet();
    }

    public synchronized ApnSetting GetNextWaitingApn() {
        ArrayList<ApnSetting> list = mWaitingApns;
        ApnSetting apn = NULL;

        If (list != NULL) {
            If (!list->IsEmpty()) {
                apn = list->Get(0);
            }
        }
        return apn;
    }

    public synchronized void RemoveWaitingApn(ApnSetting apn) {
        If (mWaitingApns != NULL) {
            mWaitingApns->Remove(apn);
        }
    }

    public synchronized ArrayList<ApnSetting> GetWaitingApns() {
        return mWaitingApns;
    }

    public synchronized void SetState(DctConstants.State s) {
        If (DBG) {
            Log("setState: " + s + ", previous state:" + mState);
        }

        mState = s;

        If (mState == DctConstants.State.FAILED) {
            If (mWaitingApns != NULL) {
                mWaitingApns->Clear(); // when teardown the connection and set to IDLE
            }
        }
    }

    public synchronized DctConstants.State GetState() {
        return mState;
    }

    public Boolean IsDisconnected() {
        DctConstants.State currentState = GetState();
        Return ((currentState == DctConstants.State.IDLE) ||
                    currentState == DctConstants.State.FAILED);
    }

    public synchronized void SetReason(String reason) {
        If (DBG) {
            Log("set reason as " + reason + ",current state " + mState);
        }
        mReason = reason;
    }

    public synchronized String GetReason() {
        return mReason;
    }

    public Boolean IsReady() {
        return mDataEnabled->Get() && mDependencyMet->Get();
    }

    public Boolean IsConnectable() {
        return IsReady() && ((mState == DctConstants.State.IDLE)
                                || (mState == DctConstants.State.SCANNING)
                                || (mState == DctConstants.State.RETRYING)
                                || (mState == DctConstants.State.FAILED));
    }

    public Boolean IsConnectedOrConnecting() {
        return IsReady() && ((mState == DctConstants.State.CONNECTED)
                                || (mState == DctConstants.State.CONNECTING)
                                || (mState == DctConstants.State.SCANNING)
                                || (mState == DctConstants.State.RETRYING));
    }

    CARAPI SetEnabled(Boolean enabled) {
        If (DBG) {
            Log("set enabled as " + enabled + ", current state is " + mDataEnabled->Get());
        }
        mDataEnabled->Set(enabled);
    }

    public Boolean IsEnabled() {
        return mDataEnabled->Get();
    }

    CARAPI SetDependencyMet(Boolean met) {
        If (DBG) {
            Log("set mDependencyMet as " + met + " current state is " + mDependencyMet->Get());
        }
        mDependencyMet->Set(met);
    }

    public Boolean GetDependencyMet() {
       return mDependencyMet->Get();
    }

    public Boolean IsProvisioningApn() {
        String provisioningApn = mContext->GetResources()
                .GetString(R::string::mobile_provisioning_apn);
        If (!TextUtils->IsEmpty(provisioningApn) &&
                (mApnSetting != NULL) && (mApnSetting.apn != NULL)) {
            Return (mApnSetting.apn->Equals(provisioningApn));
        } else {
            return FALSE;
        }
    }

    CARAPI IncRefCount() {
        Synchronized (mRefCountLock) {
            If (mRefCount++ == 0) {
                mDcTracker->SetEnabled(mDcTracker->ApnTypeToId(mApnType), TRUE);
            }
        }
    }

    CARAPI DecRefCount() {
        Synchronized (mRefCountLock) {
            If ((mRefCount > 0) && (mRefCount-- == 1)) {
                mDcTracker->SetEnabled(mDcTracker->ApnTypeToId(mApnType), FALSE);
            } else {
                Log("Ignoring defCount request as mRefCount is: " + mRefCount);
            }
        }
    }

    //@Override
    public synchronized CARAPI ToString(
        /* [out] */ String* str)
    {
        // We don't print mDataConnection because its recursive.
        return "{mApnType=" + mApnType + " mState=" + GetState() + " mWaitingApns={" + mWaitingApns +
                "} mWaitingApnsPermanentFailureCountDown=" + mWaitingApnsPermanentFailureCountDown +
                " mApnSetting={" + mApnSetting + "} mReason=" + mReason +
                " mDataEnabled=" + mDataEnabled + " mDependencyMet=" + mDependencyMet + "}";
    }

    protected void Log(String s) {
        Rlog->D(LOG_TAG, "[ApnContext:" + mApnType + "] " + s);
    }

    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("ApnContext: " + this->ToString());
    }
}
