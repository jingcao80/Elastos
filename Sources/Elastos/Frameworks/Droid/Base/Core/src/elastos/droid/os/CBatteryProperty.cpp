
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