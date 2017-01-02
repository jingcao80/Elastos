//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/uicc/IccIoResult.h"
#include "elastos/droid/internal/telephony/gsm/CSmsMessage.h"
#include "elastos/droid/internal/telephony/gsm/CUsimDataDownloadHandler.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/telephony/PhoneNumberUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Internal::Telephony::IccUtils;
using Elastos::Droid::Internal::Telephony::ISmsMessageBase;
using Elastos::Droid::Internal::Telephony::Uicc::IccIoResult;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IAsyncResult;
// TODO:
// using Elastos::Droid::Provider::Telephony::Sms::IIntents;
using Elastos::Droid::Telephony::ISmsManager;
using Elastos::Droid::Telephony::PhoneNumberUtils;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

const String CUsimDataDownloadHandler::TAG("UsimDataDownloadHandler");
const Int32 CUsimDataDownloadHandler::BER_SMS_PP_DOWNLOAD_TAG;
const Int32 CUsimDataDownloadHandler::DEV_ID_UICC;
const Int32 CUsimDataDownloadHandler::DEV_ID_NETWORK;
const Int32 CUsimDataDownloadHandler::EVENT_START_DATA_DOWNLOAD;
const Int32 CUsimDataDownloadHandler::EVENT_SEND_ENVELOPE_RESPONSE;
const Int32 CUsimDataDownloadHandler::EVENT_WRITE_SMS_COMPLETE;

CAR_INTERFACE_IMPL(CUsimDataDownloadHandler, Handler, IUsimDataDownloadHandler)

CAR_OBJECT_IMPL(CUsimDataDownloadHandler)

CUsimDataDownloadHandler::CUsimDataDownloadHandler()
{
}

ECode CUsimDataDownloadHandler::constructor()
{
    return NOERROR;
}

ECode CUsimDataDownloadHandler::constructor(
    /* [in] */ ICommandsInterface* commandsInterface)
{
    mCi = commandsInterface;
    return NOERROR;
}

ECode CUsimDataDownloadHandler::HandleUsimDataDownload(
     /* [in] */ IUsimServiceTable* ust,
     /* [in] */ ISmsMessage* smsMessage,
     /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // If we receive an SMS-PP message before the UsimServiceTable has been loaded,
    // assume that the data download service is not present. This is very unlikely to
    // happen because the IMS connection will not be established until after the ISIM
    // records have been loaded, after the USIM service table has been loaded.
    Boolean b;
    if (ust != NULL && (ust->IsAvailable(
            Elastos::Droid::Internal::Telephony::Uicc::DATA_DL_VIA_SMS_PP, &b), b)) {
        Logger::D(TAG, "Received SMS-PP data download, sending to UICC.");
        return StartDataDownload(smsMessage, result);
    }
    else {
        Logger::D(TAG, "DATA_DL_VIA_SMS_PP service not available, storing message to UICC.");
        String addr;
        ISmsMessageBase::Probe(smsMessage)->GetServiceCenterAddress(&addr);
        AutoPtr<ArrayOf<Byte> > bytes;
        PhoneNumberUtils::NetworkPortionToCalledPartyBCDWithLength(addr, (ArrayOf<Byte>**)&bytes);
        String smsc = IccUtils::BytesToHexString(bytes);

        AutoPtr<ArrayOf<Byte> > pdu;
        ISmsMessageBase::Probe(smsMessage)->GetPdu((ArrayOf<Byte>**)&pdu);
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_WRITE_SMS_COMPLETE, (IMessage**)&msg);
        mCi->WriteSmsToSim(ISmsManager::STATUS_ON_ICC_UNREAD, smsc,
                IccUtils::BytesToHexString(pdu), msg);
        *result = IActivity::RESULT_OK;  // acknowledge after response from write to USIM
    }
    return NOERROR;
}

ECode CUsimDataDownloadHandler::StartDataDownload(
     /* [in] */ ISmsMessage* smsMessage,
     /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_START_DATA_DOWNLOAD, smsMessage, (IMessage**)&msg);
    Boolean b;
    if (SendMessage(msg, &b), b) {
        *result = IActivity::RESULT_OK;  // we will send SMS ACK/ERROR based on UICC response
    }
    else {
        Logger::E(TAG, "startDataDownload failed to send message to start data download.");
        *result = 2; //IIntents::RESULT_SMS_GENERIC_ERROR;
    }
    return NOERROR;
}

ECode CUsimDataDownloadHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (what) {
        case EVENT_START_DATA_DOWNLOAD:
            HandleDataDownload(ISmsMessage::Probe(obj));
            break;

        case EVENT_SEND_ENVELOPE_RESPONSE: {
            ar = (AsyncResult*)IAsyncResult::Probe(obj);

            if (ar->mException != NULL) {
                // Logger::E(TAG, "UICC Send Envelope failure, exception: " + ar.exception);
                AcknowledgeSmsWithError(
                        ICommandsInterface::GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR);
                return NOERROR;
            }

            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mUserObj);
            Int32 dcs, pid;
            AutoPtr<IInterface> o;
            iArray->Get(0, (IInterface**)&o);
            IInteger32::Probe(o)->GetValue(&dcs);
            o = NULL;
            iArray->Get(1, (IInterface**)&o);
            IInteger32::Probe(o)->GetValue(&pid);

            SendSmsAckForEnvelopeResponse(IIccIoResult::Probe(ar->mResult), dcs, pid);
            break;
        }
        case EVENT_WRITE_SMS_COMPLETE:
        ar = (AsyncResult*)IAsyncResult::Probe(obj);
        if (ar->mException != NULL) {
                Logger::D(TAG, "Successfully wrote SMS-PP message to UICC");
                mCi->AcknowledgeLastIncomingGsmSms(TRUE, 0, NULL);
            }
            else {
                // Logger::D(TAG, "Failed to write SMS-PP message to UICC", ar.exception);
                mCi->AcknowledgeLastIncomingGsmSms(FALSE,
                        ICommandsInterface::GSM_SMS_FAIL_CAUSE_UNSPECIFIED_ERROR, NULL);
            }
            break;

        default:
            Logger::E(TAG, "Ignoring unexpected message, what=%d", what);
    }
    return NOERROR;
}

void CUsimDataDownloadHandler::HandleDataDownload(
    /* [in] */ ISmsMessage* smsMessage)
{
    Int32 dcs;
    ((CSmsMessage*)smsMessage)->GetDataCodingScheme(&dcs);
    Int32 pid;
    ISmsMessageBase::Probe(smsMessage)->GetProtocolIdentifier(&pid);
    AutoPtr<ArrayOf<Byte> > pdu;
    ISmsMessageBase::Probe(smsMessage)->GetPdu((ArrayOf<Byte>**)&pdu);           // includes SC address

    Int32 scAddressLength = (*pdu)[0] & 0xff;
    Int32 tpduIndex = scAddressLength + 1;        // start of TPDU
    Int32 tpduLength = pdu->GetLength() - tpduIndex;

    Int32 bodyLength = GetEnvelopeBodyLength(scAddressLength, tpduLength);

    // Add 1 byte for SMS-PP download tag and 1-2 bytes for BER-TLV length.
    // See ETSI TS 102 223 Annex C for encoding of length and tags.
    Int32 totalLength = bodyLength + 1 + (bodyLength > 127 ? 2 : 1);

    AutoPtr<ArrayOf<Byte> > envelope = ArrayOf<Byte>::Alloc(totalLength);
    Int32 index = 0;

    // SMS-PP download tag and length (assumed to be < 256 bytes).
    (*envelope)[index++] = (Byte) BER_SMS_PP_DOWNLOAD_TAG;
    if (bodyLength > 127) {
        (*envelope)[index++] = (Byte) 0x81;    // length 128-255 encoded as 0x81 + length
    }
    (*envelope)[index++] = (Byte) bodyLength;

    // Device identities TLV
    (*envelope)[index++] = (Byte) (0x80 |
            Elastos::Droid::Internal::Telephony::Cat::ComprehensionTlvTag_DEVICE_IDENTITIES);
    (*envelope)[index++] = (Byte) 2;
    (*envelope)[index++] = (Byte) DEV_ID_NETWORK;
    (*envelope)[index++] = (Byte) DEV_ID_UICC;

    // Address TLV (if present). Encoded length is assumed to be < 127 bytes.
    if (scAddressLength != 0) {
        (*envelope)[index++] = (Byte)
            Elastos::Droid::Internal::Telephony::Cat::ComprehensionTlvTag_ADDRESS;
        (*envelope)[index++] = (Byte) scAddressLength;
        envelope->Copy(index, pdu, 1, scAddressLength);
        index += scAddressLength;
    }

    // SMS TPDU TLV. Length is assumed to be < 256 bytes.
    (*envelope)[index++] = (Byte) (0x80 |
            Elastos::Droid::Internal::Telephony::Cat::ComprehensionTlvTag_SMS_TPDU);
    if (tpduLength > 127) {
        (*envelope)[index++] = (Byte) 0x81;    // length 128-255 encoded as 0x81 + length
    }
    (*envelope)[index++] = (Byte) tpduLength;
    envelope->Copy(index, pdu, tpduIndex, tpduLength);
    index += tpduLength;

    // Verify that we calculated the payload size correctly.
    if (index != envelope->GetLength()) {
        Logger::E(TAG, "startDataDownload() calculated incorrect envelope length, aborting.");
        AcknowledgeSmsWithError(ICommandsInterface::GSM_SMS_FAIL_CAUSE_UNSPECIFIED_ERROR);
    }

    String encodedEnvelope = IccUtils::BytesToHexString(envelope);
    AutoPtr<IArrayOf> iArray;
    CArrayOf::New(EIID_IInteger32, 2, (IArrayOf**)&iArray);
    iArray->Set(0, CoreUtils::Convert(dcs));
    iArray->Set(1, CoreUtils::Convert(pid));

    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_SEND_ENVELOPE_RESPONSE, iArray, (IMessage**)&msg);
    mCi->SendEnvelopeWithStatus(encodedEnvelope, msg);
}

Int32 CUsimDataDownloadHandler::GetEnvelopeBodyLength(
    /* [in] */ Int32 scAddressLength,
    /* [in] */ Int32 tpduLength)
{
    // Add 4 bytes for device identities TLV + 1 byte for SMS TPDU tag byte
    Int32 length = tpduLength + 5;
    // Add 1 byte for TPDU length, or 2 bytes if length > 127
    length += (tpduLength > 127 ? 2 : 1);
    // Add length of address tag, if present (+ 2 bytes for tag and length)
    if (scAddressLength != 0) {
        length = length + 2 + scAddressLength;
    }
    return length;
}

void CUsimDataDownloadHandler::SendSmsAckForEnvelopeResponse(
    /* [in] */ IIccIoResult* response,
    /* [in] */ Int32 dcs,
    /* [in] */ Int32 pid)
{
    Int32 sw1 = ((IccIoResult*)response)->mSw1;
    Int32 sw2 = ((IccIoResult*)response)->mSw2;

    Boolean success;
    String str;
    if ((sw1 == 0x90 && sw2 == 0x00) || sw1 == 0x91) {
        Logger::D(TAG, "USIM data download succeeded: %s",
                (IObject::Probe(response)->ToString(&str), str.string()));
        success = TRUE;
    }
    else if (sw1 == 0x93 && sw2 == 0x00) {
        Logger::E(TAG, "USIM data download failed: Toolkit busy");
        AcknowledgeSmsWithError(ICommandsInterface::GSM_SMS_FAIL_CAUSE_USIM_APP_TOOLKIT_BUSY);
        return;
    }
    else if (sw1 == 0x62 || sw1 == 0x63) {
        Logger::E(TAG, "USIM data download failed: %s",
                (IObject::Probe(response)->ToString(&str), str.string()));
        success = FALSE;
    }
    else {
        Logger::E(TAG, "Unexpected SW1/SW2 response from UICC: %s",
                (IObject::Probe(response)->ToString(&str), str.string()));
        success = FALSE;
    }

    AutoPtr<ArrayOf<Byte> > responseBytes = ((IccIoResult*)response)->mPayload;
    if (responseBytes == NULL || responseBytes->GetLength() == 0) {
        if (success) {
            mCi->AcknowledgeLastIncomingGsmSms(TRUE, 0, NULL);
        }
        else {
            AcknowledgeSmsWithError(
                    ICommandsInterface::GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR);
        }
        return;
    }

    AutoPtr<ArrayOf<Byte> > smsAckPdu;
    Int32 index = 0;
    if (success) {
        smsAckPdu = ArrayOf<Byte>::Alloc(responseBytes->GetLength() + 5);
        (*smsAckPdu)[index++] = 0x00;      // TP-MTI, TP-UDHI
        (*smsAckPdu)[index++] = 0x07;      // TP-PI: TP-PID, TP-DCS, TP-UDL present
    }
    else {
        smsAckPdu = ArrayOf<Byte>::Alloc(responseBytes->GetLength() + 6);
        (*smsAckPdu)[index++] = 0x00;      // TP-MTI, TP-UDHI
        (*smsAckPdu)[index++] = (Byte)
                ICommandsInterface::GSM_SMS_FAIL_CAUSE_USIM_DATA_DOWNLOAD_ERROR;  // TP-FCS
        (*smsAckPdu)[index++] = 0x07;      // TP-PI: TP-PID, TP-DCS, TP-UDL present
    }

    (*smsAckPdu)[index++] = (Byte) pid;
    (*smsAckPdu)[index++] = (Byte) dcs;

    if (Is7bitDcs(dcs)) {
        Int32 septetCount = responseBytes->GetLength() * 8 / 7;
        (*smsAckPdu)[index++] = (Byte) septetCount;
    }
    else {
        (*smsAckPdu)[index++] = (Byte) responseBytes->GetLength();
    }

    smsAckPdu->Copy(index, responseBytes, 0, responseBytes->GetLength());

    mCi->AcknowledgeIncomingGsmSmsWithPdu(success,
            IccUtils::BytesToHexString(smsAckPdu), NULL);
}

void CUsimDataDownloadHandler::AcknowledgeSmsWithError(
    /* [in] */ Int32 cause)
{
    mCi->AcknowledgeLastIncomingGsmSms(FALSE, cause, NULL);
}

Boolean CUsimDataDownloadHandler::Is7bitDcs(
    /* [in] */ Int32 dcs)
{
    // See 3GPP TS 23.038 section 4
    return ((dcs & 0x8C) == 0x00) || ((dcs & 0xF4) == 0xF0);
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos