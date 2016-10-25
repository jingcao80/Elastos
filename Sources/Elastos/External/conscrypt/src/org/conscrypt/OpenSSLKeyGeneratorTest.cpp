
#include "OpenSSLKeyGeneratorTest.h"
#include "org/conscrypt/COpenSSLSecretKey.h"
#include <elastos/utility/logging/Logger.h>

using Elastosx::Crypto::EIID_IKeyGeneratorSpi;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLKeyGeneratorTest::
//=========================================

CAR_INTERFACE_IMPL(OpenSSLKeyGeneratorTest, Object, IKeyGeneratorSpi);

ECode OpenSSLKeyGeneratorTest::EngineGenerateKey(
    /* [out] */ ISecretKey** result)
{
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineGenerateKey");
    VALIDATE_NOT_NULL(result);
    String algorithm("HMACMD5");
    AutoPtr<ArrayOf<Byte> > encoded = ArrayOf<Byte>::Alloc(10);
    for (Int32 i = 0; i < 10; ++i) {
        encoded->Set(i, 'A' + i);
    }
    COpenSSLSecretKey::New(algorithm, encoded, result);
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineGenerateKey, result:%p", *result);
    return NOERROR;
}

ECode OpenSSLKeyGeneratorTest::EngineInit(
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineInit 1");
    return NOERROR;
}

ECode OpenSSLKeyGeneratorTest::EngineInit(
    /* [in] */ Int32 keysize,
    /* [in] */ ISecureRandom* random)
{
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineInit 2");
    return NOERROR;
}

ECode OpenSSLKeyGeneratorTest::EngineInit(
    /* [in] */ ISecureRandom* random)
{
    Logger::E("leliang", "TODO OpenSSLKeyGeneratorTest::EngineInit 3");
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
