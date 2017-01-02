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

#include "elastos/droid/utility/CSize.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Utility {

CAR_INTERFACE_IMPL(CSize, Object, ISize)

CAR_OBJECT_IMPL(CSize)

CSize::CSize()
    : mWidth(0)
    , mHeight(0)
{
}

ECode CSize::constructor(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    mWidth = w;
    mHeight = h;
    return NOERROR;
}

ECode CSize::GetWidth(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mWidth;
    return NOERROR;
}

ECode CSize::GetHeight(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mHeight;
    return NOERROR;
}

ECode CSize::Equals(
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
        CSize* other = (CSize*)ISize::Probe(obj);
        *value = mWidth == other->mWidth && mHeight == other->mHeight;
    }
    return NOERROR;
}

ECode CSize::ToString(
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

ECode CSize::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    // assuming most sizes are <2^16, doing a rotate will give us perfect hashing
    *value = mHeight ^ ((mWidth << (Elastos::Core::Math::INT32_SIZE / 2))
        | (((UInt32)mWidth) >> (Elastos::Core::Math::INT32_SIZE / 2)));
    return NOERROR;
}

ECode CSize::ParseSize(
    /* [in] */ const String& string,
    /* [out] */ ISize** size)
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
    Int32 w, h;
    FAIL_RETURN(StringUtils::Parse(string.Substring(0, sep_ix), &w))
    FAIL_RETURN(StringUtils::Parse(string.Substring(sep_ix + 1), &h))

    AutoPtr<ISize> s;
    CSize::New(w, h, (ISize**)&s);
    *size = s;
    REFCOUNT_ADD(*size)
    // } catch (NumberFormatException e) {
    //     throw invalidSize(string);
    // }
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos

