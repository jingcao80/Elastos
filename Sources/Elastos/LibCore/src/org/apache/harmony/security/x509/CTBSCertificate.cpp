
#include "CTBSCertificate.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {


ECode CTBSCertificate::GetVersion(
    /* [out] */ Int32 * pVer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSerialNumber(
    /* [out] */ Elastos::Math::IBigInteger ** ppSerialNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSignature(
    /* [out] */ Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier ** ppSignature)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetIssuer(
    /* [out] */ Org::Apache::Harmony::Security::X501::IName ** ppIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetValidity(
    /* [out] */ Org::Apache::Harmony::Security::X509::IValidity ** ppValidity)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSubject(
    /* [out] */ Org::Apache::Harmony::Security::X501::IName ** ppSubject)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSubjectPublicKeyInfo(
    /* [out] */ Org::Apache::Harmony::Security::X509::ISubjectPublicKeyInfo ** ppInfo)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetIssuerUniqueID(
    /* [out, callee] */ ArrayOf<Boolean> ** ppUniqueId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetSubjectUniqueID(
    /* [out, callee] */ ArrayOf<Boolean> ** ppUniqueId)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetExtensions(
    /* [out] */ Org::Apache::Harmony::Security::X509::IExtensions ** ppExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTBSCertificate::constructor(
    /* [in] */ Int32 ver,
    /* [in] */ Elastos::Math::IBigInteger * pSerialNumber,
    /* [in] */ Org::Apache::Harmony::Security::X509::IAlgorithmIdentifier * pSignature,
    /* [in] */ Org::Apache::Harmony::Security::X501::IName * pIssuer,
    /* [in] */ Org::Apache::Harmony::Security::X509::IValidity * pValidity,
    /* [in] */ Org::Apache::Harmony::Security::X501::IName * pSubject,
    /* [in] */ Org::Apache::Harmony::Security::X509::ISubjectPublicKeyInfo * pSubjectPublicKeyInfo,
    /* [in] */ ArrayOf<Boolean> * pIssuerUniqueID,
    /* [in] */ ArrayOf<Boolean> * pSubjectUniqueID,
    /* [in] */ Org::Apache::Harmony::Security::X509::IExtensions * pExtensions)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

