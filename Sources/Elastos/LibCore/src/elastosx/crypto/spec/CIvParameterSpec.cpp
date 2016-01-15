#include "CIvParameterSpec.h"
#include "Arrays.h"

using Elastos::Utility::Arrays;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CIvParameterSpec)
CAR_INTERFACE_IMPL_2(CIvParameterSpec, Object, IIvParameterSpec, IAlgorithmParameterSpec)

CIvParameterSpec::CIvParameterSpec()
{
}

ECode CIvParameterSpec::constructor(
    /* [in] */ ArrayOf<Byte> * iv)
{
    if (iv == NULL) {
        // throw new IllegalArgumentException("iv == NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mIv = ArrayOf<Byte>::Alloc(iv->GetLength());
    mIv->Copy(0, iv, 0, iv->GetLength());
    return NOERROR;
}

ECode CIvParameterSpec::constructor(
    /* [in] */ ArrayOf<Byte> * iv,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    if ((iv == NULL)|| (iv->GetLength() - offset < byteCount)) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Arrays::CheckOffsetAndCount(iv->GetLength(), offset, byteCount);
    mIv = ArrayOf<Byte>::Alloc(byteCount);
    // System.arraycopy(iv, offset, this.iv, 0, byteCount);
    mIv->Copy(0, iv, offset, byteCount);
    return NOERROR;
}

ECode CIvParameterSpec::GetIV(
    /* [out, callee] */ ArrayOf<Byte> ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = ArrayOf<Byte>::Alloc(mIv->GetLength());
    (*result)->Copy(0, mIv, 0, mIv->GetLength());
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx