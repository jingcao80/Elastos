#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CTabWidget.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {


IVIEW_METHODS_IMPL(CTabWidget, TabWidget)
IVIEWGROUP_METHODS_IMPL(CTabWidget, TabWidget)
IVIEWPARENT_METHODS_IMPL(CTabWidget, TabWidget)
IVIEWMANAGER_METHODS_IMPL(CTabWidget, TabWidget)
IDRAWABLECALLBACK_METHODS_IMPL(CTabWidget, TabWidget)
IKEYEVENTCALLBACK_METHODS_IMPL(CTabWidget, TabWidget)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CTabWidget, TabWidget)
ILINEARLAYOUT_METHODS_IMPL(CTabWidget, TabWidget)

PInterface CTabWidget::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    else if (riid == EIID_TabWidget) {
        return reinterpret_cast<PInterface>((TabWidget*)this);
    }
    return _CTabWidget::Probe(riid);
}

ECode CTabWidget::constructor(
    /* [in] */ IContext* context)
{
    return TabWidget::Init(context);
}

ECode CTabWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TabWidget::Init(context, attrs);
}

ECode CTabWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return TabWidget::Init(context, attrs, defStyle);
}

/**
 * Returns the tab indicator view at the given index.
 *
 * @param index the zero-based index of the tab indicator view to return
 * @return the tab indicator view at the given index
 */
ECode CTabWidget::GetChildTabViewAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> v = TabWidget::GetChildTabViewAt(index);
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

/**
 * Returns the number of tab indicator views.
 * @return the number of tab indicator views.
 */
ECode CTabWidget::GetTabCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = TabWidget::GetTabCount();
    return NOERROR;
}

/**
 * Sets the drawable to use as a divider between the tab indicators.
 * @param resId the resource identifier of the drawable to use as a
 * divider.
 */
ECode CTabWidget::SetDividerDrawable(
    /* [in] */ Int32 resId)
{
    return TabWidget::SetDividerDrawable(resId);
}

/**
 * Sets the drawable to use as the left part of the strip below the
 * tab indicators.
 * @param drawable the left strip drawable
 */
ECode CTabWidget::SetLeftStripDrawable(
    /* [in] */ IDrawable* drawable)
{
    return TabWidget::SetLeftStripDrawable(drawable);
}

/**
 * Sets the drawable to use as the left part of the strip below the
 * tab indicators.
 * @param resId the resource identifier of the drawable to use as the
 * left strip drawable
 */
ECode CTabWidget::SetLeftStripDrawable(
    /* [in] */ Int32 resId)
{
    return TabWidget::SetLeftStripDrawable(resId);
}

/**
 * Sets the drawable to use as the right part of the strip below the
 * tab indicators.
 * @param drawable the right strip drawable
 */
ECode CTabWidget::SetRightStripDrawable(
    /* [in] */ IDrawable* drawable)
{
    return TabWidget::SetRightStripDrawable(drawable);
}

/**
 * Sets the drawable to use as the right part of the strip below the
 * tab indicators.
 * @param resId the resource identifier of the drawable to use as the
 * right strip drawable
 */
ECode CTabWidget::SetRightStripDrawable(
    /* [in] */ Int32 resId)
{
    return TabWidget::SetRightStripDrawable(resId);
}

/**
 * Controls whether the bottom strips on the tab indicators are drawn or
 * not.  The default is to draw them.  If the user specifies a custom
 * view for the tab indicators, then the TabHost class calls this method
 * to disable drawing of the bottom strips.
 * @param stripEnabled true if the bottom strips should be drawn.
 */
ECode CTabWidget::SetStripEnabled(
    /* [in] */ Boolean stripEnabled)
{
    return TabWidget::SetStripEnabled(stripEnabled);
}

/**
 * Indicates whether the bottom strips on the tab indicators are drawn
 * or not.
 */
ECode CTabWidget::IsStripEnabled(
    /* [out] */ Boolean* stripEnabled)
{
    VALIDATE_NOT_NULL(stripEnabled);
    *stripEnabled = TabWidget::IsStripEnabled();
    return NOERROR;
}

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
ECode CTabWidget::SetCurrentTab(
    /* [in] */ Int32 index)
{
    return TabWidget::SetCurrentTab(index);
}

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
ECode CTabWidget::FocusCurrentTab(
    /* [in] */ Int32 index)
{
    return TabWidget::FocusCurrentTab(index);
}

ECode CTabWidget::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    return TabWidget::OnFocusChange(v, hasFocus);
}


} // namespace Widget
} // namespace Droid
} // namespace Elastos
