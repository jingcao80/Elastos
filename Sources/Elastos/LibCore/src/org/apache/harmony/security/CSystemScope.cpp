
#include "CSystemScope.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {

CAR_OBJECT_IMPL(CSystemScope)

CAR_INTERFACE_IMPL(CSystemScope, Object, IIdentityScope)

ECode CSystemScope::AddCertificate(
    /* [in] */ Elastos::Security::ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::RemoveCertificate(
    /* [in] */ Elastos::Security::ICertificate * pCertificate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::Certificates(
    /* [out, callee] */ ArrayOf<Elastos::Security::ICertificate *> ** ppCerts)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::ToStringEx(
    /* [in] */ Boolean detailed,
    /* [out] */ String * pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetScope(
    /* [out] */ Elastos::Security::IIdentityScope ** ppScope)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::SetPublicKey(
    /* [in] */ Elastos::Security::IPublicKey * pKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetPublicKey(
    /* [out] */ Elastos::Security::IPublicKey ** ppPubKey)
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
    /* [out] */ Elastos::Security::IIdentity ** ppIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetIdentityEx(
    /* [in] */ Elastos::Security::IPrincipal * pPrincipal,
    /* [out] */ Elastos::Security::IIdentity ** ppIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::GetIdentityEx2(
    /* [in] */ Elastos::Security::IPublicKey * pKey,
    /* [out] */ Elastos::Security::IIdentity ** ppIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::AddIdentity(
    /* [in] */ Elastos::Security::IIdentity * pIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::RemoveIdentity(
    /* [in] */ Elastos::Security::IIdentity * pIdentity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSystemScope::Identities(
    /* [out] */ Elastos::Utility::IEnumeration ** ppIds)
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
    /* [in] */ Elastos::Security::IIdentityScope * pScope)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}

