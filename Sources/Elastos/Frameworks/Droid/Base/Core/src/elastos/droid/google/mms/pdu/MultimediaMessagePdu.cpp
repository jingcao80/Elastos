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
#include "elastos/droid/google/mms/pdu/MultimediaMessagePdu.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                         MultimediaMessagePdu
//=====================================================================
CAR_INTERFACE_IMPL(MultimediaMessagePdu, GenericPdu, IMultimediaMessagePdu);

MultimediaMessagePdu::MultimediaMessagePdu()
{
    constructor();
}

ECode MultimediaMessagePdu::constructor()
{
    return GenericPdu::constructor();
}

ECode MultimediaMessagePdu::constructor(
    /* [in] */ IPduHeaders* header,
    /* [in] */ IPduBody* body)
{
    GenericPdu::constructor(header);
    mMessageBody = body;
    return NOERROR;
}

ECode MultimediaMessagePdu::constructor(
    /* [in] */ IPduHeaders* headers)
{
    GenericPdu::constructor(headers);
    return NOERROR;
}

ECode MultimediaMessagePdu::GetBody(
    /* [out] */ IPduBody** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMessageBody;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode MultimediaMessagePdu::SetBody(
    /* [in] */ IPduBody* body)
{
    VALIDATE_NOT_NULL(body);
    mMessageBody = body;
    return NOERROR;
}

ECode MultimediaMessagePdu::GetSubject(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValue(IPduHeaders::SUBJECT, result);
}

ECode MultimediaMessagePdu::SetSubject(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetEncodedStringValue(value, IPduHeaders::SUBJECT);
}

ECode MultimediaMessagePdu::GetTo(
    /* [out] */ ArrayOf<IEncodedStringValue*>** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValues(IPduHeaders::TO, result);
}

ECode MultimediaMessagePdu::AddTo(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->AppendEncodedStringValue(value, IPduHeaders::TO);
}

ECode MultimediaMessagePdu::GetPriority(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::PRIORITY, result);
}

ECode MultimediaMessagePdu::SetPriority(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::PRIORITY);
}

ECode MultimediaMessagePdu::GetDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetLongInteger(IPduHeaders::DATE, result);
}

ECode MultimediaMessagePdu::SetDate(
    /* [in] */ Int64 value)
{
    return mPduHeaders->SetLongInteger(value, IPduHeaders::DATE);
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
