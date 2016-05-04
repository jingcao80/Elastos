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

using Elastos::Droid::IManifest;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::ITelephony;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Utility::ILog;

using Elastos::Droid::Internal::Telephony::Gsm::ISmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Cdma::ICdmaSmsBroadcastConfigInfo;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Utility::IHexDump;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IList;

using static android::Telephony::SmsManager::ISTATUS_ON_ICC_FREE;
using static android::Telephony::SmsManager::ISTATUS_ON_ICC_READ;
using static android::Telephony::SmsManager::ISTATUS_ON_ICC_UNREAD;

using Elastos::Droid::Telephony::ITelephonyManager;

/**
 * IccSmsInterfaceManager to provide an inter-process communication to
 * access Sms in Icc.
 */
public class IccSmsInterfaceManager {
    static const String LOG_TAG = "IccSmsInterfaceManager";
    static const Boolean DBG = FALSE;

    protected final Object mLock = new Object();
    protected Boolean mSuccess;
    private List<SmsRawData> mSms;

    private CellBroadcastRangeManager mCellBroadcastRangeManager =
            new CellBroadcastRangeManager();
    private CdmaBroadcastRangeManager mCdmaBroadcastRangeManager =
            new CdmaBroadcastRangeManager();

    private static const Int32 EVENT_LOAD_DONE = 1;
    private static const Int32 EVENT_UPDATE_DONE = 2;
    protected static const Int32 EVENT_SET_BROADCAST_ACTIVATION_DONE = 3;
    protected static const Int32 EVENT_SET_BROADCAST_CONFIG_DONE = 4;
    private static const Int32 SMS_CB_CODE_SCHEME_MIN = 0;
    private static const Int32 SMS_CB_CODE_SCHEME_MAX = 255;

    protected PhoneBase mPhone;
    final protected Context mContext;
    final protected AppOpsManager mAppOps;
    final private UserManager mUserManager;
    protected SMSDispatcher mDispatcher;

    protected Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            AsyncResult ar;

            Switch (msg.what) {
                case EVENT_UPDATE_DONE:
                    ar = (AsyncResult) msg.obj;
                    Synchronized (mLock) {
                        mSuccess = (ar.exception == NULL);
                        mLock->NotifyAll();
                    }
                    break;
                case EVENT_LOAD_DONE:
                    ar = (AsyncResult)msg.obj;
                    Synchronized (mLock) {
                        If (ar.exception == NULL) {
                            mSms = BuildValidRawData((ArrayList<Byte[]>) ar.result);
                            //Mark SMS as read after importing it from card.
                            MarkMessagesAsRead((ArrayList<Byte[]>) ar.result);
                        } else {
                            If (Rlog->IsLoggable("SMS", Log.DEBUG)) {
                                Log("Cannot load Sms records");
                            }
                            If (mSms != NULL)
                                mSms->Clear();
                        }
                        mLock->NotifyAll();
                    }
                    break;
                case EVENT_SET_BROADCAST_ACTIVATION_DONE:
                case EVENT_SET_BROADCAST_CONFIG_DONE:
                    ar = (AsyncResult) msg.obj;
                    Synchronized (mLock) {
                        mSuccess = (ar.exception == NULL);
                        mLock->NotifyAll();
                    }
                    break;
            }
        }
    };

    protected IccSmsInterfaceManager(PhoneBase phone) {
        mPhone = phone;
        mContext = phone->GetContext();
        mAppOps = (AppOpsManager) mContext->GetSystemService(Context.APP_OPS_SERVICE);
        mUserManager = (UserManager) mContext->GetSystemService(Context.USER_SERVICE);
        mDispatcher = new ImsSMSDispatcher(phone,
                phone.mSmsStorageMonitor, phone.mSmsUsageMonitor);
    }

    protected void MarkMessagesAsRead(ArrayList<Byte[]> messages) {
        If (messages == NULL) {
            return;
        }

        //IccFileHandler can be NULL, if icc card is absent.
        IccFileHandler fh = mPhone->GetIccFileHandler();
        If (fh == NULL) {
            //shouldn't really happen, as messages are marked as read, only
            //after importing it from icc.
            If (Rlog->IsLoggable("SMS", Log.DEBUG)) {
                Log("markMessagesAsRead - aborting, no icc card present.");
            }
            return;
        }

        Int32 count = messages->Size();

        For (Int32 i = 0; i < count; i++) {
             Byte[] ba = messages->Get(i);
             If (ba[0] == STATUS_ON_ICC_UNREAD) {
                 Int32 n = ba.length;
                 Byte[] nba = new Byte[n - 1];
                 System->Arraycopy(ba, 1, nba, 0, n - 1);
                 Byte[] record = MakeSmsRecordData(STATUS_ON_ICC_READ, nba);
                 fh->UpdateEFLinearFixed(IccConstants.EF_SMS, i + 1, record, NULL, NULL);
                 If (Rlog->IsLoggable("SMS", Log.DEBUG)) {
                     Log("SMS " + (i + 1) + " marked as read");
                 }
             }
        }
    }

    protected void UpdatePhoneObject(PhoneBase phone) {
        mPhone = phone;
        mDispatcher->UpdatePhoneObject(phone);
    }

    protected void EnforceReceiveAndSend(String message) {
        mContext->EnforceCallingOrSelfPermission(
                Manifest::permission::RECEIVE_SMS, message);
        mContext->EnforceCallingOrSelfPermission(
                Manifest::permission::SEND_SMS, message);
    }

    /**
     * Update the specified message on the Icc.
     *
     * @param index record index of message to update
     * @param status new message Status (STATUS_ON_ICC_READ,
     *                  STATUS_ON_ICC_UNREAD, STATUS_ON_ICC_SENT,
     *                  STATUS_ON_ICC_UNSENT, STATUS_ON_ICC_FREE)
     * @param pdu the raw PDU to store
     * @return success or not
     *
     */

    public Boolean
    UpdateMessageOnIccEf(String callingPackage, Int32 index, Int32 status, Byte[] pdu) {
        If (DBG) Log("updateMessageOnIccEf: index=" + index +
                " status=" + status + " ==> " +
                "("+ Arrays->ToString(pdu) + ")");
        EnforceReceiveAndSend("Updating message on Icc");
        If (mAppOps->NoteOp(AppOpsManager.OP_WRITE_ICC_SMS, Binder->GetCallingUid(),
                callingPackage) != AppOpsManager.MODE_ALLOWED) {
            return FALSE;
        }
        Synchronized(mLock) {
            mSuccess = FALSE;
            Message response = mHandler->ObtainMessage(EVENT_UPDATE_DONE);

            If (status == STATUS_ON_ICC_FREE) {
                // RIL_REQUEST_DELETE_SMS_ON_SIM vs RIL_REQUEST_CDMA_DELETE_SMS_ON_RUIM
                // Special case FREE: call deleteSmsOnSim/Ruim instead of
                // manipulating the record
                // Will eventually fail if icc card is not present.
                If (PhoneConstants.PHONE_TYPE_GSM == mPhone->GetPhoneType()) {
                    mPhone.mCi->DeleteSmsOnSim(index, response);
                } else {
                    mPhone.mCi->DeleteSmsOnRuim(index, response);
                }
            } else {
                //IccFilehandler can be NULL if ICC card is not present.
                IccFileHandler fh = mPhone->GetIccFileHandler();
                If (fh == NULL) {
                    response->Recycle();
                    return mSuccess; /* is FALSE */
                }
                Byte[] record = MakeSmsRecordData(status, pdu);
                fh->UpdateEFLinearFixed(
                        IccConstants.EF_SMS,
                        index, record, NULL, response);
            }
            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Log("interrupted while trying to update by index");
            }
        }
        return mSuccess;
    }

    /**
     * Copy a raw SMS PDU to the Icc.
     *
     * @param pdu the raw PDU to store
     * @param status message Status (STATUS_ON_ICC_READ, STATUS_ON_ICC_UNREAD,
     *               STATUS_ON_ICC_SENT, STATUS_ON_ICC_UNSENT)
     * @return success or not
     *
     */
    public Boolean CopyMessageToIccEf(String callingPackage, Int32 status, Byte[] pdu, Byte[] smsc) {
        //NOTE smsc not used in RUIM
        If (DBG) Log("copyMessageToIccEf: status=" + status + " ==> " +
                "pdu=("+ Arrays->ToString(pdu) +
                "), smsc=(" + Arrays->ToString(smsc) +")");
        EnforceReceiveAndSend("Copying message to Icc");
        If (mAppOps->NoteOp(AppOpsManager.OP_WRITE_ICC_SMS, Binder->GetCallingUid(),
                callingPackage) != AppOpsManager.MODE_ALLOWED) {
            return FALSE;
        }
        Synchronized(mLock) {
            mSuccess = FALSE;
            Message response = mHandler->ObtainMessage(EVENT_UPDATE_DONE);

            //RIL_REQUEST_WRITE_SMS_TO_SIM vs RIL_REQUEST_CDMA_WRITE_SMS_TO_RUIM
            If (PhoneConstants.PHONE_TYPE_GSM == mPhone->GetPhoneType()) {
                mPhone.mCi->WriteSmsToSim(status, IccUtils->BytesToHexString(smsc),
                        IccUtils->BytesToHexString(pdu), response);
            } else {
                mPhone.mCi->WriteSmsToRuim(status, IccUtils->BytesToHexString(pdu),
                        response);
            }

            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Log("interrupted while trying to update by index");
            }
        }
        return mSuccess;
    }

    CARAPI SynthesizeMessages(String originatingAddress, String scAddress, List<String> messages, Int64 timestampMillis) throws RemoteException {
    }

    /**
     * Retrieves all messages currently stored on Icc.
     *
     * @return list of SmsRawData of all sms on Icc
     */

    public List<SmsRawData> GetAllMessagesFromIccEf(String callingPackage) {
        If (DBG) Log("getAllMessagesFromEF");

        mContext->EnforceCallingOrSelfPermission(
                Manifest::permission::RECEIVE_SMS,
                "Reading messages from Icc");
        Synchronized(mLock) {

            IccFileHandler fh = mPhone->GetIccFileHandler();
            If (fh == NULL) {
                Rlog->E(LOG_TAG, "Cannot load Sms records. No icc card?");
                If (mSms != NULL) {
                    mSms->Clear();
                    return mSms;
                }
            }

            Message response = mHandler->ObtainMessage(EVENT_LOAD_DONE);
            fh->LoadEFLinearFixedAll(IccConstants.EF_SMS, response);

            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Log("interrupted while trying to load from the Icc");
            }
        }
        return mSms;
    }

    /**
     * Send a data based SMS to a specific application port.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param destPort the port to deliver the message to
     * @param data the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended Data ("pdu").
     */

    CARAPI SendData(String callingPackage, String destAddr, String scAddr, Int32 destPort,
            Byte[] data, PendingIntent sentIntent, PendingIntent deliveryIntent) {
        mPhone->GetContext()->EnforceCallingPermission(
                Manifest::permission::SEND_SMS,
                "Sending SMS message");
        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Log("sendData: destAddr=" + destAddr + " scAddr=" + scAddr + " destPort=" +
                destPort + " data='"+ HexDump->ToHexString(data)  + "' sentIntent=" +
                sentIntent + " deliveryIntent=" + deliveryIntent);
        }
        If (mAppOps->NoteOp(AppOpsManager.OP_SEND_SMS, Binder->GetCallingUid(),
                callingPackage) != AppOpsManager.MODE_ALLOWED) {
            return;
        }
        destAddr = FilterDestAddress(destAddr);
        mDispatcher->SendData(destAddr, scAddr, destPort, 0, data, sentIntent, deliveryIntent);
    }

    /**
     * Send a data based SMS to a specific application port.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param destPort the port to deliver the message to
     * @param origPort the originator port set by sender
     * @param data the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended Data ("pdu").
     */
    CARAPI SendDataWithOrigPort(String callingPackage, String destAddr, String scAddr,
            Int32 destPort, Int32 origPort, Byte[] data, PendingIntent sentIntent,
            PendingIntent deliveryIntent) {
        mPhone->GetContext()->EnforceCallingPermission(
                Manifest::permission::SEND_SMS,
                "Sending SMS message");
        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Log("sendDataWithOrigPort: destAddr=" + destAddr + " scAddr=" + scAddr
                + " destPort=" +destPort + "origPort=" + origPort
                + " data='"+ HexDump->ToHexString(data) +
                "' sentIntent=" + sentIntent + " deliveryIntent=" + deliveryIntent);
        }
        If (mAppOps->NoteOp(AppOpsManager.OP_SEND_SMS, Binder->GetCallingUid(),
                callingPackage) != AppOpsManager.MODE_ALLOWED) {
            return;
        }
        destAddr = FilterDestAddress(destAddr);
        mDispatcher->SendData(destAddr, scAddr, destPort, origPort,
                data, sentIntent, deliveryIntent);
    }

    /**
     * Send a text based SMS.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param text the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended Data ("pdu").
     */

    CARAPI SendText(String callingPackage, String destAddr, String scAddr,
            String text, PendingIntent sentIntent, PendingIntent deliveryIntent) {
        Int32 callingUid = Binder->GetCallingUid();

        String[] callingParts = callingPackage->Split("\\\\");
        If (callingUid == android.os.Process.PHONE_UID &&
                                         callingParts.length > 1) {
            callingUid = Integer->ParseInt(callingParts[1]);
        }

        // Reset the calling package, remove the trailing uid so
        // shouldWriteMessageForPackage can match correctly
        // if our message has been synthesized by an
        // external package
        callingPackage = callingParts[0];

        If (Binder->GetCallingPid() != android.os.Process->MyPid()) {
            mPhone->GetContext()->EnforceCallingPermission(
                    Manifest::permission::SEND_SMS,
                    "Sending SMS message");
        }
        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Log("sendText: destAddr=" + destAddr + " scAddr=" + scAddr +
                " text='"+ text + "' sentIntent=" +
                sentIntent + " deliveryIntent=" + deliveryIntent);
        }
        If (mAppOps->NoteOp(AppOpsManager.OP_SEND_SMS, callingUid,
                callingParts[0]) != AppOpsManager.MODE_ALLOWED) {
            return;
        }
        destAddr = FilterDestAddress(destAddr);
        mDispatcher->SendText(destAddr, scAddr, text, sentIntent, deliveryIntent,
                NULL/*messageUri*/, callingPackage, -1, FALSE, -1);
    }

    /**
     * Send a text based SMS with Messaging Options.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *  the current default SMSC
     * @param text the body of the message to send
     * @param sentIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully sent, or failed.
     *  The result code will be <code>Activity.RESULT_OK<code> for success,
     *  or one of these errors:<br>
     *  <code>RESULT_ERROR_GENERIC_FAILURE</code><br>
     *  <code>RESULT_ERROR_RADIO_OFF</code><br>
     *  <code>RESULT_ERROR_NULL_PDU</code><br>
     *  For <code>RESULT_ERROR_GENERIC_FAILURE</code> the sentIntent may include
     *  the extra "errorCode" containing a radio technology specific value,
     *  generally only useful for troubleshooting.<br>
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is delivered to the recipient.  The
     *  raw pdu of the status report is in the extended Data ("pdu").
     * @param priority Priority level of the message
     *  Refer specification See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1
     *  ---------------------------------
     *  PRIORITY      | Level of Priority
     *  ---------------------------------
     *      '00'      |     Normal
     *      '01'      |     Interactive
     *      '10'      |     Urgent
     *      '11'      |     Emergency
     *  ----------------------------------
     *  Any Other values included Negative considered as Invalid Priority Indicator of the message.
     * @param isExpectMore is a Boolean to indicate the sending message is multi segmented or not.
     * @param validityPeriod Validity Period of the message in mins.
     *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
     *  Validity Period(Minimum) -> 5 mins
     *  Validity Period(Maximum) -> 635040 Mins(i.e.63 weeks).
     *  Any Other values included Negative considered as Invalid Validity Period of the message.
     */

    CARAPI SendTextWithOptions(String callingPackage, String destAddr, String scAddr,
            String text, PendingIntent sentIntent, PendingIntent deliveryIntent,
            Int32 priority, Boolean isExpectMore, Int32 validityPeriod) {
        mPhone->GetContext()->EnforceCallingPermission(
                Manifest::permission::SEND_SMS,
                "Sending SMS message");
        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Log("sendText: destAddr=" + destAddr + " scAddr=" + scAddr +
                " text='"+ text + "' sentIntent=" +
                sentIntent + " deliveryIntent=" + deliveryIntent +
                "validityPeriod" + validityPeriod);
        }
        If (mAppOps->NoteOp(AppOpsManager.OP_SEND_SMS, Binder->GetCallingUid(),
                callingPackage) != AppOpsManager.MODE_ALLOWED) {
            return;
        }
        mDispatcher->SendText(destAddr, scAddr, text, sentIntent, deliveryIntent,
                NULL/*messageUri*/, callingPackage, priority,
                isExpectMore, validityPeriod);
    }

    /**
     * Inject an SMS PDU into the android application framework.
     *
     * @param pdu is the Byte array of pdu to be injected into android application framework
     * @param format is the format of SMS Pdu (3gpp or 3gpp2)
     * @param receivedIntent if not NULL this <code>PendingIntent</code> is
     *  broadcast when the message is successfully received by the
     *  android application framework. This intent is broadcasted at
     *  the same time an SMS received from radio is acknowledged back.
     */
    CARAPI InjectSmsPdu(Byte[] pdu, String format, PendingIntent receivedIntent) {
        EnforceCarrierPrivilege();
        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Log("pdu: " + pdu +
                "\n format=" + format +
                "\n receivedIntent=" + receivedIntent);
        }
        mDispatcher->InjectSmsPdu(pdu, format, receivedIntent);
    }

    /**
     * Update the status of a Pending (send-by-IP) SMS message and resend by PSTN if necessary.
     * This outbound message was handled by the carrier app. If the carrier app fails to send
     * this message, it would be resent by PSTN.
     *
     * @param messageRef the reference number of the SMS message.
     * @param success True if and only if the message was sent successfully. If its value is
     *  FALSE, this message should be resent via PSTN.
     * {@hide}
     */
    CARAPI UpdateSmsSendStatus(Int32 messageRef, Boolean success) {
        EnforceCarrierPrivilege();
        mDispatcher->UpdateSmsSendStatus(messageRef, success);
    }

    /**
     * Send a multi-part text based SMS.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *   the current default SMSC
     * @param parts an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     * @param sentIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
     *   broadcast when the corresponding message part has been sent.
     *   The result code will be <code>Activity.RESULT_OK<code> for success,
     *   or one of these errors:
     *   <code>RESULT_ERROR_GENERIC_FAILURE</code>
     *   <code>RESULT_ERROR_RADIO_OFF</code>
     *   <code>RESULT_ERROR_NULL_PDU</code>.
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
     *   broadcast when the corresponding message part has been delivered
     *   to the recipient.  The raw pdu of the status report is in the
     *   extended Data ("pdu").
     */

    CARAPI SendMultipartText(String callingPackage, String destAddr, String scAddr,
            List<String> parts, List<PendingIntent> sentIntents,
            List<PendingIntent> deliveryIntents) {
        Int32 callingUid = Binder->GetCallingUid();

        String[] callingParts = callingPackage->Split("\\\\");
        If (callingUid == android.os.Process.PHONE_UID &&
                                         callingParts.length > 1) {
            callingUid = Integer->ParseInt(callingParts[1]);
        }

        // Reset the calling package, remove the trailing uid so
        // shouldWriteMessageForPackage can match correctly
        // if our message has been synthesized by an
        // external package
        callingPackage = callingParts[0];

        If (Binder->GetCallingPid() != android.os.Process->MyPid()) {
            mPhone->GetContext()->EnforceCallingPermission(
                    Manifest::permission::SEND_SMS,
                    "Sending SMS message");
        }
        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Int32 i = 0;
            For (String part : parts) {
                Log("sendMultipartText: destAddr=" + destAddr + ", srAddr=" + scAddr +
                        ", part[" + (i++) + "]=" + part);
            }
        }
        If (mAppOps->NoteOp(AppOpsManager.OP_SEND_SMS, callingUid,
                callingParts[0]) != AppOpsManager.MODE_ALLOWED) {
            return;
        }

        If (parts->Size() > 1 && parts->Size() < 10 && !SmsMessage->HasEmsSupport()) {
            For (Int32 i = 0; i < parts->Size(); i++) {
                // If EMS is not supported, we have to break down EMS into single segment SMS
                // and add page info " x/y".
                String singlePart = parts->Get(i);
                If (SmsMessage->ShouldAppendPageNumberAsPrefix()) {
                    singlePart = String->ValueOf(i + 1) + '/' + parts->Size() + ' ' + singlePart;
                } else {
                    singlePart = singlePart->Concat(' ' + String->ValueOf(i + 1) + '/' + parts->Size());
                }

                PendingIntent singleSentIntent = NULL;
                If (sentIntents != NULL && sentIntents->Size() > i) {
                    singleSentIntent = sentIntents->Get(i);
                }

                PendingIntent singleDeliveryIntent = NULL;
                If (deliveryIntents != NULL && deliveryIntents->Size() > i) {
                    singleDeliveryIntent = deliveryIntents->Get(i);
                }

                mDispatcher->SendText(destAddr, scAddr, singlePart,
                        singleSentIntent, singleDeliveryIntent,
                        NULL/*messageUri*/, callingPackage, -1, FALSE, -1);
            }
            return;
        }

        mDispatcher->SendMultipartText(destAddr, scAddr, (ArrayList<String>) parts,
                (ArrayList<PendingIntent>) sentIntents, (ArrayList<PendingIntent>) deliveryIntents,
                NULL/*messageUri*/, callingPackage, -1, FALSE, -1);
    }

    /**
     * Send a multi-part text based SMS with Messaging Options.
     *
     * @param destAddr the address to send the message to
     * @param scAddr is the service center address or NULL to use
     *   the current default SMSC
     * @param parts an <code>ArrayList</code> of strings that, in order,
     *   comprise the original message
     * @param sentIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
     *   broadcast when the corresponding message part has been sent.
     *   The result code will be <code>Activity.RESULT_OK<code> for success,
     *   or one of these errors:
     *   <code>RESULT_ERROR_GENERIC_FAILURE</code>
     *   <code>RESULT_ERROR_RADIO_OFF</code>
     *   <code>RESULT_ERROR_NULL_PDU</code>.
     *  The per-application based SMS control checks sentIntent. If sentIntent
     *  is NULL the caller will be checked against all unknown applications,
     *  which cause smaller number of SMS to be sent in checking period.
     * @param deliveryIntents if not NULL, an <code>ArrayList</code> of
     *   <code>PendingIntent</code>S (one for each message part) that is
     *   broadcast when the corresponding message part has been delivered
     *   to the recipient.  The raw pdu of the status report is in the
     *   extended Data ("pdu").
     * @param priority Priority level of the message
     *  Refer specification See 3GPP2 C.S0015-B, v2.0, table 4.5.9-1
     *  ---------------------------------
     *  PRIORITY      | Level of Priority
     *  ---------------------------------
     *      '00'      |     Normal
     *      '01'      |     Interactive
     *      '10'      |     Urgent
     *      '11'      |     Emergency
     *  ----------------------------------
     *  Any Other values included Negative considered as Invalid Priority Indicator of the message.
     * @param isExpectMore is a Boolean to indicate the sending message is multi segmented or not.
     * @param validityPeriod Validity Period of the message in mins.
     *  Refer specification 3GPP TS 23.040 V6.8.1 section 9.2.3.12.1.
     *  Validity Period(Minimum) -> 5 mins
     *  Validity Period(Maximum) -> 635040 Mins(i.e.63 weeks).
     *  Any Other values included Negative considered as Invalid Validity Period of the message.
     */

    CARAPI SendMultipartTextWithOptions(String callingPackage, String destAddr,
            String scAddr, List<String> parts, List<PendingIntent> sentIntents,
            List<PendingIntent> deliveryIntents, Int32 priority, Boolean isExpectMore,
            Int32 validityPeriod) {
        mPhone->GetContext()->EnforceCallingPermission(
                Manifest::permission::SEND_SMS,
                "Sending SMS message");
        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Int32 i = 0;
            For (String part : parts) {
                Log("sendMultipartTextWithOptions: destAddr=" + destAddr + ", srAddr=" + scAddr +
                        ", part[" + (i++) + "]=" + part);
            }
        }
        If (mAppOps->NoteOp(AppOpsManager.OP_SEND_SMS, Binder->GetCallingUid(),
                callingPackage) != AppOpsManager.MODE_ALLOWED) {
            return;
        }
        mDispatcher->SendMultipartText(destAddr,
                                      scAddr,
                                      (ArrayList<String>) parts,
                                      (ArrayList<PendingIntent>) sentIntents,
                                      (ArrayList<PendingIntent>) deliveryIntents,
                                      NULL, callingPackage, priority, isExpectMore, validityPeriod);
    }

    public Int32 GetPremiumSmsPermission(String packageName) {
        return mDispatcher->GetPremiumSmsPermission(packageName);
    }


    CARAPI SetPremiumSmsPermission(String packageName, Int32 permission) {
        mDispatcher->SetPremiumSmsPermission(packageName, permission);
    }

    /**
     * create SmsRawData lists from all sms record Byte[]
     * Use NULL to indicate "free" record
     *
     * @param messages List of message records from EF_SMS.
     * @return SmsRawData list of all in-used records
     */
    protected ArrayList<SmsRawData> BuildValidRawData(ArrayList<Byte[]> messages) {
        Int32 count = messages->Size();
        ArrayList<SmsRawData> ret;

        ret = new ArrayList<SmsRawData>(count);

        For (Int32 i = 0; i < count; i++) {
            Byte[] ba = messages->Get(i);
            If (ba[0] == STATUS_ON_ICC_FREE) {
                ret->Add(NULL);
            } else {
                ret->Add(new SmsRawData(messages->Get(i)));
            }
        }

        return ret;
    }

    /**
     * Generates an EF_SMS record from status and raw PDU.
     *
     * @param status Message status.  See TS 51.011 10.5.3.
     * @param pdu Raw message PDU.
     * @return Byte array for the record.
     */
    protected Byte[] MakeSmsRecordData(Int32 status, Byte[] pdu) {
        Byte[] data;
        If (PhoneConstants.PHONE_TYPE_GSM == mPhone->GetPhoneType()) {
            data = new Byte[IccConstants.SMS_RECORD_LENGTH];
        } else {
            data = new Byte[IccConstants.CDMA_SMS_RECORD_LENGTH];
        }

        // Status bits for this record.  See TS 51.011 10.5.3
        data[0] = (Byte)(status & 7);

        System->Arraycopy(pdu, 0, data, 1, pdu.length);

        // Pad out with 0xFF's.
        For (Int32 j = pdu.length+1; j < data.length; j++) {
            data[j] = -1;
        }

        return data;
    }

    public Boolean EnableCellBroadcast(Int32 messageIdentifier) {
        return EnableCellBroadcastRange(messageIdentifier, messageIdentifier);
    }

    public Boolean DisableCellBroadcast(Int32 messageIdentifier) {
        return DisableCellBroadcastRange(messageIdentifier, messageIdentifier);
    }

    public Boolean EnableCellBroadcastRange(Int32 startMessageId, Int32 endMessageId) {
        If (PhoneConstants.PHONE_TYPE_GSM == mPhone->GetPhoneType()) {
            return EnableGsmBroadcastRange(startMessageId, endMessageId);
        } else {
            return EnableCdmaBroadcastRange(startMessageId, endMessageId);
        }
    }

    public Boolean DisableCellBroadcastRange(Int32 startMessageId, Int32 endMessageId) {
        If (PhoneConstants.PHONE_TYPE_GSM == mPhone->GetPhoneType()) {
            return DisableGsmBroadcastRange(startMessageId, endMessageId);
        } else {
            return DisableCdmaBroadcastRange(startMessageId, endMessageId);
        }
    }
    synchronized public Boolean EnableGsmBroadcastRange(Int32 startMessageId, Int32 endMessageId) {
        If (DBG) Log("enableGsmBroadcastRange");

        Context context = mPhone->GetContext();

        context->EnforceCallingPermission(
                "android.permission.RECEIVE_SMS",
                "Enabling cell broadcast SMS");

        String client = context->GetPackageManager()->GetNameForUid(
                Binder->GetCallingUid());

        If (!mCellBroadcastRangeManager->EnableRange(startMessageId, endMessageId, client)) {
            Log("Failed to add cell broadcast subscription for MID range " + startMessageId
                    + " to " + endMessageId + " from client " + client);
            return FALSE;
        }

        If (DBG)
            Log("Added cell broadcast subscription for MID range " + startMessageId
                    + " to " + endMessageId + " from client " + client);

        SetCellBroadcastActivation(!mCellBroadcastRangeManager->IsEmpty());

        return TRUE;
    }

    synchronized public Boolean DisableGsmBroadcastRange(Int32 startMessageId, Int32 endMessageId) {
        If (DBG) Log("disableGsmBroadcastRange");

        Context context = mPhone->GetContext();

        context->EnforceCallingPermission(
                "android.permission.RECEIVE_SMS",
                "Disabling cell broadcast SMS");

        String client = context->GetPackageManager()->GetNameForUid(
                Binder->GetCallingUid());

        If (!mCellBroadcastRangeManager->DisableRange(startMessageId, endMessageId, client)) {
            Log("Failed to remove cell broadcast subscription for MID range " + startMessageId
                    + " to " + endMessageId + " from client " + client);
            return FALSE;
        }

        If (DBG)
            Log("Removed cell broadcast subscription for MID range " + startMessageId
                    + " to " + endMessageId + " from client " + client);

        SetCellBroadcastActivation(!mCellBroadcastRangeManager->IsEmpty());

        return TRUE;
    }

    synchronized public Boolean EnableCdmaBroadcastRange(Int32 startMessageId, Int32 endMessageId) {
        If (DBG) Log("enableCdmaBroadcastRange");

        Context context = mPhone->GetContext();

        context->EnforceCallingPermission(
                "android.permission.RECEIVE_SMS",
                "Enabling cdma broadcast SMS");

        String client = context->GetPackageManager()->GetNameForUid(
                Binder->GetCallingUid());

        If (!mCdmaBroadcastRangeManager->EnableRange(startMessageId, endMessageId, client)) {
            Log("Failed to add cdma broadcast subscription for MID range " + startMessageId
                    + " to " + endMessageId + " from client " + client);
            return FALSE;
        }

        If (DBG)
            Log("Added cdma broadcast subscription for MID range " + startMessageId
                    + " to " + endMessageId + " from client " + client);

        SetCdmaBroadcastActivation(!mCdmaBroadcastRangeManager->IsEmpty());

        return TRUE;
    }

    synchronized public Boolean DisableCdmaBroadcastRange(Int32 startMessageId, Int32 endMessageId) {
        If (DBG) Log("disableCdmaBroadcastRange");

        Context context = mPhone->GetContext();

        context->EnforceCallingPermission(
                "android.permission.RECEIVE_SMS",
                "Disabling cell broadcast SMS");

        String client = context->GetPackageManager()->GetNameForUid(
                Binder->GetCallingUid());

        If (!mCdmaBroadcastRangeManager->DisableRange(startMessageId, endMessageId, client)) {
            Log("Failed to remove cdma broadcast subscription for MID range " + startMessageId
                    + " to " + endMessageId + " from client " + client);
            return FALSE;
        }

        If (DBG)
            Log("Removed cdma broadcast subscription for MID range " + startMessageId
                    + " to " + endMessageId + " from client " + client);

        SetCdmaBroadcastActivation(!mCdmaBroadcastRangeManager->IsEmpty());

        return TRUE;
    }

    class CellBroadcastRangeManager extends IntRangeManager {
        private ArrayList<SmsBroadcastConfigInfo> mConfigList =
                new ArrayList<SmsBroadcastConfigInfo>();

        /**
         * Called when the list of enabled ranges has changed. This will be
         * followed by zero or more calls to {@link #addRange} followed by
         * a call to {@link #finishUpdate}.
         */
        protected void StartUpdate() {
            mConfigList->Clear();
        }

        /**
         * Called after {@link #startUpdate} to indicate a range of enabled
         * values.
         * @param startId the first id included in the range
         * @param endId the last id included in the range
         */
        protected void AddRange(Int32 startId, Int32 endId, Boolean selected) {
            mConfigList->Add(new SmsBroadcastConfigInfo(startId, endId,
                        SMS_CB_CODE_SCHEME_MIN, SMS_CB_CODE_SCHEME_MAX, selected));
        }

        /**
         * Called to indicate the end of a range update started by the
         * previous call to {@link #startUpdate}.
         * @return TRUE if successful, FALSE otherwise
         */
        protected Boolean FinishUpdate() {
            If (mConfigList->IsEmpty()) {
                return TRUE;
            } else {
                SmsBroadcastConfigInfo[] configs =
                        mConfigList->ToArray(new SmsBroadcastConfigInfo[mConfigList->Size()]);
                return SetCellBroadcastConfig(configs);
            }
        }
    }

    class CdmaBroadcastRangeManager extends IntRangeManager {
        private ArrayList<CdmaSmsBroadcastConfigInfo> mConfigList =
                new ArrayList<CdmaSmsBroadcastConfigInfo>();

        /**
         * Called when the list of enabled ranges has changed. This will be
         * followed by zero or more calls to {@link #addRange} followed by a
         * call to {@link #finishUpdate}.
         */
        protected void StartUpdate() {
            mConfigList->Clear();
        }

        /**
         * Called after {@link #startUpdate} to indicate a range of enabled
         * values.
         * @param startId the first id included in the range
         * @param endId the last id included in the range
         */
        protected void AddRange(Int32 startId, Int32 endId, Boolean selected) {
            mConfigList->Add(new CdmaSmsBroadcastConfigInfo(startId, endId,
                    1, selected));
        }

        /**
         * Called to indicate the end of a range update started by the previous
         * call to {@link #startUpdate}.
         * @return TRUE if successful, FALSE otherwise
         */
        protected Boolean FinishUpdate() {
            If (mConfigList->IsEmpty()) {
                return TRUE;
            } else {
                CdmaSmsBroadcastConfigInfo[] configs =
                        mConfigList->ToArray(new CdmaSmsBroadcastConfigInfo[mConfigList->Size()]);
                return SetCdmaBroadcastConfig(configs);
            }
        }
    }

    private Boolean SetCellBroadcastConfig(SmsBroadcastConfigInfo[] configs) {
        If (DBG)
            Log("Calling setGsmBroadcastConfig with " + configs.length + " configurations");

        Synchronized (mLock) {
            Message response = mHandler->ObtainMessage(EVENT_SET_BROADCAST_CONFIG_DONE);

            mSuccess = FALSE;
            mPhone.mCi->SetGsmBroadcastConfig(configs, response);

            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Log("interrupted while trying to set cell broadcast config");
            }
        }

        return mSuccess;
    }

    private Boolean SetCellBroadcastActivation(Boolean activate) {
        If (DBG)
            Log("Calling SetCellBroadcastActivation(" + activate + ')');

        Synchronized (mLock) {
            Message response = mHandler->ObtainMessage(EVENT_SET_BROADCAST_ACTIVATION_DONE);

            mSuccess = FALSE;
            mPhone.mCi->SetGsmBroadcastActivation(activate, response);

            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Log("interrupted while trying to set cell broadcast activation");
            }
        }

        return mSuccess;
    }

    private Boolean SetCdmaBroadcastConfig(CdmaSmsBroadcastConfigInfo[] configs) {
        If (DBG)
            Log("Calling setCdmaBroadcastConfig with " + configs.length + " configurations");

        Synchronized (mLock) {
            Message response = mHandler->ObtainMessage(EVENT_SET_BROADCAST_CONFIG_DONE);

            mSuccess = FALSE;
            mPhone.mCi->SetCdmaBroadcastConfig(configs, response);

            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Log("interrupted while trying to set cdma broadcast config");
            }
        }

        return mSuccess;
    }

    private Boolean SetCdmaBroadcastActivation(Boolean activate) {
        If (DBG)
            Log("Calling SetCdmaBroadcastActivation(" + activate + ")");

        Synchronized (mLock) {
            Message response = mHandler->ObtainMessage(EVENT_SET_BROADCAST_ACTIVATION_DONE);

            mSuccess = FALSE;
            mPhone.mCi->SetCdmaBroadcastActivation(activate, response);

            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Log("interrupted while trying to set cdma broadcast activation");
            }
        }

        return mSuccess;
    }

    protected void Log(String msg) {
        Logger::D(LOG_TAG, "[IccSmsInterfaceManager] " + msg);
    }

    public Boolean IsImsSmsSupported() {
        return mDispatcher->IsIms();
    }

    public String GetImsSmsFormat() {
        return mDispatcher->GetImsSmsFormat();
    }

    CARAPI SendStoredText(String callingPkg, Uri messageUri, String scAddress,
            PendingIntent sentIntent, PendingIntent deliveryIntent) {
        mPhone->GetContext()->EnforceCallingPermission(Manifest::permission::SEND_SMS,
                "Sending SMS message");
        If (Rlog->IsLoggable("SMS", Log.VERBOSE)) {
            Log("sendStoredText: scAddr=" + scAddress + " messageUri=" + messageUri
                    + " sentIntent=" + sentIntent + " deliveryIntent=" + deliveryIntent);
        }
        If (mAppOps->NoteOp(AppOpsManager.OP_SEND_SMS, Binder->GetCallingUid(), callingPkg)
                != AppOpsManager.MODE_ALLOWED) {
            return;
        }
        final ContentResolver resolver = mPhone->GetContext()->GetContentResolver();
        If (!IsFailedOrDraft(resolver, messageUri)) {
            Logger::E(LOG_TAG, "[IccSmsInterfaceManager]sendStoredText: not FAILED or DRAFT message");
            ReturnUnspecifiedFailure(sentIntent);
            return;
        }
        final String[] textAndAddress = LoadTextAndAddress(resolver, messageUri);
        If (textAndAddress == NULL) {
            Logger::E(LOG_TAG, "[IccSmsInterfaceManager]sendStoredText: can not load text");
            ReturnUnspecifiedFailure(sentIntent);
            return;
        }
        textAndAddress[1] = FilterDestAddress(textAndAddress[1]);
        mDispatcher->SendText(textAndAddress[1], scAddress, textAndAddress[0],
                sentIntent, deliveryIntent, messageUri, callingPkg, -1, FALSE, -1);
    }

    CARAPI SendStoredMultipartText(String callingPkg, Uri messageUri, String scAddress,
            List<PendingIntent> sentIntents, List<PendingIntent> deliveryIntents) {
        mPhone->GetContext()->EnforceCallingPermission(Manifest::permission::SEND_SMS,
                "Sending SMS message");
        If (mAppOps->NoteOp(AppOpsManager.OP_SEND_SMS, Binder->GetCallingUid(), callingPkg)
                != AppOpsManager.MODE_ALLOWED) {
            return;
        }
        final ContentResolver resolver = mPhone->GetContext()->GetContentResolver();
        If (!IsFailedOrDraft(resolver, messageUri)) {
            Logger::E(LOG_TAG, "[IccSmsInterfaceManager]sendStoredMultipartText: "
                    + "not FAILED or DRAFT message");
            ReturnUnspecifiedFailure(sentIntents);
            return;
        }
        final String[] textAndAddress = LoadTextAndAddress(resolver, messageUri);
        If (textAndAddress == NULL) {
            Logger::E(LOG_TAG, "[IccSmsInterfaceManager]sendStoredMultipartText: can not load text");
            ReturnUnspecifiedFailure(sentIntents);
            return;
        }
        final ArrayList<String> parts = SmsManager->GetDefault()->DivideMessage(textAndAddress[0]);
        If (parts == NULL || parts->Size() < 1) {
            Logger::E(LOG_TAG, "[IccSmsInterfaceManager]sendStoredMultipartText: can not divide text");
            ReturnUnspecifiedFailure(sentIntents);
            return;
        }

        If (parts->Size() > 1 && parts->Size() < 10 && !SmsMessage->HasEmsSupport()) {
            For (Int32 i = 0; i < parts->Size(); i++) {
                // If EMS is not supported, we have to break down EMS into single segment SMS
                // and add page info " x/y".
                String singlePart = parts->Get(i);
                If (SmsMessage->ShouldAppendPageNumberAsPrefix()) {
                    singlePart = String->ValueOf(i + 1) + '/' + parts->Size() + ' ' + singlePart;
                } else {
                    singlePart = singlePart->Concat(' ' + String->ValueOf(i + 1) + '/' + parts->Size());
                }

                PendingIntent singleSentIntent = NULL;
                If (sentIntents != NULL && sentIntents->Size() > i) {
                    singleSentIntent = sentIntents->Get(i);
                }

                PendingIntent singleDeliveryIntent = NULL;
                If (deliveryIntents != NULL && deliveryIntents->Size() > i) {
                    singleDeliveryIntent = deliveryIntents->Get(i);
                }

                mDispatcher->SendText(textAndAddress[1], scAddress, singlePart,
                        singleSentIntent, singleDeliveryIntent, messageUri, callingPkg,
                        -1, FALSE, -1);
            }
            return;
        }

        textAndAddress[1] = FilterDestAddress(textAndAddress[1]);
        mDispatcher->SendMultipartText(
                textAndAddress[1], // destAddress
                scAddress,
                parts,
                (ArrayList<PendingIntent>) sentIntents,
                (ArrayList<PendingIntent>) deliveryIntents,
                messageUri,
                callingPkg, -1, FALSE, -1);
    }

    private Boolean IsFailedOrDraft(ContentResolver resolver, Uri messageUri) {
        // Clear the calling identity and query the database using the phone user id
        // Otherwise the AppOps check in TelephonyProvider would complain about mismatch
        // between the calling uid and the package uid
        final Int64 identity = Binder->ClearCallingIdentity();
        Cursor cursor = NULL;
        try {
            cursor = resolver->Query(
                    messageUri,
                    new String[]{ Telephony.Sms.TYPE },
                    NULL/*selection*/,
                    NULL/*selectionArgs*/,
                    NULL/*sortOrder*/);
            If (cursor != NULL && cursor->MoveToFirst()) {
                final Int32 type = cursor->GetInt(0);
                return type == Telephony.Sms.MESSAGE_TYPE_DRAFT
                        || type == Telephony.Sms.MESSAGE_TYPE_FAILED;
            }
        } Catch (SQLiteException e) {
            Logger::E(LOG_TAG, "[IccSmsInterfaceManager]isFailedOrDraft: query message type failed", e);
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
            Binder->RestoreCallingIdentity(identity);
        }
        return FALSE;
    }

    // Return an array including both the SMS Text (0) and Address (1)
    private String[] LoadTextAndAddress(ContentResolver resolver, Uri messageUri) {
        // Clear the calling identity and query the database using the phone user id
        // Otherwise the AppOps check in TelephonyProvider would complain about mismatch
        // between the calling uid and the package uid
        final Int64 identity = Binder->ClearCallingIdentity();
        Cursor cursor = NULL;
        try {
            cursor = resolver->Query(
                    messageUri,
                    new String[]{
                            Telephony.Sms.BODY,
                            Telephony.Sms.ADDRESS
                    },
                    NULL/*selection*/,
                    NULL/*selectionArgs*/,
                    NULL/*sortOrder*/);
            If (cursor != NULL && cursor->MoveToFirst()) {
                return new String[]{ cursor->GetString(0), cursor->GetString(1) };
            }
        } Catch (SQLiteException e) {
            Logger::E(LOG_TAG, "[IccSmsInterfaceManager]loadText: query message text failed", e);
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
            Binder->RestoreCallingIdentity(identity);
        }
        return NULL;
    }

    private void ReturnUnspecifiedFailure(PendingIntent pi) {
        If (pi != NULL) {
            try {
                pi->Send(SmsManager.RESULT_ERROR_GENERIC_FAILURE);
            } Catch (PendingIntent.CanceledException e) {
                // ignore
            }
        }
    }

    private void ReturnUnspecifiedFailure(List<PendingIntent> pis) {
        If (pis == NULL) {
            return;
        }
        For (PendingIntent pi : pis) {
            ReturnUnspecifiedFailure(pi);
        }
    }

    private void EnforceCarrierPrivilege() {
        UiccController controller = UiccController->GetInstance();
        If (controller == NULL || controller->GetUiccCard() == NULL) {
            throw new SecurityException("No Carrier Privilege: No UICC");
        }
        If (controller->GetUiccCard()->GetCarrierPrivilegeStatusForCurrentTransaction(
                mContext->GetPackageManager()) !=
                    TelephonyManager.CARRIER_PRIVILEGE_STATUS_HAS_ACCESS) {
            throw new SecurityException("No Carrier Privilege.");
        }
    }

    private String FilterDestAddress(String destAddr) {
        String result  = NULL;
        result = SmsNumberUtils->FilterDestAddr(mPhone, destAddr);
        return result != NULL ? result : destAddr;
    }

    public Int32 GetSmsCapacityOnIcc() {
        Int32 numberOnIcc = -1;
        IccRecords ir = mPhone->GetIccRecords();

        If (ir != NULL) {
            numberOnIcc = ir->GetSmsCapacityOnIcc();
        } else {
            Log("getSmsCapacityOnIcc - aborting, no icc card present.");
        }

        Log("GetSmsCapacityOnIcc().numberOnIcc = " + numberOnIcc);
        return numberOnIcc;
    }

    /** @hide **/
    public Boolean IsShortSMSCode(String destAddr) {
        TelephonyManager telephonyManager;
        Int32 smsCategory = SmsUsageMonitor.CATEGORY_NOT_SHORT_CODE;

        telephonyManager =(TelephonyManager) mContext->GetSystemService(Context.TELEPHONY_SERVICE);

        String countryIso = telephonyManager->GetSimCountryIso();
        If (countryIso == NULL || countryIso->Length() != 2) {
            countryIso = telephonyManager->GetNetworkCountryIso();
        }

        smsCategory = SmsUsageMonitor->MergeShortCodeCategories(smsCategory,
                mPhone.mSmsUsageMonitor->CheckDestination(destAddr, countryIso));

        If (smsCategory == SmsUsageMonitor.CATEGORY_NOT_SHORT_CODE
                || smsCategory == SmsUsageMonitor.CATEGORY_FREE_SHORT_CODE
                || smsCategory == SmsUsageMonitor.CATEGORY_STANDARD_SHORT_CODE) {
            return FALSE;    // not a premium short code
        }

        return TRUE;
    }
}
