
#include "CKeyPair.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CKeyPair)
CAR_INTERFACE_IMPL_2(CKeyPair, Object, IKeyPair, ISerializable)
ECode CKeyPair::constructor(
    /* [in] */ IPublicKey* publicKey,
    /* [in] */ IPrivateKey* privateKey)
{
    mPrivateKey = privateKey;
    mPublicKey = publicKey;
    return NOERROR;
}

ECode CKeyPair::GetPrivate(
    /* [out] */ IPrivateKey** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPrivateKey;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CKeyPair::GetPublic(
    /* [out] */ IPublicKey** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPublicKey;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

}
}
