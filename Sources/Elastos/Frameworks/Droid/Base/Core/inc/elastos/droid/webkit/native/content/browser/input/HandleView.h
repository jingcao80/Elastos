
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_HANDLEVIEW_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_HANDLEVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/webkit/native/content/browser/PositionObserver.h"
#include "elastos/droid/webkit/native/content/browser/input/CursorController.h"
#include "elastos/droid/webkit/native/content/browser/input/InsertionHandleController.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::SystemClock;
// import android.util.TypedValue;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::Widget::IPopupWindow;

using Elastos::Droid::Webkit::Content::Browser::PositionObserver;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {
namespace Input {

/**
 * View that displays a selection or insertion handle for text editing.
 *
 * While a HandleView is logically a child of some other view, it does not exist in that View's
 * hierarchy.
 *
 */
class HandleView : public Object
    //: public View
{
private:
    class InnerListener
        //: public Object
        : public PositionObserver::Listener
    {
    public:
        InnerListener(
            /* [in] */ HandleView* owner);

        //@Override
        CARAPI OnPositionChanged(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

    private:
        HandleView* mOwner;
    };

public:
    static const Int32 LEFT = 0;
    static const Int32 CENTER = 1;
    static const Int32 RIGHT = 2;

public:
    HandleView(
        /* [in] */ CursorController* controller,
        /* [in] */ Int32 pos,
        /* [in] */ IView* parent,
        /* [in] */ PositionObserver* parentPositionObserver);

    CARAPI_(void) SetOrientation(
        /* [in] */ Int32 pos);

    CARAPI_(void) Show();

    CARAPI_(void) Hide();

    CARAPI_(Boolean) IsShowing();

    // x and y are in physical pixels.
    CARAPI_(void) MoveTo(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) IsDragging();

    /**
     * @return Returns the x position of the handle
     */
    CARAPI_(Int32) GetPositionX();

    /**
     * @return Returns the y position of the handle
     */
    CARAPI_(Int32) GetPositionY();

    // x and y are in physical pixels.
    CARAPI_(void) PositionAt(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    // Returns the x coordinate of the position that the handle appears to be pointing to relative
    // to the handles "parent" view.
    CARAPI_(Int32) GetAdjustedPositionX();

    // Returns the y coordinate of the position that the handle appears to be pointing to relative
    // to the handles "parent" view.
    CARAPI_(Int32) GetAdjustedPositionY();

    // Returns the x coordinate of the postion that the handle appears to be pointing to relative to
    // the root view of the application.
    CARAPI_(Int32) GetRootViewRelativePositionX();

    // Returns the y coordinate of the postion that the handle appears to be pointing to relative to
    // the root view of the application.
    CARAPI_(Int32) GetRootViewRelativePositionY();

    // Returns a suitable y coordinate for the text position corresponding to the handle.
    // As the handle points to a position on the base of the line of text, this method
    // returns a coordinate a small number of pixels higher (i.e. a slightly smaller number)
    // than getAdjustedPositionY.
    CARAPI_(Int32) GetLineAdjustedPositionY();

    CARAPI_(AutoPtr<IDrawable>) GetDrawable();

    /**
     * If the handle is not visible, sets its visibility to View.VISIBLE and begins fading it in.
     */
    CARAPI_(void) BeginFadeIn();

    CARAPI_(void) ShowPastePopupWindow();

protected:
    //@Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    CARAPI OnDraw(
        /* [in] */ ICanvas* c);

private:
    CARAPI_(Boolean) IsPositionVisible();

    CARAPI_(void) UpdatePosition(
        /* [in] */ Float rawX,
        /* [in] */ Float rawY);

    CARAPI_(void) UpdateAlpha();

    CARAPI_(void) UpdateParentPosition(
        /* [in] */ Int32 parentPositionX,
        /* [in] */ Int32 parentPositionY);

    CARAPI_(Int32) GetContainerPositionX();

    CARAPI_(Int32) GetContainerPositionY();

    CARAPI_(void) OnPositionChanged();

    CARAPI_(void) ShowContainer();

private:
    static const Float FADE_DURATION;

    AutoPtr<IDrawable> mDrawable;
    const AutoPtr<IPopupWindow> mContainer;

    // The position of the handle relative to the parent view.
    Int32 mPositionX;
    Int32 mPositionY;

    // The position of the parent relative to the application's root view.
    Int32 mParentPositionX;
    Int32 mParentPositionY;

    // The offset from this handles position to the "tip" of the handle.
    Float mHotspotX;
    Float mHotspotY;

    /*const*/ AutoPtr<CursorController> mController;
    Boolean mIsDragging;
    Float mTouchToWindowOffsetX;
    Float mTouchToWindowOffsetY;

    /*const*/ Int32 mLineOffsetY;
    Float mDownPositionX, mDownPositionY;
    Int64 mTouchTimer;
    Boolean mIsInsertionHandle;
    Float mAlpha;
    Int64 mFadeStartTime;

    const AutoPtr<IView> mParent;
    AutoPtr<InsertionHandleController::PastePopupMenu> mPastePopupWindow;

    const AutoPtr<IRect> mTempRect;

    /*const*/ AutoPtr<PositionObserver> mParentPositionObserver;
    /*const*/ AutoPtr<PositionObserver::Listener> mParentPositionListener;

    // Number of dips to subtract from the handle's y position to give a suitable
    // y coordinate for the corresponding text position. This is to compensate for the fact
    // that the handle position is at the base of the line of text.
    static const Float LINE_OFFSET_Y_DIP;

    static const AutoPtr< ArrayOf<Int32> > TEXT_VIEW_HANDLE_ATTRS;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_INPUT_HANDLEVIEW_H__
