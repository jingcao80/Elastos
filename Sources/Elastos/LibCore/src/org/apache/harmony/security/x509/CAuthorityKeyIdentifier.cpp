
#include "CAuthorityKeyIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CAuthorityKeyIdentifier)

CAR_INTERFACE_IMPL(CAuthorityKeyIdentifier, Object, IAuthorityKeyIdentifier)

ECode CAuthorityKeyIdentifier::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte> ** ppEncode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::DumpValue(
    /* [in] */ Elastos::Core::IStringBuilder * pSb,
    /* [in] */ const String& prefix)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::DumpValueEx(
    /* [in] */ Elastos::Core::IStringBuilder * pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::GetKeyIdentifier(
    /* [out, callee] */ ArrayOf<Byte> ** ppKeyIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::GetAuthorityCertIssuer(
    /* [out] */ Org::Apache::Harmony::Security::X509::IGeneralNames ** ppIssuer)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::GetAuthorityCertSerialNumber(
    /* [out] */ Elastos::Math::IBigInteger ** ppNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CAuthorityKeyIdentifier::constructor(
    /* [in] */ ArrayOf<Byte> * pKeyIdentifier,
    /* [in] */ Org::Apache::Harmony::Security::X509::IGeneralNames * pAuthorityCertIssuer,
    /* [in] */ Elastos::Math::IBigInteger * pAuthorityCertSerialNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}

