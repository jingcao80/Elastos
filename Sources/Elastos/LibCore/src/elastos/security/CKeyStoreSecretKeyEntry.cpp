
#include "CKeyStoreSecretKeyEntry.h"
#include "StringBuilder.h"

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyStoreSecretKeyEntry);
CAR_INTERFACE_IMPL_2(CKeyStoreSecretKeyEntry, Object, IKeyStoreSecretKeyEntry, IKeyStoreEntry);
ECode CKeyStoreSecretKeyEntry::GetSecretKey(
    /* [out] */ ISecretKey **key)
{
    VALIDATE_NOT_NULL(key)
    *key = mSecretKey;
    REFCOUNT_ADD(*key)
    return NOERROR;
}

ECode CKeyStoreSecretKeyEntry::ToString(
    /* [out] */ String *str)
{
    StringBuilder sb("SecretKeyEntry: algorithm - ");
    String algo;
    mSecretKey->GetAlgorithm(&algo);
    sb.AppendString(algo);
    return sb.ToString(str);
}

ECode CKeyStoreSecretKeyEntry::constructor(
    /* [in] */ ISecretKey *secretKey)
{
    if (secretKey == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mSecretKey = secretKey;
    return NOERROR;
}

}
}

