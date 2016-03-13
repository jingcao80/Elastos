
#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_BRIGHTNESSDIALOG_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_BRIGHTNESSDIALOG_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/systemui/settings/BrightnessController.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::SystemUI::Settings::BrightnessController;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

/** A dialog that provides controls for adjusting the screen brightness. */
class BrightnessDialog
    : public Activity
{
protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnStop();

    // @Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

private:
    AutoPtr<BrightnessController> mBrightnessController;
};

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_BRIGHTNESSDIALOG_H__