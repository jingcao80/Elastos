
#ifndef __ELASTOS_DROID_WIDGET_TABWIDGET_H__
#define __ELASTOS_DROID_WIDGET_TABWIDGET_H__

#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnFocusChangeListener;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 *
 * Displays a list of tab labels representing each page in the parent's tab
 * collection. The container object for this widget is
 * {@link android.widget.TabHost TabHost}. When the user selects a tab, this
 * object sends a message to the parent container, TabHost, to tell it to switch
 * the displayed page. You typically won't use many methods directly on this
 * object. The container TabHost is used to add labels, add the callback
 * handler, and manage callbacks. You might call this object to iterate the list
 * of tabs, or to tweak the layout of the tab list, but most methods should be
 * called on the containing TabHost object.
 *
 * <p>See the <a href="{@docRoot}resources/tutorials/views/hello-tabwidget.html">Tab Layout
 * tutorial</a>.</p>
 *
 * @attr ref android.R.styleable#TabWidget_divider
 * @attr ref android.R.styleable#TabWidget_tabStripEnabled
 * @attr ref android.R.styleable#TabWidget_tabStripLeft
 * @attr ref android.R.styleable#TabWidget_tabStripRight
 */

class ECO_PUBLIC TabWidget
    : public LinearLayout
    , public ITabWidget
{
private:
    // registered with each tab indicator so we can notify tab host
    class TabListener
        : public Object
        , public IViewOnClickListener
        , public IViewOnFocusChangeListener
    {
        friend class TabWidget;

    public:
        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

        virtual CARAPI OnFocusChange(
            /* [in] */ IView* v,
            /* [in] */ Boolean hasFocus);
    private:
        TabListener(
            /* [in] */ Int32 tabIndex,
            /* [in] */ TabWidget* owner);

    private:
        Int32 mTabIndex;

        TabWidget* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TabWidget();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyleAttr = R::attr::tabWidgetStyle,
        /* [in] */ Int32 defStyleRes = 0);

    /**
     * Returns the tab indicator view at the given index.
     *
     * @param index the zero-based index of the tab indicator view to return
     * @return the tab indicator view at the given index
     */
    CARAPI GetChildTabViewAt(
        /* [in] */ Int32 index,
        /* [out] */ IView** v);

    /**
     * Returns the number of tab indicator views.
     * @return the number of tab indicator views.
     */
    CARAPI GetTabCount(
        /* [out] */ Int32* count);

    /**
     * Sets the drawable to use as a divider between the tab indicators.
     * @param drawable the divider drawable
     */
    CARAPI SetDividerDrawable(
        /* [in] */ IDrawable* drawable);

    /**
     * Sets the drawable to use as a divider between the tab indicators.
     * @param resId the resource identifier of the drawable to use as a
     * divider.
     */
    CARAPI SetDividerDrawable(
        /* [in] */ Int32 resId);

    /**
     * Sets the drawable to use as the left part of the strip below the
     * tab indicators.
     * @param drawable the left strip drawable
     */
    CARAPI SetLeftStripDrawable(
        /* [in] */ IDrawable* drawable);

    /**
     * Sets the drawable to use as the left part of the strip below the
     * tab indicators.
     * @param resId the resource identifier of the drawable to use as the
     * left strip drawable
     */
    CARAPI SetLeftStripDrawable(
        /* [in] */ Int32 resId);

    /**
     * Sets the drawable to use as the right part of the strip below the
     * tab indicators.
     * @param drawable the right strip drawable
     */
    CARAPI SetRightStripDrawable(
        /* [in] */ IDrawable* drawable);

    /**
     * Sets the drawable to use as the right part of the strip below the
     * tab indicators.
     * @param resId the resource identifier of the drawable to use as the
     * right strip drawable
     */
    CARAPI SetRightStripDrawable(
        /* [in] */ Int32 resId);

    /**
     * Controls whether the bottom strips on the tab indicators are drawn or
     * not.  The default is to draw them.  If the user specifies a custom
     * view for the tab indicators, then the TabHost class calls this method
     * to disable drawing of the bottom strips.
     * @param stripEnabled true if the bottom strips should be drawn.
     */
    CARAPI SetStripEnabled(
        /* [in] */ Boolean stripEnabled);

    /**
     * Indicates whether the bottom strips on the tab indicators are drawn
     * or not.
     */
    CARAPI IsStripEnabled(
        /* [out] */ Boolean* enabled);

    //@Override
    virtual CARAPI ChildDrawableStateChanged(
        /* [in] */ IView* child);

    //@Override
    virtual CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    /**
     * Sets the current tab.
     * This method is used to bring a tab to the front of the Widget,
     * and is used to post to the rest of the UI that a different tab
     * has been brought to the foreground.
     *
     * Note, this is separate from the traditional "focus" that is
     * employed from the view logic.
     *
     * For instance, if we have a list in a tabbed view, a user may be
     * navigating up and down the list, moving the UI focus (orange
     * highlighting) through the list items.  The cursor movement does
     * not effect the "selected" tab though, because what is being
     * scrolled through is all on the same tab.  The selected tab only
     * changes when we navigate between tabs (moving from the list view
     * to the next tabbed view, in this example).
     *
     * To move both the focus AND the selected tab at once, please use
     * {@link #setCurrentTab}. Normally, the view logic takes care of
     * adjusting the focus, so unless you're circumventing the UI,
     * you'll probably just focus your interest here.
     *
     *  @param index The tab that you want to indicate as the selected
     *  tab (tab brought to the front of the widget)
     *
     *  @see #focusCurrentTab
     */
    CARAPI SetCurrentTab(
        /* [in] */ Int32 index);

    /**
     * Sets the current tab and focuses the UI on it.
     * This method makes sure that the focused tab matches the selected
     * tab, normally at {@link #setCurrentTab}.  Normally this would not
     * be an issue if we go through the UI, since the UI is responsible
     * for calling TabWidget.onFocusChanged(), but in the case where we
     * are selecting the tab programmatically, we'll need to make sure
     * focus keeps up.
     *
     *  @param index The tab that you want focused (highlighted in orange)
     *  and selected (tab brought to the front of the widget)
     *
     *  @see #setCurrentTab
     */
    CARAPI FocusCurrentTab(
        /* [in] */ Int32 index);

    //@Override
    virtual CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    using LinearLayout::AddView;

    //@Override
    virtual CARAPI AddView(
        /* [in] */ IView* child);
    /**
     * Provides a way for {@link TabHost} to be notified that the user clicked on a tab indicator.
     */
    virtual CARAPI SetTabSelectionListener(
        /* [in] */ ITabWidgetOnTabSelectionChanged* listener);

    virtual CARAPI OnFocusChange(
        /* [in] */ IView* v,
        /* [in] */ Boolean hasFocus);

    virtual CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* res);

    virtual CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI SendAccessibilityEventUnchecked(
        /* [in] */ IAccessibilityEvent* event);

    virtual CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    virtual CARAPI RemoveAllViews();


protected:
    //@Override
    virtual CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    //@Override
    virtual CARAPI_(Int32) GetChildDrawingOrder(
        /* [in] */ Int32 childCount,
        /* [in] */ Int32 i);

    CARAPI_(void) MeasureChildBeforeLayout(
        /* [in] */ IView* child,
        /* [in] */ Int32 childIndex,
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 totalWidth,
        /* [in] */ Int32 heightMeasureSpec,
        /* [in] */ Int32 totalHeight);

    CARAPI_(void) MeasureHorizontal(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    CARAPI_(void) InitTabWidget();

    CARAPI InitFromAttributes(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

private:

    AutoPtr<ITabWidgetOnTabSelectionChanged> mSelectionChangedListener;

    Int32 mSelectedTab;

    AutoPtr<IDrawable> mLeftStrip;
    AutoPtr<IDrawable> mRightStrip;

    Boolean mDrawBottomStrips;
    Boolean mStripMoved;

    AutoPtr<IRect> mBounds;

    // When positive, the widths and heights of tabs will be imposed so that they fit in parent
    Int32 mImposedTabsHeight;// = -1;
    AutoPtr<ArrayOf<Int32> > mImposedTabWidths;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
