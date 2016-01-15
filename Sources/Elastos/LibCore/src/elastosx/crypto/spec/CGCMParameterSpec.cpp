#include "CGCMParameterSpec.h"
#include "Arrays.h"

using Elastos::Utility::Arrays;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CGCMParameterSpec)
CAR_INTERFACE_IMPL_2(CGCMParameterSpec, Object, IGCMParameterSpec, IAlgorithmParameterSpec)

CGCMParameterSpec::CGCMParameterSpec()
    : mTagLen(0)
{
}

ECode CGCMParameterSpec::constructor(
    /* [in] */ Int32 tagLen,
    /* [in] */ ArrayOf<Byte> * iv)
{
    if (tagLen < 0) {
        // throw new IllegalArgumentException("tag should be a non-negative integer");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (iv == NULL) {
        // throw new IllegalArgumentException("iv == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mTagLen = tagLen;
    mIv = iv->Clone();
    return NOERROR;
}

ECode CGCMParameterSpec::constructor(
    /* [in] */ Int32 tagLen,
    /* [in] */ ArrayOf<Byte> * iv,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    if (tagLen < 0) {
        // throw new IllegalArgumentException("tag should be a non-negative integer");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (iv == NULL) {
        // throw new IllegalArgumentException("iv == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // try {
        Arrays::CheckOffsetAndCount(iv->GetLength(), offset, byteCount);
    // } catch (ArrayIndexOutOfBoundsException e) {
    //     // throw new IllegalArgumentException(e);
    //     return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // }
    mTagLen = tagLen;
    Arrays::CopyOfRange(iv, offset, offset + byteCount, (ArrayOf<Byte> **)&mIv);
    return NOERROR;
}

ECode CGCMParameterSpec::GetTLen(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTagLen;
    return NOERROR;
}

ECode CGCMParameterSpec::GetIV(
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIv->Clone();
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx