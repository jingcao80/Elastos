#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_ACTIONBARHOMEVIEW_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_ACTIONBARHOMEVIEW_H__

#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

extern "C" const InterfaceID EIID_ActionBarViewHomeView;

class ActionBarViewHomeView : public FrameLayout
{
public:
    ActionBarViewHomeView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL);

    CARAPI SetUp(
        /* [in] */ Boolean isUp);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI_(Boolean) DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI_(Boolean) DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    CARAPI OnFinishInflate();

    CARAPI_(Int32) GetStartOffset();

    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

protected:
    ActionBarViewHomeView();

private:
    AutoPtr<IView> mUpView;
    AutoPtr<IImageView> mIconView;
    Int32 mUpWidth;
    static const Int64 DEFAULT_TRANSITION_DURATION = 150;
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_INTERNAL_ACTIONBARHOMEVIEW_H__
