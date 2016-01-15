#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_COMPATMODEPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_COMPATMODEPANEL_H__

#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IRadioButton;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


class CompatModePanel : public FrameLayout
{
public:
    CompatModePanel();

    CompatModePanel(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    //@Override
    virtual CARAPI OnFinishInflate();

    //@Override
    virtual Boolean IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    virtual CARAPI_(Boolean) DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI SetTrigger(
        /* [in] */ IView* v);

    CARAPI OpenPanel();

    CARAPI ClosePanel();

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

private:
    CARAPI_(void) Refresh();

private:
    static const String TAG;

    AutoPtr<IActivityManager> mAM;
    Boolean mAttached;// = false;
    AutoPtr<IContext> mContext;
    AutoPtr<IRadioButton> mOnButton;
    AutoPtr<IRadioButton> mOffButton;

    AutoPtr<IView> mTrigger;
    //    AutoPtr<IInputMethodButton> mInputMethodSwitchButton;
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_COMPATMODEPANEL_H__
