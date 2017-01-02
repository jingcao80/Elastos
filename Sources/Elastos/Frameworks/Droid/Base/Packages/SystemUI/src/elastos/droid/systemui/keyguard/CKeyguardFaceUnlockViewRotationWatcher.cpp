//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
