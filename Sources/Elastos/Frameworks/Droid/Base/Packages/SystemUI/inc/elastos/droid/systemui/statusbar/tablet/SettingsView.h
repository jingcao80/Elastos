#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_SETTINGSVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_SETTINGSVIEW_H__

#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


class SettingsView : public LinearLayout
{
public:
    SettingsView();

    SettingsView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    SettingsView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI OnClick(
        /* [in] */ IView* v);

protected:
    // @Override
    virtual CARAPI OnFinishInflate();

    virtual CARAPI OnDetachedFromWindow();

private:
    AutoPtr<IStatusBarManager> GetStatusBarManager();

    // Network
    // ----------------------------
    void OnClickNetwork();

    // Settings
    // ----------------------------
    void OnClickSettings() ;

protected:
    // AutoPtr<IAirplaneModeController> mAirplane;
 //    AutoPtr<IAutoRotateController> mRotate;
 //    AutoPtr<IBrightnessController> mBrightness;
 //    AutoPtr<IDoNotDisturbController> mDoNotDisturb;
    AutoPtr<IView> mRotationLockContainer;
    AutoPtr<IView> mRotationLockSeparator;

private:
    static const String TAG;
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_SETTINGSVIEW_H__
