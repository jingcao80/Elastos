
#ifndef __ORG_CONSCRYPT_COPENSSLECDHKEYAGREEMENT_H__
#define __ORG_CONSCRYPT_COPENSSLECDHKEYAGREEMENT_H__

#include "_Org_Conscrypt_COpenSSLECDHKeyAgreement.h"
#include <Elastos.CoreLibrary.Extensions.h>
#include <Elastos.CoreLibrary.Security.h>
#include "elastosx/crypto/KeyAgreementSpi.h"

using Elastos::Security::IKey;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastosx::Crypto::ISecretKey;
using Elastosx::Crypto::KeyAgreementSpi;

namespace Org {
namespace Conscrypt {

/**
 * Elliptic Curve Diffie-Hellman key agreement backed by the OpenSSL engine.
 */
CarClass(COpenSSLECDHKeyAgreement)
    , public IOpenSSLECDHKeyAgreement
    , public KeyAgreementSpi
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    virtual CARAPI EngineDoPhase(
        /* [in] */ IKey* key,
        /* [in] */ Boolean lastPhase,
        /* [out] */ IKey** result);

    virtual CARAPI EngineGenerateSecret(
        /* [in] */ ArrayOf<Byte>* sharedSecret,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* result);

    virtual CARAPI EngineGenerateSecret(
        /* [out] */ ArrayOf<Byte> ** result);

    virtual CARAPI EngineGenerateSecret(
        /* [in] */ const String& algorithm,
        /* [out] */ ISecretKey** skey);

    virtual CARAPI EngineInit(
        /* [in] */ IKey* key,
        /* [in] */ ISecureRandom* random);

    virtual CARAPI EngineInit(
        /* [in] */ IKey* key,
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

private:
    CARAPI CheckCompleted();

private:
    /** OpenSSL handle of the private key. Only available after the engine has been initialized. */
    AutoPtr<IOpenSSLKey> mOpenSslPrivateKey;

    /**
     * Expected length (in bytes) of the agreed key ({@link #mResult}). Only available after the
     * engine has been initialized.
     */
    Int32 mExpectedResultLength;

    /** Agreed key. Only available after {@link #engineDoPhase(Key, boolean)} completes. */
    AutoPtr<ArrayOf<Byte> > mResult;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECDHKEYAGREEMENT_H__
