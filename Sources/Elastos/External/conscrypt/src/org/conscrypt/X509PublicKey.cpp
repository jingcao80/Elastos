
#include "X509PublicKey.h"
#include "elastos/utility/Arrays.h"

using Elastos::Security::EIID_IPublicKey;
using Elastos::Utility::Arrays;

namespace Org {
namespace Conscrypt {

//=========================================
// X509PublicKey::
//=========================================
Int64 X509PublicKey::mSerialVersionUID = -8610156854731664298L;

CAR_INTERFACE_IMPL_2(X509PublicKey, Object, IPublicKey, IX509PublicKey)

ECode X509PublicKey::constructor(
    /* [in] */ String algorithm,
    /* [in] */ ArrayOf<Byte>* encoded)
{
    mAlgorithm = algorithm;
    mEncoded = encoded;
    return NOERROR;
}

ECode X509PublicKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAlgorithm;
    return NOERROR;
}

ECode X509PublicKey::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("X.509");
    return NOERROR;
}

ECode X509PublicKey::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mEncoded;
    return NOERROR;
}

ECode X509PublicKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("X509PublicKey [algorithm=") + mAlgorithm +
                String(", encoded=") + Arrays::ToString(mEncoded)
            + String("]");
    return NOERROR;
}

ECode X509PublicKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 prime = 31;
    Int32 res = 1;
    res = prime * res + ((mAlgorithm.IsNull()) ? 0 : mAlgorithm.GetHashCode());
    res = prime * res + Arrays::GetHashCode(mEncoded);
    *result = res;
    return NOERROR;
}

ECode X509PublicKey::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (this->Probe(EIID_IInterface) == obj) {
        *result = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    assert(0 && "TODO");
    // if (getClass() != obj.getClass()) {
    //     *result = FALSE;
    //     return NOERROR;
    // }
    AutoPtr<X509PublicKey> other = (X509PublicKey*) IX509PublicKey::Probe(obj);
    if (mAlgorithm.IsNull()) {
        if (other->mAlgorithm != NULL) {
            *result = FALSE;
            return NOERROR;
        }
    }
    else if (!mAlgorithm.Equals(other->mAlgorithm)) {
        *result = FALSE;
        return NOERROR;
    }
    if (!Arrays::Equals(mEncoded, other->mEncoded)) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
