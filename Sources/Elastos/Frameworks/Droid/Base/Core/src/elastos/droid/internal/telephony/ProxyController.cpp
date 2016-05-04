/*
 * Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

package com.android.internal.telephony;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IMessage;

using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDctController;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

public class ProxyController {
    static const String LOG_TAG = "ProxyController";

    //***** Class Variables
    private static ProxyController sProxyController;

    private Phone[] mProxyPhones;

    private UiccController mUiccController;

    private CommandsInterface[] mCi;

    private Context mContext;

    private static DctController mDctController;

    //UiccPhoneBookController to use proper IccPhoneBookInterfaceManagerProxy object
    private UiccPhoneBookController mUiccPhoneBookController;

    //PhoneSubInfoController to use proper PhoneSubInfoProxy object
    private PhoneSubInfoController mPhoneSubInfoController;

    //UiccSmsController to use proper IccSmsInterfaceManager object
    private UiccSmsController mUiccSmsController;

  //  private SubscriptionManager mSubscriptionManager;

    //***** Class Methods
    public static ProxyController GetInstance(Context context, Phone[] phoneProxy,
            UiccController uiccController, CommandsInterface[] ci) {
        If (sProxyController == NULL) {
            sProxyController = new ProxyController(context, phoneProxy, uiccController, ci);
        }
        return sProxyController;
    }

    static public ProxyController GetInstance() {
        return sProxyController;
    }

    private ProxyController(Context context, Phone[] phoneProxy, UiccController uiccController,
            CommandsInterface[] ci) {
        Logd("Constructor - Enter");

        mContext = context;
        mProxyPhones = phoneProxy;
        mUiccController = uiccController;
        mCi = ci;

        HandlerThread t = new HandlerThread("DctControllerThread");
        t->Start();

        mDctController = DctController->MakeDctController((PhoneProxy[])phoneProxy, t->GetLooper());
        mUiccPhoneBookController = new UiccPhoneBookController(mProxyPhones);
        mPhoneSubInfoController = new PhoneSubInfoController(mProxyPhones);
        mUiccSmsController = new UiccSmsController(mProxyPhones, context);
       // mSubscriptionManager = SubscriptionManager->GetInstance(context, uiccController, ci);

        Logd("Constructor - Exit");
    }

    CARAPI UpdateDataConnectionTracker(Int32 sub) {
        ((PhoneProxy) mProxyPhones[sub]).UpdateDataConnectionTracker();
    }

    CARAPI EnableDataConnectivity(Int32 sub) {
        ((PhoneProxy) mProxyPhones[sub]).SetInternalDataEnabled(TRUE);
    }

    CARAPI DisableDataConnectivity(Int32 sub,
            Message dataCleanedUpMsg) {
        ((PhoneProxy) mProxyPhones[sub]).SetInternalDataEnabled(FALSE, dataCleanedUpMsg);
    }

    CARAPI UpdateCurrentCarrierInProvider(Int32 sub) {
        ((PhoneProxy) mProxyPhones[sub]).UpdateCurrentCarrierInProvider();
    }

    CARAPI RegisterForAllDataDisconnected(Int64 subId, Handler h, Int32 what, Object obj) {
        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);

        If (phoneId >= 0 && phoneId < TelephonyManager->GetDefault()->GetPhoneCount()) {
            ((PhoneProxy) mProxyPhones[phoneId]).RegisterForAllDataDisconnected(h, what, obj);
        }
    }

    CARAPI UnregisterForAllDataDisconnected(Int64 subId, Handler h) {
        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);

        If (phoneId >= 0 && phoneId < TelephonyManager->GetDefault()->GetPhoneCount()) {
            ((PhoneProxy) mProxyPhones[phoneId]).UnregisterForAllDataDisconnected(h);
        }
    }

    public Boolean IsDataDisconnected(Int64 subId) {
        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId);

        If (phoneId >= 0 && phoneId < TelephonyManager->GetDefault()->GetPhoneCount()) {
            Phone activePhone = ((PhoneProxy) mProxyPhones[phoneId]).GetActivePhone();
            Return ((PhoneBase) activePhone).mDcTracker->IsDisconnected();
        } else {
            return FALSE;
        }
    }

    private void Logd(String string) {
        Rlog->D(LOG_TAG, string);
    }
}
