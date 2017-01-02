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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSERVICEWRAPPER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSERVICEWRAPPER_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Policy::IIKeyguardExitCallback;
using Elastos::Droid::Internal::Policy::IIKeyguardService;
using Elastos::Droid::Internal::Policy::IIKeyguardShowCallback;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {
namespace Keyguard {

class KeyguardServiceWrapper
    : public Object
    , public IIKeyguardService
{
public:
    CAR_INTERFACE_DECL()

    KeyguardServiceWrapper();

    CARAPI constructor(
        /* [in] */ IIKeyguardService* service);

    CARAPI IsShowing(
        /* [out] */ Boolean* showing);

    CARAPI IsSecure(
        /* [out] */ Boolean* secure);

    CARAPI IsShowingAndNotOccluded(
        /* [out] */ Boolean* showingAndNotOcclude);

    CARAPI IsInputRestricted(
        /* [out] */ Boolean* intpuRestricted);

    CARAPI IsDismissable(
        /* [out] */ Boolean* dismissable);

    CARAPI VerifyUnlock(
        /* [in] */ IIKeyguardExitCallback* cb);

    CARAPI KeyguardDone(
        /* [in] */ Boolean authenticated,
        /* [in] */ Boolean wakeup);

    CARAPI SetOccluded(
        /* [in] */ Boolean isOccluded,
        /* [out] */ Int32* result);

    CARAPI Dismiss();

    CARAPI OnDreamingStarted();

    CARAPI OnDreamingStopped();

    CARAPI OnScreenTurnedOff(
        /* [in] */ Int32 reason);

    CARAPI OnScreenTurnedOn(
        /* [in] */ IIKeyguardShowCallback* cb);

    CARAPI SetKeyguardEnabled(
        /* [in] */ Boolean enabled);

    CARAPI OnSystemReady();

    CARAPI DoKeyguardTimeout(
        /* [in] */ IBundle* options);

    CARAPI SetCurrentUser(
        /* [in] */ Int32 userId);

    CARAPI ShowAssistant();

    CARAPI Dispatch(
        /* [in] */ IMotionEvent* event);

    CARAPI LaunchCamera();

    CARAPI OnBootCompleted();

    CARAPI StartKeyguardExitAnimation(
        /* [in] */ Int64 startTime,
        /* [in] */ Int64 fadeoutDuration);

    CARAPI OnActivityDrawn();

    CARAPI AsBinder(
        /* [out] */ IBinder** binder);

private:
    AutoPtr<IIKeyguardService> mService;
    static String TAG;
};

} // namespace Keyguard
} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_KEYGUARD_KEYGUARDSERVICEWRAPPER_H__
