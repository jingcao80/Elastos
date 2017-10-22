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

#include "elastos/droid/telephony/SmsCbCmasInfo.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Telephony {

//=====================================================================
//                            SmsCbCmasInfo
//=====================================================================
CAR_INTERFACE_IMPL_2(SmsCbCmasInfo, Object, ISmsCbCmasInfo, IParcelable);

SmsCbCmasInfo::SmsCbCmasInfo()
    : mMessageClass(0)
    , mCategory(0)
    , mResponseType(0)
    , mSeverity(0)
    , mUrgency(0)
    , mCertainty(0)
{
}

ECode SmsCbCmasInfo::constructor(
    /* [in] */ Int32 messageClass,
    /* [in] */ Int32 category,
    /* [in] */ Int32 responseType,
    /* [in] */ Int32 severity,
    /* [in] */ Int32 urgency,
    /* [in] */ Int32 certainty)
{
    mMessageClass = messageClass;
    mCategory = category;
    mResponseType = responseType;
    mSeverity = severity;
    mUrgency = urgency;
    mCertainty = certainty;
    return NOERROR;
}

ECode SmsCbCmasInfo::constructor()
{
    return NOERROR;
}

ECode SmsCbCmasInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteInt32(mMessageClass);
    dest->WriteInt32(mCategory);
    dest->WriteInt32(mResponseType);
    dest->WriteInt32(mSeverity);
    dest->WriteInt32(mUrgency);
    dest->WriteInt32(mCertainty);
    return NOERROR;
}

ECode SmsCbCmasInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mMessageClass);
    source->ReadInt32(&mCategory);
    source->ReadInt32(&mResponseType);
    source->ReadInt32(&mSeverity);
    source->ReadInt32(&mUrgency);
    source->ReadInt32(&mCertainty);
    return NOERROR;
}


ECode SmsCbCmasInfo::GetMessageClass(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMessageClass;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCategory;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetResponseType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResponseType;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetSeverity(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSeverity;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetUrgency(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUrgency;
    return NOERROR;
}

ECode SmsCbCmasInfo::GetCertainty(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCertainty;
    return NOERROR;
}

ECode SmsCbCmasInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb("SmsCbCmasInfo{messageClass=");
    sb += mMessageClass;
    sb += ", category=";
    sb += mCategory;
    sb += ", responseType=";
    sb += mResponseType;
    sb += ", severity=";
    sb += mSeverity;
    sb += ", urgency=";
    sb += mUrgency;
    sb += ", certainty=";
    sb += mCertainty;
    sb += "}";
    return sb.ToString(result);
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
