
#ifndef __ELASTOS_DROID_WIDGET_POPUPWINDOW_H__
#define __ELASTOS_DROID_WIDGET_POPUPWINDOW_H__

#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Graphics::Drawable::IDrawableCallback;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyEventCallback;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IOnScrollChangedListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IWindowManager;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC PopupWindow
    : public Object
    , public IPopupWindow
{
private:
    class ECO_LOCAL PopupViewContainer
        : public FrameLayout
    {
    public:
        ~PopupViewContainer();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IWeakReference* host);

        // @Override
        virtual CARAPI DispatchKeyEvent(
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* res);

        // @Override
        virtual CARAPI DispatchTouchEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* res);

        // @Override
        virtual CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* res);

        // @Override
        virtual CARAPI SendAccessibilityEvent(
            /* [in] */ Int32 eventType);

    protected:
        // @Override
        virtual CARAPI OnCreateDrawableState(
            /* [in] */ Int32 extraSpace,
            /* [out] */ ArrayOf<Int32>** drawableState);

        AutoPtr<PopupWindow> GetHost();
    private:
        AutoPtr<IWeakReference> mWeakHost;
        static const String TAG;
    };

    class ECO_LOCAL PopupWindowScrollChangedListener
        : public Object
        , public IOnScrollChangedListener
    {
    public:
        PopupWindowScrollChangedListener(
            /* [in] */ PopupWindow* host);

        CAR_INTERFACE_DECL()

        CARAPI OnScrollChanged();

    private:
        PopupWindow* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    PopupWindow();

    ~PopupWindow();

    /**
     * <p>Create a new empty, non focusable popup window of dimension (0,0).</p>
     *
     * <p>The popup does not provide any background. This should be handled
     * by the content view.</p>
     */
    CARAPI constructor();

    /**
     * <p>Create a new empty, non focusable popup window of dimension (0,0).</p>
     *
     * <p>The popup does provide a background.</p>
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * <p>Create a new empty, non focusable popup window of dimension (0,0).</p>
     *
     * <p>The popup does provide a background.</p>
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * <p>Create a new empty, non focusable popup window of dimension (0,0).</p>
     *
     * <p>The popup does provide a background.</p>
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    /**
     * <p>Create a new, empty, non focusable popup window of dimension (0,0).</p>
     *
     * <p>The popup does not provide a background.</p>
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
     * <p>Create a new non focusable popup window which can display the
     * <tt>contentView</tt>. The dimension of the window are (0,0).</p>
     *
     * <p>The popup does not provide any background. This should be handled
     * by the content view.</p>
     *
     * @param contentView the popup's content
     */
    CARAPI constructor(
        /* [in] */ IView* contentView);

    /**
     * <p>Create a new empty, non focusable popup window. The dimension of the
     * window must be passed to this constructor.</p>
     *
     * <p>The popup does not provide any background. This should be handled
     * by the content view.</p>
     *
     * @param width the popup's width
     * @param height the popup's height
     */
    CARAPI constructor(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * <p>Create a new non focusable popup window which can display the
     * <tt>contentView</tt>. The dimension of the window must be passed to
     * this constructor.</p>
     *
     * <p>The popup does not provide any background. This should be handled
     * by the content view.</p>
     *
     * @param contentView the popup's content
     * @param width the popup's width
     * @param height the popup's height
     */
    CARAPI constructor(
        /* [in] */ IView* contentView,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * <p>Create a new popup window which can display the <tt>contentView</tt>.
     * The dimension of the window must be passed to this constructor.</p>
     *
     * <p>The popup does not provide any background. This should be handled
     * by the content view.</p>
     *
     * @param contentView the popup's content
     * @param width the popup's width
     * @param height the popup's height
     * @param focusable true if the popup can be focused, false otherwise
     */
    CARAPI constructor(
        /* [in] */ IView* contentView,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean focusable);

    /**
     * Return the drawable used as the popup window's background.
     *
     * @return the background drawable or {@code null} if not set
     * @see #setBackgroundDrawable(Drawable)
     * @attr ref android.R.styleable#PopupWindow_popupBackground
     */
    CARAPI GetBackground(
        /* [out] */ IDrawable** background);

    /**
     * Specifies the background drawable for this popup window. The background
     * can be set to {@code null}.
     *
     * @param background the popup's background
     * @see #getBackground()
     * @attr ref android.R.styleable#PopupWindow_popupBackground
     */
    CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* background);

    /**
     * @return the elevation for this popup window in pixels
     * @see #setElevation(float)
     * @attr ref android.R.styleable#PopupWindow_popupElevation
     */
    CARAPI GetElevation(
        /* [out] */ Float* elevation);

    /**
     * Specifies the elevation for this popup window.
     *
     * @param elevation the popup's elevation in pixels
     * @see #getElevation()
     * @attr ref android.R.styleable#PopupWindow_popupElevation
     */
    CARAPI SetElevation(
        /* [in] */ Float elevation);

    /**
     * <p>Return the animation style to use the popup appears and disappears</p>
     *
     * @return the animation style to use the popup appears and disappears
     */
    CARAPI GetAnimationStyle(
        /* [out] */ Int32* style);

    /**
     * Set the flag on popup to ignore cheek press eventt; by default this flag
     * is set to false
     * which means the pop wont ignore cheek press dispatch events.
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown or through a manual call to one of
     * the {@link #update()} methods.</p>
     *
     * @see #update()
     */
    CARAPI SetIgnoreCheekPress();

    /**
     * <p>Change the animation style resource for this popup.</p>
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown or through a manual call to one of
     * the {@link #update()} methods.</p>
     *
     * @param animationStyle animation style to use when the popup appears
     *      and disappears.  Set to -1 for the default animation, 0 for no
     *      animation, or a resource identifier for an explicit animation.
     *
     * @see #update()
     */
    CARAPI SetAnimationStyle(
        /* [in] */ Int32 animationStyle);

    /**
     * <p>Return the view used as the content of the popup window.</p>
     *
     * @return a {@link android.view.View} representing the popup's content
     *
     * @see #setContentView(android.view.View)
     */
    CARAPI GetContentView(
        /* [out] */ IView** contentView);

    /**
     * <p>Change the popup's content. The content is represented by an instance
     * of {@link android.view.View}.</p>
     *
     * <p>This method has no effect if called when the popup is showing.  To
     * apply it while a popup is showing, call </p>
     *
     * @param contentView the new content for the popup
     *
     * @see #getContentView()
     * @see #isShowing()
     */
    CARAPI SetContentView(
        /* [in] */ IView* contentView);

    /**
     * Set a callback for all touch events being dispatched to the popup
     * window.
     */
    CARAPI SetTouchInterceptor(
        /* [in] */ IViewOnTouchListener* l);

    /**
     * <p>Indicate whether the popup window can grab the focus.</p>
     *
     * @return true if the popup is focusable, false otherwise
     *
     * @see #setFocusable(boolean)
     */
    CARAPI IsFocusable(
        /* [out] */ Boolean* isFocusable);

    /**
     * <p>Changes the focusability of the popup window. When focusable, the
     * window will grab the focus from the current focused widget if the popup
     * contains a focusable {@link android.view.View}.  By default a popup
     * window is not focusable.</p>
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown or through a manual call to one of
     * the {@link #update()} methods.</p>
     *
     * @param focusable true if the popup should grab focus, false otherwise.
     *
     * @see #isFocusable()
     * @see #isShowing()
     * @see #update()
     */
    CARAPI SetFocusable(
        /* [in] */ Boolean focusable);

    /**
     * Return the current value in {@link #setInputMethodMode(int)}.
     *
     * @see #setInputMethodMode(int)
     */
    CARAPI GetInputMethodMode(
        /* [out] */ Int32* inputMethodMode);

    /**
     * Control how the popup operates with an input method: one of
     * {@link #INPUT_METHOD_FROM_FOCUSABLE}, {@link #INPUT_METHOD_NEEDED},
     * or {@link #INPUT_METHOD_NOT_NEEDED}.
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown or through a manual call to one of
     * the {@link #update()} methods.</p>
     *
     * @see #getInputMethodMode()
     * @see #update()
     */
    CARAPI SetInputMethodMode(
        /* [in] */ Int32 mode);

    /**
     * Sets the operating mode for the soft input area.
     *
     * @param mode The desired mode, see
     *        {@link android.view.WindowManager.LayoutParams#softInputMode}
     *        for the full list
     *
     * @see android.view.WindowManager.LayoutParams#softInputMode
     * @see #getSoftInputMode()
     */
    CARAPI SetSoftInputMode(
        /* [in] */ Int32 mode);

    /**
     * Returns the current value in {@link #setSoftInputMode(int)}.
     *
     * @see #setSoftInputMode(int)
     * @see android.view.WindowManager.LayoutParams#softInputMode
     */
    CARAPI GetSoftInputMode(
        /* [out] */ Int32* softInputMode);

    /**
     * <p>Indicates whether the popup window receives touch events.</p>
     *
     * @return true if the popup is touchable, false otherwise
     *
     * @see #setTouchable(boolean)
     */
    CARAPI IsTouchable(
        /* [out] */ Boolean* touchable);

    /**
     * <p>Changes the touchability of the popup window. When touchable, the
     * window will receive touch events, otherwise touch events will go to the
     * window below it. By default the window is touchable.</p>
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown or through a manual call to one of
     * the {@link #update()} methods.</p>
     *
     * @param touchable true if the popup should receive touch events, false otherwise
     *
     * @see #isTouchable()
     * @see #isShowing()
     * @see #update()
     */
    CARAPI SetTouchable(
        /* [in] */ Boolean touchable);

    /**
     * <p>Indicates whether the popup window will be informed of touch events
     * outside of its window.</p>
     *
     * @return true if the popup is outside touchable, false otherwise
     *
     * @see #setOutsideTouchable(boolean)
     */
    CARAPI IsOutsideTouchable(
        /* [out] */ Boolean* touchable);

    /**
     * <p>Controls whether the pop-up will be informed of touch events outside
     * of its window.  This only makes sense for pop-ups that are touchable
     * but not focusable, which means touches outside of the window will
     * be delivered to the window behind.  The default is false.</p>
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown or through a manual call to one of
     * the {@link #update()} methods.</p>
     *
     * @param touchable true if the popup should receive outside
     * touch events, false otherwise
     *
     * @see #isOutsideTouchable()
     * @see #isShowing()
     * @see #update()
     */
    CARAPI SetOutsideTouchable(
        /* [in] */ Boolean touchable);

    /**
     * <p>Indicates whether clipping of the popup window is enabled.</p>
     *
     * @return true if the clipping is enabled, false otherwise
     *
     * @see #setClippingEnabled(boolean)
     */
    CARAPI IsClippingEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * <p>Allows the popup window to extend beyond the bounds of the screen. By default the
     * window is clipped to the screen boundaries. Setting this to false will allow windows to be
     * accurately positioned.</p>
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown or through a manual call to one of
     * the {@link #update()} methods.</p>
     *
     * @param enabled false if the window should be allowed to extend outside of the screen
     * @see #isShowing()
     * @see #isClippingEnabled()
     * @see #update()
     */
    CARAPI SetClippingEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Clip this popup window to the screen, but not to the containing window.
     *
     * @param enabled True to clip to the screen.
     * @hide
     */
    CARAPI SetClipToScreenEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Allow PopupWindow to scroll the anchor's parent to provide more room
     * for the popup. Enabled by default.
     *
     * @param enabled True to scroll the anchor's parent when more room is desired by the popup.
     */
    CARAPI SetAllowScrollingAnchorParent(
        /* [in] */ Boolean enabled);

    /**
     * <p>Indicates whether the popup window supports splitting touches.</p>
     *
     * @return true if the touch splitting is enabled, false otherwise
     *
     * @see #setSplitTouchEnabled(boolean)
     * @hide
     */
    CARAPI IsSplitTouchEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * <p>Allows the popup window to split touches across other windows that also
     * support split touch.  When this flag is not set, the first pointer
     * that goes down determines the window to which all subsequent touches
     * go until all pointers go up.  When this flag is set, each pointer
     * (not necessarily the first) that goes down determines the window
     * to which all subsequent touches of that pointer will go until that
     * pointer goes up thereby enabling touches with multiple pointers
     * to be split across multiple windows.</p>
     *
     * @param enabled true if the split touches should be enabled, false otherwise
     * @see #isSplitTouchEnabled()
     * @hide
     */
    CARAPI SetSplitTouchEnabled(
        /* [in] */ Boolean enabled);

    /**
     * <p>Indicates whether the popup window will be forced into using absolute screen coordinates
     * for positioning.</p>
     *
     * @return true if the window will always be positioned in screen coordinates.
     * @hide
     */
    CARAPI IsLayoutInScreenEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * <p>Allows the popup window to force the flag
     * {@link WindowManager.LayoutParams#FLAG_LAYOUT_IN_SCREEN}, overriding default behavior.
     * This will cause the popup to be positioned in absolute screen coordinates.</p>
     *
     * @param enabled true if the popup should always be positioned in screen coordinates
     * @hide
     */
    CARAPI SetLayoutInScreenEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Allows the popup window to force the flag
     * {@link WindowManager.LayoutParams#FLAG_LAYOUT_INSET_DECOR}, overriding default behavior.
     * This will cause the popup to inset its content to account for system windows overlaying
     * the screen, such as the status bar.
     *
     * <p>This will often be combined with {@link #setLayoutInScreenEnabled(boolean)}.
     *
     * @param enabled true if the popup's views should inset content to account for system windows,
     *                the way that decor views behave for full-screen windows.
     * @hide
     */
    CARAPI SetLayoutInsetDecor(
        /* [in] */ Boolean enabled);

    /**
     * Set the layout type for this window. Should be one of the TYPE constants defined in
     * {@link WindowManager.LayoutParams}.
     *
     * @param layoutType Layout type for this window.
     * @hide
     */
    CARAPI SetWindowLayoutType(
        /* [in] */ Int32 layoutType);

    /**
     * @return The layout type for this window.
     * @hide
     */
    CARAPI_(Int32) GetWindowLayoutType(
        /* [out] */ Int32* layoutType);

    /**
     * Set whether this window is touch modal or if outside touches will be sent to
     * other windows behind it.
     * @hide
     */
    CARAPI SetTouchModal(
        /* [in] */ Boolean touchModal);

    /**
     * <p>Change the width and height measure specs that are given to the
     * window manager by the popup.  By default these are 0, meaning that
     * the current width or height is requested as an explicit size from
     * the window manager.  You can supply
     * {@link ViewGroup.LayoutParams#WRAP_CONTENT} or
     * {@link ViewGroup.LayoutParams#MATCH_PARENT} to have that measure
     * spec supplied instead, replacing the absolute width and height that
     * has been set in the popup.</p>
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown.</p>
     *
     * @param widthSpec an explicit width measure spec mode, either
     * {@link ViewGroup.LayoutParams#WRAP_CONTENT},
     * {@link ViewGroup.LayoutParams#MATCH_PARENT}, or 0 to use the absolute
     * width.
     * @param heightSpec an explicit height measure spec mode, either
     * {@link ViewGroup.LayoutParams#WRAP_CONTENT},
     * {@link ViewGroup.LayoutParams#MATCH_PARENT}, or 0 to use the absolute
     * height.
     */
    CARAPI SetWindowLayoutMode(
        /* [in] */ Int32 widthSpec,
        /* [in] */ Int32 heightSpec);

    /**
     * <p>Return this popup's height MeasureSpec</p>
     *
     * @return the height MeasureSpec of the popup
     *
     * @see #setHeight(int)
     */
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * <p>Change the popup's height MeasureSpec</p>
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown.</p>
     *
     * @param height the height MeasureSpec of the popup
     *
     * @see #getHeight()
     * @see #isShowing()
     */
    CARAPI SetHeight(
        /* [in] */ Int32 height);

    /**
     * <p>Return this popup's width MeasureSpec</p>
     *
     * @return the width MeasureSpec of the popup
     *
     * @see #setWidth(int)
     */
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * <p>Change the popup's width MeasureSpec</p>
     *
     * <p>If the popup is showing, calling this method will take effect only
     * the next time the popup is shown.</p>
     *
     * @param width the width MeasureSpec of the popup
     *
     * @see #getWidth()
     * @see #isShowing()
     */
    CARAPI SetWidth(
        /* [in] */ Int32 width);

    /**
     * <p>Indicate whether this popup window is showing on screen.</p>
     *
     * @return true if the popup is showing, false otherwise
     */
    CARAPI IsShowing(
        /* [out] */ Boolean* isShowing);

    /**
     * <p>
     * Display the content view in a popup window at the specified location. If the popup window
     * cannot fit on screen, it will be clipped. See {@link android.view.WindowManager.LayoutParams}
     * for more information on how gravity and the x and y parameters are related. Specifying
     * a gravity of {@link android.view.Gravity#NO_GRAVITY} is similar to specifying
     * <code>Gravity.LEFT | Gravity.TOP</code>.
     * </p>
     *
     * @param parent a parent view to get the {@link android.view.View#getWindowToken()} token from
     * @param gravity the gravity which controls the placement of the popup window
     * @param x the popup's x location offset
     * @param y the popup's y location offset
     */
    CARAPI ShowAtLocation(
        /* [in] */ IView* parent,
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    /**
     * Display the content view in a popup window at the specified location.
     *
     * @param token Window token to use for creating the new window
     * @param gravity the gravity which controls the placement of the popup window
     * @param x the popup's x location offset
     * @param y the popup's y location offset
     *
     * @hide Internal use only. Applications should use
     *       {@link #showAtLocation(View, int, int, int)} instead.
     */
    CARAPI ShowAtLocation(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 gravity,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    /**
     * <p>Display the content view in a popup window anchored to the bottom-left
     * corner of the anchor view. If there is not enough room on screen to show
     * the popup in its entirety, this method tries to find a parent scroll
     * view to scroll. If no parent scroll view can be scrolled, the bottom-left
     * corner of the popup is pinned at the top left corner of the anchor view.</p>
     *
     * @param anchor the view on which to pin the popup window
     *
     * @see #dismiss()
     */
    CARAPI ShowAsDropDown(
        /* [in] */ IView* anchor);

    /**
     * <p>Display the content view in a popup window anchored to the bottom-left
     * corner of the anchor view offset by the specified x and y coordinates.
     * If there is not enough room on screen to show
     * the popup in its entirety, this method tries to find a parent scroll
     * view to scroll. If no parent scroll view can be scrolled, the bottom-left
     * corner of the popup is pinned at the top left corner of the anchor view.</p>
     * <p>If the view later scrolls to move <code>anchor</code> to a different
     * location, the popup will be moved correspondingly.</p>
     *
     * @param anchor the view on which to pin the popup window
     * @param xoff A horizontal offset from the anchor in pixels
     * @param yoff A vertical offset from the anchor in pixels
     *
     * @see #dismiss()
     */
    CARAPI ShowAsDropDown(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff);

    /**
     * <p>Display the content view in a popup window anchored to the bottom-left
     * corner of the anchor view offset by the specified x and y coordinates.
     * If there is not enough room on screen to show
     * the popup in its entirety, this method tries to find a parent scroll
     * view to scroll. If no parent scroll view can be scrolled, the bottom-left
     * corner of the popup is pinned at the top left corner of the anchor view.</p>
     * <p>If the view later scrolls to move <code>anchor</code> to a different
     * location, the popup will be moved correspondingly.</p>
     *
     * @param anchor the view on which to pin the popup window
     * @param xoff A horizontal offset from the anchor in pixels
     * @param yoff A vertical offset from the anchor in pixels
     * @param gravity Alignment of the popup relative to the anchor
     *
     * @see #dismiss()
     */
    CARAPI ShowAsDropDown(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 gravity);

    /**
     * Indicates whether the popup is showing above (the y coordinate of the popup's bottom
     * is less than the y coordinate of the anchor) or below the anchor view (the y coordinate
     * of the popup is greater than y coordinate of the anchor's bottom).
     *
     * The value returned
     * by this method is meaningful only after {@link #showAsDropDown(android.view.View)}
     * or {@link #showAsDropDown(android.view.View, int, int)} was invoked.
     *
     * @return True if this popup is showing above the anchor view, false otherwise.
     */
    CARAPI IsAboveAnchor(
        /* [out] */ Boolean* isAboveAnchor);

    /**
     * Returns the maximum height that is available for the popup to be
     * completely shown. It is recommended that this height be the maximum for
     * the popup's height, otherwise it is possible that the popup will be
     * clipped.
     *
     * @param anchor The view on which the popup window must be anchored.
     * @return The maximum available height for the popup to be completely
     *         shown.
     */
    CARAPI GetMaxAvailableHeight(
        /* [in] */ IView* anchor,
        /* [out] */ Int32* maxAvailableHeight);

    /**
     * Returns the maximum height that is available for the popup to be
     * completely shown. It is recommended that this height be the maximum for
     * the popup's height, otherwise it is possible that the popup will be
     * clipped.
     *
     * @param anchor The view on which the popup window must be anchored.
     * @param yOffset y offset from the view's bottom edge
     * @return The maximum available height for the popup to be completely
     *         shown.
     */
    CARAPI GetMaxAvailableHeight(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 yOffset,
        /* [out] */ Int32* maxAvailableHeight);

    /**
     * Returns the maximum height that is available for the popup to be
     * completely shown, optionally ignoring any bottom decorations such as
     * the input method. It is recommended that this height be the maximum for
     * the popup's height, otherwise it is possible that the popup will be
     * clipped.
     *
     * @param anchor The view on which the popup window must be anchored.
     * @param yOffset y offset from the view's bottom edge
     * @param ignoreBottomDecorations if true, the height returned will be
     *        all the way to the bottom of the display, ignoring any
     *        bottom decorations
     * @return The maximum available height for the popup to be completely
     *         shown.
     *
     * @hide Pending API council approval.
     */
    CARAPI GetMaxAvailableHeight(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 yOffset,
        /* [in] */ Boolean ignoreBottomDecorations,
        /* [out] */ Int32* maxAvailableHeight);

    /**
     * <p>Dispose of the popup window. This method can be invoked only after
     * {@link #showAsDropDown(android.view.View)} has been executed. Failing that, calling
     * this method will have no effect.</p>
     *
     * @see #showAsDropDown(android.view.View)
     */
    CARAPI Dismiss();

    /**
     * Sets the listener to be called when the window is dismissed.
     *
     * @param onDismissListener The listener.
     */
    CARAPI SetOnDismissListener(
        /* [in] */ IPopupWindowOnDismissListener* onDismissListener);

    /**
     * Updates the state of the popup window, if it is currently being displayed,
     * from the currently set state.  This includes:
     * {@link #setClippingEnabled(boolean)}, {@link #setFocusable(boolean)},
     * {@link #setIgnoreCheekPress()}, {@link #setInputMethodMode(int)},
     * {@link #setTouchable(boolean)}, and {@link #setAnimationStyle(int)}.
     */
    CARAPI Update();

    /**
     * <p>Updates the dimension of the popup window. Calling this function
     * also updates the window with the current popup state as described
     * for {@link #update()}.</p>
     *
     * @param width the new width
     * @param height the new height
     */
    CARAPI Update(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * <p>Updates the position and the dimension of the popup window. Width and
     * height can be set to -1 to update location only.  Calling this function
     * also updates the window with the current popup state as
     * described for {@link #update()}.</p>
     *
     * @param x the new x location
     * @param y the new y location
     * @param width the new width, can be -1 to ignore
     * @param height the new height, can be -1 to ignore
     */
    CARAPI Update(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * <p>Updates the position and the dimension of the popup window. Width and
     * height can be set to -1 to update location only.  Calling this function
     * also updates the window with the current popup state as
     * described for {@link #update()}.</p>
     *
     * @param x the new x location
     * @param y the new y location
     * @param width the new width, can be -1 to ignore
     * @param height the new height, can be -1 to ignore
     * @param force reposition the window even if the specified position
     *              already seems to correspond to the LayoutParams
     */
    CARAPI Update(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Boolean force);

    /**
     * <p>Updates the position and the dimension of the popup window. Calling this
     * function also updates the window with the current popup state as described
     * for {@link #update()}.</p>
     *
     * @param anchor the popup's anchor view
     * @param width the new width, can be -1 to ignore
     * @param height the new height, can be -1 to ignore
     */
    CARAPI Update(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    /**
     * <p>Updates the position and the dimension of the popup window. Width and
     * height can be set to -1 to update location only.  Calling this function
     * also updates the window with the current popup state as
     * described for {@link #update()}.</p>
     * <p>If the view later scrolls to move <code>anchor</code> to a different
     * location, the popup will be moved correspondingly.</p>
     *
     * @param anchor the popup's anchor view
     * @param xoff x offset from the view's left edge
     * @param yoff y offset from the view's bottom edge
     * @param width the new width, can be -1 to ignore
     * @param height the new height, can be -1 to ignore
     */
    CARAPI Update(
        /* [in] */ IView* anchor,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    CARAPI_(void) UpdateAboveAnchor(
        /* [in] */ Boolean aboveAnchor);

    /**
     * <p>Prepare the popup by embedding in into a new ViewGroup if the
     * background drawable is not null. If embedding is required, the layout
     * parameters' height is mnodified to take into account the background's
     * padding.</p>
     *
     * @param p the layout parameters of the popup's content view
     */
    CARAPI PreparePopup(
        /* [in] */ IWindowManagerLayoutParams* p);

    /**
     * <p>Invoke the popup window by adding the content view to the window
     * manager.</p>
     *
     * <p>The content view must be non-null when this method is invoked.</p>
     *
     * @param p the layout parameters of the popup's content view
     */
    CARAPI_(void) InvokePopup(
        /* [in] */ IWindowManagerLayoutParams* p);

    CARAPI_(void) SetLayoutDirectionFromAnchor();

    /**
     * <p>Generate the layout parameters for the popup window.</p>
     *
     * @param token the window token used to bind the popup's window
     *
     * @return the layout parameters to pass to the window manager
     */
   CARAPI_(AutoPtr<IWindowManagerLayoutParams>) CreatePopupLayout(
       /* [in] */ IBinder* token);

   CARAPI_(Int32) ComputeFlags(
       /* [in] */ Int32 curFlags);

   CARAPI_(Int32) ComputeAnimationResource();

    /**
     * Positions the popup window on screen. When the popup window is too tall
     * to fit under the anchor, a parent scroll view is seeked and scrolled up
     * to reclaim space. If scrolling is not possible or not enough, the popup
     * window gets moved on top of the anchor.
     * <p>
     * The height must have been set on the layout parameters prior to calling
     * this method.
     *
     * @param anchor the view on which the popup window must be anchored
     * @param p the layout parameters used to display the drop down
     * @param xoff horizontal offset used to adjust for background padding
     * @param yoff vertical offset used to adjust for background padding
     * @param gravity horizontal gravity specifying popup alignment
     * @return true if the popup is translated upwards to fit on screen
     */
    CARAPI_(Boolean) FindDropDownPosition(
        /* [in] */ IView* anchor,
        /* [in] */ IWindowManagerLayoutParams* p,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Int32 gravity);

    CARAPI_(void) Update(
        /* [in] */ IView* anchor,
        /* [in] */ Boolean updateLocation,
        /* [in] */ Int32 xoff,
        /* [in] */ Int32 yoff,
        /* [in] */ Boolean updateDimension,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 gravity);

   CARAPI_(void) UnregisterForScrollChanged();

   CARAPI_(void) RegisterForScrollChanged(
       /* [in] */ IView* anchor,
       /* [in] */ Int32 xoff,
       /* [in] */ Int32 yoff,
       /* [in] */ Int32 gravity);

private:
    ECO_LOCAL static const Int32 DEFAULT_ANCHORED_GRAVITY;
    IContext* mContext; // weak-ref
    AutoPtr<IWindowManager> mWindowManager;

    Boolean mIsShowing;
    Boolean mIsDropdown;

    AutoPtr<IView> mContentView;
    AutoPtr<IView> mPopupView;
    Boolean mFocusable;
    Int32 mInputMethodMode;
    Int32 mSoftInputMode;// = WindowManager.LayoutParams.SOFT_INPUT_STATE_UNCHANGED;
    Boolean mTouchable;
    Boolean mOutsideTouchable;
    Boolean mClippingEnabled;
    Int32 mSplitTouchEnabled;// = -1;
    Boolean mLayoutInScreen;
    Boolean mClipToScreen;
    Boolean mAllowScrollingAnchorParent;// = true;
    Boolean mLayoutInsetDecor;// = false;
    Boolean mNotTouchModal;

    AutoPtr<IViewOnTouchListener> mTouchInterceptor;

    Int32 mWidthMode;
    Int32 mWidth;
    Int32 mLastWidth;
    Int32 mHeightMode;
    Int32 mHeight;
    Int32 mLastHeight;

    Int32 mPopupWidth;
    Int32 mPopupHeight;

    Float mElevation;

    AutoPtr<ArrayOf<Int32> > mDrawingLocation;//[2];
    AutoPtr<ArrayOf<Int32> > mScreenLocation;//[2];
    AutoPtr<IRect> mTempRect;

    AutoPtr<IDrawable> mBackground;
    AutoPtr<IDrawable> mAboveAnchorBackgroundDrawable;
    AutoPtr<IDrawable> mBelowAnchorBackgroundDrawable;

    // Temporary animation centers. Should be moved into window params?
    Int32 mAnchorRelativeX;
    Int32 mAnchorRelativeY;

    Boolean mAboveAnchor;
    Int32 mWindowLayoutType;

    AutoPtr<IPopupWindowOnDismissListener> mOnDismissListener;
    Boolean mIgnoreCheekPress;

    Int32 mAnimationStyle;

    ECO_LOCAL static AutoPtr<ArrayOf<Int32> > ABOVE_ANCHOR_STATE_SET;//[1];

    // WeakReference<View>
    AutoPtr<IWeakReference> mAnchor;
    AutoPtr<IOnScrollChangedListener> mOnScrollChangedListener;
    Int32 mAnchorXoff;
    Int32 mAnchorYoff;
    Int32 mAnchoredGravity;
    Boolean mOverlapAnchor;

    Boolean mPopupViewInitialLayoutDirectionInherited;

    friend class ErrorPopup;
};


}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_POPUPWINDOW_H__
