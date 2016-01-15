
#include "elastos/droid/utility/CSizeF.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CSizeF, Object, ISizeF)

CAR_OBJECT_IMPL(CSizeF)

CSizeF::CSizeF()
    : mWidth(0)
    , mHeight(0)
{
}

static ECode CheckArgumentFinite(Float value)
{
    if (Elastos::Core::Math::IsNaN(value)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (Elastos::Core::Math::IsInfinite(value)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CSizeF::constructor(
    /* [in] */ Float w,
    /* [in] */ Float h)
{
    FAIL_RETURN(CheckArgumentFinite(w))
    FAIL_RETURN(CheckArgumentFinite(h))
    mWidth = w;
    mHeight = h;
    return NOERROR;
}

ECode CSizeF::GetWidth(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mWidth;
    return NOERROR;
}

ECode CSizeF::GetHeight(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mHeight;
    return NOERROR;
}

ECode CSizeF::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    if (obj == NULL) {
        return NOERROR;
    }
    if (TO_IINTERFACE(this) == obj) {
        *value = TRUE;
        return NOERROR;
    }

    if (ISize::Probe(obj) != NULL) {
        CSizeF* other = (CSizeF*)ISize::Probe(obj);
        *value = mWidth == other->mWidth && mHeight == other->mHeight;
    }
    return NOERROR;
}

ECode CSizeF::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    sb.Append(mWidth);
    sb.Append("x");
    sb.Append(mHeight);
    *str = sb.ToString();
    return NOERROR;
}

ECode CSizeF::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = Elastos::Core::Math::FloatToInt32Bits(mWidth)
        ^ Elastos::Core::Math::FloatToInt32Bits(mHeight);
    return NOERROR;
}

ECode CSizeF::ParseSizeF(
    /* [in] */ const String& string,
    /* [out] */ ISizeF** size)
{
    VALIDATE_NOT_NULL(size)
    *size = NULL;

    if (string.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //checkNotNull(string, "string must not be null");

    Int32 sep_ix = string.IndexOf('*');
    if (sep_ix < 0) {
        sep_ix = string.IndexOf('x');
    }
    if (sep_ix < 0) {
        // throw invalidSize(string);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    // try {
    Float w, h;
    FAIL_RETURN(StringUtils::Parse(string.Substring(0, sep_ix), &w))
    FAIL_RETURN(StringUtils::Parse(string.Substring(sep_ix + 1), &h))

    AutoPtr<CSizeF> s;
    CSizeF::NewByFriend(w, h, (CSizeF**)&s);
    *size = (ISizeF*)s.Get();
    REFCOUNT_ADD(*size)
    // } catch (NumberFormatException e) {
    //     throw invalidSize(string);
    // }
    return NOERROR;
}


} // namespace Utility
} // namespace Droid
} // namespace Elastos

