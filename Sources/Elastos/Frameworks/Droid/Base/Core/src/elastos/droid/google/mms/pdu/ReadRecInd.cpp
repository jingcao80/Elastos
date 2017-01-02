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
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/google/mms/pdu/ReadRecInd.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                              ReadRecInd
//=====================================================================
CAR_INTERFACE_IMPL(ReadRecInd, GenericPdu, IReadRecInd);

ReadRecInd::ReadRecInd()
{
}

ECode ReadRecInd::constructor(
    /* [in] */ IEncodedStringValue* from,
    /* [in] */ ArrayOf<Byte>* messageId,
    /* [in] */ Int32 mmsVersion,
    /* [in] */ Int32 readStatus,
    /* [in] */ ArrayOf<IEncodedStringValue*>* to)
{
    GenericPdu::constructor();
    SetMessageType(IPduHeaders::MESSAGE_TYPE_READ_REC_IND);
    SetFrom(from);
    SetMessageId(messageId);
    SetMmsVersion(mmsVersion);
    SetTo(to);
    SetReadStatus(readStatus);
    return NOERROR;
}

ECode ReadRecInd::constructor(
    /* [in] */ IPduHeaders* headers)
{
    GenericPdu::constructor(headers);
    return NOERROR;
}

ECode ReadRecInd::GetDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetLongInteger(IPduHeaders::DATE, result);
}

ECode ReadRecInd::SetDate(
    /* [in] */ Int64 value)
{
    mPduHeaders->SetLongInteger(value, IPduHeaders::DATE);
    return NOERROR;
}

ECode ReadRecInd::GetMessageId(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetTextString(IPduHeaders::MESSAGE_ID, result);
}

ECode ReadRecInd::SetMessageId(
    /* [in] */ ArrayOf<Byte>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetTextString(value, IPduHeaders::MESSAGE_ID);
    return NOERROR;
}

ECode ReadRecInd::GetTo(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValues(IPduHeaders::TO, result);
}

ECode ReadRecInd::SetTo(
    /* [in] */ ArrayOf<IEncodedStringValue*>* value)
{
    VALIDATE_NOT_NULL(value);
    mPduHeaders->SetEncodedStringValues(value, IPduHeaders::TO);
    return NOERROR;
}

ECode ReadRecInd::GetReadStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::READ_STATUS, result);
}

ECode ReadRecInd::SetReadStatus(
    /* [in] */ Int32 value)
{
    mPduHeaders->SetOctet(value, IPduHeaders::READ_STATUS);
    return NOERROR;
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
