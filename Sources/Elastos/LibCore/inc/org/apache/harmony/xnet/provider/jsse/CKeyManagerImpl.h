
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CKEYMANAGERIMPL_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CKEYMANAGERIMPL_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CKeyManagerImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CKeyManagerImpl)
{
public:
    CARAPI ChooseClientAlias(
        /* [in] */ ArrayOf<String> * pKeyType,
        /* [in] */ ArrayOf<Elastos::Security::IPrincipal *> * pIssuers,
        /* [in] */ Elastos::Net::ISocket * pSocket,
        /* [out] */ String * pAlias);

    CARAPI ChooseServerAlias(
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<Elastos::Security::IPrincipal *> * pIssuers,
        /* [in] */ Elastos::Net::ISocket * pSocket,
        /* [out] */ String * pAlias);

    CARAPI GetCertificateChain(
        /* [in] */ const String& alias,
        /* [out, callee] */ ArrayOf<Elastos::Security::Cert::IX509Certificate *> ** ppChain);

    CARAPI GetClientAliases(
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<Elastos::Security::IPrincipal *> * pIssuers,
        /* [out, callee] */ ArrayOf<String> ** ppAlias);

    CARAPI GetServerAliases(
        /* [in] */ const String& keyType,
        /* [in] */ ArrayOf<Elastos::Security::IPrincipal *> * pIssuers,
        /* [out, callee] */ ArrayOf<String> ** ppAlias);

    CARAPI GetPrivateKey(
        /* [in] */ const String& alias,
        /* [out] */ Elastos::Security::IPrivateKey ** ppKey);

    CARAPI constructor(
        /* [in] */ Elastos::Security::IKeyStore * pKeyStore,
        /* [in] */ ArrayOf<Char32> * pPwd);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CKEYMANAGERIMPL_H__
