
#ifndef __ELASTOSX_CRYPTO_SPEC_CPBEKEYSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CPBEKEYSPEC_H__

#include "_Elastosx_Crypto_Spec_CPBEKeySpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::EIID_IKeySpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CPBEKeySpec)
    , public Object
    , public IPBEKeySpec
    , public IKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CPBEKeySpec();

    /**
     * Creates a new <code>PBEKeySpec</code> with the specified password.
     *
     * @param password
     *            the password.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Char32> * password);

    /**
     * Creates a new <code>PBEKeySpec</code> with the specified password, salt,
     * iteration count and the desired length of the derived key.
     *
     * @param password
     *            the password.
     * @param salt
     *            the salt.
     * @param iterationCount
     *            the iteration count.
     * @param keyLength
     *            the desired key length of the derived key,
     * @throws NullPointerException
     *             if the salt is null.
     * @throws IllegalArgumentException
     *             if the salt is empty, iteration count is zero or negative or
     *             the key length is zero or negative.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Char32> * password,
        /* [in] */ ArrayOf<Byte> * salt,
        /* [in] */ Int32 iterationCount,
        /* [in] */ Int32 keyLength);

    /**
     * Creates a new <code>PBEKeySpec</code> with the specified password, salt
     * and iteration count.
     *
     * @param password
     *            the password.
     * @param salt
     *            the salt.
     * @param iterationCount
     *            the iteration count.
     * @throws NullPointerException
     *             if salt is null.
     * @throws IllegalArgumentException
     *             if the salt is empty or iteration count is zero or negative.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Char32> * password,
        /* [in] */ ArrayOf<Byte> * salt,
        /* [in] */ Int32 iterationCount);

    /**
     * Clears the password by overwriting it.
     */
    CARAPI ClearPassword();

    /**
     * Returns a copy of the password of this key specification.
     *
     * @return a copy of the password of this key specification.
     * @throws IllegalStateException
     *             if the password has been cleared before.
     */
    CARAPI GetPassword(
        /* [out, callee] */ ArrayOf<Char32> ** pwd);

    /**
     * Returns a copy of the salt of this key specification.
     *
     * @return a copy of the salt of this key specification or null if none is
     *         specified.
     */
    CARAPI GetSalt(
        /* [out, callee] */ ArrayOf<Byte> ** salt);

    /**
     * Returns the iteration count of this key specification.
     *
     * @return the iteration count of this key specification.
     */
    CARAPI GetIterationCount(
        /* [out] */ Int32 * count);

    /**
     * Returns the desired key length of the derived key.
     *
     * @return the desired key length of the derived key.
     */
    CARAPI GetKeyLength(
        /* [out] */ Int32 * length);

private:
    AutoPtr<ArrayOf<Char32> > mPassword;
    AutoPtr<ArrayOf<Byte> > mSalt;
    Int32 mIterationCount;
    Int32 mKeyLength;

};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CPBEKEYSPEC_H__
