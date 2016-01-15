
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOWSAVEDSTATE_H__
#define  __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOWSAVEDSTATE_H__

#include "_Elastos_Droid_Internal_Policy_Impl_CPhoneWindowSavedState.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

CarClass(CPhoneWindowSavedState)
{
public:
    CPhoneWindowSavedState();

    CARAPI constructor();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

public:
    Int32 mFeatureId;
    Boolean mIsOpen;
    Boolean mIsInExpandedMode;
    AutoPtr<IBundle> mMenuState;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namepsace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CPHONEWINDOWSAVEDSTATE_H__
