
#ifndef __ELASTOS_DROID_WIDGET_ZOOMCONTROLS_H__
#define __ELASTOS_DROID_WIDGET_ZOOMCONTROLS_H__

#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * The {@code ZoomControls} class displays a simple set of controls used for zooming and
 * provides callbacks to register for events. */
class ZoomControls
    : public LinearLayout
    , public IZoomControls
{
public:
    CAR_INTERFACE_DECL()

    ZoomControls();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetOnZoomInClickListener(
        /* [in] */ IViewOnClickListener* listener);

    CARAPI SetOnZoomOutClickListener(
        /* [in] */ IViewOnClickListener* listener);

    /*
     * Sets how fast you get zoom events when the user holds down the
     * zoom in/out buttons.
     */
    CARAPI SetZoomSpeed(
        /* [in] */ Int64 speed);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI Show();

    CARAPI Hide();

    CARAPI SetIsZoomInEnabled(
        /* [in] */ Boolean isEnabled);

    CARAPI SetIsZoomOutEnabled(
        /* [in] */ Boolean isEnabled);

    // @Override
    CARAPI HasFocus(
        /* [out] */ Boolean* has);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

private:
    CARAPI Fade(
        /* [in] */ Int32 visibility,
        /* [in] */ Float startAlpha,
        /* [in] */ Float endAlpha);

    AutoPtr<IZoomButton> mZoomIn;
    AutoPtr<IZoomButton> mZoomOut;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_ZOOMCONTROLS_H__
