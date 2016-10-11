#include "elastos/droid/keystore/security/CCredentialsHelper.h"

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

CAR_INTERFACE_IMPL(CCredentialsHelper, Singleton, ICredentialsHelper)
CAR_SINGLETON_IMPL(CCredentialsHelper)

ECode CCredentialsHelper::ConvertToPem(
    /* [in] */ ArrayOf<ICertificate*>* objects,
    /* [out, callee] */ ArrayOf<Byte>** ret)
{
    return Credentials::ConvertToPem(objects, ret);
    return NOERROR;
}

ECode CCredentialsHelper::ConvertFromPem(
    /* [in] */ ArrayOf<Byte>* bytes,
    /* [out] */ IList** ret)
{
    return Credentials::ConvertFromPem(bytes, ret);
}

ECode CCredentialsHelper::GetInstance(
    /* [out] */ ICredentials** instance)
{
    return Credentials::GetInstance(instance);
}

ECode CCredentialsHelper::DeleteAllTypesForAlias(
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* ret)
{
    return Credentials::DeleteAllTypesForAlias(keystore, alias, ret);
}

ECode CCredentialsHelper::DeleteCertificateTypesForAlias(
    /* [in] */ IKeyStore* keystore,
    /* [in] */ const String& alias,
    /* [out] */ Boolean* ret)
{
    return Credentials::DeleteCertificateTypesForAlias(keystore, alias, ret);
}

} // namespace Security
} // namespace KeyStore
} // namespace Droid
} // namespace Elastos
