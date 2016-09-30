
#include "org/apache/harmony/security/x509/AlgorithmIdentifier.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_INTERFACE_IMPL(AlgorithmIdentifier, Object, IAlgorithmIdentifier)

ECode AlgorithmIdentifier::GetAlgorithm(
    /* [out] */ String* pAlgorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::GetAlgorithmName(
    /* [out] */ String* pAlgorithmName)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::GetParameters(
    /* [out, callee] */ ArrayOf<Byte>** ppParameters)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::Equals(
    /* [in] */ IInterface* pAi,
    /* [out] */ Boolean* pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::GetHashCode(
    /* [out] */ Int32* pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::constructor(
    /* [in] */ const String& algorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::constructor(
    /* [in] */ const String& algorithm,
    /* [in] */ ArrayOf<Byte>* pParameters)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::GetASN1(
    /* [out] */ IASN1Sequence** ppAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode AlgorithmIdentifier::SetASN1(
    /* [in] */ IASN1Sequence* pAsn1)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}


} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org