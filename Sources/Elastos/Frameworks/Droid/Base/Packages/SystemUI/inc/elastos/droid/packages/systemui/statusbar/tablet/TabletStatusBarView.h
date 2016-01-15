#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_TABLESTATUSBARVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_TABLESTATUSBARVIEW_H__

#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/systemui/statusbar/DelegateViewHelper.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::SystemUI::StatusBar::DelegateViewHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


class TabletStatusBarView : public FrameLayout
{
public:
    TabletStatusBarView();

    TabletStatusBarView(
        /* [in] */ IContext* context);

    TabletStatusBarView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetDelegateView(
        /* [in] */ IView* view);

    CARAPI SetBar(
        /* [in] */ IBaseStatusBar* bar);

    //@Override
    virtual Boolean OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    //@Override
    virtual Boolean OnInterceptTouchEvent(
        /* [in] */ IMotionEvent*  ev);

    CARAPI SetHandler(
        /* [in] */ IHandler* h);

    /**
     * Let the status bar know that if you tap on ignore while panel is showing, don't do anything.
     *
     * Debounces taps on, say, a popup's trigger when the popup is already showing.
     */
    CARAPI SetIgnoreChildren(
        /* [in] */ Int32 index,
        /* [in] */ IView* ignore,
        /* [in] */ IView* panel);

    CARAPI SetShowVolume(
        /* [in] */ Boolean show,
        /* [in] */ IContext* mContext);

protected:
    CARAPI Init(
        /* [in] */ IContext* context);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    virtual void OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    virtual CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    Boolean EventInside(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* ev);

private:
    static const Boolean DEBUG;

    AutoPtr<IHandler> mHandler;

    static const Int32 MAX_PANELS;// = 5;
    static const Int32 PORT_WIDTH_DP;// = 650;

    Boolean mDisplayLock;// = true;
    AutoPtr<ArrayOf<IView*> > mIgnoreChildren;// = new View[MAX_PANELS];
    AutoPtr<ArrayOf<IView*> > mPanels;// = new View[MAX_PANELS];
    Int32 mPos[2];
    AutoPtr<DelegateViewHelper> mDelegateHelper;

};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_TABLESTATUSBARVIEW_H__
