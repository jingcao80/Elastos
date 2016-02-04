
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/pm/KeySet.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(KeySet, Object, IKeySet, IParcelable)

ECode KeySet::constructor(
    /* [in] */ IBinder* token)
{
    mToken = token;
    return NOERROR;
}

ECode KeySet::constructor()
{
    return NOERROR;
}

KeySet::~KeySet()
{
}

ECode KeySet::GetToken(
    /* [out] */ IBinder** token)
{
    VALIDATE_NOT_NULL(token)
    *token = mToken;
    return NOERROR;
}

ECode KeySet::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IKeySet* ks = IKeySet::Probe(other);
    if (ks) {
        *result = mToken.Get() == ((KeySet*)ks)->mToken.Get();
    }
    return NOERROR;
}

ECode KeySet::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = Object::GetHashCode(mToken);
    return NOERROR;
}

ECode KeySet::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadInterfacePtr((Handle32*)&mToken);
}

ECode KeySet::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteInterfacePtr(mToken);
}


} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos