
#include "CTBSCertList.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CTBSCertList)

CAR_INTERFACE_IMPL(CTBSCertList, Object, ITBSCertList)

ECode CTBSCertList::GetVersion(
    /* [out] */ Int32 * pVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetSignature(
    /* [out] */ Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier ** ppSignature)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetIssuer(
    /* [out] */ Org::Apache::Harmony::Security::X501::IName ** ppIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetThisUpdate(
    /* [out] */ Elastos::Utility::IDate ** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetNextUpdate(
    /* [out] */ Elastos::Utility::IDate ** ppDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetRevokedCertificates(
    /* [out] */ Elastos::Utility::IList ** ppCertificates)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetCrlExtensions(
    /* [out] */ Org::Apache::Harmony::Security::X509::IExtensions ** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::Equals(
    /* [in] */ IInterface * pRc,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::GetHashCode(
    /* [out] */ Int32 * pHashCode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertList::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

