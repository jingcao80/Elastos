
#include "elastos/droid/impl/CPhoneWindowSavedState.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CPhoneWindowSavedState::CPhoneWindowSavedState()
    : mFeatureId(0)
    , mIsOpen(FALSE)
    , mIsInExpandedMode(FALSE)
{
}

ECode CPhoneWindowSavedState::constructor()
{
    return NOERROR;
}

ECode CPhoneWindowSavedState::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mFeatureId);
    dest->WriteBoolean(mIsOpen);
    dest->WriteBoolean(mIsInExpandedMode);

    if (mIsOpen) {
        dest->WriteInterfacePtr(mMenuState);
    }
    return NOERROR;
}

ECode CPhoneWindowSavedState::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mFeatureId);
    source->ReadBoolean(&mIsOpen);
    source->ReadBoolean(&mIsInExpandedMode);

    if (mIsOpen) {
        source->ReadInterfacePtr((Handle32*)&mMenuState);
    }
    return NOERROR;
}

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
