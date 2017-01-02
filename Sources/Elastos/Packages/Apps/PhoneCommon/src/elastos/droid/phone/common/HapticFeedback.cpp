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

#include "Elastos.Droid.Phone.Common.h"
#include "elastos/droid/phone/common/HapticFeedback.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {

const Int64 HapticFeedback::DURATION = 10;  // millisec.
/** Play the haptic pattern only once. */
const Int32 HapticFeedback::NO_REPEAT = -1;

const String HapticFeedback::TAG("HapticFeedback");

CAR_INTERFACE_IMPL(HapticFeedback, Object, IHapticFeedback)

ECode HapticFeedback::constructor()
{
    return NOERROR;
}

ECode HapticFeedback::Init(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled)
{
    mEnabled = enabled;
    if (enabled) {
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::VIBRATOR_SERVICE, (IInterface**)&obj);
        mVibrator = IVibrator::Probe(obj);
        mHapticPattern = ArrayOf<Int64>::Alloc(4);
        (*mHapticPattern)[0] = 0;
        (*mHapticPattern)[1] = DURATION;
        (*mHapticPattern)[2] = 2 * DURATION;
        (*mHapticPattern)[3] = 3 * DURATION;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&mSystemSettings);
        context->GetContentResolver((IContentResolver**)&mContentResolver);
    }
    return NOERROR;
}

/**
 * Reload the system settings to check if the user enabled the
 * haptic feedback.
 */
ECode HapticFeedback::CheckSystemSetting()
{
    if (!mEnabled) {
        return NOERROR;
    }
    // try {
    Int32 val;
    ECode ec = mSystemSettings->GetInt32(mContentResolver, ISettingsSystem::HAPTIC_FEEDBACK_ENABLED, 0, &val);
    if (SUCCEEDED(ec)) {
        mSettingEnabled = val != 0;
    }
    else {
        Logger::E(TAG, "Could not retrieve system setting.");
        mSettingEnabled = FALSE;
    }
    // } catch (Resources.NotFoundException nfe) {
    //     Log.e(TAG, "Could not retrieve system setting.", nfe);
    //     mSettingEnabled = false;
    // }
    return NOERROR;
}

/**
 * Generate the haptic feedback vibration. Only one thread can
 * request it. If the phone is already in a middle of an haptic
 * feedback sequence, the request is ignored.
 */
ECode HapticFeedback::Vibrate()
{
    if (!mEnabled || !mSettingEnabled) {
        return NOERROR;
    }
    // System-wide configuration may return different styles of haptic feedback pattern.
    // - an array with one value implies "one-shot vibration"
    // - an array with multiple values implies "pattern vibration"
    // We need to switch methods to call depending on the difference.
    // See also PhoneWindowManager#performHapticFeedbackLw() for another example.
    if (mHapticPattern != NULL && mHapticPattern->GetLength() == 1) {
        mVibrator->Vibrate((*mHapticPattern)[0]);
    }
    else {
        mVibrator->Vibrate(mHapticPattern, NO_REPEAT);
    }
    return NOERROR;
}

} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos
