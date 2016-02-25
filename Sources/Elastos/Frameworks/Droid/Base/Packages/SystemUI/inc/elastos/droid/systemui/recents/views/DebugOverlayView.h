
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_DEBUGOVERLAYVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_DEBUGOVERLAYVIEW_H__

#include "elastos/droid/systemui/recents/RecentsConfiguration.h"
#include <elastos/droid/widget/FrameLayout.h>
#include <elastos/utility/etl/Pair.h>

using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::ISeekBar;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Utility::Etl::Pair;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/**
 * A full screen overlay layer that allows us to draw views from throughout the system on the top
 * most layer.
 */
class DebugOverlayView
    : public FrameLayout
    , public IDebugOverlayView
{
private:
    class OnSeekBarChangeListener
        : public Object
        , public ISeekBarOnSeekBarChangeListener
    {
    public:
        OnSeekBarChangeListener(
            /* [in] */ DebugOverlayView* host);

        CAR_INTERFACE_DECL()

        CARAPI OnStopTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        // @Override
        CARAPI OnStartTrackingTouch(
            /* [in] */ ISeekBar* seekBar);

        // @Override
        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromUser);

    private:
        DebugOverlayView* mHost;
    };

public:
    DebugOverlayView();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI SetCallbacks(
        /* [in] */ IDebugOverlayViewCallbacks* cb);

    /** Enables the debug overlay drawing. */
    CARAPI Enable();

    /** Disables the debug overlay drawing. */
    CARAPI Disable();

    /** Clears all debug rects. */
    CARAPI Clear();

    /** Adds a rect to be drawn. */
    CARAPI_(void) AddRect(
        /* [in] */ IRect* r,
        /* [in] */ Int32 color);

    /** Adds a view's global rect to be drawn. */
    CARAPI_(void) AddViewRect(
        /* [in] */ IView* v,
        /* [in] */ Int32 color);

    /** Adds a rect, relative to a given view to be drawn. */
    CARAPI_(void) AddRectRelativeToView(
        /* [in] */ IView* v,
        /* [in] */ IRect* r,
        /* [in] */ Int32 color);

    /** Sets the debug text at the bottom of the screen. */
    CARAPI_(void) SetText(
        /* [in] */ const String& message);

    /**** SeekBar.OnSeekBarChangeListener Implementation ****/

    // @Override
    CARAPI OnStopTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

    // @Override
    CARAPI OnStartTrackingTouch(
        /* [in] */ ISeekBar* seekBar);

    // @Override
    CARAPI OnProgressChanged(
        /* [in] */ ISeekBar* seekBar,
        /* [in] */ Int32 progress,
        /* [in] */ Boolean fromUser);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

public:
    static const Int32 sCornerRectSize = 50;

    AutoPtr<RecentsConfiguration> mConfig;
    AutoPtr<IDebugOverlayViewCallbacks> mCb;

    List<AutoPtr<Pair<AutoPtr<IRect>, Int32> > > mRects;
    String mText;
    AutoPtr<IPaint> mDebugOutline;
    AutoPtr<IPaint> mTmpPaint;
    AutoPtr<IRect> mTmpRect;
    Boolean mEnabled;

    AutoPtr<ISeekBar> mPrimarySeekBar;
    AutoPtr<ISeekBar> mSecondarySeekBar;
    AutoPtr<OnSeekBarChangeListener> mListener;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_DEBUGOVERLAYVIEW_H__
