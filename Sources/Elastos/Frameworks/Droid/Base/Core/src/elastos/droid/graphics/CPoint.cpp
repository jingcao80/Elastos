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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CPoint);
CAR_INTERFACE_IMPL_2(CPoint, Object, IPoint, IParcelable);
ECode CPoint::constructor()
{
    mX = 0;
    mY = 0;
    return NOERROR;
}

ECode CPoint::constructor(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mX = x;
    mY = y;
    return NOERROR;
}

ECode CPoint::constructor(
    /* [in] */ IPoint* src)
{
    mX = ((CPoint*)src)->mX;
    mY = ((CPoint*)src)->mY;
    return NOERROR;
}

ECode CPoint::Set(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    mX = x;
    mY = y;
    return NOERROR;
}

ECode CPoint::Get(
    /* [out] */ Int32* x,
    /* [out] */ Int32* y)
{
    if (x != NULL) *x = mX;
    if (y != NULL) *y = mY;
    return NOERROR;
}

ECode CPoint::Negate()
{
    mX = -mX;
    mY = -mY;
    return NOERROR;
}

ECode CPoint::Offset(
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    mX += dx;
    mY += dy;
    return NOERROR;
}

ECode CPoint::Equals(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mX == x && mY == y;
    return NOERROR;
}

ECode CPoint::Equals(
    /* [in] */ IPoint* p,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 x, y;
    p->Get(&x, &y);
    return Equals(x, y, result);
}

ECode CPoint::Equals(
    /* [in] */ IInterface* p,
    /* [out] */ Boolean* result)
{
    return Equals(IPoint::Probe(p), result);
}

ECode CPoint::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);

    Int32 result = mX;
    result = 31 * result + mY;
    *hash = result;
    return NOERROR;
}

ECode CPoint::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb(28);
    sb += "Point(";
    sb += mX;
    sb += ", ";
    sb += mY;
    sb += ")";
    sb.ToString(result);
    return NOERROR;
}

ECode CPoint::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mX);
    source->ReadInt32(&mY);
    return NOERROR;
}

ECode CPoint::WriteToParcel(
    /* [out] */ IParcel* dest)
{
    dest->WriteInt32(mX);
    dest->WriteInt32(mY);
    return NOERROR;
}

ECode CPoint::GetX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mX;
    return NOERROR;
}

ECode CPoint::GetY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mY;
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
