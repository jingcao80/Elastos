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

#include "elastos/droid/ims/CImsCallForwardInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL_2(CImsCallForwardInfo, Object, IImsCallForwardInfo, IParcelable)

CAR_OBJECT_IMPL(CImsCallForwardInfo)

CImsCallForwardInfo::CImsCallForwardInfo()
    : mCondition(0)
    , mStatus(0)
    , mToA(0)
    , mTimeSeconds(0)
{
}

CImsCallForwardInfo::~CImsCallForwardInfo()
{
}

ECode CImsCallForwardInfo::constructor()
{
    return NOERROR;
}

ECode CImsCallForwardInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mCondition);
    source->ReadInt32(&mStatus);
    source->ReadInt32(&mToA);
    source->ReadString(&mNumber);
    source->ReadInt32(&mTimeSeconds);
    return NOERROR;
}

ECode CImsCallForwardInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mCondition);
    dest->WriteInt32(mStatus);
    dest->WriteInt32(mToA);
    dest->WriteString(mNumber);
    dest->WriteInt32(mTimeSeconds);
    return NOERROR;
}

ECode CImsCallForwardInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    sb.Append("{ Condition=");
    sb.Append(mCondition);
    sb.Append(", Status=");
    sb.Append((mStatus == 0) ? "disabled" : "enabled");
    sb.Append(", ToA=");
    sb.Append(mToA);
    sb.Append(", Number=");
    sb.Append(mNumber);
    sb.Append(", Time (seconds)=");
    sb.Append(mTimeSeconds);
    sb.Append(" }");

    return sb.ToString(result);
}

} // namespace Ims
} // namespace Droid
} // namespace Elastos