#ifndef __ORG_CONSCRYPT_COPENSSLKEYGENERATORTEST_H__
#define __ORG_CONSCRYPT_COPENSSLKEYGENERATORTEST_H__

#include "_Org_Conscrypt_COpenSSLKeyGeneratorTest.h"
#include "org/conscrypt/OpenSSLKeyGeneratorTest.h"

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLKeyGeneratorTest)
    , public OpenSSLKeyGeneratorTest
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Conscrypt
} // namespace Org

#endif // __ORG_CONSCRYPT_COPENSSLKEYGENERATORTEST_H__
