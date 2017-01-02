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

#include "elastos/droid/hardware/camera2/params/CColorSpaceTransform.h"
#include "elastos/droid/hardware/camera2/utils/HashCodeHelpers.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/utility/CRational.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Utility::CRational;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Hardware::Camera2::Utils::HashCodeHelpers;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

const Int32 CColorSpaceTransform::ROWS = 3;
const Int32 CColorSpaceTransform::COLUMNS = 3;
const Int32 CColorSpaceTransform::COUNT = ROWS * COLUMNS;
const Int32 CColorSpaceTransform::RATIONAL_SIZE = 2;
const Int32 CColorSpaceTransform::OFFSET_NUMERATOR = 0;
const Int32 CColorSpaceTransform::OFFSET_DENOMINATOR = 1;
const Int32 CColorSpaceTransform::COUNT_INT = ROWS * COLUMNS * RATIONAL_SIZE;

CAR_INTERFACE_IMPL(CColorSpaceTransform, Object, IColorSpaceTransform)

CAR_OBJECT_IMPL(CColorSpaceTransform)

CColorSpaceTransform::CColorSpaceTransform()
{
}

CColorSpaceTransform::~CColorSpaceTransform()
{
}

ECode CColorSpaceTransform::constructor()
{
    return NOERROR;
}

ECode CColorSpaceTransform::constructor(
    /* [in] */ ArrayOf<IRational*>* elements)
{
    FAIL_RETURN(Preconditions::CheckNotNull(elements))
    if (elements == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (elements->GetLength() != COUNT) {
        Logger::E("CColorSpaceTransform", "elements must be  %d length", COUNT);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mElements = ArrayOf<Int32>::Alloc(COUNT_INT);

    for (Int32 i = 0; i < elements->GetLength(); ++i) {
        StringBuilder sb;
        sb += "element ";
        sb += i;
        sb += " must not be null";
        FAIL_RETURN(Preconditions::CheckNotNull(TO_IINTERFACE((*elements)[i]), sb.ToString()))

        Int32 numerator;
        (*elements)[i]->GetNumerator(&numerator);
        Int32 denominator;
        (*elements)[i]->GetDenominator(&denominator);
        (*mElements)[i * RATIONAL_SIZE + OFFSET_NUMERATOR] = numerator;
        (*mElements)[i * RATIONAL_SIZE + OFFSET_DENOMINATOR] = denominator;
    }
    return NOERROR;
}

ECode CColorSpaceTransform::constructor(
    /* [in] */ ArrayOf<Int32>* elements)
{
    FAIL_RETURN(Preconditions::CheckNotNull(elements))
    if (elements == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (elements->GetLength() != COUNT_INT) {
        Logger::E("CColorSpaceTransform", "elements must be  %d length", COUNT_INT);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return Arrays::CopyOf(elements, elements->GetLength(), (ArrayOf<Int32>**)&mElements);
}

ECode CColorSpaceTransform::GetElement(
    /* [in] */ Int32 column,
    /* [in] */ Int32 row,
    /* [out] */ IRational** outrat)
{
    VALIDATE_NOT_NULL(outrat)

    if (column < 0 || column >= COLUMNS) {
        Logger::E("CColorSpaceTransform", "column out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (row < 0 || row >= ROWS) {
        Logger::E("CColorSpaceTransform", "row out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 numerator = (*mElements)[(row * COLUMNS + column) * RATIONAL_SIZE + OFFSET_NUMERATOR];
    Int32 denominator = (*mElements)[(row * COLUMNS + column) * RATIONAL_SIZE + OFFSET_DENOMINATOR];

    return CRational::New(numerator, denominator, outrat);
}

ECode CColorSpaceTransform::CopyElements(
    /* [in] */ ArrayOf<IRational*>* destination,
    /* [in] */ Int32 offset)
{
    FAIL_RETURN(Preconditions::CheckArgumentNonnegative(offset, String("offset must not be negative")))
    FAIL_RETURN(Preconditions::CheckNotNull(destination, String("destination must not be null")))
    if (destination == NULL) {
        Logger::E("CColorSpaceTransform", "destination must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (destination->GetLength() - offset < COUNT) {
        //throw new ArrayIndexOutOfBoundsException("destination too small to fit elements");
        Logger::E("CColorSpaceTransform", "destination too small to fit elements");
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    for (Int32 i = 0, j = 0; i < COUNT; ++i, j += RATIONAL_SIZE) {
        Int32 numerator = (*mElements)[j + OFFSET_NUMERATOR];
        Int32 denominator = (*mElements)[j + OFFSET_DENOMINATOR];

        AutoPtr<IRational> rational;
        CRational::New(numerator, denominator, (IRational**)&rational);
        destination->Set(i + offset, rational);
    }
    return NOERROR;
}

ECode CColorSpaceTransform::CopyElements(
    /* [in] */ ArrayOf<Int32>* destination,
    /* [in] */ Int32 offset)
{
    FAIL_RETURN(Preconditions::CheckArgumentNonnegative(offset, String("offset must not be negative")))
    FAIL_RETURN(Preconditions::CheckNotNull(destination, String("destination must not be null")))
    if (destination == NULL) {
        Logger::E("CColorSpaceTransform", "destination must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (destination->GetLength() - offset < COUNT_INT) {
        Logger::E("CColorSpaceTransform", "destination too small to fit elements");
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    // Manual copy faster than System#arraycopy for very small loops
    for (Int32 i = 0; i < COUNT_INT; ++i) {
        (*destination)[i + offset] = (*mElements)[i];
    }
    return NOERROR;
}


ECode CColorSpaceTransform::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal)
    *equal = FALSE;

    IColorSpaceTransform* cst = IColorSpaceTransform::Probe(obj);
    if (cst == NULL) {
        return NOERROR;
    }

    CColorSpaceTransform* other = (CColorSpaceTransform*)cst;
    for (Int32 i = 0, j = 0; i < COUNT; ++i, j += RATIONAL_SIZE) {
        Int32 numerator = (*mElements)[j + OFFSET_NUMERATOR];
        Int32 denominator = (*mElements)[j + OFFSET_DENOMINATOR];
        Int32 numeratorOther = (*(other->mElements))[j + OFFSET_NUMERATOR];
        Int32 denominatorOther = (*(other->mElements))[j + OFFSET_DENOMINATOR];
        AutoPtr<IRational> r;
        CRational::New(numerator, denominator, (IRational**)&r);
        AutoPtr<IRational> rOther;
        CRational::New(numeratorOther, denominatorOther, (IRational**)&rOther);
        Boolean result;
        r->Equals(rOther, &result);
        if (!result) {
            *equal = FALSE;
            return NOERROR;
        }
    }
    *equal = TRUE;
    return NOERROR;

}

ECode CColorSpaceTransform::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    return  HashCodeHelpers::GetHashCode(mElements, hashCode);
}

ECode CColorSpaceTransform::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "ColorSpaceTransform";
    sb += ToShortString();
    return sb.ToString(str);
}

String CColorSpaceTransform::ToShortString()
{
    StringBuilder sb;
    sb += "(";
    for (Int32 row = 0, i = 0; row < ROWS; row++) {
        sb += "[";
        for (Int32 col = 0; col < COLUMNS; col++, i += RATIONAL_SIZE) {
            Int32 numerator = (*mElements)[i + OFFSET_NUMERATOR];
            Int32 denominator = (*mElements)[i + OFFSET_DENOMINATOR];
            sb += numerator;
            sb += "/";
            sb += denominator;
            if (col < COLUMNS - 1) {
                sb += ", ";
            }
        }
        sb += "]";
        if (row < ROWS - 1) {
            sb += ", ";
        }
    }
    sb += ")";
    return sb.ToString();
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
