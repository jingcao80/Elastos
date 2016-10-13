
#ifndef __ORG_CONSCRYPT_COPENSSLECPUBLICKEYHELPER_H__
#define __ORG_CONSCRYPT_COPENSSLECPUBLICKEYHELPER_H__

#include "_Org_Conscrypt_COpenSSLECPublicKeyHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Security::Interfaces::IECPublicKey;

namespace Org {
namespace Conscrypt {

CarClass(COpenSSLECPublicKeyHelper)
    , public Singleton
    , public IOpenSSLECPublicKeyHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetInstance(
        /* [in] */ IECPublicKey* ecPublicKey,
        /* [out] */ IOpenSSLKey** result);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_COPENSSLECPUBLICKEYHELPER_H__
