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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/ResultException.h"

#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

//=====================================================================
//                           ResultException
//=====================================================================
CAR_INTERFACE_IMPL(ResultException, Object, IResultException);

ResultException::ResultException()
{
}

ECode ResultException::constructor(
    /* [in] */ ResultCode result)
{
    // super();

    // ETSI TS 102 223, 8.12 -- For the general results '20', '21', '26',
    // '38', '39', '3A', '3C', and '3D', it is mandatory for the terminal
    // to provide a specific cause value as additional information.
    switch (result) {
        case ResultCode_TERMINAL_CRNTLY_UNABLE_TO_PROCESS:    // 0x20
        case ResultCode_NETWORK_CRNTLY_UNABLE_TO_PROCESS:     // 0x21
        case ResultCode_LAUNCH_BROWSER_ERROR:                 // 0x26
        case ResultCode_MULTI_CARDS_CMD_ERROR:                // 0x38
        case ResultCode_USIM_CALL_CONTROL_PERMANENT:          // 0x39
        case ResultCode_BIP_ERROR:                            // 0x3a
        case ResultCode_FRAMES_ERROR:                         // 0x3c
        case ResultCode_MMS_ERROR:                            // 0x3d
            // throw new AssertionError(
            //         "For result code, " + result +
            //         ", additional information must be given!");
            return E_ASSERTION_ERROR;
        default:
            break;
    }

    mResult = result;
    mAdditionalInfo = -1;
    mExplanation = "";
    return NOERROR;
}

ECode ResultException::constructor(
    /* [in] */ ResultCode result,
    /* [in] */ const String& explanation)
{
    constructor(result);
    mExplanation = explanation;
    return NOERROR;
}

ECode ResultException::constructor(
    /* [in] */ ResultCode result,
    /* [in] */ Int32 additionalInfo)
{
    constructor(result);

    if (additionalInfo < 0) {
        // throw new AssertionError(
        //         "Additional info must be greater than zero!");
        return E_ASSERTION_ERROR;
    }

    mAdditionalInfo = additionalInfo;
    return NOERROR;
}

ECode ResultException::constructor(
    /* [in] */ ResultCode result,
    /* [in] */ Int32 additionalInfo,
    /* [in] */ const String& explanation)
{
    constructor(result, additionalInfo);
    mExplanation = explanation;
    return NOERROR;
}

ECode ResultException::Result(
    /* [out] */ ResultCode* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResult;
    return NOERROR;
}

ECode ResultException::HasAdditionalInfo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdditionalInfo >= 0;
    return NOERROR;
}

ECode ResultException::AdditionalInfo(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAdditionalInfo;
    return NOERROR;
}

ECode ResultException::Explanation(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExplanation;
    return NOERROR;
}

ECode ResultException::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String("result=") + StringUtils::ToString(mResult)
            + String(" additionalInfo=") + StringUtils::ToString(mAdditionalInfo)
            + String(" explantion=") + mExplanation;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
