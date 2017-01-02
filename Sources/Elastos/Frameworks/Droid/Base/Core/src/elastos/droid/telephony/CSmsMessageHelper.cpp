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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/telephony/CSmsMessageHelper.h"
#include "elastos/droid/telephony/SmsMessage.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

CAR_INTERFACE_IMPL(CSmsMessageHelper, Singleton, ISmsMessageHelper)
CAR_SINGLETON_IMPL(CSmsMessageHelper)

ECode CSmsMessageHelper::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessage> smsMessage = SmsMessage::CreateFromPdu(pdu);
    *result = smsMessage;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [in] */ const String& format,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessage> smsMessage = SmsMessage::CreateFromPdu(pdu, format);
    *result = smsMessage;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::NewFromCMT(
    /* [in] */ ArrayOf<String>* lines,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessage> smsMessage = SmsMessage::NewFromCMT(lines);
    *result = smsMessage;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::NewFromParcel(
    /* [in] */ IParcel* p,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessage> smsMessage = SmsMessage::NewFromParcel(p);
    *result = smsMessage;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessage> smsMessage = SmsMessage::CreateFromEfRecord(index, data);
    *result = smsMessage;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::CreateFromEfRecord(
    /* [in] */ Int32 index,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int64 subId,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessage> smsMessage = SmsMessage::CreateFromEfRecord(index, data, subId);
    *result = smsMessage;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SmsMessage::GetTPLayerLengthForPDU(pdu);
    return NOERROR;
}

ECode CSmsMessageHelper::CalculateLength(
    /* [in] */ ICharSequence* msgBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Int32> > lengthArray = SmsMessage::CalculateLength(msgBody, use7bitOnly);
    *result = lengthArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::FragmentText(
    /* [in] */ const String& text,
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayList> arrayList = SmsMessage::FragmentText(text);
    *result = arrayList;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::CalculateLength(
    /* [in] */ const String& messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Int32> > lenArray = SmsMessage::CalculateLength(messageBody, use7bitOnly);
    *result = lenArray;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessageSubmitPdu> pdu = SmsMessage::GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested);
    *result = pdu;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ Int64 subId,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessageSubmitPdu> pdu = SmsMessage::GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested, subId);
    *result = pdu;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

CARAPI CSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISmsMessageSubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ISmsMessageSubmitPdu> pdu = SmsMessage::GetSubmitPdu(scAddress, destinationAddress, destinationPort, data, statusReportRequested);
    *result = pdu;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


ECode CSmsMessageHelper::HasEmsSupport(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SmsMessage::HasEmsSupport();
    return NOERROR;
}

ECode CSmsMessageHelper::ShouldAppendPageNumberAsPrefix(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SmsMessage::ShouldAppendPageNumberAsPrefix();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
