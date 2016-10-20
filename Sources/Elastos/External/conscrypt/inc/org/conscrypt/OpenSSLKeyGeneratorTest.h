
#ifndef __ORG_CONSCRYPT_OPENSSLKEYGENERATORTEST_H__
#define __ORG_CONSCRYPT_OPENSSLKEYGENERATORTEST_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastosx::Crypto::IKeyGeneratorSpi;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastosx::Crypto::ISecretKey;

namespace Org {
namespace Conscrypt {

class OpenSSLKeyGeneratorTest
    : public Object
    , public IKeyGeneratorSpi
{
public:
    CAR_INTERFACE_DECL()

    CARAPI EngineGenerateKey(
        /* [out] */ ISecretKey** result);

    CARAPI EngineInit(
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

    CARAPI EngineInit(
        /* [in] */ Int32 keysize,
        /* [in] */ ISecureRandom* random);

    CARAPI EngineInit(
        /* [in] */ ISecureRandom* random);

};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLKEYGENERATORTEST_H__
