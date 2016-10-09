#ifndef __ELASTOS_DROID_KEYSTORE_SECURITY_CCREDENTIALSHELPER_H__
#define __ELASTOS_DROID_KEYSTORE_SECURITY_CCREDENTIALSHELPER_H__

#include "_Elastos_Droid_KeyStore_Security_CCredentialsHelper.h"
#include "elastos/droid/keystore/security/Credentials.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CarClass(CCredentialsHelper)
    , public Singleton
    , public ICredentialsHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI ConvertToPem(
        /* [in] */ ArrayOf<ICertificate*>* objects,
        /* [out, callee] */ ArrayOf<Byte>** ret);

    CARAPI ConvertFromPem(
        /* [in] */ ArrayOf<Byte>* bytes,
        /* [out] */ IList** ret);

    CARAPI GetInstance(
        /* [out] */ ICredentials** instance);

    CARAPI DeleteAllTypesForAlias(
        /* [in] */ IKeyStore* keystore,
        /* [in] */ const String& alias,
        /* [out] */ Boolean* ret);

    CARAPI DeleteCertificateTypesForAlias(
        /* [in] */ IKeyStore* keystore,
        /* [in] */ const String& alias,
        /* [out] */ Boolean* ret);

};

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_KEYSTORE_SECURITY_CCREDENTIALSHELPER_H__
