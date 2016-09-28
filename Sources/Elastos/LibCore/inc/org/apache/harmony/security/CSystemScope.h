
#ifndef __ORG_APACHE_HARMONY_SECURITY_CSYSTEMSCOPE_H__
#define __ORG_APACHE_HARMONY_SECURITY_CSYSTEMSCOPE_H__

#include "_Org_Apache_Harmony_Security_CSystemScope.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IEnumeration;
using Elastos::Security::IIdentityScope;
using Elastos::Security::ICertificate;
using Elastos::Security::IPublicKey;
using Elastos::Security::IIdentity;
using Elastos::Security::IPrincipal;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {

CarClass(CSystemScope)
    , public Object
    , public ISystemScope
    , public IIdentityScope
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI AddCertificate(
        /* [in] */ ICertificate* pCertificate);

    CARAPI RemoveCertificate(
        /* [in] */ ICertificate* pCertificate);

    CARAPI Certificates(
        /* [out, callee] */ ArrayOf<ICertificate*>** ppCerts);

    CARAPI ToString(
        /* [in] */ Boolean detailed,
        /* [out] */ String* pStr);

    CARAPI GetScope(
        /* [out] */ IIdentityScope** ppScope);

    CARAPI SetPublicKey(
        /* [in] */ IPublicKey* pKey);

    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** ppPubKey);

    CARAPI SetInfo(
        /* [in] */ const String& info);

    CARAPI GetInfo(
        /* [out] */ String* pStr);

    CARAPI GetSize(
        /* [out] */ Int32* pSize);

    CARAPI GetIdentity(
        /* [in] */ const String& name,
        /* [out] */ IIdentity** ppIdentity);

    CARAPI GetIdentity(
        /* [in] */ IPrincipal* pPrincipal,
        /* [out] */ IIdentity** ppIdentity);

    CARAPI GetIdentity(
        /* [in] */ IPublicKey* pKey,
        /* [out] */ IIdentity** ppIdentity);

    CARAPI AddIdentity(
        /* [in] */ IIdentity* pIdentity);

    CARAPI RemoveIdentity(
        /* [in] */ IIdentity* pIdentity);

    CARAPI Identities(
        /* [out] */ IEnumeration** ppIds);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IIdentityScope* pScope);

private:
    // TODO: Add your private member variables here.
};

} //namespace Security
} //namespace Harmony
} //namespace Apache
} //namespace Org

#endif // __ORG_APACHE_HARMONY_SECURITY_CSYSTEMSCOPE_H__
