#include "elastos/droid/systemui/settings/BrightnessDialog.h"
#include "Elastos.Droid.Widget.h"
#include "../R.h"

using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

ECode BrightnessDialog::OnCreate(
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
    FindViewById(R::id::brightness_icon, (IView**)&sliderV);
    AutoPtr<ToggleSlider> slider = (ToggleSlider*)sliderV.Get();
    mBrightnessController = new BrightnessController(this, icon, slider);
    return NOERROR;
}

ECode BrightnessDialog::OnStart()
{
    Activity::OnStart();
    mBrightnessController->RegisterCallbacks();
    return NOERROR;
}

ECode BrightnessDialog::OnStop()
{
    Activity::OnStop();
    mBrightnessController->UnregisterCallbacks();
    return NOERROR;
}

ECode BrightnessDialog::OnKeyDown(
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