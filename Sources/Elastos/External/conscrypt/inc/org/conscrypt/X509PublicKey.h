
#ifndef __ORG_CONSCRYPT_X509PUBLICKEY_H__
#define __ORG_CONSCRYPT_X509PUBLICKEY_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IPublicKey;

namespace Org {
namespace Conscrypt {

/**
 * A simple but useless key class that holds X.509 public key information when
 * the appropriate KeyFactory for the key algorithm is not available.
 */
class X509PublicKey
    : public Object
    , public IPublicKey
    , public IX509PublicKey
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ String algorithm,
        /* [in] */ ArrayOf<Byte>* encoded);

    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    CARAPI GetFormat(
        /* [out] */ String* result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

private:
    static Int64 mSerialVersionUID;

    String mAlgorithm;

    AutoPtr<ArrayOf<Byte> > mEncoded;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_X509PUBLICKEY_H__