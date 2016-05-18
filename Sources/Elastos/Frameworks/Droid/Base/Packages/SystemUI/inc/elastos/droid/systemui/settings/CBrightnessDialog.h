
#ifndef __ELASTOS_DROID_SYSTEMUI_SETTINGS_CBRIGHTNESSDIALOG_H__
#define __ELASTOS_DROID_SYSTEMUI_SETTINGS_CBRIGHTNESSDIALOG_H__

#include "_Elastos_Droid_SystemUI_Settings_CBrightnessDialog.h"
#include "elastos/droid/systemui/settings/BrightnessController.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::SystemUI::Settings::BrightnessController;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

/** A dialog that provides controls for adjusting the screen brightness. */
CarClass(CBrightnessDialog), public Activity
{
public:
    CAR_OBJECT_DECL()

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

#endif //__ELASTOS_DROID_SYSTEMUI_SETTINGS_CBRIGHTNESSDIALOG_H__
