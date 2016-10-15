
#include "org/apache/harmony/security/x509/CX509PublicKey.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;
using Elastos::IO::EIID_ISerializable;
using Elastos::Security::EIID_IPublicKey;
using Elastos::Security::EIID_IKey;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CAR_OBJECT_IMPL(CX509PublicKey)
CAR_INTERFACE_IMPL_4(CX509PublicKey, Object, IX509PublicKey, IPublicKey, IKey, ISerializable)

ECode CX509PublicKey::GetAlgorithm(
    /* [out] */ String* pAlgorithm)
{
    VALIDATE_NOT_NULL(pAlgorithm);
    *pAlgorithm = mAlgorithm;
    return NOERROR;
}

ECode CX509PublicKey::GetFormat(
    /* [out] */ String* pFormat)
{
    VALIDATE_NOT_NULL(pFormat);
    *pFormat = String("X.509");
    return NOERROR;
}

ECode CX509PublicKey::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded);
    *ppEncoded = mEncoded;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CX509PublicKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder buf("algorithm = ");
    buf.Append(mAlgorithm);
    buf.Append(String(", params unparsed, unparsed keybits = \n"));
    // TODO: implement compatible toString method()
    // buf.append(Arrays.toString(keyBytes));
    return buf.ToString(result);
}

ECode CX509PublicKey::constructor(
    /* [in] */ const String& algorithm,
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [in] */ ArrayOf<Byte>* keyBytes)
{
    mAlgorithm = algorithm;
    mEncoded = encoded;
    mKeyBytes = keyBytes;
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org