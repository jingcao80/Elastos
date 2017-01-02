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

#include "elastos/droid/os/CBatteryProperty.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL_2(CBatteryProperty, Object, IBatteryProperty, IParcelable)

CAR_OBJECT_IMPL(CBatteryProperty)

CBatteryProperty::CBatteryProperty()
    : mValueLong(0)
{}

ECode CBatteryProperty::constructor()
{
    mValueLong = Elastos::Core::Math::INT64_MIN_VALUE;
    return NOERROR;
}

ECode CBatteryProperty::GetInt64(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mValueLong;
    return NOERROR;
}

ECode CBatteryProperty::WriteToParcel(
    /* [in] */ IParcel* p)
{
    p->WriteInt64(mValueLong);
    return NOERROR;
}

ECode CBatteryProperty::ReadFromParcel(
    /* [in] */ IParcel* p)
{
    p->ReadInt64(&mValueLong);
    return NOERROR;
}

ECode CBatteryProperty::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CBatteryProperty{0x");
    sb += StringUtils::ToHexString((Int32)this);
    sb += ", mValueLong=";
    sb += mValueLong;

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos