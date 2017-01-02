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
#include "elastos/droid/google/mms/pdu/GenericPdu.h"
#include "elastos/droid/google/mms/pdu/PduHeaders.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Pdu {

//=====================================================================
//                              GenericPdu
//=====================================================================
CAR_INTERFACE_IMPL(GenericPdu, Object, IGenericPdu);

GenericPdu::GenericPdu()
{
    constructor();
}

ECode GenericPdu::constructor()
{
    mPduHeaders = new PduHeaders();
    return NOERROR;
}

ECode GenericPdu::constructor(
    /* [in] */ IPduHeaders* headers)
{
    mPduHeaders = headers;
    return NOERROR;
}

ECode GenericPdu::GetPduHeaders(
    /* [out] */ IPduHeaders** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPduHeaders;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode GenericPdu::GetMessageType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::MESSAGE_TYPE, result);
}

ECode GenericPdu::SetMessageType(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::MESSAGE_TYPE);
}

ECode GenericPdu::GetMmsVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetOctet(IPduHeaders::MMS_VERSION, result);
}

ECode GenericPdu::SetMmsVersion(
    /* [in] */ Int32 value)
{
    return mPduHeaders->SetOctet(value, IPduHeaders::MMS_VERSION);
}

ECode GenericPdu::GetFrom(
    /* [out] */ IEncodedStringValue** result)
{
    VALIDATE_NOT_NULL(result);
    return mPduHeaders->GetEncodedStringValue(IPduHeaders::FROM, result);
}

ECode GenericPdu::SetFrom(
    /* [in] */ IEncodedStringValue* value)
{
    VALIDATE_NOT_NULL(value);
    return mPduHeaders->SetEncodedStringValue(value, IPduHeaders::FROM);
}

} // namespace Pdu
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
