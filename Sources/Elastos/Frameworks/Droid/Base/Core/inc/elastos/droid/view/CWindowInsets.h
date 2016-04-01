
#ifndef __ELASTOS_DROID_VIEW_CWINDOWINSETS_H__
#define __ELASTOS_DROID_VIEW_CWINDOWINSETS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "_Elastos_Droid_View_CWindowInsets.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Describes a set of insets for window content.
 *
 * <p>WindowInsets are immutable and may be expanded to include more inset types in the future.
 * To adjust insets, use one of the supplied clone methods to obtain a new WindowInsets instance
 * with the adjusted properties.</p>
 *
 * @see View.OnApplyWindowInsetsListener
 * @see View#onApplyWindowInsets(WindowInsets)
 */
CarClass(CWindowInsets)
    , public Object
    , public IWindowInsets
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CWindowInsets();

    /** @hide */
    CARAPI constructor(
        /* [in] */ IRect* systemWindowInsets,
        /* [in] */ IRect* windowDecorInsets,
        /* [in] */ IRect* stableInsets,
        /* [in] */ Boolean isRound);

    /**
     * Construct a new WindowInsets, copying all values from a source WindowInsets.
     *
     * @param src Source to copy insets from
     */
    CARAPI constructor(
        /* [in] */ IWindowInsets* src);

    /** @hide */
    CARAPI constructor(
        /* [in] */ IRect* systemWindowInsets);

    /**
     * Used to provide a safe copy of the system window insets to pass through
     * to the existing fitSystemWindows method and other similar internals.
     * @hide
     */
    CARAPI GetSystemWindowInsets(
        /* [out] */ IRect** rect);

    /**
     * Returns the left system window inset in pixels.
     *
     * <p>The system window inset represents the area of a full-screen window that is
     * partially or fully obscured by the status bar, navigation bar, IME or other system windows.
     * </p>
     *
     * @return The left system window inset
     */
    CARAPI GetSystemWindowInsetLeft(
        /* [out] */ Int32* result);

    /**
     * Returns the top system window inset in pixels.
     *
     * <p>The system window inset represents the area of a full-screen window that is
     * partially or fully obscured by the status bar, navigation bar, IME or other system windows.
     * </p>
     *
     * @return The top system window inset
     */
    CARAPI GetSystemWindowInsetTop(
        /* [out] */ Int32* result);

    /**
     * Returns the right system window inset in pixels.
     *
     * <p>The system window inset represents the area of a full-screen window that is
     * partially or fully obscured by the status bar, navigation bar, IME or other system windows.
     * </p>
     *
     * @return The right system window inset
     */
    CARAPI GetSystemWindowInsetRight(
        /* [out] */ Int32* result);

    /**
     * Returns the bottom system window inset in pixels.
     *
     * <p>The system window inset represents the area of a full-screen window that is
     * partially or fully obscured by the status bar, navigation bar, IME or other system windows.
     * </p>
     *
     * @return The bottom system window inset
     */
    CARAPI GetSystemWindowInsetBottom(
        /* [out] */ Int32* result);

    /**
     * Returns the left window decor inset in pixels.
     *
     * <p>The window decor inset represents the area of the window content area that is
     * partially or fully obscured by decorations within the window provided by the framework.
     * This can include action bars, title bars, toolbars, etc.</p>
     *
     * @return The left window decor inset
     * @hide pending API
     */
    CARAPI GetWindowDecorInsetLeft(
        /* [out] */ Int32* result);

    /**
     * Returns the top window decor inset in pixels.
     *
     * <p>The window decor inset represents the area of the window content area that is
     * partially or fully obscured by decorations within the window provided by the framework.
     * This can include action bars, title bars, toolbars, etc.</p>
     *
     * @return The top window decor inset
     * @hide pending API
     */
    CARAPI GetWindowDecorInsetTop(
        /* [out] */ Int32* result);

    /**
     * Returns the right window decor inset in pixels.
     *
     * <p>The window decor inset represents the area of the window content area that is
     * partially or fully obscured by decorations within the window provided by the framework.
     * This can include action bars, title bars, toolbars, etc.</p>
     *
     * @return The right window decor inset
     * @hide pending API
     */
    CARAPI GetWindowDecorInsetRight(
        /* [out] */ Int32* result);

    /**
     * Returns the bottom window decor inset in pixels.
     *
     * <p>The window decor inset represents the area of the window content area that is
     * partially or fully obscured by decorations within the window provided by the framework.
     * This can include action bars, title bars, toolbars, etc.</p>
     *
     * @return The bottom window decor inset
     * @hide pending API
     */
    CARAPI GetWindowDecorInsetBottom(
        /* [out] */ Int32* result);

    /**
     * Returns true if this WindowInsets has nonzero system window insets.
     *
     * <p>The system window inset represents the area of a full-screen window that is
     * partially or fully obscured by the status bar, navigation bar, IME or other system windows.
     * </p>
     *
     * @return true if any of the system window inset values are nonzero
     */
    CARAPI HasSystemWindowInsets(
        /* [out] */ Boolean* has);

    /**
     * Returns true if this WindowInsets has nonzero window decor insets.
     *
     * <p>The window decor inset represents the area of the window content area that is
     * partially or fully obscured by decorations within the window provided by the framework.
     * This can include action bars, title bars, toolbars, etc.</p>
     *
     * @return true if any of the window decor inset values are nonzero
     * @hide pending API
     */
    CARAPI HasWindowDecorInsets(
        /* [out] */ Boolean* has);

    /**
     * Returns true if this WindowInsets has any nonzero insets.
     *
     * @return true if any inset values are nonzero
     */
    CARAPI HasInsets(
        /* [out] */ Boolean* has);

    /**
     * Check if these insets have been fully consumed.
     *
     * <p>Insets are considered "consumed" if the applicable <code>consume*</code> methods
     * have been called such that all insets have been set to zero. This affects propagation of
     * insets through the view hierarchy; insets that have not been fully consumed will continue
     * to propagate down to child views.</p>
     *
     * <p>The result of this method is equivalent to the return value of
     * {@link View#fitSystemWindows(android.graphics.Rect)}.</p>
     *
     * @return true if the insets have been fully consumed.
     */
    CARAPI IsConsumed(
        /* [out] */ Boolean* result);

    /**
     * Returns true if the associated window has a round shape.
     *
     * <p>A round window's left, top, right and bottom edges reach all the way to the
     * associated edges of the window but the corners may not be visible. Views responding
     * to round insets should take care to not lay out critical elements within the corners
     * where they may not be accessible.</p>
     *
     * @return True if the window is round
     */
    CARAPI IsRound(
        /* [out] */ Boolean* result);

    /**
     * Returns a copy of this WindowInsets with the system window insets fully consumed.
     *
     * @return A modified copy of this WindowInsets
     */
    CARAPI ConsumeSystemWindowInsets(
        /* [out] */ IWindowInsets** insets);

    /**
     * Returns a copy of this WindowInsets with selected system window insets fully consumed.
     *
     * @param left true to consume the left system window inset
     * @param top true to consume the top system window inset
     * @param right true to consume the right system window inset
     * @param bottom true to consume the bottom system window inset
     * @return A modified copy of this WindowInsets
     * @hide pending API
     */
    CARAPI ConsumeSystemWindowInsets(
        /* [in] */ Boolean left,
        /* [in] */ Boolean top,
        /* [in] */ Boolean right,
        /* [in] */ Boolean bottom,
        /* [out] */ IWindowInsets** insets);

    /**
     * Returns a copy of this WindowInsets with selected system window insets replaced
     * with new values.
     *
     * @param left New left inset in pixels
     * @param top New top inset in pixels
     * @param right New right inset in pixels
     * @param bottom New bottom inset in pixels
     * @return A modified copy of this WindowInsets
     */
    CARAPI ReplaceSystemWindowInsets(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ IWindowInsets** insets);

    /**
     * Returns a copy of this WindowInsets with selected system window insets replaced
     * with new values.
     *
     * @param systemWindowInsets New system window insets. Each field is the inset in pixels
     *                           for that edge
     * @return A modified copy of this WindowInsets
     */
    CARAPI ReplaceSystemWindowInsets(
        /* [in] */ IRect* systemWindowInsets,
        /* [out] */ IWindowInsets** insets);

    /**
     * @hide
     */
    CARAPI ConsumeWindowDecorInsets(
        /* [out] */ IWindowInsets** insets);

    /**
     * @hide
     */
    CARAPI ConsumeWindowDecorInsets(
        /* [in] */ Boolean left,
        /* [in] */ Boolean top,
        /* [in] */ Boolean right,
        /* [in] */ Boolean bottom,
        /* [out] */ IWindowInsets** insets);

    /**
     * @hide
     */
    CARAPI ReplaceWindowDecorInsets(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ IWindowInsets** insets);

    /**
     * Returns the top stable inset in pixels.
     *
     * <p>The stable inset represents the area of a full-screen window that <b>may</b> be
     * partially or fully obscured by the system UI elements.  This value does not change
     * based on the visibility state of those elements; for example, if the status bar is
     * normally shown, but temporarily hidden, the stable inset will still provide the inset
     * associated with the status bar being shown.</p>
     *
     * @return The top stable inset
     */
    CARAPI GetStableInsetTop(
        /* [out] */ Int32* result);

    /**
     * Returns the left stable inset in pixels.
     *
     * <p>The stable inset represents the area of a full-screen window that <b>may</b> be
     * partially or fully obscured by the system UI elements.  This value does not change
     * based on the visibility state of those elements; for example, if the status bar is
     * normally shown, but temporarily hidden, the stable inset will still provide the inset
     * associated with the status bar being shown.</p>
     *
     * @return The left stable inset
     */
    CARAPI GetStableInsetLeft(
        /* [out] */ Int32* result);

    /**
     * Returns the right stable inset in pixels.
     *
     * <p>The stable inset represents the area of a full-screen window that <b>may</b> be
     * partially or fully obscured by the system UI elements.  This value does not change
     * based on the visibility state of those elements; for example, if the status bar is
     * normally shown, but temporarily hidden, the stable inset will still provide the inset
     * associated with the status bar being shown.</p>
     *
     * @return The right stable inset
     */
    CARAPI GetStableInsetRight(
        /* [out] */ Int32* result);

    /**
     * Returns the bottom stable inset in pixels.
     *
     * <p>The stable inset represents the area of a full-screen window that <b>may</b> be
     * partially or fully obscured by the system UI elements.  This value does not change
     * based on the visibility state of those elements; for example, if the status bar is
     * normally shown, but temporarily hidden, the stable inset will still provide the inset
     * associated with the status bar being shown.</p>
     *
     * @return The bottom stable inset
     */
    CARAPI GetStableInsetBottom(
        /* [out] */ Int32* result);

    /**
     * Returns true if this WindowInsets has nonzero stable insets.
     *
     * <p>The stable inset represents the area of a full-screen window that <b>may</b> be
     * partially or fully obscured by the system UI elements.  This value does not change
     * based on the visibility state of those elements; for example, if the status bar is
     * normally shown, but temporarily hidden, the stable inset will still provide the inset
     * associated with the status bar being shown.</p>
     *
     * @return true if any of the stable inset values are nonzero
     */
    CARAPI HasStableInsets(
        /* [out] */ Boolean* has);

    /**
     * Returns a copy of this WindowInsets with the stable insets fully consumed.
     *
     * @return A modified copy of this WindowInsets
     */
    CARAPI ConsumeStableInsets(
        /* [out] */ IWindowInsets** insets);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    static AutoPtr<IWindowInsets> GetCONSUMED();
private:
    /**
     * Since new insets may be added in the future that existing apps couldn't
     * know about, this fully empty constant shouldn't be made available to apps
     * since it would allow them to inadvertently consume unknown insets by returning it.
     * @hide
     */
    static AutoPtr<IWindowInsets> CONSUMED;

    AutoPtr<IRect> mSystemWindowInsets;
    AutoPtr<IRect> mWindowDecorInsets;
    AutoPtr<IRect> mStableInsets;
    AutoPtr<IRect> mTempRect;
    Boolean mIsRound;

    Boolean mSystemWindowInsetsConsumed;
    Boolean mWindowDecorInsetsConsumed;
    Boolean mStableInsetsConsumed;

    static AutoPtr<IRect> EMPTY_RECT;
};

}// namespace View
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_VIEW_CWINDOWINSETS_H__
