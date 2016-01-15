
#include "elastos/droid/hardware/camera2/params/CColorSpaceTransform.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/utility/CRational.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::CRational;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Slogger;

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
    //FAIL_RETURN(Preconditions::CheckNotNull(elements))
    if (elements == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (elements->GetLength() != COUNT) {
        //throw new IllegalArgumentException("elements must be " + COUNT + " length");
        Slogger::E("CColorSpaceTransform", "elements must be  %d length", COUNT);
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
    //FAIL_RETURN(Preconditions::CheckNotNull(elements))
    if (elements == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (elements->GetLength() != COUNT_INT) {
        //throw new IllegalArgumentException("elements must be " + COUNT_INT + " length");
        Slogger::E("CColorSpaceTransform", "elements must be  %d length", COUNT_INT);
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
        //throw new IllegalArgumentException("column out of range");
        Slogger::E("CColorSpaceTransform", "column out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (row < 0 || row >= ROWS) {
        //throw new IllegalArgumentException("row out of range");
        Slogger::E("CColorSpaceTransform", "row out of range");
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
    //FAIL_RETURN(Preconditions::CheckNotNull(destination, Strinfg("destination must not be null")))
    if (destination == NULL) {
        Slogger::E("CColorSpaceTransform", "destination must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (destination->GetLength() - offset < COUNT) {
        //throw new ArrayIndexOutOfBoundsException("destination too small to fit elements");
        Slogger::E("CColorSpaceTransform", "destination too small to fit elements");
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
    //FAIL_RETURN(Preconditions::CheckNotNull(destination, Strinfg("destination must not be null")))
    if (destination == NULL) {
        Slogger::E("CColorSpaceTransform", "destination must not be null");
        return E_NULL_POINTER_EXCEPTION;
    }

    if (destination->GetLength() - offset < COUNT_INT) {
        //throw new ArrayIndexOutOfBoundsException("destination too small to fit elements");
        Slogger::E("CColorSpaceTransform", "destination too small to fit elements");
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

    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }
    else if (THIS_PROBE(IInterface) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    else if (IColorSpaceTransform::Probe(obj) != NULL) {
        const AutoPtr<CColorSpaceTransform> other = (CColorSpaceTransform*)IColorSpaceTransform::Probe(obj);
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
    *equal = FALSE;
    return NOERROR;
}

ECode CColorSpaceTransform::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    assert(0 && "TODO: weit Hardware/Camera2/Utils/HashCodeHelpers");
    //*hashCode = HashCodeHelpers::GetHashCode(mElements);
    return NOERROR;
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
