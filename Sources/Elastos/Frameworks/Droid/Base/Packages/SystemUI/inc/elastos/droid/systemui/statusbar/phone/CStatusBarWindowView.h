
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSTATUSBARWINDOWVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSTATUSBARWINDOWVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CStatusBarWindowView.h"
#include "elastos/droid/systemui/statusbar/DragDownHelper.h"
#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/core/Object.h>

using Elastos::Droid::SystemUI::StatusBar::Stack::INotificationStackScrollLayout;
using Elastos::Droid::SystemUI::StatusBar::DragDownHelper;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CStatusBarWindowView)
    , public FrameLayout
    , public IStatusBarWindowView
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    // @Override
    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    CARAPI CancelExpandHelper();

protected:
    // @Override
    CARAPI FitSystemWindows(
        /* [in] */ IRect* insets,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnAttachedToWindow ();

public:
    static const String TAG;
    static const Boolean DEBUG;
    IPhoneStatusBar* mService;

private:
    AutoPtr<DragDownHelper> mDragDownHelper;
    AutoPtr<INotificationStackScrollLayout> mStackScrollLayout;
    AutoPtr<INotificationPanelView> mNotificationPanel;
    AutoPtr<IView> mBrightnessMirror;

    AutoPtr<IPaint> mTransparentSrcPaint;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CSTATUSBARWINDOWVIEW_H__
