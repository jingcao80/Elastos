
#ifndef __ELASTOS_DROID_WIDGET_ZOOMBUTTON_H__
#define __ELASTOS_DROID_WIDGET_ZOOMBUTTON_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/widget/ImageButton.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnLongClickListener;

namespace Elastos {
namespace Droid {
namespace Widget {

class ZoomButton
    : public ImageButton
    , public IZoomButton
    , public IViewOnLongClickListener
{
private:
    class ZoomButtonRunnable
        : public Runnable
    {
    public:
        ZoomButtonRunnable(
            /* [in] */ ZoomButton* host);

        CARAPI Run();

    private:
        ZoomButton* mHost;
    };

    class LongClickListener
        : public Object
        , public IViewOnLongClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        LongClickListener(
            /* [in] */ ZoomButton* host);

        CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);

    private:
        ZoomButton* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    ZoomButton();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetZoomSpeed(
        /* [in] */ Int64 speed);

    CARAPI OnLongClick(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI DispatchUnhandledMove(
        /* [in] */ IView* focused,
        /* [in] */ Int32 direction,
        /* [out] */ Boolean* result);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* event);

private:
    AutoPtr<IHandler> mHandler;
    AutoPtr<IRunnable> mRunnable;

    Int64 mZoomSpeed;
    Boolean mIsInLongpress;
};

} //namespace Widget
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_ZOOMBUTTON_H__
