#include "CRC5ParameterSpec.h"
#include "Arrays.h"

using Elastos::Utility::Arrays;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CRC5ParameterSpec)
CAR_INTERFACE_IMPL_2(CRC5ParameterSpec, Object, IRC5ParameterSpec, IAlgorithmParameterSpec)

CRC5ParameterSpec::CRC5ParameterSpec()
    : mVersion(0)
    , mRounds(0)
    , mWordSize(0)
{
}

ECode CRC5ParameterSpec::constructor(
    /* [in] */ Int32 version,
    /* [in] */ Int32 rounds,
    /* [in] */ Int32 wordSize)
{
    mVersion = version;
    mRounds = rounds;
    mWordSize = wordSize;
    return NOERROR;
}

ECode CRC5ParameterSpec::constructor(
    /* [in] */ Int32 version,
    /* [in] */ Int32 rounds,
    /* [in] */ Int32 wordSize,
    /* [in] */ ArrayOf<Byte> * iv)
{
    if (iv == NULL) {
        // throw new IllegalArgumentException("iv == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if(iv->GetLength() < 2 * (wordSize / 8)) {
        // throw new IllegalArgumentException("iv.length < 2 * (wordSize / 8)");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mVersion = version;
    mRounds = rounds;
    mWordSize = wordSize;
    mIv = ArrayOf<Byte>::Alloc(2*(wordSize/8));
    mIv->Copy(0, iv, 0, 2*(wordSize/8));
    return NOERROR;
}

ECode CRC5ParameterSpec::constructor(
    /* [in] */ Int32 version,
    /* [in] */ Int32 rounds,
    /* [in] */ Int32 wordSize,
    /* [in] */ ArrayOf<Byte> * iv,
    /* [in] */ Int32 offset)
{
    if (iv == NULL) {
        // throw new IllegalArgumentException("iv == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0) {
        // throw new ArrayIndexOutOfBoundsException("offset < 0: " + offset);
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    if(iv->GetLength() - offset < 2 * (wordSize / 8)) {
        // throw new IllegalArgumentException("iv.length - offset < 2 * (wordSize / 8)");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mVersion = version;
    mRounds = rounds;
    mWordSize = wordSize;
    mIv = ArrayOf<Byte>::Alloc(2 * (wordSize / 8));
    mIv->Copy(0, iv, 0, 2 * (wordSize / 8));
    return NOERROR;
}

ECode CRC5ParameterSpec::GetVersion(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVersion;
    return NOERROR;
}

ECode CRC5ParameterSpec::GetRounds(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRounds;
    return NOERROR;
}

ECode CRC5ParameterSpec::GetWordSize(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWordSize;
    return NOERROR;
}

ECode CRC5ParameterSpec::GetIV(
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mIv == NULL) {
        return NOERROR;
    }
    *result = ArrayOf<Byte>::Alloc(mIv->GetLength());
    (*result)->Copy(0, mIv, 0, mIv->GetLength());
    return NOERROR;
}

ECode CRC5ParameterSpec::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = mVersion + mRounds + mWordSize;
    if (mIv == NULL) {
        return NOERROR;
    }
    for(Int32 i = 0; i < mIv->GetLength(); i++) {
        *hashCode += (*mIv)[i] & 0xFF;
    }
    return NOERROR;
}

ECode CRC5ParameterSpec::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if ((CRC5ParameterSpec*)(IRC5ParameterSpec*)obj == this) {
        *result = TRUE;
        return NOERROR;
    }
    if (IRC5ParameterSpec::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<CRC5ParameterSpec> ps = (CRC5ParameterSpec*)(IRC5ParameterSpec*) obj;
    *result = (mVersion == ps->mVersion)
        && (mRounds == ps->mRounds)
        && (mWordSize == ps->mWordSize)
        && (Arrays::Equals(mIv, ps->mIv));
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx