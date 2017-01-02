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

#include "org/apache/harmony/security/x509/tsp/PKIFailureInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {
namespace Tsp {

Int32 PKIFailureInfo::mMaxValue = 0;

CAR_INTERFACE_IMPL(PKIFailureInfo, Object, IPKIFailureInfo)

PKIFailureInfo::PKIFailureInfo(
    /* [in] */ Int32 value)
    : mValue(value)
{
}

ECode PKIFailureInfo::GetValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mValue;
    return NOERROR;
}

ECode PKIFailureInfo::GetMaxValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (mMaxValue == 0) {
        *value = IPKIFailureInfo::PKIFailureInfo_SYSTEM_FAILURE;
    }
    *value =  mMaxValue;
    return NOERROR;
}

ECode PKIFailureInfo::GetInstance(
    /* [in] */ Int32 value,
    /* [out] */ IPKIFailureInfo** instance)
{
    VALIDATE_NOT_NULL(instance);

    AutoPtr<IPKIFailureInfo> tmp;
    switch (value) {
        case PKIFailureInfo_BAD_ALG :
            tmp = new PKIFailureInfo(PKIFailureInfo_BAD_ALG);
            break;
        case PKIFailureInfo_BAD_REQUEST :
            tmp = new PKIFailureInfo(PKIFailureInfo_BAD_REQUEST);
            break;
        case PKIFailureInfo_BAD_DATA_FORMAT :
            tmp = new PKIFailureInfo(PKIFailureInfo_BAD_DATA_FORMAT);
            break;
        case PKIFailureInfo_TIME_NOT_AVAILABLE :
            tmp = new PKIFailureInfo(PKIFailureInfo_TIME_NOT_AVAILABLE);
            break;
        case PKIFailureInfo_UNACCEPTED_POLICY :
            tmp = new PKIFailureInfo(PKIFailureInfo_UNACCEPTED_POLICY);
            break;
        case PKIFailureInfo_UNACCEPTED_EXTENSION :
            tmp = new PKIFailureInfo(PKIFailureInfo_UNACCEPTED_EXTENSION);
            break;
        case PKIFailureInfo_ADD_INFO_NOT_AVAILABLE :
            tmp = new PKIFailureInfo(PKIFailureInfo_ADD_INFO_NOT_AVAILABLE);
            break;
        case PKIFailureInfo_SYSTEM_FAILURE :
            tmp = new PKIFailureInfo(PKIFailureInfo_SYSTEM_FAILURE);
            break;
        default: {
            *instance = NULL;
            return E_INVALID_PARAMETER_EXCEPTION;
        }
    }
    *instance = tmp;
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

} // namespace Tsp
} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org