#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NOTIFICATIONROWLAYOUT_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NOTIFICATIONROWLAYOUT_H__

#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::SystemUI::ISwipeHelper;
using Elastos::Droid::SystemUI::StatusBar::Policy::IOnSizeChangedListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class NotificationRowLayout : public LinearLayout
{
public:
    NotificationRowLayout();

    NotificationRowLayout(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    NotificationRowLayout(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetLongPressListener(
        /* [in] */ IViewOnLongClickListener* listener);

    CARAPI SetOnSizeChangedListener(
        /* [in] */ IOnSizeChangedListener* l);

    CARAPI SetAnimateBounds(
        /* [in] */ Boolean anim);

    CARAPI_(Boolean) CanChildBeDismissed(
        /* [in] */ IView* v);

    CARAPI_(Boolean) CanChildBeExpanded(
        /* [in] */ IView* v);

    CARAPI_(Boolean) SetUserExpandedChild(
        /* [in] */ IView* v,
        /* [in] */ Boolean userExpanded);

    CARAPI_(Boolean) SetUserLockedChild(
        /* [in] */ IView* v,
        /* [in] */ Boolean userLocked);

    CARAPI OnChildDismissed(
        /* [in] */ IView* v);

    CARAPI OnBeginDrag(
        /* [in] */ IView* v);

    CARAPI OnDragCancelled(
        /* [in] */ IView* v);

    CARAPI_(AutoPtr<IView>) GetChildAtPosition(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(AutoPtr<IView>) GetChildAtRawPosition(
        /* [in] */ Float touchX,
        /* [in] */ Float touchY);

    CARAPI_(AutoPtr<IView>) GetChildAtPosition(
        /* [in] */ Float touchX,
        /* [in] */ Float touchY);

    CARAPI_(AutoPtr<IView>) GetChildContentView(
        /* [in] */ IView* v);

    CARAPI SetViewRemoval(
        /* [in] */ Boolean removeViews);

    CARAPI SetLayoutTransitionsEnabled(
        /* [in] */ Boolean b);

    CARAPI DismissRowAnimated(
        /* [in] */ IView* child);

    CARAPI DismissRowAnimated(
        /* [in] */ IView* child,
        /* [in] */ Int32 vel);

    CARAPI OnWindowFocusChanged(
        /* [in] */ Boolean hasWindowFocus);

    virtual Boolean OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    virtual Boolean OnTouchEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI OnFinishInflate();

    virtual void OnDraw(
        /* [in] */ ICanvas* c);

protected:
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    CARAPI Init(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI InitImpl(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);
private:
    CARAPI_(void) LogLayoutTransition();

private:
    static const String TAG;// = "NotificationRowLayout";
    static const Boolean DEBUG;// = FALSE;
    static const Boolean SLOW_ANIMATIONS;// = DEBUG;

    static const Int32 APPEAR_ANIM_LEN;// = SLOW_ANIMATIONS ? 5000 : 250;
    static const Int32 DISAPPEAR_ANIM_LEN;// = APPEAR_ANIM_LEN;

    Boolean mAnimateBounds; // = TRUE;

    AutoPtr<IRect> mTmpRect; // = new Rect();

    HashMap<AutoPtr<IView>, AutoPtr<IValueAnimator> > mAppearingViews; // = new Map<AutoPtr<IView>, AutoPtr<IValueAnimator> >();
    HashMap<AutoPtr<IView>, AutoPtr<IValueAnimator> > mDisappearingViews; // = new Map<AutoPtr<IView>, AutoPtr<IValueAnimator> >();

    AutoPtr<ISwipeHelper> mSwipeHelper;

    AutoPtr<IOnSizeChangedListener> mOnSizeChangedListener;

    // Flag set during notification removal animation to avoid causing too much work until
    // animation is done
    Boolean mRemoveViews; // = TRUE;

    AutoPtr<ILayoutTransition> mRealLayoutTransition;

};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_NOTIFICATIONROWLAYOUT_H__
