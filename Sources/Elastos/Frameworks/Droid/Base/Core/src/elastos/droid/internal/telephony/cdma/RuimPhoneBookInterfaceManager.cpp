/*
** Copyright 2007, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

package com.android.internal.telephony.cdma;

using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telephony::IRlog;

using Elastos::Droid::Internal::Telephony::IIccPhoneBookInterfaceManager;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;

/**
 * RuimPhoneBookInterfaceManager to provide an inter-process communication to
 * access ADN-like SIM records.
 */


public class RuimPhoneBookInterfaceManager extends IccPhoneBookInterfaceManager {
    static const String LOG_TAG = "RuimPhoneBookIM";

    public RuimPhoneBookInterfaceManager(CDMAPhone phone) {
        Super(phone);
        //NOTE service "simphonebook" added by IccSmsInterfaceManagerProxy
    }

    //@Override
    CARAPI Dispose() {
        super->Dispose();
    }

    //@Override
    protected void Finalize() {
        try {
            super->Finalize();
        } Catch (Throwable throwable) {
            Rlog->E(LOG_TAG, "Error while finalizing:", throwable);
        }
        If(DBG) Rlog->D(LOG_TAG, "RuimPhoneBookInterfaceManager finalized");
    }

    //@Override
    public Int32[] GetAdnRecordsSize(Int32 efid) {
        If (DBG) Logd("getAdnRecordsSize: efid=" + efid);
        Synchronized(mLock) {
            CheckThread();
            mRecordSize = new Int32[3];

            //Using mBaseHandler, no difference in EVENT_GET_SIZE_DONE handling
            AtomicBoolean status = new AtomicBoolean(FALSE);
            Message response = mBaseHandler->ObtainMessage(EVENT_GET_SIZE_DONE, status);

            IccFileHandler fh = mPhone->GetIccFileHandler();
            //IccFileHandler can be NULL if there is no icc card present.
            If (fh != NULL) {
                fh->GetEFLinearRecordSize(efid, response);
                WaitForResult(status);
            }
        }

        return mRecordSize;
    }

    //@Override
    protected void Logd(String msg) {
        Rlog->D(LOG_TAG, "[RuimPbInterfaceManager] " + msg);
    }

    //@Override
    protected void Loge(String msg) {
        Rlog->E(LOG_TAG, "[RuimPbInterfaceManager] " + msg);
    }
}

