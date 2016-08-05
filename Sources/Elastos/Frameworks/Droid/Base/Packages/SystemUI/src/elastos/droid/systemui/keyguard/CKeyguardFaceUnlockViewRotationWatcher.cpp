
#include "elastos/droid/systemui/keyguard/CKeyguardFaceUnlockViewRotationWatcher.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::View::EIID_IRotationWatcher;
using Elastos::Core::Math;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CKeyguardFaceUnlockViewRotationWatcher)

CAR_INTERFACE_IMPL_2(CKeyguardFaceUnlockViewRotationWatcher, Object, IBinder,
        IRotationWatcher)

ECode CKeyguardFaceUnlockViewRotationWatcher::constructor(
    /* [in] */ IKeyguardFaceUnlockView* host)
{
    mHost = (CKeyguardFaceUnlockView*)host;
    return NOERROR;
}

ECode CKeyguardFaceUnlockViewRotationWatcher::OnRotationChanged(
    /* [in] */ Int32 rotation)
{
    if (mHost->DEBUG) Logger::D("CKeyguardFaceUnlockViewRotationWatcher",
            "onRotationChanged(): %d->%d", mHost->mLastRotation, rotation);

    // If the difference between the new rotation value and the previous rotation value is
    // equal to 2, the rotation change was 180 degrees.  This stops the biometric unlock
    // and starts it in the new position.  This is not performed for 90 degree rotations
    // since a 90 degree rotation is a configuration change, which takes care of this for
    // us.
    if (Elastos::Core::Math::Abs(rotation - mHost->mLastRotation) == 2) {
        if (mHost->mBiometricUnlock != NULL) {
            Boolean res;
            mHost->mBiometricUnlock->Stop(&res);
            mHost->MaybeStartBiometricUnlock();
        }
    }
    mHost->mLastRotation = rotation;
    return NOERROR;
}

ECode CKeyguardFaceUnlockViewRotationWatcher::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    *info = String(NULL);
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
