
#ifndef __ELASTOS_DROID_WIDGET_LISTPOPUPWINDOW_H__
#define __ELASTOS_DROID_WIDGET_LISTPOPUPWINDOW_H__

#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/Int32Property.h"
#include "elastos/droid/widget/ListView.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Widget::IAbsListViewAutoScroller;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Int32Property;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewOnAttachStateChangeListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAbsListViewOnScrollListener;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IListPopupWindow;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::IPopupWindow;
using Elastos::Droid::Widget::IPopupWindowOnDismissListener;
using Elastos::Droid::Widget::ListView;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC ListPopupWindow
    : public Object
    , public IListPopupWindow
{
public:
    class ForwardingListener
        : public Object
        , public IViewOnTouchListener
        , public IViewOnAttachStateChangeListener
    {
    private:
        class DisallowIntercept
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            DisallowIntercept(
                /* [in] */ ForwardingListener* owner);

            // @Override
            CARAPI Run();

        private:
            ForwardingListener* mOwner;
        };

        class TriggerLongPress
            : public Object
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL()

            TriggerLongPress(
                /* [in] */ ForwardingListener* owner);

            // @Override
            CARAPI Run();

        private:
            ForwardingListener* mOwner;
        };

    public:
        CAR_INTERFACE_DECL()

        ForwardingListener();

        CARAPI constructor(
            /* [in] */ IView* src);

        /**
          * Returns the popup to which this listener is forwarding events.
          * <p>
          * Override this to return the correct popup. If the popup is displayed
          * asynchronously, you may also need to override
          * {@link #onForwardingStopped} to prevent premature cancelation of
          * forwarding.
          *
          * @return the popup to which this listener is forwarding events
          */
        virtual CARAPI GetPopup(
            /* [out] */ IListPopupWindow** result);

        // @Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnViewAttachedToWindow(
            /* [in] */ IView* v);

        // @Override
        CARAPI OnViewDetachedFromWindow(
            /* [in] */ IView* v);

    protected:
        /**
          * Called when forwarding would like to start.
          * <p>
          * By default, this will show the popup returned by {@link #getPopup()}.
          * It may be overridden to perform another action, like clicking the
          * source view or preparing the popup before showing it.
          *
          * @return true to start forwarding, false otherwise
          */
        virtual CARAPI_(Boolean) OnForwardingStarted();

        /**
          * Called when forwarding would like to stop.
          * <p>
          * By default, this will dismiss the popup returned by
          * {@link #getPopup()}. It may be overridden to perform some other
          * action.
          *
          * @return true to stop forwarding, false otherwise
          */
        virtual CARAPI_(Boolean) OnForwardingStopped();

    private:
        /**
          * Observes motion events and determines when to start forwarding.
          *
          * @param srcEvent motion event in source view coordinates
          * @return true to start forwarding motion events, false otherwise
          */
        CARAPI_(Boolean) OnTouchObserved(
            /* [in] */ IMotionEvent* srcEvent);

        CARAPI_(void) ClearCallbacks();

        CARAPI_(void) OnLongPress();

        /**
          * Handled forwarded motion events and determines when to stop
          * forwarding.
          *
          * @param srcEvent motion event in source view coordinates
          * @return true to continue forwarding motion events, false to cancel
          */
        CARAPI_(Boolean) OnTouchForwarded(
            /* [in] */ IMotionEvent* srcEvent);

    private:
        /*const*/ Float mScaledTouchSlop;
        /*const*/ Int32 mTapTimeout;
        /*const*/ Int32 mLongPressTimeout;
        /*const*/ AutoPtr<IView> mSrc;
        AutoPtr<IRunnable> mDisallowIntercept;
        AutoPtr<IRunnable> mTriggerLongPress;
        Boolean mForwarding;
        Boolean mWasLongPress;
        Int32 mActivePointerId;
    };

private:
    class InnerForwardingListener
        : public ForwardingListener
    {
    public:
        InnerForwardingListener(
            /* [in] */ ListPopupWindow* owner);

        // @Override
        CARAPI GetPopup(
            /* [out] */ IListPopupWindow** result);

    private:
        ListPopupWindow* mOwner;
    };

    /**
      * <p>Wrapper class for a ListView. This wrapper can hijack the focus to
      * make sure the list uses the appropriate drawables and states when
      * displayed on screen within a drop down. The focus is never actually
      * passed to the drop down in this mode; the list only looks focused.</p>
      */
    class DropDownListView
        : public ListView
    {
    private:
        class InnerAnimatorListenerAdapter
            : public AnimatorListenerAdapter
        {
        public:
            InnerAnimatorListenerAdapter(
                /* [in] */ DropDownListView* owner,
                /* [in] */ IView* child,
                /* [in] */ Int32 position,
                /* [in] */ Int64 id);

            // @Override
            CARAPI OnAnimationEnd(
                /* [in] */ IAnimator* animation);

        private:
            DropDownListView* mOwner;
            IView* mChild;
            Int32 mPosition;
            Int64 mId;
        };

    public:
        DropDownListView();

        /**
          * <p>Creates a new list view wrapper.</p>
          *
          * @param context this view's context
          */
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Boolean hijackFocus);

        /**
          * Handles forwarded events.
          *
          * @param activePointerId id of the pointer that activated forwarding
          * @return whether the event was handled
          */
        virtual CARAPI OnForwardedEvent(
            /* [in] */ IMotionEvent* event,
            /* [in] */ Int32 activePointerId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI_(Boolean) TouchModeDrawsInPressedState();

        /**
          * <p>Avoids jarring scrolling effect by ensuring that list elements
          * made of a text view fit on a single line.</p>
          *
          * @param position the item index in the list to get a view for
          * @return the view for the specified item
          */
        // @Override
        CARAPI_(AutoPtr<IView>) ObtainView(
            /* [in] */ Int32 position,
            /* [in] */ ArrayOf<Boolean>* isScrap);

        // @Override
        CARAPI IsInTouchMode(
            /* [out] */ Boolean* result);

        /**
          * <p>Returns the focus state in the drop down.</p>
          *
          * @return true always if hijacking focus
          */
        // @Override
        CARAPI HasWindowFocus(
            /* [out] */ Boolean* result);

        /**
          * <p>Returns the focus state in the drop down.</p>
          *
          * @return true always if hijacking focus
          */
        // @Override
        CARAPI IsFocused(
            /* [out] */ Boolean* result);

        /**
          * <p>Returns the focus state in the drop down.</p>
          *
          * @return true always if hijacking focus
          */
        // @Override
        CARAPI HasFocus(
            /* [out] */ Boolean* result);

    private:
        /**
          * Starts an alpha animation on the selector. When the animation ends,
          * the list performs a click on the item.
          */
        CARAPI_(void) ClickPressedItem(
            /* [in] */ IView* child,
            /* [in] */ Int32 position);

        CARAPI_(void) ClearPressedItem();

        CARAPI_(void) SetPressedItem(
            /* [in] */ IView* child,
            /* [in] */ Int32 position,
            /* [in] */ Float x,
            /* [in] */ Float y);

    public:
        Boolean mListSelectionHidden;

    private:
        static const Int64 CLICK_ANIM_DURATION = 150;
        static const Int32 CLICK_ANIM_ALPHA = 0x80;
        static const AutoPtr<Int32Property> DRAWABLE_ALPHA;
        Boolean mHijackFocus;
        Boolean mDrawsInPressedState;
        AutoPtr<IAnimator> mClickAnimation;
        AutoPtr<IAbsListViewAutoScroller> mScrollHelper;
    };

    class InnerInt32Property
        : public Int32Property
    {
    public:
        InnerInt32Property(
            /* [in] */ const String& property);

        // @Override
        CARAPI SetValue(
            /* [in] */ IInterface* object,
            /* [in] */ Int32 value);

        // @Override
        CARAPI Get(
            /* [in] */ IInterface* object,
            /* [out] */ IInterface** result);
    };

    class PopupDataSetObserver
        : public Object
        , public IDataSetObserver
    {
    public:
        CAR_INTERFACE_DECL()

        PopupDataSetObserver(
            /* [in] */ ListPopupWindow* owner);

        // @Override
        CARAPI OnChanged();

        // @Override
        CARAPI OnInvalidated();

    private:
        ListPopupWindow* mOwner;
    };

    class ListSelectorHider
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        ListSelectorHider(
            /* [in] */ ListPopupWindow* owner);

        virtual CARAPI Run();

    private:
        ListPopupWindow* mOwner;
    };

    class ResizePopupRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        ResizePopupRunnable(
            /* [in] */ ListPopupWindow* owner);

        virtual CARAPI Run();

    private:
        ListPopupWindow* mOwner;
    };

    class PopupTouchInterceptor
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        PopupTouchInterceptor(
            /* [in] */ ListPopupWindow* owner);

        virtual CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        ListPopupWindow* mOwner;
    };

    class PopupScrollListener
        : public Object
        , public IAbsListViewOnScrollListener
    {
    public:
        CAR_INTERFACE_DECL()

        PopupScrollListener(
            /* [in] */ ListPopupWindow* owner);

        virtual CARAPI OnScroll(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 firstVisibleItem,
            /* [in] */ Int32 visibleItemCount,
            /* [in] */ Int32 totalItemCount);

        virtual CARAPI OnScrollStateChanged(
            /* [in] */ IAbsListView* view,
            /* [in] */ Int32 scrollState);

    private:
        ListPopupWindow* mOwner;
    };

    class InnerShowRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        InnerShowRunnable(
            /* [in] */ ListPopupWindow* owner);

        // @Override
        CARAPI Run();

    private:
        ListPopupWindow* mOwner;
    };

    class InnerAdapterViewOnItemSelectedListener
        : public Object
        , public IAdapterViewOnItemSelectedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerAdapterViewOnItemSelectedListener(
            /* [in] */ ListPopupWindow* owner);

        // @Override
        CARAPI OnItemSelected(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        // @Override
        CARAPI OnNothingSelected(
            /* [in] */ IAdapterView* parent);

    private:
        ListPopupWindow* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    ListPopupWindow();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
      * Create a new, empty popup window capable of displaying items from a ListAdapter.
      * Backgrounds should be set using {@link #setBackgroundDrawable(Drawable)}.
      *
      * @param context Context used for contained views.
      * @param attrs Attributes from inflating parent views used to style the popup.
      */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
      * Create a new, empty popup window capable of displaying items from a ListAdapter.
      * Backgrounds should be set using {@link #setBackgroundDrawable(Drawable)}.
      *
      * @param context Context used for contained views.
      * @param attrs Attributes from inflating parent views used to style the popup.
      * @param defStyleAttr Default style attribute to use for popup content.
      */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    /**
      * Create a new, empty popup window capable of displaying items from a ListAdapter.
      * Backgrounds should be set using {@link #setBackgroundDrawable(Drawable)}.
      *
      * @param context Context used for contained views.
      * @param attrs Attributes from inflating parent views used to style the popup.
      * @param defStyleAttr Style attribute to read for default styling of popup content.
      * @param defStyleRes Style resource ID to use for default styling of popup content.
      */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    /**
      * Sets the adapter that provides the data and the views to represent the data
      * in this popup window.
      *
      * @param adapter The adapter to use to create this window's content.
      */
    virtual CARAPI SetAdapter(
        /* [in] */ IListAdapter* adapter);

    /**
      * Set where the optional prompt view should appear. The default is
      * {@link #POSITION_PROMPT_ABOVE}.
      *
      * @param position A position constant declaring where the prompt should be displayed.
      *
      * @see #POSITION_PROMPT_ABOVE
      * @see #POSITION_PROMPT_BELOW
      */
    virtual CARAPI SetPromptPosition(
        /* [in] */ Int32 position);

    /**
      * @return Where the optional prompt view should appear.
      *
      * @see #POSITION_PROMPT_ABOVE
      * @see #POSITION_PROMPT_BELOW
      */
    virtual CARAPI GetPromptPosition(
        /* [out] */ Int32* result);

    /**
      * Set whether this window should be modal when shown.
      *
      * <p>If a popup window is modal, it will receive all touch and key input.
      * If the user touches outside the popup window's content area the popup window
      * will be dismissed.
      *
      * @param modal {@code true} if the popup window should be modal, {@code false} otherwise.
      */
    virtual CARAPI SetModal(
        /* [in] */ Boolean modal);

    /**
      * Returns whether the popup window will be modal when shown.
      *
      * @return {@code true} if the popup window will be modal, {@code false} otherwise.
      */
    virtual CARAPI IsModal(
        /* [out] */ Boolean* result);

    /**
      * Forces outside touches to be ignored. Normally if {@link #isDropDownAlwaysVisible()} is
      * false, we allow outside touch to dismiss the dropdown. If this is set to true, then we
      * ignore outside touch even when the drop down is not set to always visible.
      *
      * @hide Used only by AutoCompleteTextView to handle some internal special cases.
      */
    virtual CARAPI SetForceIgnoreOutsideTouch(
        /* [in] */ Boolean forceIgnoreOutsideTouch);

    /**
      * Sets whether the drop-down should remain visible under certain conditions.
      *
      * The drop-down will occupy the entire screen below {@link #getAnchorView} regardless
      * of the size or content of the list.  {@link #getBackground()} will fill any space
      * that is not used by the list.
      *
      * @param dropDownAlwaysVisible Whether to keep the drop-down visible.
      *
      * @hide Only used by AutoCompleteTextView under special conditions.
      */
    virtual CARAPI SetDropDownAlwaysVisible(
        /* [in] */ Boolean dropDownAlwaysVisible);

    /**
      * @return Whether the drop-down is visible under special conditions.
      *
      * @hide Only used by AutoCompleteTextView under special conditions.
      */
    virtual CARAPI IsDropDownAlwaysVisible(
        /* [out] */ Boolean* result);

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
    virtual CARAPI SetSoftInputMode(
        /* [in] */ Int32 mode);

    /**
      * Returns the current value in {@link #setSoftInputMode(int)}.
      *
      * @see #setSoftInputMode(int)
      * @see android.view.WindowManager.LayoutParams#softInputMode
      */
    virtual CARAPI GetSoftInputMode(
        /* [out] */ Int32* result);

    /**
      * Sets a drawable to use as the list item selector.
      *
      * @param selector List selector drawable to use in the popup.
      */
    virtual CARAPI SetListSelector(
        /* [in] */ IDrawable* selector);

    /**
      * @return The background drawable for the popup window.
      */
    virtual CARAPI GetBackground(
        /* [out] */ IDrawable** result);

    /**
      * Sets a drawable to be the background for the popup window.
      *
      * @param d A drawable to set as the background.
      */
    virtual CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* d);

    /**
      * Set an animation style to use when the popup window is shown or dismissed.
      *
      * @param animationStyle Animation style to use.
      */
    virtual CARAPI SetAnimationStyle(
        /* [in] */ Int32 animationStyle);

    /**
      * Returns the animation style that will be used when the popup window is
      * shown or dismissed.
      *
      * @return Animation style that will be used.
      */
    virtual CARAPI GetAnimationStyle(
        /* [out] */ Int32* result);

    /**
      * Returns the view that will be used to anchor this popup.
      *
      * @return The popup's anchor view
      */
    virtual CARAPI GetAnchorView(
        /* [out] */ IView** result);

    /**
      * Sets the popup's anchor view. This popup will always be positioned relative to
      * the anchor view when shown.
      *
      * @param anchor The view to use as an anchor.
      */
    virtual CARAPI SetAnchorView(
        /* [in] */ IView* anchor);

    /**
      * @return The horizontal offset of the popup from its anchor in pixels.
      */
    virtual CARAPI GetHorizontalOffset(
        /* [out] */ Int32* result);

    /**
      * Set the horizontal offset of this popup from its anchor view in pixels.
      *
      * @param offset The horizontal offset of the popup from its anchor.
      */
    virtual CARAPI SetHorizontalOffset(
        /* [in] */ Int32 offset);

    /**
      * @return The vertical offset of the popup from its anchor in pixels.
      */
    virtual CARAPI GetVerticalOffset(
        /* [out] */ Int32* result);

    /**
      * Set the vertical offset of this popup from its anchor view in pixels.
      *
      * @param offset The vertical offset of the popup from its anchor.
      */
    virtual CARAPI SetVerticalOffset(
        /* [in] */ Int32 offset);

    /**
      * Set the gravity of the dropdown list. This is commonly used to
      * set gravity to START or END for alignment with the anchor.
      *
      * @param gravity Gravity value to use
      */
    virtual CARAPI SetDropDownGravity(
        /* [in] */ Int32 gravity);

    /**
      * @return The width of the popup window in pixels.
      */
    virtual CARAPI GetWidth(
        /* [out] */ Int32* result);

    /**
      * Sets the width of the popup window in pixels. Can also be {@link #MATCH_PARENT}
      * or {@link #WRAP_CONTENT}.
      *
      * @param width Width of the popup window.
      */
    virtual CARAPI SetWidth(
        /* [in] */ Int32 width);

    /**
      * Sets the width of the popup window by the size of its content. The final width may be
      * larger to accommodate styled window dressing.
      *
      * @param width Desired width of content in pixels.
      */
    virtual CARAPI SetContentWidth(
        /* [in] */ Int32 width);

    /**
      * @return The height of the popup window in pixels.
      */
    virtual CARAPI GetHeight(
        /* [out] */ Int32* result);

    /**
      * Sets the height of the popup window in pixels. Can also be {@link #MATCH_PARENT}.
      *
      * @param height Height of the popup window.
      */
    virtual CARAPI SetHeight(
        /* [in] */ Int32 height);

    /**
      * Sets a listener to receive events when a list item is clicked.
      *
      * @param clickListener Listener to register
      *
      * @see ListView#setOnItemClickListener(android.widget.AdapterView.OnItemClickListener)
      */
    virtual CARAPI SetOnItemClickListener(
        /* [in] */ IAdapterViewOnItemClickListener* clickListener);

    /**
      * Sets a listener to receive events when a list item is selected.
      *
      * @param selectedListener Listener to register.
      *
      * @see ListView#setOnItemSelectedListener(android.widget.AdapterView.OnItemSelectedListener)
      */
    virtual CARAPI SetOnItemSelectedListener(
        /* [in] */ IAdapterViewOnItemSelectedListener* selectedListener);

    /**
      * Set a view to act as a user prompt for this popup window. Where the prompt view will appear
      * is controlled by {@link #setPromptPosition(int)}.
      *
      * @param prompt View to use as an informational prompt.
      */
    virtual CARAPI SetPromptView(
        /* [in] */ IView* prompt);

    /**
      * Post a {@link #show()} call to the UI thread.
      */
    virtual CARAPI PostShow();

    /**
      * Show the popup list. If the list is already showing, this method
      * will recalculate the popup's size and position.
      */
    virtual CARAPI Show();

    /**
      * Dismiss the popup window.
      */
    virtual CARAPI Dismiss();

    /**
      * Set a listener to receive a callback when the popup is dismissed.
      *
      * @param listener Listener that will be notified when the popup is dismissed.
      */
    virtual CARAPI SetOnDismissListener(
        /* [in] */ IPopupWindowOnDismissListener* listener);

    /**
      * Control how the popup operates with an input method: one of
      * {@link #INPUT_METHOD_FROM_FOCUSABLE}, {@link #INPUT_METHOD_NEEDED},
      * or {@link #INPUT_METHOD_NOT_NEEDED}.
      *
      * <p>If the popup is showing, calling this method will take effect only
      * the next time the popup is shown or through a manual call to the {@link #show()}
      * method.</p>
      *
      * @see #getInputMethodMode()
      * @see #show()
      */
    virtual CARAPI SetInputMethodMode(
        /* [in] */ Int32 mode);

    /**
      * Return the current value in {@link #setInputMethodMode(int)}.
      *
      * @see #setInputMethodMode(int)
      */
    virtual CARAPI GetInputMethodMode(
        /* [out] */ Int32* result);

    /**
      * Set the selected position of the list.
      * Only valid when {@link #isShowing()} == {@code true}.
      *
      * @param position List position to set as selected.
      */
    virtual CARAPI SetSelection(
        /* [in] */ Int32 position);

    /**
      * Clear any current list selection.
      * Only valid when {@link #isShowing()} == {@code true}.
      */
    virtual CARAPI ClearListSelection();

    /**
      * @return {@code true} if the popup is currently showing, {@code false} otherwise.
      */
    virtual CARAPI IsShowing(
        /* [out] */ Boolean* result);

    /**
      * @return {@code true} if this popup is configured to assume the user does not need
      * to interact with the IME while it is showing, {@code false} otherwise.
      */
    virtual CARAPI IsInputMethodNotNeeded(
        /* [out] */ Boolean* result);

    /**
      * Perform an item click operation on the specified list adapter position.
      *
      * @param position Adapter position for performing the click
      * @return true if the click action could be performed, false if not.
      *         (e.g. if the popup was not showing, this method would return false.)
      */
    virtual CARAPI PerformItemClick(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* result);

    /**
      * @return The currently selected item or null if the popup is not showing.
      */
    virtual CARAPI GetSelectedItem(
        /* [out] */ IInterface** result);

    /**
      * @return The position of the currently selected item or {@link ListView#INVALID_POSITION}
      * if {@link #isShowing()} == {@code false}.
      *
      * @see ListView#getSelectedItemPosition()
      */
    virtual CARAPI GetSelectedItemPosition(
        /* [out] */ Int32* result);

    /**
      * @return The ID of the currently selected item or {@link ListView#INVALID_ROW_ID}
      * if {@link #isShowing()} == {@code false}.
      *
      * @see ListView#getSelectedItemId()
      */
    virtual CARAPI GetSelectedItemId(
        /* [out] */ Int64* result);

    /**
      * @return The View for the currently selected item or null if
      * {@link #isShowing()} == {@code false}.
      *
      * @see ListView#getSelectedView()
      */
    virtual CARAPI GetSelectedView(
        /* [out] */ IView** result);

    /**
      * @return The {@link ListView} displayed within the popup window.
      * Only valid when {@link #isShowing()} == {@code true}.
      */
    virtual CARAPI GetListView(
        /* [out] */ IListView** result);

    /**
      * The maximum number of list items that can be visible and still have
      * the list expand when touched.
      *
      * @param max Max number of items that can be visible and still allow the list to expand.
      */
    virtual CARAPI SetListItemExpandMax(
        /* [in] */ Int32 max);

    /**
      * Filter key down events. By forwarding key down events to this function,
      * views using non-modal ListPopupWindow can have it handle key selection of items.
      *
      * @param keyCode keyCode param passed to the host view's onKeyDown
      * @param event event param passed to the host view's onKeyDown
      * @return true if the event was handled, false if it was ignored.
      *
      * @see #setModal(boolean)
      */
    virtual CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
      * Filter key down events. By forwarding key up events to this function,
      * views using non-modal ListPopupWindow can have it handle key selection of items.
      *
      * @param keyCode keyCode param passed to the host view's onKeyUp
      * @param event event param passed to the host view's onKeyUp
      * @return true if the event was handled, false if it was ignored.
      *
      * @see #setModal(boolean)
      */
    virtual CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
      * Filter pre-IME key events. By forwarding {@link View#onKeyPreIme(int, KeyEvent)}
      * events to this function, views using ListPopupWindow can have it dismiss the popup
      * when the back key is pressed.
      *
      * @param keyCode keyCode param passed to the host view's onKeyPreIme
      * @param event event param passed to the host view's onKeyPreIme
      * @return true if the event was handled, false if it was ignored.
      *
      * @see #setModal(boolean)
      */
    virtual CARAPI OnKeyPreIme(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    /**
      * Returns an {@link OnTouchListener} that can be added to the source view
      * to implement drag-to-open behavior. Generally, the source view should be
      * the same view that was passed to {@link #setAnchorView}.
      * <p>
      * When the listener is set on a view, touching that view and dragging
      * outside of its bounds will open the popup window. Lifting will select the
      * currently touched list item.
      * <p>
      * Example usage:
      * <pre>
      * ListPopupWindow myPopup = new ListPopupWindow(context);
      * myPopup.setAnchor(myAnchor);
      * OnTouchListener dragListener = myPopup.createDragToOpenListener(myAnchor);
      * myAnchor.setOnTouchListener(dragListener);
      * </pre>
      *
      * @param src the view on which the resulting listener will be set
      * @return a touch listener that controls drag-to-open behavior
      */
    virtual CARAPI CreateDragToOpenListener(
        /* [in] */ IView* src,
        /* [out] */ IViewOnTouchListener** result);

private:
    CARAPI RemovePromptView();

    /**
      * <p>Builds the popup window's content and returns the height the popup
      * should have. Returns -1 when the content already exists.</p>
      *
      * @return the content's height or -1 if content already exists
      */
    CARAPI_(Int32) BuildDropDown();

public:
    Int32 mListItemExpandMaximum;
    static const Int32 POSITION_PROMPT_ABOVE = 0;
    static const Int32 POSITION_PROMPT_BELOW = 1;
    static const Int32 MATCH_PARENT = IViewGroupLayoutParams::MATCH_PARENT;
    static const Int32 WRAP_CONTENT = IViewGroupLayoutParams::WRAP_CONTENT;
    static const Int32 INPUT_METHOD_FROM_FOCUSABLE = IPopupWindow::INPUT_METHOD_FROM_FOCUSABLE;
    static const Int32 INPUT_METHOD_NEEDED = IPopupWindow::INPUT_METHOD_NEEDED;
    static const Int32 INPUT_METHOD_NOT_NEEDED = IPopupWindow::INPUT_METHOD_NOT_NEEDED;

private:
    static const String TAG;
    static const Boolean DEBUG;
    static const Int32 EXPAND_LIST_TIMEOUT = 250;
    AutoPtr<IContext> mContext;
    AutoPtr<IPopupWindow> mPopup;
    AutoPtr<IListAdapter> mAdapter;
    AutoPtr<DropDownListView> mDropDownList;
    Int32 mDropDownHeight;
    Int32 mDropDownWidth;
    Int32 mDropDownHorizontalOffset;
    Int32 mDropDownVerticalOffset;
    Boolean mDropDownVerticalOffsetSet;
    Int32 mDropDownGravity;
    Boolean mDropDownAlwaysVisible;
    Boolean mForceIgnoreOutsideTouch;
    AutoPtr<IView> mPromptView;
    Int32 mPromptPosition;
    AutoPtr<IDataSetObserver> mObserver;
    AutoPtr<IView> mDropDownAnchorView;
    AutoPtr<IDrawable> mDropDownListHighlight;
    AutoPtr<IAdapterViewOnItemClickListener> mItemClickListener;
    AutoPtr<IAdapterViewOnItemSelectedListener> mItemSelectedListener;
    /*const*/ AutoPtr<ResizePopupRunnable> mResizePopupRunnable;
    /*const*/ AutoPtr<PopupTouchInterceptor> mTouchInterceptor;
    /*const*/ AutoPtr<PopupScrollListener> mScrollListener;
    /*const*/ AutoPtr<ListSelectorHider> mHideSelector;
    AutoPtr<IRunnable> mShowDropDownRunnable;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IRect> mTempRect;
    Boolean mModal;
    Int32 mLayoutDirection;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_LISTPOPUPWINDOW_H__

