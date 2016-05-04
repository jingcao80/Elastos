/*
 * Copyright (C) 2011 The Android Open Source Project
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

package com.android.internal.telephony.gsm;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::ISmsManager;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Cat::IComprehensionTlvTag;
using Elastos::Droid::Internal::Telephony::Uicc::IIccIoResult;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
using Elastos::Droid::Internal::Telephony::Uicc::IUsimServiceTable;

/**
 * Handler for SMS-PP data download messages.
 * See 3GPP TS 31.111 section 7.1.1
 */
public class UsimDataDownloadHandler extends Handler {
    private static const String TAG = "UsimDataDownloadHandler";

    /** BER-TLV tag for SMS-PP download. TS 31.111 section 9.1. */
    private static const Int32 BER_SMS_PP_DOWNLOAD_TAG      = 0xd1;

    /** Device identity value for UICC (destination). */
    private static const Int32 DEV_ID_UICC        = 0x81;

    /** Device identity value for Network (source). */
    private static const Int32 DEV_ID_NETWORK     = 0x83;

    /** Message containing new SMS-PP message to process. */
    private static const Int32 EVENT_START_DATA_DOWNLOAD = 1;

    /** Response to SMS-PP download envelope command. */
    private static const Int32 EVENT_SEND_ENVELOPE_RESPONSE = 2;

    /** Result of writing SM to UICC (when SMS-PP service is not available). */
    private static const Int32 EVENT_WRITE_SMS_COMPLETE = 3;

    private final CommandsInterface mCi;

    public UsimDataDownloadHandler(CommandsInterface commandsInterface) {
        mCi = commandsInterface;
    }

    /**
     * Handle SMS-PP data download messages. Normally these are automatically handled by the
     * radio, but we may have to deal with this type of SM arriving via the IMS stack. If the
     * data download service is not enabled, try to write to the USIM as an SMS, and send the
     * UICC response as the acknowledgment to the SMSC.
     *
     * @param ust the UsimServiceTable, to check if data download is enabled
     * @param smsMessage the SMS message to process
     * @return {@code Activity.RESULT_OK} on success; {@code RESULT_SMS_GENERIC_ERROR} on failure
     */
    Int32 HandleUsimDataDownload(UsimServiceTable ust, SmsMessage smsMessage) {
        // If we receive an SMS-PP message before the UsimServiceTable has been loaded,
        // assume that the data download service is not present. This is very unlikely to
        // happen because the IMS connection will not be established until after the ISIM
        // records have been loaded, after the USIM service table has been loaded.
        If (ust != NULL && ust->IsAvailable(
                UsimServiceTable.UsimService.DATA_DL_VIA_SMS_PP)) {
            Rlog->D(TAG, "Received SMS-PP data download, sending to UICC.");
            return StartDataDownload(smsMessage);
        } else {
            Rlog->D(TAG, "DATA_DL_VIA_SMS_PP service not available, storing message to UICC.");
            String smsc = IccUtils->BytesToHexString(
                    PhoneNumberUtils->NetworkPortionToCalledPartyBCDWithLength(
                            smsMessage->GetServiceCenterAddress()));
            mCi->WriteSmsToSim(SmsManager.STATUS_ON_ICC_UNREAD, smsc,
                    IccUtils->BytesToHexString(smsMessage->GetPdu()),
                    ObtainMessage(EVENT_WRITE_SMS_COMPLETE));
            return Activity.RESULT_OK;  // acknowledge after response from write to USIM
        }

    }

    /**
     * Start an SMS-PP data download for the specified message. Can be called from a different
     * thread than this Handler is running on.
     *
     * @param smsMessage the message to process
     * @return {@code Activity.RESULT_OK} on success; {@code RESULT_SMS_GENERIC_ERROR} on failure
     */
    public Int32 StartDataDownload(SmsMessage smsMessage) {
        If (SendMessage(ObtainMessage(EVENT_START_DATA_DOWNLOAD, smsMessage))) {
            return Activity.RESULT_OK;  // we will send SMS ACK/ERROR based on UICC response
        } else {
            Rlog->E(TAG, "startDataDownload failed to send message to start data download.");
            return Intents.RESULT_SMS_GENERIC_ERROR;
        }
    }

    private void HandleDataDownload(SmsMessage smsMessage) {
        Int32 dcs = smsMessage->GetDataCodingScheme();
        Int32 pid = smsMessage->GetProtocolIdentifier();
        Byte[] pdu = smsMessage->GetPdu();           // includes SC address

        Int32 scAddressLength = pdu[0] & 0xff;
        Int32 tpduIndex = scAddressLength + 1;        // start of TPDU
        Int32 tpduLength = pdu.length - tpduIndex;

        Int32 bodyLength = GetEnvelopeBodyLength(scAddressLength, tpduLength);

        // Add 1 Byte for SMS-PP download tag and 1-2 bytes for BER-TLV length.
        // See ETSI TS 102 223 Annex C for encoding of length and tags.
        Int32 totalLength = bodyLength + 1 + (bodyLength > 127 ? 2 : 1);

        Byte[] envelope = new Byte[totalLength];
        Int32 index = 0;

        // SMS-PP download tag and Length (assumed to be < 256 bytes).
        envelope[index++] = (Byte) BER_SMS_PP_DOWNLOAD_TAG;
        If (bodyLength > 127) {
            envelope[index++] = (Byte) 0x81;    // length 128-255 encoded as 0x81 + length
        }
        envelope[index++] = (Byte) bodyLength;

        // Device identities TLV
        envelope[index++] = (Byte) (0x80 | ComprehensionTlvTag.DEVICE_IDENTITIES->Value());
        envelope[index++] = (Byte) 2;
        envelope[index++] = (Byte) DEV_ID_NETWORK;
        envelope[index++] = (Byte) DEV_ID_UICC;

        // Address TLV (if present). Encoded length is assumed to be < 127 bytes.
        If (scAddressLength != 0) {
            envelope[index++] = (Byte) ComprehensionTlvTag.ADDRESS->Value();
            envelope[index++] = (Byte) scAddressLength;
            System->Arraycopy(pdu, 1, envelope, index, scAddressLength);
            index += scAddressLength;
        }

        // SMS TPDU TLV. Length is assumed to be < 256 bytes.
        envelope[index++] = (Byte) (0x80 | ComprehensionTlvTag.SMS_TPDU->Value());
        If (tpduLength > 127) {
            envelope[index++] = (Byte) 0x81;    // length 128-255 encoded as 0x81 + length
        }
        envelope[index++] = (Byte) tpduLength;
        System->Arraycopy(pdu, tpduIndex, envelope, index, tpduLength);
        index += tpduLength;

        // Verify that we calculated the payload size correctly.
        If (index != envelope.length) {
            Rlog->E(TAG, "StartDataDownload() calculated incorrect envelope length, aborting.");
            AcknowledgeSmsWithError(CommandsInterface.GSM_SMS_FAIL_CAUSE_UNSPECIFIED_ERROR);
            return;
        }

        String encodedEnvelope = IccUtils->BytesToHexString(envelope);
        mCi->SendEnvelopeWithStatus(encodedEnvelope, ObtainMessage(
                EVENT_SEND_ENVELOPE_RESPONSE, new Int32[]{ dcs, pid }));
    }

    /**
     * Return the size in bytes of the envelope to send to the UICC, excluding the
     * SMS-PP download tag Byte and length Byte(s). If the size returned is <= 127,
     * the BER-TLV length will be encoded in 1 Byte, otherwise 2 bytes are required.
     *
     * @param scAddressLength the length of the SMSC address, or zero if not present
     * @param tpduLength the length of the TPDU from the SMS-PP message
     * @return the number of bytes to allocate for the envelope command
     */
    private static Int32 GetEnvelopeBodyLength(Int32 scAddressLength, Int32 tpduLength) {
        // Add 4 bytes for device identities TLV + 1 Byte for SMS TPDU tag Byte
        Int32 length = tpduLength + 5;
        // Add 1 Byte for TPDU length, or 2 bytes if length > 127
        length += (tpduLength > 127 ? 2 : 1);
        // Add length of address tag, if Present (+ 2 bytes for tag and length)
        If (scAddressLength != 0) {
            length = length + 2 + scAddressLength;
        }
        return length;
    }

    /**
     * Handle the response to the ENVELOPE command.
     * @param response UICC response encoded as hexadecimal digits. First two bytes are the
     *  UICC SW1 and SW2 status bytes.
     */
    private void SendSmsAckForEnvelopeResponse(IccIoResult response, Int32 dcs, Int32 pid) {
        Int32 sw1 = response.sw1;
        Int32 sw2 = response.sw2;

        Boolean success;
        If ((sw1 == 0x90 && sw2 == 0x00) || sw1 == 0x91) {
            Rlog->D(TAG, "USIM data download succeeded: " + response->ToString());
            success = TRUE;
        } else If (sw1 == 0x93 && sw2 == 0x00) {
            Rlog->E(TAG, "USIM data download failed: Toolkit busy");
            AcknowledgeSmsWithError(CommandsInterface.GSM_SMS_FAIL_CAUSE_USIM_APP_TOOLKIT_BUSY);
            return;
        } else If (sw1 == 0x62 || sw1 == 0x63) {
            Rlog->E(TAG, "USIM data download failed: " + response->ToString());
            success = FALSE;
        } else {
            Rlog->E(TAG, "Unexpected SW1/SW2 response from UICC: " + response->ToString());
            success = FALSE;
        }

        Byte[] responseBytes = response.payload;
        If (responseBytes == NULL || responseBytes.length == 0) {
            If (success) {
                mCi->AcknowledgeLastIncomingGsmSms(TRUE, 0, NULL);
            } else {
                AcknowledgeSmsWithError(
                        CommandsInterface.GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR);
            }
            return;
        }

        Byte[] smsAckPdu;
        Int32 index = 0;
        If (success) {
            smsAckPdu = new Byte[responseBytes.length + 5];
            smsAckPdu[index++] = 0x00;      // TP-MTI, TP-UDHI
            smsAckPdu[index++] = 0x07;      // TP-PI: TP-PID, TP-DCS, TP-UDL present
        } else {
            smsAckPdu = new Byte[responseBytes.length + 6];
            smsAckPdu[index++] = 0x00;      // TP-MTI, TP-UDHI
            smsAckPdu[index++] = (Byte)
                    CommandsInterface.GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR;  // TP-FCS
            smsAckPdu[index++] = 0x07;      // TP-PI: TP-PID, TP-DCS, TP-UDL present
        }

        smsAckPdu[index++] = (Byte) pid;
        smsAckPdu[index++] = (Byte) dcs;

        If (Is7bitDcs(dcs)) {
            Int32 septetCount = responseBytes.length * 8 / 7;
            smsAckPdu[index++] = (Byte) septetCount;
        } else {
            smsAckPdu[index++] = (Byte) responseBytes.length;
        }

        System->Arraycopy(responseBytes, 0, smsAckPdu, index, responseBytes.length);

        mCi->AcknowledgeIncomingGsmSmsWithPdu(success,
                IccUtils->BytesToHexString(smsAckPdu), NULL);
    }

    private void AcknowledgeSmsWithError(Int32 cause) {
        mCi->AcknowledgeLastIncomingGsmSms(FALSE, cause, NULL);
    }

    /**
     * Returns whether the DCS is 7 bit. If so, set TP-UDL to the septet count of TP-UD;
     * otherwise, set TP-UDL to the octet count of TP-UD.
     * @param dcs the TP-Data-Coding-Scheme field from the original download SMS
     * @return TRUE if the DCS specifies 7 bit encoding; FALSE otherwise
     */
    private static Boolean Is7bitDcs(Int32 dcs) {
        // See 3GPP TS 23.038 section 4
        Return ((dcs & 0x8C) == 0x00) || ((dcs & 0xF4) == 0xF0);
    }

    /**
     * Handle UICC envelope response and send SMS acknowledgement.
     *
     * @param msg the message to handle
     */
    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;

        Switch (msg.what) {
            case EVENT_START_DATA_DOWNLOAD:
                HandleDataDownload((SmsMessage) msg.obj);
                break;

            case EVENT_SEND_ENVELOPE_RESPONSE:
                ar = (AsyncResult) msg.obj;

                If (ar.exception != NULL) {
                    Rlog->E(TAG, "UICC Send Envelope failure, exception: " + ar.exception);
                    AcknowledgeSmsWithError(
                            CommandsInterface.GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR);
                    return;
                }

                Int32[] dcsPid = (Int32[]) ar.userObj;
                SendSmsAckForEnvelopeResponse((IccIoResult) ar.result, dcsPid[0], dcsPid[1]);
                break;

            case EVENT_WRITE_SMS_COMPLETE:
                ar = (AsyncResult) msg.obj;
                If (ar.exception == NULL) {
                    Rlog->D(TAG, "Successfully wrote SMS-PP message to UICC");
                    mCi->AcknowledgeLastIncomingGsmSms(TRUE, 0, NULL);
                } else {
                    Rlog->D(TAG, "Failed to write SMS-PP message to UICC", ar.exception);
                    mCi->AcknowledgeLastIncomingGsmSms(FALSE,
                            CommandsInterface.GSM_SMS_FAIL_CAUSE_UNSPECIFIED_ERROR, NULL);
                }
                break;

            default:
                Rlog->E(TAG, "Ignoring unexpected message, what=" + msg.what);
        }
    }
}
