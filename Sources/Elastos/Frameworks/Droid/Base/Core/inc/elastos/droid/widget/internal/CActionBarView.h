#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_CACTIONBARVIEW_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_CACTIONBARVIEW_H__

#include "_Elastos_Droid_Widget_Internal_CActionBarView.h"


#include "elastos/droid/widget/internal/ActionBarView.h"

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

CarClass(CActionBarView), public ActionBarView
{
public:
    IVIEW_METHODS_DECL()
    IVIEWGROUP_METHODS_DECL()
    IVIEWPARENT_METHODS_DECL()
    IVIEWMANAGER_METHODS_DECL()
    IDRAWABLECALLBACK_METHODS_DECL()
    IKEYEVENTCALLBACK_METHODS_DECL()
    IACCESSIBILITYEVENTSOURCE_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetWindowCallback(
        /* [in] */ IWindowCallback* cb);

    CARAPI InitProgress();

    CARAPI InitIndeterminateProgress();

    CARAPI IsSplitActionBar(
        /* [out] */ Boolean* res);

    CARAPI HasEmbeddedTabs(
        /* [out] */ Boolean* res);

    CARAPI SetEmbeddedTabView(
        /* [in] */ IScrollingTabContainerView* tabs);

    CARAPI SetCallback(
        /* [in] */ IActionBarOnNavigationListener* cb);

    CARAPI SetMenu(
        /* [in] */ IMenu* menu,
        /* [in] */ IMenuPresenterCallback* cb);

    CARAPI HasExpandedActionView(
        /* [out] */ Boolean* res);

    CARAPI CollapseActionView();

    CARAPI SetCustomNavigationView(
        /* [in] */ IView* view);

    CARAPI GetTitle(
        /* [out] */ ICharSequence** title);

    CARAPI SetTitle(
        /* [in] */ ICharSequence* title);

    CARAPI SetWindowTitle(
        /* [in] */ ICharSequence* title);

    CARAPI GetSubtitle(
        /* [out] */ ICharSequence** subTitle);

    CARAPI SetSubtitle(
        /* [in] */ ICharSequence* subtitle);

    CARAPI SetHomeButtonEnabled(
        /* [in] */ Boolean able);

    CARAPI SetDisplayOptions(
        /* [in] */ Int32 options);

    CARAPI SetIcon(
        /* [in] */ IDrawable* icon);

    CARAPI SetIcon(
        /* [in] */ Int32 resId);

    CARAPI SetLogo(
        /* [in] */ IDrawable* logo);

    CARAPI SetLogo(
        /* [in] */ Int32 resId);

    CARAPI SetNavigationMode(
        /* [in] */ Int32 mode);

    CARAPI SetDropdownAdapter(
        /* [in] */ ISpinnerAdapter* adapter);

    CARAPI GetDropdownAdapter(
        /* [out] */ ISpinnerAdapter** adapter);

    CARAPI SetDropdownSelectedPosition(
        /* [in] */ Int32 position);

    CARAPI GetDropdownSelectedPosition(
        /* [out] */ Int32* pos);

    CARAPI GetCustomNavigationView(
        /* [out] */ IView** view);

    CARAPI GetNavigationMode(
        /* [out] */ Int32* mode);

    CARAPI GetDisplayOptions(
        /* [out] */ Int32* options);

    CARAPI SetContextView(
        /* [in] */ IActionBarContextView* view);

    CARAPI SetCollapsable(
        /* [in] */ Boolean collapsable);

    CARAPI IsCollapsed(
        /* [out] */ Boolean* collapsed);

    CARAPI IsTitleTruncated(
        /* [out] */ Boolean* isTruncated);

    CARAPI SetSplitActionBar(
        /* [in] */ Boolean split);

    CARAPI SetSplitWhenNarrow(
        /* [in] */ Boolean splitWhenNarrow);

    CARAPI SetContentHeight(
        /* [in] */ Int32 height);

    CARAPI GetContentHeight(
        /* [out] */ Int32* height);

    CARAPI SetSplitView(
        /* [in] */ IActionBarContainer* splitView);

    CARAPI GetAnimatedVisibility(
        /* [out] */ Int32* visible);

    CARAPI AnimateToVisibility(
        /* [in] */ Int32 visibility);

    CARAPI ShowOverflowMenu(
        /* [out] */ Boolean* menu);

    CARAPI PostShowOverflowMenu();

    CARAPI HideOverflowMenu(
        /* [out] */ Boolean* menu);

    CARAPI IsOverflowMenuShowing(
        /* [out] */ Boolean* showing);

    CARAPI IsOverflowReserved(
        /* [out] */ Boolean* reserved);

    CARAPI DismissPopupMenus();
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_INTERNAL_CACTIONBARVIEW_H__
