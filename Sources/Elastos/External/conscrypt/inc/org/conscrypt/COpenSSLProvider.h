
#ifndef __ORG_CONSCRYPT_COPENSSLPROVIDER_H__
#define __ORG_CONSCRYPT_COPENSSLPROVIDER_H__

#include "_Org_Conscrypt_COpenSSLProvider.h"
#include "elastos/security/Provider.h"

using Elastos::Security::Provider;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLProvider) , public Provider
{
public:
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& providerName);

    CAR_OBJECT_DECL()

private:
    CARAPI Put(
        /* [in] */ const char* key,
        /* [in] */ const char* value);

    CARAPI Put(
        /* [in] */ const char* key,
        /* [in] */ const String& value);

    CARAPI Put(
        /* [in] */ const String& key,
        /* [in] */ const String& value);

public:
    static const String PROVIDER_NAME;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLPROVIDER_H__
