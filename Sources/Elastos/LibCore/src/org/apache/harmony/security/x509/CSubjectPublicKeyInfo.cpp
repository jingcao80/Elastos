
#include "org/apache/harmony/security/x509/CSubjectPublicKeyInfo.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CSubjectPublicKeyInfo)

CAR_INTERFACE_IMPL(CSubjectPublicKeyInfo, Object, ISubjectPublicKeyInfo)

ECode CSubjectPublicKeyInfo::GetAlgorithmIdentifier(
    /* [out] */ IAlgorithmIdentifier** ppAlgorithmIdentifier)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectPublicKeyInfo::GetSubjectPublicKey(
    /* [out, callee] */ ArrayOf<Byte>** ppPublicKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectPublicKeyInfo::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectPublicKeyInfo::GetPublicKey(
    /* [out] */ IPublicKey** ppPublicKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectPublicKeyInfo::constructor(
    /* [in] */ IAlgorithmIdentifier* pAlgID,
    /* [in] */ ArrayOf<Byte>* pSubjectPublicKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSubjectPublicKeyInfo::constructor(
    /* [in] */ IAlgorithmIdentifier* pAlgID,
    /* [in] */ ArrayOf<Byte>* pSubjectPublicKey,
    /* [in] */ Int32 unused)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org