
#include "elastos/droid/systemui/settings/CBrightnessDialog.h"
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

CAR_OBJECT_IMPL(CBrightnessDialog)

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