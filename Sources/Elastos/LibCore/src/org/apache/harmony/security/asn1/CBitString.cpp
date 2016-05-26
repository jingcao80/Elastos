
#include "CBitString.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Asn1 {

static AutoPtr<ArrayOf<Byte> > SET_MASK;

static AutoPtr<ArrayOf<Byte> > RESET_MASK = InitStatic();

CAR_OBJECT_IMPL(CBitString)

CAR_INTERFACE_IMPL(CBitString, Object, IBitString)

AutoPtr<ArrayOf<Byte> > CBitString::InitStatic()
{
    SET_MASK = ArrayOf<Byte>::Alloc(8);
    Byte[] tmp1 = { (Byte) 128, 64, 32, 16, 8, 4, 2, 1 };
    for (Int32 i = 0; i < sizeof(tmp1) / sizeof(tmp1[0]); i++) {
        (*SET_MASK)[i] = tmp1[i];
    }
    AutoPtr<ArrayOf<Byte> > ret = ArrayOf<Byte>::Alloc(8);
    Byte[] tmp2 = { 0x7f, (Byte) 0xbf, (Byte) 0xdf,
        (Byte) 0xef, (Byte) 0xf7, (Byte) 0xfb, (Byte) 0xfd, (Byte) 0xfe};
    for (Int32 i =0; i < sizeof(tmp2)/sizeof(tmp2[0]); i++) {
        (*ret)[i] = tmp2[i];
    }
    return ret;
}

ECode CBitString::SetBytes(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    mBytes = bytes;
    return NOERROR;
}

ECode CBitString::GetBytes(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = mBytes;
    REFCOUNT_ADD(*bytes)
    return NOERROR;
}

ECode CBitString::SetUnusedBits(
    /* [in] */ Int32 unusedBits)
{
    mUnusedBits = unusedBits;
    return NOERROR;
}

ECode CBitString::GetUnusedBits(
    /* [out] */ Int32* unusedBits)
{
    VALIDATE_NOT_NULL(unusedBits)
    *unusedBits = mUnusedBits;
    return NOERROR;
}

ECode CBitString::GetBit(
    /* [in] */ Int32 bit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    Int32 offset = bit % 8;
    Int32 index = bit / 8;
    *value = ((*mBytes)[index] & (*SET_MASK)[offset]) != 0;
    return NOERROR;
}

ECode CBitString::SetBit(
    /* [in] */ Int32 bit,
    /* [in] */ Boolean value)
{
    Int32 offset = bit % 8;
    Int32 index = bit / 8;
    if (value) {
        (*mBytes)[index] |= (*SET_MASK)[offset];
    } else {
        (*mBytes)[index] &= (*RESET_MASK)[offset];
    }
    return NOERROR;
}

ECode CBitString::ToBooleanArray(
    /* [out, callee] */ ArrayOf<Boolean>** array)
{
    VALIDATE_NOT_NULL(array)
    AutoPtr<ArrayOf<Boolean> > result = ArrayOf<Boolean>::Alloc(mBytes->GetLength() * 8
        - mUnusedBits);
    for (Int32 i = 0; i < result->GetLength(); i++) {
        GetBit(i, &(*result)[i]);
    }
    *array = result;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CBitString::constructor(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [in] */ Int32 unusedBits)
{
        // constraints are set according X.690
        if (unusedBits < 0 || unusedBits > 7) {
            //throw new IllegalArgumentException("Number of unused bits MUST be in range 0-7");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        if (bytes->GetLength() == 0 && unusedBits != 0) {
            //throw new IllegalArgumentException("For empty bit string unused bits MUST be 0");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        mBytes = bytes;
        mUnusedBits = unusedBits;
        return NOERROR;
}

ECode CBitString::constructor(
    /* [in] */ ArrayOf<Boolean>* values)
{
    mUnusedBits = values->GetLength() % 8;
    Int32 size = values->GetLength() / 8;
    if (mUnusedBits != 0) {
        size++;
    }
    mBytes = ArrayOf<Byte>::Alloc(size);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        SetBit(i, (*values)[i]);
    }
    return NOERROR;
}

} // namespace Asn1
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org

