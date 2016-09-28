
#include "CSystemScope.h"

using Elastos::Security::EIID_IIdentityScope;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {

CAR_OBJECT_IMPL(CSystemScope)

CAR_INTERFACE_IMPL_2(CSystemScope, Object, ISystemScope, IIdentityScope)

ECode CSystemScope::AddCertificate(
    /* [in] */ ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::RemoveCertificate(
    /* [in] */ ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::Certificates(
    /* [out, callee] */ ArrayOf<ICertificate *> ** ppCerts)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::ToString(
    /* [in] */ Boolean detailed,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetScope(
    /* [out] */ IIdentityScope ** ppScope)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::SetPublicKey(
    /* [in] */ IPublicKey * pKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetPublicKey(
    /* [out] */ IPublicKey ** ppPubKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::SetInfo(
    /* [in] */ const String& info)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetInfo(
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetSize(
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetIdentity(
    /* [in] */ const String& name,
    /* [out] */ IIdentity ** ppIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetIdentity(
    /* [in] */ IPrincipal * pPrincipal,
    /* [out] */ IIdentity ** ppIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetIdentity(
    /* [in] */ IPublicKey * pKey,
    /* [out] */ IIdentity ** ppIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::AddIdentity(
    /* [in] */ IIdentity * pIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::RemoveIdentity(
    /* [in] */ IIdentity * pIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::Identities(
    /* [out] */ IEnumeration ** ppIds)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::constructor()
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::constructor(
    /* [in] */ const String& name)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::constructor(
    /* [in] */ const String& name,
    /* [in] */ IIdentityScope * pScope)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org