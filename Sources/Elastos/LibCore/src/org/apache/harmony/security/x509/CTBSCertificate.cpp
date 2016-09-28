
#include "org/apache/harmony/security/x509/CTBSCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CTBSCertificate)

CAR_INTERFACE_IMPL(CTBSCertificate, Object, ITBSCertificate)

ECode CTBSCertificate::GetVersion(
    /* [out] */ Int32 * pVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSerialNumber(
    /* [out] */ IBigInteger** ppSerialNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSignature(
    /* [out] */ IAlgorithmIdentifier** ppSignature)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetIssuer(
    /* [out] */ IName** ppIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetValidity(
    /* [out] */ IValidity** ppValidity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSubject(
    /* [out] */ IName** ppSubject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSubjectPublicKeyInfo(
    /* [out] */ ISubjectPublicKeyInfo** ppInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetIssuerUniqueID(
    /* [out, callee] */ ArrayOf<Boolean>** ppUniqueId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSubjectUniqueID(
    /* [out, callee] */ ArrayOf<Boolean>** ppUniqueId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetExtensions(
    /* [out] */ IExtensions** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::constructor(
    /* [in] */ Int32 ver,
    /* [in] */ IBigInteger* pSerialNumber,
    /* [in] */ IAlgorithmIdentifier* pSignature,
    /* [in] */ IName* pIssuer,
    /* [in] */ IValidity* pValidity,
    /* [in] */ IName* pSubject,
    /* [in] */ ISubjectPublicKeyInfo* pSubjectPublicKeyInfo,
    /* [in] */ ArrayOf<Boolean>* pIssuerUniqueID,
    /* [in] */ ArrayOf<Boolean>* pSubjectUniqueID,
    /* [in] */ IExtensions* pExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org