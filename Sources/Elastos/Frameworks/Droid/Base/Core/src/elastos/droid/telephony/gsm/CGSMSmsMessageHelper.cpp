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

#include "elastos/droid/telephony/gsm/CGSMSmsMessageHelper.h"

namespace Elastos {
namespace Droid {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CGSMSmsMessageHelper, Singleton, ISmsMessageHelper)

CAR_SINGLETON_IMPL(CGSMSmsMessageHelper)

ECode CGSMSmsMessageHelper::CreateFromPdu(
    /* [in] */ ArrayOf<Byte>* pdu,
    /* [out] */ ISmsMessage** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<GSMSmsMessage> sm = GSMSmsMessage::CreateFromPdu(pdu);
    *result = sm;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::GetTPLayerLengthForPDU(
    /* [in] */ const String& pdu,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = GSMSmsMessage::GetTPLayerLengthForPDU(pdu);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::CalculateLength(
    /* [in] */ ICharSequence* messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Int32> > cl = GSMSmsMessage::CalculateLength(messageBody, use7bitOnly);
    *result = cl;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::CalculateLength(
    /* [in] */ const String& messageBody,
    /* [in] */ Boolean use7bitOnly,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<ArrayOf<Int32> > cl = GSMSmsMessage::CalculateLength(messageBody, use7bitOnly);
    *result = cl;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [in] */ ArrayOf<Byte>* header,
    /* [out] */ ISubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<GSMSmsMessage::SubmitPdu> sp = GSMSmsMessage::GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested, header);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}
ECode CGSMSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ const String& message,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<GSMSmsMessage::SubmitPdu> sp = GSMSmsMessage::GetSubmitPdu(scAddress, destinationAddress, message, statusReportRequested);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGSMSmsMessageHelper::GetSubmitPdu(
    /* [in] */ const String& scAddress,
    /* [in] */ const String& destinationAddress,
    /* [in] */ Int16 destinationPort,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Boolean statusReportRequested,
    /* [out] */ ISubmitPdu** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<GSMSmsMessage::SubmitPdu> sp = GSMSmsMessage::GetSubmitPdu(scAddress, destinationAddress, destinationPort, data, statusReportRequested);
    *result = sp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


} // namespace Gsm
} // namespace Telephony
} // namespace Droid
} // namespace Elastos
