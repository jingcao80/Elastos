
#ifndef __ORG_CONSCRYPT_OPENSSLSIGNATURERAWRSA_H__
#define __ORG_CONSCRYPT_OPENSSLSIGNATURERAWRSA_H__

#include "_Org.Conscrypt.h"
#include "elastos/security/SignatureSpi.h"

using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::SignatureSpi;

namespace Org {
namespace Conscrypt {

/**
 * Implements the JDK Signature interface needed for RAW RSA signature
 * generation and verification using OpenSSL.
 */
class OpenSSLSignatureRawRSA
    : public SignatureSpi
    , public IOpenSSLSignatureRawRSA
{
public:
    CAR_INTERFACE_DECL()

    CARAPI EngineUpdate(
        /* [in] */ Byte input);

    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI EngineGetParameter(
        /* [in] */ const String& param,
        /* [out] */ IInterface** value);

    CARAPI EngineInitSign(
        /* [in] */ IPrivateKey* privateKey);

    CARAPI EngineInitVerify(
        /* [in] */ IPublicKey* publicKey);

    CARAPI EngineSetParameter(
        /* [in] */ const String& param,
        /* [in] */ IInterface* value);

    CARAPI EngineSign(
        /* [out, callee] */ ArrayOf<Byte>** sign);

    CARAPI EngineVerify(
        /* [in] */ ArrayOf<Byte>* sigBytes,
        /* [out] */ Boolean* result);

private:
    /**
     * The current OpenSSL key we're operating on.
     */
    AutoPtr<IOpenSSLKey> mKey;

    /**
     * Buffer to hold value to be signed or verified.
     */
    AutoPtr<ArrayOf<Byte> > mInputBuffer;

    /**
     * Current offset in input buffer.
     */
    Int32 mInputOffset;

    /**
     * Provides a flag to specify when the input is too long.
     */
    Boolean mInputIsTooLong;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLSIGNATURERAWRSA_H__
