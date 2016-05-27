
#include "CKeyManagerImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CAR_OBJECT_IMPL(CKeyManagerImpl)

ECode CKeyManagerImpl::ChooseClientAlias(
    /* [in] */ ArrayOf<String> * pKeyType,
    /* [in] */ ArrayOf<Elastos::Security::IPrincipal *> * pIssuers,
    /* [in] */ Elastos::Net::ISocket * pSocket,
    /* [out] */ String * pAlias)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyManagerImpl::ChooseServerAlias(
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<Elastos::Security::IPrincipal *> * pIssuers,
    /* [in] */ Elastos::Net::ISocket * pSocket,
    /* [out] */ String * pAlias)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyManagerImpl::GetCertificateChain(
    /* [in] */ const String& alias,
    /* [out, callee] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> ** ppChain)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyManagerImpl::GetClientAliases(
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<Elastos::Security::IPrincipal *> * pIssuers,
    /* [out, callee] */ ArrayOf<String> ** ppAlias)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyManagerImpl::GetServerAliases(
    /* [in] */ const String& keyType,
    /* [in] */ ArrayOf<Elastos::Security::IPrincipal *> * pIssuers,
    /* [out, callee] */ ArrayOf<String> ** ppAlias)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyManagerImpl::GetPrivateKey(
    /* [in] */ const String& alias,
    /* [out] */ Elastos::Security::IPrivateKey ** ppKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CKeyManagerImpl::constructor(
    /* [in] */ Elastos::Security::IKeyStore * pKeyStore,
    /* [in] */ ArrayOf<Char32> * pPwd)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

