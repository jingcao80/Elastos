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

#include "elastos/droid/internal/telephony/gsm/CSsData.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Gsm {

CAR_INTERFACE_IMPL(CSsData, Object, ISsData)

CAR_OBJECT_IMPL(CSsData)

CSsData::CSsData()
    : mServiceClass(0)
    , mResult(0)
{
}

ECode CSsData::constructor()
{
    return NOERROR;
}

ECode CSsData::ServiceTypeFromRILInt(
    /* [in] */ Int32 type,
    /* [out] */ ServiceType* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    assert(0);
    // return ServiceType.values()[type];
    // } catch (IndexOutOfBoundsException e) {
    //     Rlog.e(GSMPhone.LOG_TAG, "Invalid Service type");
    //     return null;
    // }
    *result = 0;
    return NOERROR;
}

ECode CSsData::RequestTypeFromRILInt(
    /* [in] */ Int32 type,
    /* [out] */ RequestType* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    assert(0);
    // return RequestType.values()[type];
    // } catch (IndexOutOfBoundsException e) {
    //     Rlog.e(GSMPhone.LOG_TAG, "Invalid Request type");
    //     return null;
    // }
    *result = 0;
    return NOERROR;
}

ECode CSsData::TeleserviceTypeFromRILInt(
    /* [in] */ Int32 type,
    /* [out] */ TeleserviceType* result)
{
    VALIDATE_NOT_NULL(result)
    // try {
    assert(0);
    // return TeleserviceType.values()[type];
    // } catch (IndexOutOfBoundsException e) {
    //     Rlog.e(GSMPhone.LOG_TAG, "Invalid Teleservice type");
    //     return null;
    // }
    *result = 0;
    return NOERROR;
}

ECode CSsData::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("[SsData] ServiceType: ") + StringUtils::ToString(mServiceType)
        + " RequestType: " + StringUtils::ToString(mRequestType)
        + " TeleserviceType: " + StringUtils::ToString(mTeleserviceType)
        + " ServiceClass: " + StringUtils::ToString(mServiceClass)
        + " Result: " + StringUtils::ToString(mResult);
// TODO: Need ServiceType::isTypeCF
        // + " Is Service Type CF: " + StringUtils::ToString(mServiceType).isTypeCF();
    return NOERROR;
}

} // namespace Gsm
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos