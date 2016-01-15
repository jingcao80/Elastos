#include "elastos/droid/ext/frameworkext.h"
#include "CTransportControlViewSavedState.h"

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {


ECode CTransportControlViewSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return TransportControlViewSavedState::ReadFromParcel(source);
}

ECode CTransportControlViewSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return TransportControlViewSavedState::WriteToParcel(dest);
}

ECode CTransportControlViewSavedState::GetSuperState(
    /* [out] */ IParcelable** superState)
{
    VALIDATE_NOT_NULL(superState);
    AutoPtr<IParcelable> temp = TransportControlViewSavedState::GetSuperState();
    *superState = temp;
    REFCOUNT_ADD(*superState);
    return NOERROR;
}

ECode CTransportControlViewSavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return TransportControlViewSavedState::Init(superState);
}

ECode CTransportControlViewSavedState::constructor()
{
    return TransportControlViewSavedState::Init();
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
