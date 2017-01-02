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

#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/pdu/RetrieveConf.h"

#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                             RetrieveConf
//=====================================================================
CAR_INTERFACE_IMPL(RetrieveConf, MultimediaMessagePdu, IRetrieveConf);

RetrieveConf::RetrieveConf()
{
}

ECode RetrieveConf::constructor()
{
    MultimediaMessagePdu::constructor();
    SetMessageType(IPduHeaders::MESSAGE_TYPE_RETRIEVE_CONF);
    SetTransactionId(GenerateTransactionId());
    return NOERROR;
}

ECode RetrieveConf::constructor(
    /* [in] */ IPduHeaders* headers)
{
    MultimediaMessagePdu::constructor(headers);
    SetTransactionId(GenerateTransactionId());
    return NOERROR;
}

ECode RetrieveConf::constructor(
    /* [in] */ IPduHeaders* headers,
    /* [in] */ IPduBody* body)
{
    MultimediaMessagePdu::constructor(headers, body);
    SetTransactionId(GenerateTransactionId());
    return NOERROR;
}

ECode RetrieveConf::GetCc(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValues(IPduHeaders::CC, result);
}

ECode RetrieveConf::AddCc(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->AppendEncodedStringValue(value, IPduHeaders::CC);
    return NOERROR;
}

ECode RetrieveConf::GetContentType(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::CONTENT_TYPE, result);
}

ECode RetrieveConf::SetContentType(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetTextString(value, IPduHeaders::CONTENT_TYPE);
    return NOERROR;
}

ECode RetrieveConf::GetDeliveryReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::DELIVERY_REPORT, result);
}

ECode RetrieveConf::SetDeliveryReport(
    /* [in] */ Int32 value)
{
    mPduHeaders->SetOctet(value, IPduHeaders::DELIVERY_REPORT);
    return NOERROR;
}

ECode RetrieveConf::GetFrom(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValue(IPduHeaders::FROM, result);
}

ECode RetrieveConf::SetFrom(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetEncodedStringValue(value, IPduHeaders::FROM);
    return NOERROR;
}

ECode RetrieveConf::GetMessageClass(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::MESSAGE_CLASS, result);
}

ECode RetrieveConf::SetMessageClass(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetTextString(value, IPduHeaders::MESSAGE_CLASS);
    return NOERROR;
}

ECode RetrieveConf::GetMessageId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::MESSAGE_ID, result);
}

ECode RetrieveConf::SetMessageId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetTextString(value, IPduHeaders::MESSAGE_ID);
    return NOERROR;
}

ECode RetrieveConf::GetReadReport(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::READ_REPORT, result);
}

ECode RetrieveConf::SetReadReport(
    /* [in] */ Int32 value)
{
    mPduHeaders->SetOctet(value, IPduHeaders::READ_REPORT);
    return NOERROR;
}

ECode RetrieveConf::GetRetrieveStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::RETRIEVE_STATUS, result);
}

ECode RetrieveConf::SetRetrieveStatus(
    /* [in] */ Int32 value)
{
    mPduHeaders->SetOctet(value, IPduHeaders::RETRIEVE_STATUS);
    return NOERROR;
}

ECode RetrieveConf::GetRetrieveText(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValue(IPduHeaders::RETRIEVE_TEXT, result);
}

ECode RetrieveConf::SetRetrieveText(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetEncodedStringValue(value, IPduHeaders::RETRIEVE_TEXT);
    return NOERROR;
}

ECode RetrieveConf::GetTransactionId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::TRANSACTION_ID, result);
}

ECode RetrieveConf::SetTransactionId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetTextString(value, IPduHeaders::TRANSACTION_ID);
    return NOERROR;
}

AutoPtr<ArrayOf<Byte> > RetrieveConf::GenerateTransactionId()
{
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 mills = 0;
    sys->GetCurrentTimeMillis(&mills);
    String transactionId(String("T") + StringUtils::ToHexString(mills));
    return transactionId.GetBytes();
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
