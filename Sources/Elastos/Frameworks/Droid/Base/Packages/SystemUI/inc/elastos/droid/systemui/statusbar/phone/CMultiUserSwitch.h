
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CMULTIUSERSWITCH_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CMULTIUSERSWITCH_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CMultiUserSwitch.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::SystemUI::Qs::IQSPanel;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardUserSwitcher;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Container for image of the multi user switcher (tappable).
 */
CarClass(CMultiUserSwitch)
    , public FrameLayout
    , public IMultiUserSwitch
{
private:
    class ViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewOnClickListener(
            /* [in] */ CMultiUserSwitch* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMultiUserSwitch* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CMultiUserSwitch();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetQsPanel(
        /* [in] */ IQSPanel* qsPanel);

    CARAPI SetKeyguardUserSwitcher(
        /* [in] */ IKeyguardUserSwitcher* keyguardUserSwitcher);

    CARAPI SetKeyguardMode(
        /* [in] */ Boolean keyguardShowing);

    // @Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    // @Override
    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(Boolean) OpensUserSwitcherWhenClicked();

private:
    AutoPtr<IQSPanel> mQsPanel;
    AutoPtr<IKeyguardUserSwitcher> mKeyguardUserSwitcher;
    Boolean mKeyguardMode;
    AutoPtr<IUserManager> mUserManager;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CMULTIUSERSWITCH_H__
