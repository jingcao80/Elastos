/*
 * Copyright (C) 2013 The Android Open Source Project
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

package com.android.internal.telephony.cdma;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::ISmsCbMessage;

using Elastos::Droid::Internal::Telephony::ICellBroadcastHandler;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IInboundSmsHandler;
using Elastos::Droid::Internal::Telephony::IInboundSmsTracker;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::ISmsConstants;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::ISmsStorageMonitor;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::IWspTypeDecoder;
using Elastos::Droid::Internal::Telephony::Cdma::Sms::ISmsEnvelope;

using Elastos::Utility::IArrays;

/**
 * Subclass of {@link InboundSmsHandler} for 3GPP2 type messages.
 */
public class CdmaInboundSmsHandler extends InboundSmsHandler {

    private final CdmaSMSDispatcher mSmsDispatcher;
    private final CdmaServiceCategoryProgramHandler mServiceCategoryProgramHandler;

    private Byte[] mLastDispatchedSmsFingerprint;
    private Byte[] mLastAcknowledgedSmsFingerprint;

    private final Boolean mCheckForDuplicatePortsInOmadmWapPush = Resources->GetSystem()->GetBoolean(
            R.bool.config_duplicate_port_omadm_wappush);

    /**
     * Create a new inbound SMS handler for CDMA.
     */
    private CdmaInboundSmsHandler(Context context, SmsStorageMonitor storageMonitor,
            PhoneBase phone, CdmaSMSDispatcher smsDispatcher) {
        Super("CdmaInboundSmsHandler", context, storageMonitor, phone,
                CellBroadcastHandler->MakeCellBroadcastHandler(context, phone));
        mSmsDispatcher = smsDispatcher;
        mServiceCategoryProgramHandler = CdmaServiceCategoryProgramHandler->MakeScpHandler(context,
                phone.mCi);
        phone.mCi->SetOnNewCdmaSms(GetHandler(), EVENT_NEW_SMS, NULL);
    }

    /**
     * Unregister for CDMA SMS.
     */
    //@Override
    protected void OnQuitting() {
        mPhone.mCi->UnSetOnNewCdmaSms(GetHandler());
        mCellBroadcastHandler->Dispose();

        If (DBG) Log("unregistered for 3GPP2 SMS");
        super->OnQuitting();
    }

    /**
     * Wait for state machine to enter startup state. We can't send any messages until then.
     */
    public static CdmaInboundSmsHandler MakeInboundSmsHandler(Context context,
            SmsStorageMonitor storageMonitor, PhoneBase phone, CdmaSMSDispatcher smsDispatcher) {
        CdmaInboundSmsHandler handler = new CdmaInboundSmsHandler(context, storageMonitor,
                phone, smsDispatcher);
        handler->Start();
        return handler;
    }

    /**
     * Return whether the device is in Emergency Call Mode (only for 3GPP2).
     * @return TRUE if the device is in ECM; FALSE otherwise
     */
    private static Boolean IsInEmergencyCallMode() {
        String inEcm = SystemProperties->Get(TelephonyProperties.PROPERTY_INECM_MODE, "FALSE");
        return "TRUE".Equals(inEcm);
    }

    /**
     * Return TRUE if this handler is for 3GPP2 messages; FALSE for 3GPP format.
     * @return TRUE (3GPP2)
     */
    //@Override
    protected Boolean Is3gpp2() {
        return TRUE;
    }

    /**
     * Process Cell Broadcast, Voicemail Notification, and other 3GPP/3GPP2-specific messages.
     * @param smsb the SmsMessageBase object from the RIL
     * @return TRUE if the message was handled here; FALSE to continue processing
     */
    //@Override
    protected Int32 DispatchMessageRadioSpecific(SmsMessageBase smsb) {
        If (IsInEmergencyCallMode()) {
            return Activity.RESULT_OK;
        }

        SmsMessage sms = (SmsMessage) smsb;
        Boolean isBroadcastType = (SmsEnvelope.MESSAGE_TYPE_BROADCAST == sms->GetMessageType());

        // Handle CMAS emergency broadcast messages.
        If (isBroadcastType) {
            Log("Broadcast type message");
            SmsCbMessage cbMessage = sms->ParseBroadcastSms();
            If (cbMessage != NULL) {
                mCellBroadcastHandler->DispatchSmsMessage(cbMessage);
            } else {
                Loge("error trying to parse broadcast SMS");
            }
            return Intents.RESULT_SMS_HANDLED;
        }

        // Initialize fingerprint field, and see if we have a network duplicate SMS.
        mLastDispatchedSmsFingerprint = sms->GetIncomingSmsFingerprint();
        If (mLastAcknowledgedSmsFingerprint != NULL &&
                Arrays->Equals(mLastDispatchedSmsFingerprint, mLastAcknowledgedSmsFingerprint)) {
            return Intents.RESULT_SMS_HANDLED;
        }

        // Decode BD stream and set sms variables.
        sms->ParseSms();
        Int32 teleService = sms->GetTeleService();

        Switch (teleService) {
            case SmsEnvelope.TELESERVICE_VMN:
            case SmsEnvelope.TELESERVICE_MWI:
                // handle voicemail indication
                HandleVoicemailTeleservice(sms);
                return Intents.RESULT_SMS_HANDLED;

            case SmsEnvelope.TELESERVICE_WMT:
            case SmsEnvelope.TELESERVICE_WEMT:
                If (sms->IsStatusReportMessage()) {
                    mSmsDispatcher->SendStatusReportMessage(sms);
                    return Intents.RESULT_SMS_HANDLED;
                }
                break;

            case SmsEnvelope.TELESERVICE_SCPT:
                mServiceCategoryProgramHandler->DispatchSmsMessage(sms);
                return Intents.RESULT_SMS_HANDLED;

            case SmsEnvelope.TELESERVICE_WAP:
                // handled below, after storage check
                break;

            case SmsEnvelope.TELESERVICE_CT_WAP:
                // handled below, after TELESERVICE_WAP
                break;

            default:
                Loge("unsupported teleservice 0x" + Integer->ToHexString(teleService));
                return Intents.RESULT_SMS_UNSUPPORTED;
        }

        If (!mStorageMonitor->IsStorageAvailable() &&
                sms->GetMessageClass() != SmsConstants.MessageClass.CLASS_0) {
            // It's a storable message and there's no storage available.  Bail.
            // (See C.S0015-B v2.0 for a description of "Immediate Display"
            // messages, which we represent as CLASS_0.)
            return Intents.RESULT_SMS_OUT_OF_MEMORY;
        }

        If (SmsEnvelope.TELESERVICE_WAP == teleService) {
            return ProcessCdmaWapPdu(sms->GetUserData(), sms.mMessageRef,
                    sms->GetOriginatingAddress(), sms->GetTimestampMillis());
        } else If (SmsEnvelope.TELESERVICE_CT_WAP == teleService) {
            /* China Telecom WDP header contains Message identifier
               and User data subparametrs extract these fields */
            If (!sms->ProcessCdmaCTWdpHeader(sms)) {
                return Intents.RESULT_SMS_HANDLED;
            }
            return ProcessCdmaWapPdu(sms->GetUserData(), sms.mMessageRef,
                    sms->GetOriginatingAddress(), sms->GetTimestampMillis());
        }

        return DispatchNormalMessage(smsb);
    }

    /**
     * Send an acknowledge message.
     * @param success indicates that last message was successfully received.
     * @param result result code indicating any error
     * @param response callback message sent when operation completes.
     */
    //@Override
    protected void AcknowledgeLastIncomingSms(Boolean success, Int32 result, Message response) {
        If (IsInEmergencyCallMode()) {
            return;
        }

        Int32 causeCode = ResultToCause(result);
        mPhone.mCi->AcknowledgeLastIncomingCdmaSms(success, causeCode, response);

        If (causeCode == 0) {
            mLastAcknowledgedSmsFingerprint = mLastDispatchedSmsFingerprint;
        }
        mLastDispatchedSmsFingerprint = NULL;
    }

    /**
     * Called when the phone changes the default method updates mPhone
     * mStorageMonitor and mCellBroadcastHandler.updatePhoneObject.
     * Override if different or other behavior is desired.
     *
     * @param phone
     */
    //@Override
    protected void OnUpdatePhoneObject(PhoneBase phone) {
        super->OnUpdatePhoneObject(phone);
        mCellBroadcastHandler->UpdatePhoneObject(phone);
    }

    /**
     * Convert Android result code to CDMA SMS failure cause.
     * @param rc the Android SMS intent result value
     * @return 0 for success, or a CDMA SMS failure cause value
     */
    private static Int32 ResultToCause(Int32 rc) {
        Switch (rc) {
        case Activity.RESULT_OK:
        case Intents.RESULT_SMS_HANDLED:
            // Cause code is ignored on success.
            return 0;
        case Intents.RESULT_SMS_OUT_OF_MEMORY:
            return CommandsInterface.CDMA_SMS_FAIL_CAUSE_RESOURCE_SHORTAGE;
        case Intents.RESULT_SMS_UNSUPPORTED:
            return CommandsInterface.CDMA_SMS_FAIL_CAUSE_INVALID_TELESERVICE_ID;
        case Intents.RESULT_SMS_GENERIC_ERROR:
        default:
            return CommandsInterface.CDMA_SMS_FAIL_CAUSE_ENCODING_PROBLEM;
        }
    }

    /**
     * Handle {@link SmsEnvelope#TELESERVICE_VMN} and {@link SmsEnvelope#TELESERVICE_MWI}.
     * @param sms the message to process
     */
    private void HandleVoicemailTeleservice(SmsMessage sms) {
        Int32 voicemailCount = sms->GetNumOfVoicemails();
        If (DBG) Log("Voicemail count=" + voicemailCount);

        mPhone->SetVoiceMessageWaiting(1, voicemailCount);
        // range check
        If (voicemailCount < 0) {
            voicemailCount = -1;
        } else If (voicemailCount > 99) {
            // C.S0015-B v2, 4.5.12
            // range: 0-99
            voicemailCount = 99;
        }
        // update voice mail count in phone
        mPhone->SetVoiceMessageCount(voicemailCount);
        // store voice mail count in preferences
        StoreVoiceMailCount();
    }

    /**
     * Processes inbound messages that are in the WAP-WDP PDU format. See
     * wap-259-wdp-20010614-a section 6.5 for details on the WAP-WDP PDU format.
     * WDP segments are gathered until a datagram completes and gets dispatched.
     *
     * @param pdu The WAP-WDP PDU segment
     * @return a result code from {@link android.provider.Telephony.Sms.Intents}, or
     *         {@link Activity#RESULT_OK} if the message has been broadcast
     *         to applications
     */
    private Int32 ProcessCdmaWapPdu(Byte[] pdu, Int32 referenceNumber, String address,
            Int64 timestamp) {
        Int32 index = 0;

        Int32 msgType = (0xFF & pdu[index++]);
        If (msgType != 0) {
            Log("Received a WAP SMS which is not WDP. Discard.");
            return Intents.RESULT_SMS_HANDLED;
        }
        Int32 totalSegments = (0xFF & pdu[index++]);   // >= 1
        Int32 segment = (0xFF & pdu[index++]);         // >= 0

        If (segment >= totalSegments) {
            Loge("WDP bad segment #" + segment + " expecting 0-" + (totalSegments - 1));
            return Intents.RESULT_SMS_HANDLED;
        }

        // Only the first segment contains sourcePort and destination Port
        Int32 sourcePort = 0;
        Int32 destinationPort = 0;
        If (segment == 0) {
            //process WDP segment
            sourcePort = (0xFF & pdu[index++]) << 8;
            sourcePort |= 0xFF & pdu[index++];
            destinationPort = (0xFF & pdu[index++]) << 8;
            destinationPort |= 0xFF & pdu[index++];
            // Some carriers incorrectly send duplicate port fields in omadm wap pushes.
            // If configured, check for that here
            If (mCheckForDuplicatePortsInOmadmWapPush) {
                If (CheckDuplicatePortOmadmWapPush(pdu, index)) {
                    index = index + 4; // skip duplicate port fields
                }
            }
        }

        // Lookup all other related parts
        Log("Received WAP PDU. Type = " + msgType + ", originator = " + address
                + ", src-port = " + sourcePort + ", dst-port = " + destinationPort
                + ", ID = " + referenceNumber + ", segment# = " + segment + '/' + totalSegments);

        // pass the user data portion of the PDU to the shared handler in SMSDispatcher
        Byte[] userData = new Byte[pdu.length - index];
        System->Arraycopy(pdu, index, userData, 0, pdu.length - index);

        InboundSmsTracker tracker = new InboundSmsTracker(userData, timestamp, destinationPort,
                TRUE, address, referenceNumber, segment, totalSegments, TRUE);

        return AddTrackerToRawTableAndSendMessage(tracker);
    }

    /**
     * Optional check to see if the received WapPush is an OMADM notification with erroneous
     * extra port fields.
     * - Some carriers make this mistake.
     * ex: MSGTYPE-TotalSegments-CurrentSegment
     *       -SourcePortDestPort-SourcePortDestPort-OMADM PDU
     * @param origPdu The WAP-WDP PDU segment
     * @param index Current Index while parsing the PDU.
     * @return True if OrigPdu is OmaDM Push Message which has duplicate ports.
     *         False if OrigPdu is NOT OmaDM Push Message which has duplicate ports.
     */
    private static Boolean CheckDuplicatePortOmadmWapPush(Byte[] origPdu, Int32 index) {
        index += 4;
        Byte[] omaPdu = new Byte[origPdu.length - index];
        System->Arraycopy(origPdu, index, omaPdu, 0, omaPdu.length);

        WspTypeDecoder pduDecoder = new WspTypeDecoder(omaPdu);
        Int32 wspIndex = 2;

        // Process header length field
        If (!pduDecoder->DecodeUintvarInteger(wspIndex)) {
            return FALSE;
        }

        wspIndex += pduDecoder->GetDecodedDataLength();  // advance to next field

        // Process content type field
        If (!pduDecoder->DecodeContentType(wspIndex)) {
            return FALSE;
        }

        String mimeType = pduDecoder->GetValueString();
        Return (WspTypeDecoder.CONTENT_TYPE_B_PUSH_SYNCML_NOTI->Equals(mimeType));
    }
}
