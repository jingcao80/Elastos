
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Utility {

const Float CTypedValue::MANTISSA_MULT = 1.0f / (1 << COMPLEX_MANTISSA_SHIFT);
const Float CTypedValue::RADIX_MULTS[] = {
    1.0f * MANTISSA_MULT, 1.0f / (1 << 7) * MANTISSA_MULT,
    1.0f / (1 << 15) * MANTISSA_MULT, 1.0f / (1 << 23) * MANTISSA_MULT
};
const String CTypedValue::DIMENSION_UNIT_STRS[] = {
    String("px"), String("dip"), String("sp"), String("pt"), String("in"), String("mm")
};
const String CTypedValue::FRACTION_UNIT_STRS[] = {
    String("%"), String("%p")
};

CAR_INTERFACE_IMPL(CTypedValue, Object, ITypedValue)

CAR_OBJECT_IMPL(CTypedValue)

CTypedValue::CTypedValue()
    : mType(0)
    , mData(0)
    , mAssetCookie(0)
    , mResourceId(0)
    , mChangingConfigurations(-1)
    , mDensity(0)
{}

CTypedValue::~CTypedValue()
{}

ECode CTypedValue::GetFloat(
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f);
    *f = Elastos::Core::Math::Int32BitsToFloat(mData);
    return NOERROR;
}

Float CTypedValue::ComplexToFloat(Int32 complex)
{
    return (complex & (COMPLEX_MANTISSA_MASK << COMPLEX_MANTISSA_SHIFT))
            * RADIX_MULTS[(complex >> COMPLEX_RADIX_SHIFT) & COMPLEX_RADIX_MASK];
}

Float CTypedValue::ComplexToDimension(
    /* [in] */ Int32 data,
    /* [in] */ IDisplayMetrics* metrics)
{
    return ApplyDimension(
            (data >> COMPLEX_UNIT_SHIFT) & COMPLEX_UNIT_MASK,
            ComplexToFloat(data),
            metrics);
}

Int32 CTypedValue::ComplexToDimensionPixelOffset(
    /* [in] */ Int32 data,
    /* [in] */ IDisplayMetrics* metrics)
{
    return (Int32)ApplyDimension(
                (data >> COMPLEX_UNIT_SHIFT) & COMPLEX_UNIT_MASK,
                ComplexToFloat(data),
                metrics);
}

Int32 CTypedValue::ComplexToDimensionPixelSize(
    /* [in] */ Int32 data,
    /* [in] */ IDisplayMetrics* metrics)
{
    const Float value = ComplexToFloat(data);
    const Float f = ApplyDimension(
            (data >> COMPLEX_UNIT_SHIFT) & COMPLEX_UNIT_MASK,
            value,
            metrics);
    const Int32 res = (Int32)(f + 0.5f);
    if (res != 0) return res;
    if (value == 0) return 0;
    if (value > 0) return 1;
    return -1;
}

Float CTypedValue::ComplexToDimensionNoisy(
    /* [in] */ Int32 data,
    /* [in] */ IDisplayMetrics* metrics)
{
    return ComplexToDimension(data, metrics);
}

Float CTypedValue::ApplyDimension(
    /* [in] */ Int32 unit,
    /* [in] */ Float value,
    /* [in] */ IDisplayMetrics* metrics)
{
    assert(metrics);

    switch (unit) {
    case COMPLEX_UNIT_PX:
        return value;
    case COMPLEX_UNIT_DIP: {
        Float density;
        metrics->GetDensity(&density);
        return value * density;
    }
    case COMPLEX_UNIT_SP: {
        Float density;
        metrics->GetScaledDensity(&density);
        return value * density;
    }
    case COMPLEX_UNIT_PT: {
        Float xdpi;
        metrics->GetXdpi(&xdpi);
        return value * xdpi * (1.0f / 72);
    }
    case COMPLEX_UNIT_IN: {
        Float xdpi;
        metrics->GetXdpi(&xdpi);
        return value * xdpi;
    }
    case COMPLEX_UNIT_MM: {
        Float xdpi;
        metrics->GetXdpi(&xdpi);
        return value * xdpi * (1.0f / 25.4f);
    }
    }
    return 0;
}

ECode CTypedValue::GetDimension(
    /* [in] */ IDisplayMetrics* metrics,
    /* [out] */ Float* dimension)
{
    VALIDATE_NOT_NULL(dimension);
    *dimension = ComplexToDimension(mData, metrics);
    return NOERROR;
}

Float CTypedValue::ComplexToFraction(
    /* [in] */ Int32 data,
    /* [in] */ Float base,
    /* [in] */ Float pbase)
{
    switch ((data >> COMPLEX_UNIT_SHIFT) & COMPLEX_UNIT_MASK) {
    case COMPLEX_UNIT_FRACTION:
        return ComplexToFloat(data) * base;
    case COMPLEX_UNIT_FRACTION_PARENT:
        return ComplexToFloat(data) * pbase;
    }
    return 0;
}

ECode CTypedValue::GetFraction(
    /* [in] */ Float base,
    /* [in] */ Float pbase,
    /* [out] */ Float* fraction)
{
    VALIDATE_NOT_NULL(fraction);
    *fraction = ComplexToFraction(mData, base, pbase);
    return NOERROR;
}

ECode CTypedValue::CoerceToString(
    /* [out] */ ICharSequence** csq)
{
    VALIDATE_NOT_NULL(csq);

    Int32 t = mType;
    if (t == TYPE_STRING) {
        *csq = mString.Get();
        REFCOUNT_ADD(*csq);
        return NOERROR;
    }
    String s = CoerceToString(t, mData);
    return CString::New(s, csq);
}

String CTypedValue::CoerceToString(
    /* [in] */ Int32 type,
    /* [in] */ Int32 data)
{
    switch (type) {
        case TYPE_NULL: {
            return String(NULL);
        }
        case TYPE_REFERENCE: {
            StringBuilder sb;
            sb += '@';
            sb += data;
            return sb.ToString();
        }
        case TYPE_ATTRIBUTE: {
            StringBuilder sb;
            sb += '?';
            sb += data;
            return sb.ToString();
        }
        case TYPE_FLOAT: {
            Float f = Elastos::Core::Math::Int32BitsToFloat(data);
            return StringUtils::ToString(f);
        }
        case TYPE_DIMENSION: {
            return StringUtils::ToString(ComplexToFloat(data)) +
                    DIMENSION_UNIT_STRS[(data >> COMPLEX_UNIT_SHIFT) & COMPLEX_UNIT_MASK];
        }
        case TYPE_FRACTION: {
            return StringUtils::ToString(ComplexToFloat(data)  *100) +
                    FRACTION_UNIT_STRS[(data >> COMPLEX_UNIT_SHIFT) & COMPLEX_UNIT_MASK];
        }
        case TYPE_INT_HEX: {
            return String("0x") + StringUtils::ToString(data, 16);
        }
        case TYPE_INT_BOOLEAN: {
            return data != 0 ? String("TRUE") : String("FALSE");
        }
    }

    if (type >= TYPE_FIRST_COLOR_INT && type <= TYPE_LAST_COLOR_INT) {
        return String("#") + StringUtils::ToString(data, 16);
    }
    else if (type >= TYPE_FIRST_INT && type <= TYPE_LAST_INT) {
        return StringUtils::ToString(data);
    }

    return String(NULL);
}

ECode CTypedValue::SetTo(
    /* [in] */ ITypedValue* other)
{
    assert(other != NULL);
    other->GetType(&mType);
    mString = NULL;
    other->GetString((ICharSequence**)&mString);
    other->GetData(&mData);
    other->GetAssetCookie(&mAssetCookie);
    other->GetResourceId(&mResourceId);
    other->GetDensity(&mDensity);
    return NOERROR;
}

ECode CTypedValue::GetDescription(
    /* [out] */ String* str)
{
//        StringBuilder sb = new StringBuilder();
//        sb.append("TypedValue{t=0x").append(Integer.toHexString(type));
//        sb.append("/d=0x").append(Integer.toHexString(data));
//        if (type == TYPE_STRING) {
//            sb.append(" \"").append(string != null ? string : "<null>").append("\"");
//        }
//        if (assetCookie != 0) {
//            sb.append(" a=").append(assetCookie);
//        }
//        if (resourceId != 0) {
//            sb.append(" r=0x").append(Integer.toHexString(resourceId));
//        }
//        sb.append("}");
//        return sb.toString();
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CTypedValue::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CTypedValue::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode CTypedValue::GetString(
    /* [out] */ ICharSequence** str)
{
    VALIDATE_NOT_NULL(str);
    *str = mString;
    REFCOUNT_ADD(*str)
    return NOERROR;
}

ECode CTypedValue::SetString(
    /* [in] */ ICharSequence* string)
{
    mString = string;
    return NOERROR;
}

ECode CTypedValue::GetData(
    /* [out] */ Int32* data)
{
    VALIDATE_NOT_NULL(data);
    *data = mData;
    return NOERROR;
}

ECode CTypedValue::SetData(
    /* [in] */ Int32 data)
{
    mData = data;
    return NOERROR;
}

ECode CTypedValue::GetAssetCookie(
    /* [out] */ Int32* cookie)
{
    VALIDATE_NOT_NULL(cookie);
    *cookie = mAssetCookie;
    return NOERROR;
}

ECode CTypedValue::SetAssetCookie(
    /* [in] */ Int32 cookie)
{
    mAssetCookie = cookie;
    return NOERROR;
}

ECode CTypedValue::GetResourceId(
    /* [out] */ Int32* resId)
{
    VALIDATE_NOT_NULL(resId);
    *resId = mResourceId;
    return NOERROR;
}

ECode CTypedValue::SetResourceId(
    /* [in] */ Int32 resId)
{
    mResourceId = resId;
    return NOERROR;
}

ECode CTypedValue::SetChangingConfigurations(
    /* [in] */ Int32 configurations)
{
    mChangingConfigurations = configurations;
    return NOERROR;
}

ECode CTypedValue::GetDensity(
    /* [out] */ Int32* density)
{
    VALIDATE_NOT_NULL(density);
    *density = mDensity;
    return NOERROR;
}

ECode CTypedValue::SetDensity(
    /* [in] */ Int32 density)
{
    mDensity = density;
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
