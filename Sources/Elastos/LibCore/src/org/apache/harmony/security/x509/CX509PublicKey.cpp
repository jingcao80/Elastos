
#include "org/apache/harmony/security/x509/CX509PublicKey.h"

using Elastos::Security::EIID_IPublicKey;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CX509PublicKey)

CAR_INTERFACE_IMPL_2(CX509PublicKey, Object, IX509PublicKey, IPublicKey)

ECode CX509PublicKey::GetAlgorithm(
    /* [out] */ String* pAlgorithm)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509PublicKey::GetFormat(
    /* [out] */ String* pFormat)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509PublicKey::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509PublicKey::ToString(
    /* [out] */ String* pStr)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CX509PublicKey::constructor(
    /* [in] */ const String& algorithm,
    /* [in] */ ArrayOf<Byte>* pEncoded,
    /* [in] */ ArrayOf<Byte>* pKeyBytes)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org