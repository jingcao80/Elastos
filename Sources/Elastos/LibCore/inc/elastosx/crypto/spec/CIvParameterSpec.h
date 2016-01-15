
#ifndef __ELASTOSX_CRYPTO_SPEC_CIVPARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CIVPARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_CIvParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CIvParameterSpec)
    , public Object
    , public IIvParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CIvParameterSpec();

    /**
     * Creates a new <code>IvParameterSpec</code> instance with the bytes from
     * the specified buffer <i>iv</i> used as <i>initialization vector</i>.
     *
     * @param iv
     *            the buffer used as initialization vector.
     * @throws NullPointerException
     *             if the specified buffer is null.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * iv);

    /**
     * Creates a new <code>IvParameterSpec</code> instance with <code>byteCount</code>
     * bytes from the specified buffer <code>iv</code> starting at
     * <code>offset</code>.
     *
     * @throws IllegalArgumentException
     *             if the specified buffer is null or <code>offset</code> and
     *             <code>byteCount</code> do not specify a valid chunk in the
     *             specified buffer.
     * @throws ArrayIndexOutOfBoundsException
     *             if <code>offset</code> or <code>byteCount</code> are negative.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * iv,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

    /**
     * Returns a copy of the <i>initialization vector</i> data.
     *
     * @return a copy of the initialization vector data.
     */
    CARAPI GetIV(
        /* [out, callee] */ ArrayOf<Byte> ** result);

private:
    AutoPtr<ArrayOf<Byte> > mIv;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CIVPARAMETERSPEC_H__
