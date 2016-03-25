#include "elastos/droid/widget/internal/CActionBarView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

IVIEW_METHODS_IMPL(CActionBarView, ActionBarView)
IVIEWGROUP_METHODS_IMPL(CActionBarView, ActionBarView)
IVIEWPARENT_METHODS_IMPL(CActionBarView, ActionBarView)
IVIEWMANAGER_METHODS_IMPL(CActionBarView, ActionBarView)
IDRAWABLECALLBACK_METHODS_IMPL(CActionBarView, ActionBarView)
IKEYEVENTCALLBACK_METHODS_IMPL(CActionBarView, ActionBarView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CActionBarView, ActionBarView)

PInterface CActionBarView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CActionBarView::Probe(riid);
}

ECode CActionBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ActionBarView::Init(context, attrs);
}

ECode CActionBarView::SetWindowCallback(
    /* [in] */ IWindowCallback* cb)
{
    return ActionBarView::SetWindowCallback(cb);
}

ECode CActionBarView::InitProgress()
{
    return ActionBarView::InitProgress();
}

ECode CActionBarView::InitIndeterminateProgress()
{
    return ActionBarView::InitIndeterminateProgress();
}

ECode CActionBarView::IsSplitActionBar(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ActionBarView::IsSplitActionBar();
    return NOERROR;
}

ECode CActionBarView::HasEmbeddedTabs(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ActionBarView::HasEmbeddedTabs();
    return NOERROR;
}

ECode CActionBarView::SetEmbeddedTabView(
    /* [in] */ IScrollingTabContainerView* tabs)
{
    return ActionBarView::SetEmbeddedTabView(tabs);
}

ECode CActionBarView::SetCallback(
    /* [in] */ IActionBarOnNavigationListener* cb)
{
    return ActionBarView::SetCallback(cb);
}

ECode CActionBarView::SetMenu(
    /* [in] */ IMenu* menu,
    /* [in] */ IMenuPresenterCallback* cb)
{
    return ActionBarView::SetMenu(menu, cb);
}

ECode CActionBarView::HasExpandedActionView(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = ActionBarView::HasExpandedActionView();
    return NOERROR;
}

ECode CActionBarView::CollapseActionView()
{
    return ActionBarView::CollapseActionView();
}

ECode CActionBarView::SetCustomNavigationView(
    /* [in] */ IView* view)
{
    return ActionBarView::SetCustomNavigationView(view);
}

ECode CActionBarView::GetTitle(
    /* [out] */ ICharSequence** title)
{
    VALIDATE_NOT_NULL(title);
    AutoPtr<ICharSequence> temp = ActionBarView::GetTitle();
    *title = temp;
    REFCOUNT_ADD(*title);
    return NOERROR;
}

ECode CActionBarView::SetTitle(
    /* [in] */ ICharSequence* title)
{
    return ActionBarView::SetTitle(title);
}

ECode CActionBarView::SetWindowTitle(
    /* [in] */ ICharSequence* title)
{
    return ActionBarView::SetWindowTitle(title);
}

ECode CActionBarView::GetSubtitle(
    /* [out] */ ICharSequence** subTitle)
{
    VALIDATE_NOT_NULL(subTitle);
    AutoPtr<ICharSequence> temp = ActionBarView::GetSubtitle();
    *subTitle = temp;
    REFCOUNT_ADD(*subTitle);
    return NOERROR;
}

ECode CActionBarView::SetSubtitle(
    /* [in] */ ICharSequence* subtitle)
{
    return ActionBarView::SetSubtitle(subtitle);
}

ECode CActionBarView::SetHomeButtonEnabled(
    /* [in] */ Boolean able)
{
    return ActionBarView::SetHomeButtonEnabled(able);
}

ECode CActionBarView::SetDisplayOptions(
    /* [in] */ Int32 options)
{
    return ActionBarView::SetDisplayOptions(options);
}

ECode CActionBarView::SetIcon(
    /* [in] */ IDrawable* icon)
{
    return ActionBarView::SetIcon(icon);
}

ECode CActionBarView::SetIcon(
    /* [in] */ Int32 resId)
{
    return ActionBarView::SetIcon(resId);
}

ECode CActionBarView::SetLogo(
    /* [in] */ IDrawable* logo)
{
    return ActionBarView::SetLogo(logo);
}

ECode CActionBarView::SetLogo(
    /* [in] */ Int32 resId)
{
    return ActionBarView::SetLogo(resId);
}

ECode CActionBarView::SetNavigationMode(
    /* [in] */ Int32 mode)
{
    return ActionBarView::SetNavigationMode(mode);
}

ECode CActionBarView::SetDropdownAdapter(
    /* [in] */ ISpinnerAdapter* adapter)
{
    return ActionBarView::SetDropdownAdapter(adapter);
}

ECode CActionBarView::GetDropdownAdapter(
    /* [out] */ ISpinnerAdapter** adapter)
{
    VALIDATE_NOT_NULL(adapter);
    AutoPtr<ISpinnerAdapter> temp = ActionBarView::GetDropdownAdapter();
    *adapter = temp;
    REFCOUNT_ADD(*adapter);
    return NOERROR;
}

ECode CActionBarView::SetDropdownSelectedPosition(
    /* [in] */ Int32 position)
{
    return ActionBarView::SetDropdownSelectedPosition(position);
}

ECode CActionBarView::GetDropdownSelectedPosition(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos);
    *pos = ActionBarView::GetDropdownSelectedPosition();
    return NOERROR;
}

ECode CActionBarView::GetCustomNavigationView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = ActionBarView::GetCustomNavigationView();
    return NOERROR;
}

ECode CActionBarView::GetNavigationMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = ActionBarView::GetNavigationMode();
    return NOERROR;
}

ECode CActionBarView::GetDisplayOptions(
    /* [out] */ Int32* options)
{
    VALIDATE_NOT_NULL(options);
    *options = ActionBarView::GetDisplayOptions();
    return NOERROR;
}

ECode CActionBarView::SetContextView(
    /* [in] */ IActionBarContextView* view)
{
    return ActionBarView::SetContextView(view);
}

ECode CActionBarView::SetCollapsable(
    /* [in] */ Boolean collapsable)
{
    return ActionBarView::SetCollapsable(collapsable);
}

ECode CActionBarView::IsCollapsed(
    /* [out] */ Boolean* collapsed)
{
    VALIDATE_NOT_NULL(collapsed);
    *collapsed = ActionBarView::IsCollapsed();
    return NOERROR;
}

ECode CActionBarView::IsTitleTruncated(
    /* [out] */ Boolean* isTruncated)
{
    VALIDATE_NOT_NULL(isTruncated);
    *isTruncated = ActionBarView::IsTitleTruncated();
    return NOERROR;
}

ECode CActionBarView::SetSplitActionBar(
    /* [in] */ Boolean split)
{
    return ActionBarView::SetSplitActionBar(split);
}

ECode CActionBarView::SetSplitWhenNarrow(
    /* [in] */ Boolean splitWhenNarrow)
{
    return ActionBarView::SetSplitWhenNarrow(splitWhenNarrow);
}

ECode CActionBarView::SetContentHeight(
    /* [in] */ Int32 height)
{
    return ActionBarView::SetContentHeight(height);
}

ECode CActionBarView::GetContentHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = ActionBarView::GetContentHeight();
    return NOERROR;
}

ECode CActionBarView::SetSplitView(
    /* [in] */ IActionBarContainer* splitView)
{
    return ActionBarView::SetSplitView(splitView);
}

ECode CActionBarView::GetAnimatedVisibility(
    /* [out] */ Int32* visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = ActionBarView::GetAnimatedVisibility();
    return NOERROR;
}

ECode CActionBarView::AnimateToVisibility(
    /* [in] */ Int32 visibility)
{
    return ActionBarView::AnimateToVisibility(visibility);
}

ECode CActionBarView::ShowOverflowMenu(
    /* [out] */ Boolean* menu)
{
    VALIDATE_NOT_NULL(menu);
    *menu = ActionBarView::ShowOverflowMenu();
    return NOERROR;
}

ECode CActionBarView::PostShowOverflowMenu()
{
    return ActionBarView::PostShowOverflowMenu();
}

ECode CActionBarView::HideOverflowMenu(
    /* [out] */ Boolean* menu)
{
    VALIDATE_NOT_NULL(menu);
    *menu = ActionBarView::HideOverflowMenu();
    return NOERROR;
}

ECode CActionBarView::IsOverflowMenuShowing(
    /* [out] */ Boolean* showing)
{
    VALIDATE_NOT_NULL(showing);
    *showing = ActionBarView::IsOverflowMenuShowing();
    return NOERROR;
}

ECode CActionBarView::IsOverflowReserved(
    /* [out] */ Boolean* reserved)
{
    VALIDATE_NOT_NULL(reserved);
    *reserved = ActionBarView::IsOverflowReserved();
    return NOERROR;
}

ECode CActionBarView::DismissPopupMenus()
{
    return ActionBarView::DismissPopupMenus();
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
