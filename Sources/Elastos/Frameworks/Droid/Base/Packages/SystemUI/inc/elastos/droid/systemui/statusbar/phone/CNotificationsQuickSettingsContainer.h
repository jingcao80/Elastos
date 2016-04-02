
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CNotificationsQuickSettingsContainer.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::View::IViewStub;
using Elastos::Droid::View::IViewStubOnInflateListener;
using Elastos::Droid::View::IWindowInsets;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * The container with notification stack scroller and quick settings inside.
 */
CarClass(CNotificationsQuickSettingsContainer)
    , public FrameLayout
    , public INotificationsQuickSettingsContainer
    , public IViewStubOnInflateListener
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CNotificationsQuickSettingsContainer();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI OnApplyWindowInsets(
        /* [in] */ IWindowInsets* insets,
        /* [out] */ IWindowInsets** result);

    // @Override
    CARAPI OnInflate(
        /* [in] */ IViewStub* stub,
        /* [in] */ IView* inflated);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(Boolean) DrawChild(
        /* [in] */ ICanvas* canvas,
        /* [in] */ IView* child,
        /* [in] */ Int64 drawingTime);

private:
    AutoPtr<IView> mScrollView;
    AutoPtr<IView> mUserSwitcher;
    AutoPtr<IView> mStackScroller;
    AutoPtr<IView> mKeyguardStatusBar;
    Boolean mInflated;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDINDICATIONTEXTVIEW_H__
