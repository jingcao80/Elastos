
#ifndef __ORG_CONSCRYPT_OPENSSLDSAKEYPAIRGENERATOR_H__
#define __ORG_CONSCRYPT_OPENSSLDSAKEYPAIRGENERATOR_H__

#include <elastos/coredef.h>
#include <Elastos.CoreLibrary.Security.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/security/KeyPairGeneratorSpi.h>
#include "_Org.Conscrypt.h"

using Elastos::Math::IBigInteger;
using Elastos::Security::IKeyPair;
using Elastos::Security::ISecureRandom;
using Elastos::Security::KeyPairGeneratorSpi;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Org {
namespace Conscrypt {

class OpenSSLDSAKeyPairGenerator
    : public IOpenSSLDSAKeyPairGenerator
    , public KeyPairGeneratorSpi
{
public:
    CAR_INTERFACE_DECL()

    OpenSSLDSAKeyPairGenerator();

    virtual CARAPI GenerateKeyPair(
        /* [out] */  IKeyPair** result);

    virtual CARAPI Initialize(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    virtual CARAPI Initialize(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

private:
    Int32 mPrimeBits; // = 1024;

    AutoPtr<ISecureRandom> mRandom;

    AutoPtr<ArrayOf<Byte> > mG;

    AutoPtr<ArrayOf<Byte> > mP;

    AutoPtr<ArrayOf<Byte> > mQ;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLDSAKEYPAIRGENERATOR_H__
