
#include "OpenSSLKeyGeneratorTest.h"
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
