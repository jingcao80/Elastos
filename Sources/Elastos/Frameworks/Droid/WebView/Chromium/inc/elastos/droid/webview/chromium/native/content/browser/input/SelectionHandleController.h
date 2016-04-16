
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTIONHANDLECONTROLLER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTIONHANDLECONTROLLER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/CursorController.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/input/HandleView.h"

using Elastos::Droid::View::IView;

// import com.google.common.annotations.VisibleForTesting;

// import org.chromium.content.browser.PositionObserver;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

class PositionObserver;

namespace Input {

/**
 * CursorController for selecting a range of text.
 */
class SelectionHandleController
    : public CursorController
{
public:
    CAR_INTERFACE_DECL();

    SelectionHandleController(
        /* [in] */ IView* parent,
        /* [in] */ PositionObserver* positionObserver);

    /** Automatically show selection anchors when text is selected. */
    CARAPI_(void) AllowAutomaticShowing();

    /** Hide selection anchors, and don't automatically show them. */
    CARAPI_(void) HideAndDisallowAutomaticShowing();

    //@Override
    CARAPI IsShowing(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI Hide();

    CARAPI_(void) CancelFadeOutAnimation();

    /**
     * Updates the selection for a movement of the given handle (which
     * should be the start handle or end handle) to coordinates x,y.
     * Note that this will not actually result in the handle moving to (x,y):
     * selectBetweenCoordinates(x1,y1,x2,y2) will trigger the selection and set the
     * actual coordinates later via set[Start|End]HandlePosition.
     */
    //@Override
    CARAPI UpdatePosition(
        /* [in] */ HandleView* handle,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    //@Override
    CARAPI BeforeStartUpdatingPosition(
        /* [in] */ HandleView* handle);

    /**
     * @return true iff this controller is being used to move the selection start.
     */
    CARAPI_(Boolean) IsSelectionStartDragged();

    /**
     * @return true iff this controller is being used to drag either the selection start or end.
     */
    CARAPI_(Boolean) IsDragging();

    //@Override
    CARAPI OnTouchModeChanged(
        /* [in] */ Boolean isInTouchMode);

    //@Override
    CARAPI OnDetached();

    /**
     * Moves the start handle so that it points at the given coordinates.
     * @param x The start handle position X in physical pixels.
     * @param y The start handle position Y in physical pixels.
     */
    CARAPI_(void) SetStartHandlePosition(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * Moves the end handle so that it points at the given coordinates.
     * @param x The end handle position X in physical pixels.
     * @param y The end handle position Y in physical pixels.
     */
    CARAPI_(void) SetEndHandlePosition(
        /* [in] */ Float x,
        /* [in] */ Float y);

    /**
     * If the handles are not visible, sets their visibility to View.VISIBLE and begins fading them
     * in.
     */
    CARAPI_(void) BeginHandleFadeIn();

    /**
     * Sets the start and end handles to the given visibility.
     */
    CARAPI_(void) SetHandleVisibility(
        /* [in] */ Int32 visibility);

    /**
     * Shows the handles if allowed.
     *
     * @param startDir Direction (left/right) of start handle.
     * @param endDir Direction (left/right) of end handle.
     */
    CARAPI_(void) OnSelectionChanged(
        /* [in] */ Int32 startDir,
        /* [in] */ Int32 endDir);

    /**
     * Sets both start and end position and show the handles.
     * Note: this method does not trigger a selection, see
     * selectBetweenCoordinates()
     *
     * @param startDir Direction (left/right) of start handle.
     * @param endDir Direction (left/right) of end handle.
     */
    CARAPI_(void) ShowHandles(
        /* [in] */ Int32 startDir,
        /* [in] */ Int32 endDir);

    //@VisibleForTesting
    CARAPI_(AutoPtr<HandleView>) GetStartHandleViewForTest();

    //@VisibleForTesting
    CARAPI_(AutoPtr<HandleView>) GetEndHandleViewForTest();

protected:
    /**
     * The concrete implementation must trigger a selection between the given
     * coordinates and (possibly asynchronously) set the actual handle positions
     * after the selection is made via set[Start|End]HandlePosition.
     */
    virtual CARAPI_(void) SelectBetweenCoordinates(
        /* [in] */ Int32 x1,
        /* [in] */ Int32 y1,
        /* [in] */ Int32 x2,
        /* [in] */ Int32 y2) = 0;

private:
    CARAPI_(void) CreateHandlesIfNeeded(
        /* [in] */ Int32 startDir,
        /* [in] */ Int32 endDir);

    CARAPI_(void) ShowHandlesIfNeeded();

private:
    // The following constants match the ones in
    // third_party/WebKit/public/web/WebTextDirection.h
    static const Int32 TEXT_DIRECTION_DEFAULT = 0;
    static const Int32 TEXT_DIRECTION_LTR = 1;
    static const Int32 TEXT_DIRECTION_RTL = 2;

    /** The cursor controller images, lazily created when shown. */
    AutoPtr<HandleView> mStartHandle;
    AutoPtr<HandleView> mEndHandle;

    /** Whether handles should show automatically when text is selected. */
    Boolean mAllowAutomaticShowing;

    /** Whether selection anchors are active. */
    Boolean mIsShowing;

    AutoPtr<IView> mParent;

    Int32 mFixedHandleX;
    Int32 mFixedHandleY;

    AutoPtr<PositionObserver> mPositionObserver;
};

} // namespace Input
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_INPUT_SELECTIONHANDLECONTROLLER_H__
