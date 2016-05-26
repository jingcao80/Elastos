
#ifndef __ORG_APACHE_HARMONY_SECURITY_CSYSTEMSCOPE_H__
#define __ORG_APACHE_HARMONY_SECURITY_CSYSTEMSCOPE_H__

#include "_Org_Apache_Harmony_Security_CSystemScope.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {

CarClass(CSystemScope)
    , public Object
    , public IIdentityScope
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI AddCertificate(
        /* [in] */ Elastos::Security::ICertificate * pCertificate);

    CARAPI RemoveCertificate(
        /* [in] */ Elastos::Security::ICertificate * pCertificate);

    CARAPI Certificates(
        /* [out, callee] */ ArrayOf<Elastos::Security::ICertificate *> ** ppCerts);

    CARAPI ToStringEx(
        /* [in] */ Boolean detailed,
        /* [out] */ String * pStr);

    CARAPI GetScope(
        /* [out] */ Elastos::Security::IIdentityScope ** ppScope);

    CARAPI SetPublicKey(
        /* [in] */ Elastos::Security::IPublicKey * pKey);

    CARAPI GetPublicKey(
        /* [out] */ Elastos::Security::IPublicKey ** ppPubKey);

    CARAPI SetInfo(
        /* [in] */ const String& info);

    CARAPI GetInfo(
        /* [out] */ String * pStr);

    CARAPI GetSize(
        /* [out] */ Int32 * pSize);

    CARAPI GetIdentity(
        /* [in] */ const String& name,
        /* [out] */ Elastos::Security::IIdentity ** ppIdentity);

    CARAPI GetIdentityEx(
        /* [in] */ Elastos::Security::IPrincipal * pPrincipal,
        /* [out] */ Elastos::Security::IIdentity ** ppIdentity);

    CARAPI GetIdentityEx2(
        /* [in] */ Elastos::Security::IPublicKey * pKey,
        /* [out] */ Elastos::Security::IIdentity ** ppIdentity);

    CARAPI AddIdentity(
        /* [in] */ Elastos::Security::IIdentity * pIdentity);

    CARAPI RemoveIdentity(
        /* [in] */ Elastos::Security::IIdentity * pIdentity);

    CARAPI Identities(
        /* [out] */ Elastos::Utility::IEnumeration ** ppIds);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Elastos::Security::IIdentityScope * pScope);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}

#endif // __ORG_APACHE_HARMONY_SECURITY_CSYSTEMSCOPE_H__
