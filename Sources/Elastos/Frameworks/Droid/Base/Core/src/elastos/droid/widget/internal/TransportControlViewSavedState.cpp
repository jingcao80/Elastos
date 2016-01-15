
#include "elastos/droid/internal/TransportControlViewSavedState.h"

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

TransportControlViewSavedState::TransportControlViewSavedState()
    : ViewBaseSavedState()
{}

TransportControlViewSavedState::TransportControlViewSavedState(
    /* [in] */ IParcelable* superState)
    : ViewBaseSavedState(superState)
{}

ECode TransportControlViewSavedState::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    FAIL_RETURN(ViewBaseSavedState::ReadFromParcel(in));
    Int32 res;
    in->ReadInt32(&res);
    this->wasShowing = res != 0;
    return NOERROR;
}

ECode TransportControlViewSavedState::WriteToParcel(
    /* [in] */ IParcel* out)
{
    FAIL_RETURN(ViewBaseSavedState::WriteToParcel(out));
    return out->WriteInt32(this->wasShowing ? 1 : 0);
}

ECode TransportControlViewSavedState::Init()
{
    return ViewBaseSavedState::Init();
}

ECode TransportControlViewSavedState::Init(
    /* [in] */ IParcelable* superState)
{
    return ViewBaseSavedState::Init(superState);
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
