
#ifndef __ELASTOSX_CRYPTO_SPEC_CGCMPARAMETERSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CGCMPARAMETERSPEC_H__

#include "_Elastosx_Crypto_Spec_CGCMParameterSpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::Security::Spec::EIID_IAlgorithmParameterSpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CGCMParameterSpec)
    , public Object
    , public IGCMParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CGCMParameterSpec();

    /**
     * Creates a new {@code GCMParameterSpec} instance from the specified
     * Initial Vector (IV) from buffer {@code iv} and a tag length of
     * {@code tagLen} in bits.
     *
     * @throws IllegalArgumentException if the specified {@code iv} is null or
     *             {@code offset} and {@code byteCount} do not specify a valid
     *             chunk in the specified buffer.
     */
    CARAPI constructor(
        /* [in] */ Int32 tagLen,
        /* [in] */ ArrayOf<Byte> * iv);

    /**
     * Creates a new {@code GCMParameterSpec} instance with the Initial Vector
     * (IV) of {@code byteCount} bytes from the specified buffer {@code iv}
     * starting at {@code offset} and a tag length of {@code tagLen} in bits.
     *
     * @throws IllegalArgumentException if the specified {@code iv} is null or
     *             {@code offset} and {@code byteCount} do not specify a valid
     *             chunk in the specified buffer.
     * @throws ArrayIndexOutOfBoundsException if {@code offset} or
     *             {@code byteCount} are negative.
     */
    CARAPI constructor(
        /* [in] */ Int32 tagLen,
        /* [in] */ ArrayOf<Byte> * iv,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

    /**
     * Returns the size of the tag in bits.
     */
    CARAPI GetTLen(
        /* [out] */ Int32 * result);

    /**
     * Returns the Initial Vector (IV) used by this parameter spec.
     */
    CARAPI GetIV(
        /* [out, callee] */ ArrayOf<Byte> ** result);

private:
    Int32 mTagLen;
    AutoPtr<ArrayOf<Byte> > mIv;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CGCMPARAMETERSPEC_H__
