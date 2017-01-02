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

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/statusbar/phone/BarTransitions.h"
#include "Elastos.Droid.Animation.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class PhoneStatusBarTransitions
    : public BarTransitions
    , public IPhoneStatusBarTransitions
{
public:
    CAR_INTERFACE_DECL()

    PhoneStatusBarTransitions(
        /* [in] */ IPhoneStatusBarView* view);

    CARAPI Init();

    CARAPI AnimateTransitionTo(
        /* [in] */ IView* v,
        /* [in] */ Float toAlpha,
        /* [out] */ IObjectAnimator** result);

protected:
    // @Override
    CARAPI_(void) OnTransition(
        /* [in] */ Int32 oldMode,
        /* [in] */ Int32 newMode,
        /* [in] */ Boolean animate);

private:
    CARAPI_(Float) GetNonBatteryClockAlphaFor(
        /* [in] */ Int32 mode);

    CARAPI_(Float) GetBatteryClockAlpha(
        /* [in] */ Int32 mode);

    CARAPI_(Boolean) IsOpaque(
        /* [in] */ Int32 mode);

    CARAPI_(void) ApplyMode(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean animate);

private:
    static const Float ICON_ALPHA_WHEN_NOT_OPAQUE;
    static const Float ICON_ALPHA_WHEN_LIGHTS_OUT_BATTERY_CLOCK;
    static const Float ICON_ALPHA_WHEN_LIGHTS_OUT_NON_BATTERY_CLOCK;

    AutoPtr<IPhoneStatusBarView> mView;
    Float mIconAlphaWhenOpaque;

    AutoPtr<IView> mLeftSide;
    AutoPtr<IView> mStatusIcons;
    AutoPtr<IView> mSignalCluster;
    AutoPtr<IView> mBattery;
    AutoPtr<IView> mClock;
    AutoPtr<IAnimator> mCurrentAnimation;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__
