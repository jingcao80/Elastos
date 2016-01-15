#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_INTRUDERALERTVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_INTRUDERALERTVIEW_H__

#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::IRemoteViews;
using Elastos::Droid::SystemUI::ISwipeHelper;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class IntruderAlertView : public LinearLayout
{
public:
    IntruderAlertView();

    IntruderAlertView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    IntruderAlertView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetBar(
        /* [in] */ IBaseStatusBar* bar);

    CARAPI ApplyIntruderContent(
        /* [in] */ IRemoteViews* intruderView,
        /* [in] */ IViewOnClickListener* listener);

    //@Override
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI_(Boolean) OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    virtual CARAPI_(Boolean) OnTouchEvent(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(AutoPtr<IView>) GetChildAtPosition(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(AutoPtr<IView>) GetChildContentView(
        /* [in] */ IView* v);

    CARAPI_(Boolean) CanChildBeDismissed(
        /* [in] */ IView* v);

    CARAPI OnBeginDrag(
        /* [in] */ IView* v);

    CARAPI OnChildDismissed(
        /* [in] */ IView* v);

    CARAPI OnDragCancelled(
        /* [in] */ IView* v);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* c);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //override
    virtual void OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    static const String TAG;// = "IntruderAlertView";
    static const Boolean DEBUG;// = false;

    AutoPtr<IRect> mTmpRect;// = new Rect();

    AutoPtr<ISwipeHelper> mSwipeHelper;

    AutoPtr<IBaseStatusBar> mBar;
    AutoPtr<IViewGroup> mContentHolder;

    AutoPtr<IRemoteViews> mIntruderRemoteViews;
    AutoPtr<IViewOnClickListener> mOnClickListener;
};

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_INTRUDERALERTVIEW_H__
