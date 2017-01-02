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

#include <elastos/core/IntegralToString.h>
#include "elastos/droid/internal/telephony/CSmsResponse.h"

using Elastos::Core::IntegralToString;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Object returned by the RIL upon successful completion of sendSMS.
 * Contains message reference and ackPdu.
 *
 */
CAR_OBJECT_IMPL(CSmsResponse)

CAR_INTERFACE_IMPL(CSmsResponse, Object, ISmsResponse)

ECode CSmsResponse::constructor(
    /* [in] */ Int32 messageRef,
    /* [in] */ const String& ackPdu,
    /* [in] */ Int32 errorCode)
{
    mMessageRef = messageRef;
    mAckPdu = ackPdu;
    mErrorCode = errorCode;
    return NOERROR;
}

//@Override
ECode CSmsResponse::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    String ret = String("{ mMessageRef = ") + IntegralToString::ToString(mMessageRef)
        + String(", mErrorCode = ") + IntegralToString::ToString(mErrorCode)
        + String(", mAckPdu = ") + mAckPdu
        + String("}");
    *str = ret;
    return NOERROR;
}

} //namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
