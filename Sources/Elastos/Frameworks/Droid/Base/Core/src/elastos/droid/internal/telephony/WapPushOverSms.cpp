/*
 * Copyright (C) 2008 The Android Open Source Project
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

using com::Google::Android::Mms::Pdu::IDeliveryInd;
using com::Google::Android::Mms::Pdu::IGenericPdu;
using com::Google::Android::Mms::Pdu::INotificationInd;
using com::Google::Android::Mms::Pdu::IPduHeaders;
using com::Google::Android::Mms::Pdu::IPduParser;
using com::Google::Android::Mms::Pdu::IPduPersister;
using com::Google::Android::Mms::Pdu::IReadOrigInd;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IDatabaseUtils;
using Elastos::Droid::Database::Sqlite::ISqliteWrapper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;

using static com::Google::Android::Mms::Pdu::PduHeaders::IMESSAGE_TYPE_DELIVERY_IND;
using static com::Google::Android::Mms::Pdu::PduHeaders::IMESSAGE_TYPE_NOTIFICATION_IND;
using static com::Google::Android::Mms::Pdu::PduHeaders::IMESSAGE_TYPE_READ_ORIG_IND;

/**
 * WAP push handler class.
 *
 * @hide
 */
public class WapPushOverSms implements ServiceConnection {
    private static const String TAG = "WAP PUSH";
    private static const Boolean DBG = TRUE;

    private final Context mContext;

    /** Assigned from ServiceConnection callback on main threaad. */
    private volatile IWapPushManager mWapPushManager;

    //@Override
    CARAPI OnServiceConnected(ComponentName name, IBinder service) {
        mWapPushManager = IWapPushManager.Stub->AsInterface(service);
        If (DBG) Rlog->V(TAG, "wappush manager connected to " + HashCode());
    }

    //@Override
    CARAPI OnServiceDisconnected(ComponentName name) {
        mWapPushManager = NULL;
        If (DBG) Rlog->V(TAG, "wappush manager disconnected.");
    }

    public WapPushOverSms(Context context) {
        mContext = context;
        Intent intent = new Intent(IWapPushManager.class->GetName());
        ComponentName comp = intent->ResolveSystemService(context->GetPackageManager(), 0);
        intent->SetComponent(comp);
        If (comp == NULL || !context->BindService(intent, this, Context.BIND_AUTO_CREATE)) {
            Rlog->E(TAG, "BindService() for wappush manager failed");
        } else {
            If (DBG) Rlog->V(TAG, "BindService() for wappush manager succeeded");
        }
    }

    void Dispose() {
        If (mWapPushManager != NULL) {
            If (DBG) Rlog->V(TAG, "dispose: unbind wappush manager");
            mContext->UnbindService(this);
        } else {
            Rlog->E(TAG, "dispose: not bound to a wappush manager");
        }
    }

    /**
     * Dispatches inbound messages that are in the WAP PDU format. See
     * wap-230-wsp-20010705-a section 8 for details on the WAP PDU format.
     *
     * @param pdu The WAP PDU, made up of one or more SMS PDUs
     * @param address The originating address
     * @return a result code from {@link android.provider.Telephony.Sms.Intents}, or
     *         {@link Activity#RESULT_OK} if the message has been broadcast
     *         to applications
     */
    public Int32 DispatchWapPdu(Byte[] pdu, BroadcastReceiver receiver, InboundSmsHandler handler,
            String address) {

        If (DBG) Rlog->D(TAG, "Rx: " + IccUtils->BytesToHexString(pdu));

        try {
            Int32 index = 0;
            Int32 transactionId = pdu[index++] & 0xFF;
            Int32 pduType = pdu[index++] & 0xFF;

            // Should we "abort" if no subId for now just no supplying extra param below
            Int32 phoneId = handler->GetPhone()->GetPhoneId();

            If ((pduType != WspTypeDecoder.PDU_TYPE_PUSH) &&
                    (pduType != WspTypeDecoder.PDU_TYPE_CONFIRMED_PUSH)) {
                index = mContext->GetResources()->GetInteger(
                        R.integer.config_valid_wappush_index);
                If (index != -1) {
                    transactionId = pdu[index++] & 0xff;
                    pduType = pdu[index++] & 0xff;
                    If (DBG)
                        Rlog->D(TAG, "index = " + index + " PDU Type = " + pduType +
                                " transactionID = " + transactionId);

                    // recheck wap push pduType
                    If ((pduType != WspTypeDecoder.PDU_TYPE_PUSH)
                            && (pduType != WspTypeDecoder.PDU_TYPE_CONFIRMED_PUSH)) {
                        If (DBG) Rlog->W(TAG, "Received non-PUSH WAP PDU. Type = " + pduType);
                        return Intents.RESULT_SMS_HANDLED;
                    }
                } else {
                    If (DBG) Rlog->W(TAG, "Received non-PUSH WAP PDU. Type = " + pduType);
                    return Intents.RESULT_SMS_HANDLED;
                }
            }

            WspTypeDecoder pduDecoder = new WspTypeDecoder(pdu);

            /**
             * Parse HeaderLen(unsigned integer).
             * From wap-230-wsp-20010705-a section 8.1.2
             * The maximum size of a uintvar is 32 bits.
             * So it will be encoded in no more than 5 octets.
             */
            If (pduDecoder->DecodeUintvarInteger(index) == FALSE) {
                If (DBG) Rlog->W(TAG, "Received PDU. Header Length error.");
                return Intents.RESULT_SMS_GENERIC_ERROR;
            }
            Int32 headerLength = (Int32) pduDecoder->GetValue32();
            index += pduDecoder->GetDecodedDataLength();

            Int32 headerStartIndex = index;

            /**
             * Parse Content-Type.
             * From wap-230-wsp-20010705-a section 8.4.2.24
             *
             * Content-type-value = Constrained-media | Content-general-form
             * Content-general-form = Value-length Media-type
             * Media-type = (Well-known-media | Extension-Media) *(Parameter)
             * Value-length = Short-length | (Length-quote Length)
             * Short-length = <Any octet 0-30>   (octet <= WAP_PDU_SHORT_LENGTH_MAX)
             * Length-quote = <Octet 31>         (WAP_PDU_LENGTH_QUOTE)
             * Length = Uintvar-integer
             */
            If (pduDecoder->DecodeContentType(index) == FALSE) {
                If (DBG) Rlog->W(TAG, "Received PDU. Header Content-Type error.");
                return Intents.RESULT_SMS_GENERIC_ERROR;
            }

            String mimeType = pduDecoder->GetValueString();
            Int64 binaryContentType = pduDecoder->GetValue32();
            index += pduDecoder->GetDecodedDataLength();

            Byte[] header = new Byte[headerLength];
            System->Arraycopy(pdu, headerStartIndex, header, 0, header.length);

            Byte[] intentData;

            If (mimeType != NULL && mimeType->Equals(WspTypeDecoder.CONTENT_TYPE_B_PUSH_CO)) {
                intentData = pdu;
            } else {
                Int32 dataIndex = headerStartIndex + headerLength;
                intentData = new Byte[pdu.length - dataIndex];
                System->Arraycopy(pdu, dataIndex, intentData, 0, intentData.length);
            }

            If (SmsManager->GetDefault()->GetAutoPersisting()) {
                // Store the wap push data in telephony
                Int64 [] subIds = SubscriptionManager->GetSubId(phoneId);
                // FIXME (tomtaylor) - when we've updated SubscriptionManager, change
                // SubscriptionManager.DEFAULT_SUB_ID to SubscriptionManager->GetDefaultSmsSubId()
                Int64 subId = (subIds != NULL) && (subIds.length > 0) ? subIds[0] :
                    SmsManager->GetDefaultSmsSubId();
                WriteInboxMessage(subId, intentData);
            }

            /**
             * Seek for application ID field in WSP header.
             * If application ID is found, WapPushManager substitute the message
             * processing. Since WapPushManager is optional module, if WapPushManager
             * is not found, legacy message processing will be continued.
             */
            If (pduDecoder->SeekXWapApplicationId(index, index + headerLength - 1)) {
                index = (Int32) pduDecoder->GetValue32();
                pduDecoder->DecodeXWapApplicationId(index);
                String wapAppId = pduDecoder->GetValueString();
                If (wapAppId == NULL) {
                    wapAppId = Integer->ToString((Int32) pduDecoder->GetValue32());
                }

                String contentType = ((mimeType == NULL) ?
                        Long->ToString(binaryContentType) : mimeType);
                If (DBG) Rlog->V(TAG, "appid found: " + wapAppId + ":" + contentType);

                try {
                    Boolean processFurther = TRUE;
                    IWapPushManager wapPushMan = mWapPushManager;

                    If (wapPushMan == NULL) {
                        If (DBG) Rlog->W(TAG, "wap push manager not found!");
                    } else {
                        Intent intent = new Intent();
                        intent->PutExtra("transactionId", transactionId);
                        intent->PutExtra("pduType", pduType);
                        intent->PutExtra("header", header);
                        intent->PutExtra("data", intentData);
                        intent->PutExtra("contentTypeParameters",
                                pduDecoder->GetContentParameters());
                        If (!TextUtils->IsEmpty(address)) {
                            intent->PutExtra("address", address);
                        }
                        SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, phoneId);

                        Int32 procRet = wapPushMan->ProcessMessage(wapAppId, contentType, intent);
                        If (DBG) Rlog->V(TAG, "procRet:" + procRet);
                        If ((procRet & WapPushManagerParams.MESSAGE_HANDLED) > 0
                                && (procRet & WapPushManagerParams.FURTHER_PROCESSING) == 0) {
                            processFurther = FALSE;
                        }
                    }
                    If (!processFurther) {
                        return Intents.RESULT_SMS_HANDLED;
                    }
                } Catch (RemoteException e) {
                    If (DBG) Rlog->W(TAG, "remote func failed...");
                }
            }
            If (DBG) Rlog->V(TAG, "fall back to existing handler");

            If (mimeType == NULL) {
                If (DBG) Rlog->W(TAG, "Header Content-Type error.");
                return Intents.RESULT_SMS_GENERIC_ERROR;
            }

            String permission;
            Int32 appOp;

            If (mimeType->Equals(WspTypeDecoder.CONTENT_TYPE_B_MMS)) {
                permission = Manifest::permission::RECEIVE_MMS;
                appOp = AppOpsManager.OP_RECEIVE_MMS;
            } else {
                permission = Manifest::permission::RECEIVE_WAP_PUSH;
                appOp = AppOpsManager.OP_RECEIVE_WAP_PUSH;
            }

            Intent intent = new Intent(Intents.WAP_PUSH_DELIVER_ACTION);
            intent->SetType(mimeType);
            intent->PutExtra("transactionId", transactionId);
            intent->PutExtra("pduType", pduType);
            intent->PutExtra("header", header);
            intent->PutExtra("data", intentData);
            intent->PutExtra("contentTypeParameters", pduDecoder->GetContentParameters());
            If (!TextUtils->IsEmpty(address)) {
                intent->PutExtra("address", address);
            }
            SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, phoneId);

            // Direct the intent to only the default MMS app. If we can't find a default MMS app
            // then sent it to all broadcast receivers.
            ComponentName componentName = SmsApplication->GetDefaultMmsApplication(mContext, TRUE);
            If (componentName != NULL) {
                // Deliver MMS message only to this receiver
                intent->SetComponent(componentName);
                If (DBG) Rlog->V(TAG, "Delivering MMS to: " + componentName->GetPackageName() +
                        " " + componentName->GetClassName());
            }

            handler->DispatchIntent(intent, permission, appOp, receiver, UserHandle.OWNER);
            return Activity.RESULT_OK;
        } Catch (ArrayIndexOutOfBoundsException aie) {
            // 0-Byte WAP PDU or other unexpected WAP PDU contents can easily throw this;
            // log exception string without stack trace and return FALSE.
            Rlog->E(TAG, "ignoring DispatchWapPdu() array index exception: " + aie);
            return Intents.RESULT_SMS_GENERIC_ERROR;
        }
    }

    private void WriteInboxMessage(Int64 subId, Byte[] pushData) {
        final GenericPdu pdu = new PduParser(pushData).Parse();
        If (pdu == NULL) {
            Rlog->E(TAG, "Invalid PUSH PDU");
        }
        final PduPersister persister = PduPersister->GetPduPersister(mContext);
        final Int32 type = pdu->GetMessageType();
        try {
            Switch (type) {
                case MESSAGE_TYPE_DELIVERY_IND:
                case MESSAGE_TYPE_READ_ORIG_IND: {
                    final Int64 threadId = GetDeliveryOrReadReportThreadId(mContext, pdu);
                    If (threadId == -1) {
                        // The associated SendReq isn't found, therefore skip
                        // processing this PDU.
                        Rlog->E(TAG, "Failed to find delivery or read report's thread id");
                        break;
                    }
                    final Uri uri = persister->Persist(
                            pdu,
                            Telephony.Mms.Inbox.CONTENT_URI,
                            TRUE/*createThreadId*/,
                            TRUE/*groupMmsEnabled*/,
                            NULL/*preOpenedFiles*/);
                    If (uri == NULL) {
                        Rlog->E(TAG, "Failed to persist delivery or read report");
                        break;
                    }
                    // Update thread ID for ReadOrigInd & DeliveryInd.
                    final ContentValues values = new ContentValues(1);
                    values->Put(Telephony.Mms.THREAD_ID, threadId);
                    If (SqliteWrapper->Update(
                            mContext,
                            mContext->GetContentResolver(),
                            uri,
                            values,
                            NULL/*where*/,
                            NULL/*selectionArgs*/) != 1) {
                        Rlog->E(TAG, "Failed to update delivery or read report thread id");
                    }
                    break;
                }
                case MESSAGE_TYPE_NOTIFICATION_IND: {
                    final NotificationInd nInd = (NotificationInd) pdu;

                    Bundle configs = SmsManager->GetSmsManagerForSubscriber(subId)
                            .GetCarrierConfigValues();
                    If (configs != NULL && configs->GetBoolean(
                        SmsManager.MMS_CONFIG_APPEND_TRANSACTION_ID, FALSE)) {
                        final Byte [] contentLocation = nInd->GetContentLocation();
                        If ('=' == contentLocation[contentLocation.length - 1]) {
                            Byte [] transactionId = nInd->GetTransactionId();
                            Byte [] contentLocationWithId = new Byte [contentLocation.length
                                    + transactionId.length];
                            System->Arraycopy(contentLocation, 0, contentLocationWithId,
                                    0, contentLocation.length);
                            System->Arraycopy(transactionId, 0, contentLocationWithId,
                                    contentLocation.length, transactionId.length);
                            nInd->SetContentLocation(contentLocationWithId);
                        }
                    }
                    If (!IsDuplicateNotification(mContext, nInd)) {
                        final Uri uri = persister->Persist(
                                pdu,
                                Telephony.Mms.Inbox.CONTENT_URI,
                                TRUE/*createThreadId*/,
                                TRUE/*groupMmsEnabled*/,
                                NULL/*preOpenedFiles*/);
                        If (uri == NULL) {
                            Rlog->E(TAG, "Failed to save MMS WAP push notification ind");
                        }
                    } else {
                        Rlog->D(TAG, "Skip storing duplicate MMS WAP push notification ind: "
                                + new String(nInd->GetContentLocation()));
                    }
                    break;
                }
                default:
                    Logger::E(TAG, "Received unrecognized WAP Push PDU.");
            }
        } Catch (MmsException e) {
            Logger::E(TAG, "Failed to save MMS WAP push data: type=" + type, e);
        } Catch (RuntimeException e) {
            Logger::E(TAG, "Unexpected RuntimeException in persisting MMS WAP push data", e);
        }

    }

    private static const String THREAD_ID_SELECTION =
            Telephony.Mms.MESSAGE_ID + "=? AND " + Telephony.Mms.MESSAGE_TYPE + "=?";

    private static Int64 GetDeliveryOrReadReportThreadId(Context context, GenericPdu pdu) {
        String messageId;
        If (pdu instanceof DeliveryInd) {
            messageId = new String(((DeliveryInd) pdu).GetMessageId());
        } else If (pdu instanceof ReadOrigInd) {
            messageId = new String(((ReadOrigInd) pdu).GetMessageId());
        } else {
            Rlog->E(TAG, "WAP Push data is neither delivery or read report type: "
                    + pdu->GetClass()->GetCanonicalName());
            return -1L;
        }
        Cursor cursor = NULL;
        try {
            cursor = SqliteWrapper->Query(
                    context,
                    context->GetContentResolver(),
                    Telephony.Mms.CONTENT_URI,
                    new String[]{ Telephony.Mms.THREAD_ID },
                    THREAD_ID_SELECTION,
                    new String[]{
                            DatabaseUtils->SqlEscapeString(messageId),
                            Integer->ToString(PduHeaders.MESSAGE_TYPE_SEND_REQ)
                    },
                    NULL/*sortOrder*/);
            If (cursor != NULL && cursor->MoveToFirst()) {
                return cursor->GetLong(0);
            }
        } Catch (SQLiteException e) {
            Rlog->E(TAG, "Failed to query delivery or read report thread id", e);
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }
        return -1L;
    }

    private static const String LOCATION_SELECTION =
            Telephony.Mms.MESSAGE_TYPE + "=? AND " + Telephony.Mms.CONTENT_LOCATION + " =?";

    private static Boolean IsDuplicateNotification(Context context, NotificationInd nInd) {
        final Byte[] rawLocation = nInd->GetContentLocation();
        If (rawLocation != NULL) {
            String location = new String(rawLocation);
            String[] selectionArgs = new String[] { location };
            Cursor cursor = NULL;
            try {
                cursor = SqliteWrapper->Query(
                        context,
                        context->GetContentResolver(),
                        Telephony.Mms.CONTENT_URI,
                        new String[]{Telephony.Mms._ID},
                        LOCATION_SELECTION,
                        new String[]{
                                Integer->ToString(PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND),
                                new String(rawLocation)
                        },
                        NULL/*sortOrder*/);
                If (cursor != NULL && cursor->GetCount() > 0) {
                    // We already received the same notification before.
                    return TRUE;
                }
            } Catch (SQLiteException e) {
                Rlog->E(TAG, "failed to query existing notification ind", e);
            } finally {
                If (cursor != NULL) {
                    cursor->Close();
                }
            }
        }
        return FALSE;
    }
}
