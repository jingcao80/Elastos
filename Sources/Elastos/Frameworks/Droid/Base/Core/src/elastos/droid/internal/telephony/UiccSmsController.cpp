/*
 * Copyright (C) 2008 The Android Open Source Project
 * Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 * Copyright (c) 2015 The CyanogenMod Project
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

using Elastos::Droid::IManifest;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Telephony::ISubscriptionManager;

using Elastos::Droid::Internal::Telephony::IISms;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::ISmsRawData;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

/**
 * UiccSmsController to provide an inter-process communication to
 * access Sms in Icc.
 */
public class UiccSmsController extends ISms.Stub {

    static const String LOG_TAG = "RIL_UiccSmsController";

    protected Phone[] mPhone;

    protected UiccSmsController(Phone[] phone, Context context){
        mPhone = phone;
        mContext = context;
        If (ServiceManager->GetService("isms") == NULL) {
            ServiceManager->AddService("isms", this);
        }

        CreateWakelock();
    }

    private void CreateWakelock() {
        PowerManager pm = (PowerManager)mContext->GetSystemService(Context.POWER_SERVICE);
        mWakeLock = pm->NewWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "IccSmsInterfaceManager");
        mWakeLock->SetReferenceCounted(TRUE);
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        CARAPI OnReceive(Context context, Intent intent) {
            // check if the message was aborted
            If (GetResultCode() != Activity.RESULT_OK) {
                return;
            }
            String destAddr = GetResultData();
            String scAddr = intent->GetStringExtra("scAddr");
            Int64 subId = intent->GetLongExtra("subId", GetDefaultSmsSubId());
            String callingPackage = intent->GetStringExtra("callingPackage");
            ArrayList<String> parts = intent->GetStringArrayListExtra("parts");
            ArrayList<PendingIntent> sentIntents = intent->GetParcelableArrayListExtra("sentIntents");
            ArrayList<PendingIntent> deliveryIntents = intent->GetParcelableArrayListExtra("deliveryIntents");

            If (intent->GetIntExtra("callingUid", 0) != 0) {
                callingPackage = callingPackage + "\\" + intent->GetIntExtra("callingUid", 0);
            }

            If (intent->GetBooleanExtra("multipart", FALSE)) {
                If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
                    Log("ProxiedMultiPartSms destAddr: " + destAddr +
                            "\n scAddr= " + scAddr +
                            "\n subId= " + subId +
                            "\n callingPackage= " + callingPackage +
                            "\n partsSize= " + parts->Size());
                }
                GetIccSmsInterfaceManager(subId)
                        .SendMultipartText(callingPackage, destAddr, scAddr, parts,
                                sentIntents, deliveryIntents);
                return;
            }

            PendingIntent sentIntent = NULL;
            If (sentIntents != NULL && sentIntents->Size() > 0) {
                sentIntent = sentIntents->Get(0);
            }
            PendingIntent deliveryIntent = NULL;
            If (deliveryIntents != NULL && deliveryIntents->Size() > 0) {
                deliveryIntent = deliveryIntents->Get(0);
            }
            String text = NULL;
            If (parts != NULL && parts->Size() > 0) {
                text = parts->Get(0);
            }
            If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
                Log("ProxiedSms destAddr: " + destAddr +
                        "\n scAddr=" + scAddr +
                        "\n subId= " + subId +
                        "\n callingPackage=" + callingPackage);
            }
            GetIccSmsInterfaceManager(subId).SendText(callingPackage, destAddr,
                    scAddr, text, sentIntent, deliveryIntent);
        }
    };

    private Context mContext;
    private PowerManager.WakeLock mWakeLock;
    private static const Int32 WAKE_LOCK_TIMEOUT = 5000;
    private final Handler mHandler = new Handler();
    private void DispatchPdus(Byte[][] pdus) {
        Intent intent = new Intent(Intents.SMS_DELIVER_ACTION);
        // Direct the intent to only the default SMS app. If we can't find a default SMS app
        // then send it to all broadcast receivers.
        ComponentName componentName = SmsApplication->GetDefaultSmsApplication(mContext, TRUE);
        If (componentName == NULL)
            return;

        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Log("dispatchPdu pdus: " + pdus +
                    "\n componentName=" + componentName +
                    "\n format=" + SmsMessage.FORMAT_SYNTHETIC);
        }

        // Deliver SMS message only to this receiver
        intent->SetComponent(componentName);
        intent->PutExtra("pdus", pdus);
        intent->PutExtra("format", SmsMessage.FORMAT_SYNTHETIC);
        Dispatch(intent, Manifest::permission::RECEIVE_SMS);

        intent->SetAction(Intents.SMS_RECEIVED_ACTION);
        intent->SetComponent(NULL);
        Dispatch(intent, Manifest::permission::RECEIVE_SMS);
    }

    private void Dispatch(Intent intent, String permission) {
        // Hold a wake lock for WAKE_LOCK_TIMEOUT seconds, enough to give any
        // receivers time to take their own wake locks.
        mWakeLock->Acquire(WAKE_LOCK_TIMEOUT);
        intent->AddFlags(IIntent::FLAG_RECEIVER_NO_ABORT);
        mContext->SendOrderedBroadcast(intent, permission, AppOpsManager.OP_RECEIVE_SMS, NULL,
                mHandler, Activity.RESULT_OK, NULL, NULL);
    }

    private void BroadcastOutgoingSms(
            Int64 subId, String callingPackage, String destAddr, String scAddr, Boolean multipart,
            ArrayList<String> parts, ArrayList<PendingIntent> sentIntents,
            ArrayList<PendingIntent> deliveryIntents, Int32 priority, Boolean isExpectMore,
            Int32 validityPeriod) {
        Intent broadcast = new Intent(IIntent::ACTION_NEW_OUTGOING_SMS);
        broadcast->PutExtra("destAddr", destAddr);
        broadcast->PutExtra("scAddr", scAddr);
        broadcast->PutExtra("subId", subId);
        broadcast->PutExtra("multipart", multipart);
        broadcast->PutExtra("callingPackage", callingPackage);
        broadcast->PutExtra("callingUid", android.os.Binder->GetCallingUid());
        broadcast->PutStringArrayListExtra("parts", parts);
        broadcast->PutParcelableArrayListExtra("sentIntents", sentIntents);
        broadcast->PutParcelableArrayListExtra("deliveryIntents", deliveryIntents);
        broadcast->PutExtra("priority", priority);
        broadcast->PutExtra("isExpectMore", isExpectMore);
        broadcast->PutExtra("validityPeriod", validityPeriod);

        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Log("Broadcasting sms destAddr: " + destAddr +
                    "\n scAddr= " + scAddr +
                    "\n subId= " + subId +
                    "\n multipart= " + multipart +
                    "\n callingPackager= " + callingPackage +
                    "\n callingUid= " + android.os.Binder->GetCallingUid() +
                    "\n parts= " + parts->Size() +
                    "\n sentIntents= " + sentIntents->Size() +
                    "\n deliveryIntents= " + deliveryIntents->Size() +
                    "\n priority= " + priority +
                    "\n isExpectMore= " + isExpectMore +
                    "\n validityPeriod= " + validityPeriod);
        }
        mContext->SendOrderedBroadcastAsUser(broadcast, UserHandle.OWNER,
                Manifest::permission::INTERCEPT_SMS,
                mReceiver, NULL, Activity.RESULT_OK, destAddr, NULL);
    }

    public Boolean
    UpdateMessageOnIccEf(String callingPackage, Int32 index, Int32 status, Byte[] pdu)
            throws android.os.RemoteException {
        return  UpdateMessageOnIccEfForSubscriber(GetDefaultSmsSubId(), callingPackage,
                index, status, pdu);
    }

    public Boolean
    UpdateMessageOnIccEfForSubscriber(Int64 subId, String callingPackage, Int32 index, Int32 status,
                Byte[] pdu) throws android.os.RemoteException {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL) {
            return iccSmsIntMgr->UpdateMessageOnIccEf(callingPackage, index, status, pdu);
        } else {
            Rlog->E(LOG_TAG,"updateMessageOnIccEf iccSmsIntMgr is NULL" +
                          " for Subscription: " + subId);
            return FALSE;
        }
    }

    public Boolean CopyMessageToIccEf(String callingPackage, Int32 status, Byte[] pdu, Byte[] smsc)
            throws android.os.RemoteException {
        return CopyMessageToIccEfForSubscriber(GetDefaultSmsSubId(), callingPackage, status,
                pdu, smsc);
    }

    public Boolean CopyMessageToIccEfForSubscriber(Int64 subId, String callingPackage, Int32 status,
            Byte[] pdu, Byte[] smsc) throws android.os.RemoteException {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL) {
            return iccSmsIntMgr->CopyMessageToIccEf(callingPackage, status, pdu, smsc);
        } else {
            Rlog->E(LOG_TAG,"copyMessageToIccEf iccSmsIntMgr is NULL" +
                          " for Subscription: " + subId);
            return FALSE;
        }
    }

    public List<SmsRawData> GetAllMessagesFromIccEf(String callingPackage)
            throws android.os.RemoteException {
        return GetAllMessagesFromIccEfForSubscriber(GetDefaultSmsSubId(), callingPackage);
    }

    public List<SmsRawData> GetAllMessagesFromIccEfForSubscriber(Int64 subId, String callingPackage)
                throws android.os.RemoteException {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL) {
            return iccSmsIntMgr->GetAllMessagesFromIccEf(callingPackage);
        } else {
            Rlog->E(LOG_TAG,"getAllMessagesFromIccEf iccSmsIntMgr is" +
                          " NULL for Subscription: " + subId);
            return NULL;
        }
    }

    CARAPI SendData(String callingPackage, String destAddr, String scAddr, Int32 destPort,
            Byte[] data, PendingIntent sentIntent, PendingIntent deliveryIntent) {
         SendDataForSubscriber(GetDefaultSmsSubId(), callingPackage, destAddr, scAddr,
                 destPort, data, sentIntent, deliveryIntent);
    }

    CARAPI SendDataForSubscriber(Int64 subId, String callingPackage, String destAddr,
            String scAddr, Int32 destPort, Byte[] data, PendingIntent sentIntent,
            PendingIntent deliveryIntent) {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL) {
            iccSmsIntMgr->SendData(callingPackage, destAddr, scAddr, destPort, data,
                    sentIntent, deliveryIntent);
        } else {
            Rlog->E(LOG_TAG,"sendText iccSmsIntMgr is NULL for" +
                          " Subscription: " + subId);
        }
    }

    CARAPI SendDataWithOrigPort(String callingPackage, String destAddr, String scAddr,
            Int32 destPort, Int32 origPort, Byte[] data, PendingIntent sentIntent,
            PendingIntent deliveryIntent) {
         SendDataWithOrigPortUsingSubscriber(GetDefaultSmsSubId(), callingPackage, destAddr,
                 scAddr, destPort, origPort, data, sentIntent, deliveryIntent);
    }

    CARAPI SendDataWithOrigPortUsingSubscriber(Int64 subId, String callingPackage,
            String destAddr, String scAddr, Int32 destPort, Int32 origPort, Byte[] data,
            PendingIntent sentIntent, PendingIntent deliveryIntent) {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL) {
            iccSmsIntMgr->SendDataWithOrigPort(callingPackage, destAddr, scAddr, destPort,
                    origPort, data, sentIntent, deliveryIntent);
        } else {
            Rlog->E(LOG_TAG,"sendTextWithOrigPort iccSmsIntMgr is NULL for" +
                          " Subscription: " + subId);
        }
    }

    CARAPI SendText(String callingPackage, String destAddr, String scAddr,
            String text, PendingIntent sentIntent, PendingIntent deliveryIntent) {
        SendTextForSubscriber(GetDefaultSmsSubId(), callingPackage, destAddr, scAddr,
            text, sentIntent, deliveryIntent);
    }

    CARAPI SendTextForSubscriber(Int64 subId, String callingPackage, String destAddr,
            String scAddr, String text, PendingIntent sentIntent, PendingIntent deliveryIntent) {
        SendTextWithOptionsUsingSubscriber(subId, callingPackage, destAddr, scAddr, text,
                sentIntent, deliveryIntent, -1, FALSE, -1);
    }

    CARAPI SendTextWithOptionsUsingSubscriber(Int64 subId, String callingPackage,
            String destAddr, String scAddr, String text, PendingIntent sentIntent,
            PendingIntent deliveryIntent, Int32 priority, Boolean isExpectMore,
            Int32 validityPeriod) {
        mContext->EnforceCallingPermission(
                Manifest::permission::SEND_SMS,
                "Sending SMS message");
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr->IsShortSMSCode(destAddr)) {
            iccSmsIntMgr->SendTextWithOptions(callingPackage, destAddr, scAddr, text,
                    sentIntent, deliveryIntent, priority, isExpectMore, validityPeriod);
            return;
        }
        ArrayList<String> parts = new ArrayList<String>();
        parts->Add(text);
        ArrayList<PendingIntent> sentIntents = new ArrayList<PendingIntent>();
        sentIntents->Add(sentIntent);
        ArrayList<PendingIntent> deliveryIntents = new ArrayList<PendingIntent>();
        deliveryIntents->Add(deliveryIntent);
        BroadcastOutgoingSms(subId, callingPackage, destAddr, scAddr, FALSE, parts, sentIntents,
                deliveryIntents, priority, isExpectMore, validityPeriod);
    }

    CARAPI SendMultipartText(String callingPackage, String destAddr, String scAddr,
            List<String> parts, List<PendingIntent> sentIntents,
            List<PendingIntent> deliveryIntents) throws android.os.RemoteException {
         SendMultipartTextForSubscriber(GetDefaultSmsSubId(), callingPackage, destAddr,
                 scAddr, parts, sentIntents, deliveryIntents);
    }

    CARAPI SendMultipartTextForSubscriber(Int64 subId, String callingPackage, String destAddr,
            String scAddr, List<String> parts, List<PendingIntent> sentIntents,
            List<PendingIntent> deliveryIntents)
            throws android.os.RemoteException {
        SendMultipartTextWithOptionsUsingSubscriber(subId, callingPackage, destAddr,
                scAddr, parts, sentIntents, deliveryIntents, -1, FALSE, -1);
    }

    CARAPI SendMultipartTextWithOptionsUsingSubscriber(Int64 subId, String callingPackage,
            String destAddr, String scAddr, List<String> parts, List<PendingIntent> sentIntents,
            List<PendingIntent> deliveryIntents, Int32 priority, Boolean isExpectMore,
            Int32 validityPeriod) {
        mContext->EnforceCallingPermission(
                Manifest::permission::SEND_SMS,
                "Sending SMS message");
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr->IsShortSMSCode(destAddr)) {
            iccSmsIntMgr->SendMultipartTextWithOptions(callingPackage, destAddr,
                    scAddr, parts, sentIntents, deliveryIntents, -1, FALSE, -1);
            return;
        }
        BroadcastOutgoingSms(subId, callingPackage, destAddr, scAddr, TRUE,
                parts != NULL ? new ArrayList<String>(parts) : NULL,
                sentIntents != NULL ? new ArrayList<PendingIntent>(sentIntents) : NULL,
                deliveryIntents != NULL ? new ArrayList<PendingIntent>(deliveryIntents) : NULL,
                -1, FALSE, -1);
    }

    public Boolean EnableCellBroadcast(Int32 messageIdentifier) throws android.os.RemoteException {
        return EnableCellBroadcastForSubscriber(GetDefaultSmsSubId(), messageIdentifier);
    }

    public Boolean EnableCellBroadcastForSubscriber(Int64 subId, Int32 messageIdentifier)
                throws android.os.RemoteException {
        return EnableCellBroadcastRangeForSubscriber(subId, messageIdentifier, messageIdentifier);
    }

    public Boolean EnableCellBroadcastRange(Int32 startMessageId, Int32 endMessageId)
            throws android.os.RemoteException {
        return EnableCellBroadcastRangeForSubscriber(GetDefaultSmsSubId(), startMessageId,
                endMessageId);
    }

    public Boolean EnableCellBroadcastRangeForSubscriber(Int64 subId, Int32 startMessageId,
            Int32 endMessageId) throws android.os.RemoteException {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL ) {
            return iccSmsIntMgr->EnableCellBroadcastRange(startMessageId, endMessageId);
        } else {
            Rlog->E(LOG_TAG,"enableCellBroadcast iccSmsIntMgr is NULL for" +
                          " Subscription: " + subId);
        }
        return FALSE;
    }

    public Boolean DisableCellBroadcast(Int32 messageIdentifier) throws android.os.RemoteException {
        return DisableCellBroadcastForSubscriber(GetDefaultSmsSubId(), messageIdentifier);
    }

    public Boolean DisableCellBroadcastForSubscriber(Int64 subId, Int32 messageIdentifier)
                throws android.os.RemoteException {
        return DisableCellBroadcastRangeForSubscriber(subId, messageIdentifier, messageIdentifier);
    }

    public Boolean DisableCellBroadcastRange(Int32 startMessageId, Int32 endMessageId)
            throws android.os.RemoteException {
        return DisableCellBroadcastRangeForSubscriber(GetDefaultSmsSubId(), startMessageId,
                endMessageId);
    }

    public Boolean DisableCellBroadcastRangeForSubscriber(Int64 subId, Int32 startMessageId,
            Int32 endMessageId) throws android.os.RemoteException {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL ) {
            return iccSmsIntMgr->DisableCellBroadcastRange(startMessageId, endMessageId);
        } else {
            Rlog->E(LOG_TAG,"disableCellBroadcast iccSmsIntMgr is NULL for" +
                          " Subscription:"+subId);
        }
       return FALSE;
    }

    public Int32 GetPremiumSmsPermission(String packageName) {
        return GetPremiumSmsPermissionForSubscriber(GetDefaultSmsSubId(), packageName);
    }

    //@Override
    public Int32 GetPremiumSmsPermissionForSubscriber(Int64 subId, String packageName) {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL ) {
            return iccSmsIntMgr->GetPremiumSmsPermission(packageName);
        } else {
            Rlog->E(LOG_TAG, "getPremiumSmsPermission iccSmsIntMgr is NULL");
        }
        //TODO Rakesh
        return 0;
    }

    CARAPI SetPremiumSmsPermission(String packageName, Int32 permission) {
         SetPremiumSmsPermissionForSubscriber(GetDefaultSmsSubId(), packageName, permission);
    }

    //@Override
    CARAPI SetPremiumSmsPermissionForSubscriber(Int64 subId, String packageName, Int32 permission) {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL ) {
            iccSmsIntMgr->SetPremiumSmsPermission(packageName, permission);
        } else {
            Rlog->E(LOG_TAG, "setPremiumSmsPermission iccSmsIntMgr is NULL");
        }
    }

    public Boolean IsImsSmsSupported() {
        return IsImsSmsSupportedForSubscriber(GetDefaultSmsSubId());
    }

    //@Override
    public Boolean IsImsSmsSupportedForSubscriber(Int64 subId) {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL ) {
            return iccSmsIntMgr->IsImsSmsSupported();
        } else {
            Rlog->E(LOG_TAG, "isImsSmsSupported iccSmsIntMgr is NULL");
        }
        return FALSE;
    }

    public String GetImsSmsFormat() {
        return GetImsSmsFormatForSubscriber(GetDefaultSmsSubId());
    }

    //@Override
    public String GetImsSmsFormatForSubscriber(Int64 subId) {
       IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL ) {
            return iccSmsIntMgr->GetImsSmsFormat();
        } else {
            Rlog->E(LOG_TAG, "getImsSmsFormat iccSmsIntMgr is NULL");
        }
        return NULL;
    }

    //@Override
    CARAPI UpdateSmsSendStatus(Int32 messageRef, Boolean success) {
        GetIccSmsInterfaceManager(GetDefaultSmsSubId())
            .UpdateSmsSendStatus(messageRef, success);
    }

    //@Override
    CARAPI InjectSmsPdu(Byte[] pdu, String format, PendingIntent receivedIntent) {
        InjectSmsPduForSubscriber(GetDefaultSmsSubId(), pdu, format, receivedIntent);
    }

    // FIXME: Add injectSmsPduForSubscriber to ISms.aidl
    CARAPI InjectSmsPduForSubscriber(Int64 subId, Byte[] pdu, String format,
            PendingIntent receivedIntent) {
        GetIccSmsInterfaceManager(subId).InjectSmsPdu(pdu, format, receivedIntent);
    }

    //@Override
    CARAPI SynthesizeMessages(String originatingAddress,
            String scAddress, List<String> messages, Int64 timestampMillis) throws RemoteException {
        mContext->EnforceCallingPermission(
                Manifest::permission::BROADCAST_SMS, "");
        Byte[][] pdus = new Byte[messages->Size()][];
        For (Int32 i = 0; i < messages->Size(); i++) {
            SyntheticSmsMessage message = new SyntheticSmsMessage(originatingAddress,
                    scAddress, messages->Get(i), timestampMillis);
            pdus[i] = message->GetPdu();
        }
        DispatchPdus(pdus);
    }

    /**
     * get sms interface manager object based on subscription.
     **/
    private IccSmsInterfaceManager GetIccSmsInterfaceManager(Int64 subId) {
        Int32 phoneId = SubscriptionController->GetInstance()->GetPhoneId(subId) ;
        //Fixme: for multi-subscription case
        If (!SubscriptionManager->IsValidPhoneId(phoneId)
                || phoneId == SubscriptionManager.DEFAULT_PHONE_ID) {
            phoneId = 0;
        }

        try {
            Return (IccSmsInterfaceManager)
                ((PhoneProxy)mPhone[phoneId]).GetIccSmsInterfaceManager();
        } Catch (NullPointerException e) {
            Rlog->E(LOG_TAG, "Exception is :"+e->ToString()+" For subscription :"+subId );
            e->PrintStackTrace(); //This will print stact trace
            return NULL;
        } Catch (ArrayIndexOutOfBoundsException e) {
            Rlog->E(LOG_TAG, "Exception is :"+e->ToString()+" For subscription :"+subId );
            e->PrintStackTrace(); //This will print stack trace
            return NULL;
        }
    }

    private Int64 GetDefaultSmsSubId() {
        return SubscriptionController->GetInstance()->GetDefaultSmsSubId();
    }

    //@Override
    CARAPI SendStoredText(Int64 subId, String callingPkg, Uri messageUri, String scAddress,
            PendingIntent sentIntent, PendingIntent deliveryIntent) throws RemoteException {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL) {
            iccSmsIntMgr->SendStoredText(callingPkg, messageUri, scAddress, sentIntent,
                    deliveryIntent);
        } else {
            Rlog->E(LOG_TAG,"sendStoredText iccSmsIntMgr is NULL for subscription: " + subId);
        }
    }

    //@Override
    CARAPI SendStoredMultipartText(Int64 subId, String callingPkg, Uri messageUri,
            String scAddress, List<PendingIntent> sentIntents, List<PendingIntent> deliveryIntents)
            throws RemoteException {
        IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);
        If (iccSmsIntMgr != NULL ) {
            iccSmsIntMgr->SendStoredMultipartText(callingPkg, messageUri, scAddress, sentIntents,
                    deliveryIntents);
        } else {
            Rlog->E(LOG_TAG,"sendStoredMultipartText iccSmsIntMgr is NULL for subscription: "
                    + subId);
        }
    }

    /**
     * Get the capacity count of sms on Icc card.
     **/
    public Int32 GetSmsCapacityOnIccForSubscriber(Int64 subId)
            throws android.os.RemoteException {
       IccSmsInterfaceManager iccSmsIntMgr = GetIccSmsInterfaceManager(subId);

        If (iccSmsIntMgr != NULL ) {
            return iccSmsIntMgr->GetSmsCapacityOnIcc();
        } else {
            Rlog->E(LOG_TAG, "iccSmsIntMgr is NULL for " + " subId: " + subId);
            return -1;
        }
    }

    protected void Log(String msg) {
        Logger::D(LOG_TAG, "[UiccSmsController] " + msg);
    }
}
