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

#include "elastos/droid/systemui/settings/CBrightnessDialog.h"
#include "Elastos.Droid.Widget.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::IImageView;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

static const String TAG("CBrightnessDialog");

CAR_OBJECT_IMPL(CBrightnessDialog)

ECode CBrightnessDialog::constructor()
{
    return Activity::constructor();
}

ECode CBrightnessDialog::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);

    window->SetGravity(IGravity::TOP);
    window->ClearFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);
    Boolean result;
    window->RequestFeature(IWindow::FEATURE_NO_TITLE, &result);

    SetContentView(R::layout::quick_settings_brightness_dialog);

    AutoPtr<IView> iconV;
    FindViewById(R::id::brightness_icon, (IView**)&iconV);
    AutoPtr<IImageView> icon = IImageView::Probe(iconV);
    AutoPtr<IView> sliderV;
    FindViewById(R::id::brightness_slider, (IView**)&sliderV);
    mBrightnessController = new BrightnessController();
    mBrightnessController->constructor(this, icon, IToggleSlider::Probe(sliderV));
    return NOERROR;
}

ECode CBrightnessDialog::OnStart()
{
    Activity::OnStart();
    mBrightnessController->RegisterCallbacks();
    return NOERROR;
}

ECode CBrightnessDialog::OnStop()
{
    Activity::OnStop();
    mBrightnessController->UnregisterCallbacks();
    return NOERROR;
}

ECode CBrightnessDialog::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN
            || keyCode == IKeyEvent::KEYCODE_VOLUME_UP
            || keyCode == IKeyEvent::KEYCODE_VOLUME_MUTE) {
        Finish();
    }

    return Activity::OnKeyDown(keyCode, event, result);
}

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos