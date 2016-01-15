
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/TabActivity.h"
#include "elastos/droid/R.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(TabActivity, ActivityGroup, ITabActivity)

TabActivity::TabActivity()
    : mDefaultTabIndex(-1)
{}

TabActivity::~TabActivity()
{}

ECode TabActivity::constructor()
{
    return ActivityGroup::constructor();
}

ECode TabActivity::SetDefaultTab(
    /* [in] */ const String& tag)
{
    mDefaultTab = tag;
    mDefaultTabIndex = -1;
    return NOERROR;
}

ECode TabActivity::SetDefaultTab(
    /* [in] */ Int32 index)
{
    mDefaultTab = NULL;
    mDefaultTabIndex = index;
    return NOERROR;
}

ECode TabActivity::OnRestoreInstanceState(
    /* [in] */ IBundle* state)
{
    ActivityGroup::OnRestoreInstanceState(state);
    FAIL_RETURN(EnsureTabHost())
    String cur;
    state->GetString(String("currentTab"), &cur);
    if (cur != NULL) {
        mTabHost->SetCurrentTabByTag(cur);
    }
    Int32 curTab;
    mTabHost->GetCurrentTab(&curTab);
    if (curTab < 0) {
        if (mDefaultTab != NULL) {
            mTabHost->SetCurrentTabByTag(mDefaultTab);
        }
        else if (mDefaultTabIndex >= 0) {
            mTabHost->SetCurrentTab(mDefaultTabIndex);
        }
    }
    return NOERROR;
}

ECode TabActivity::OnPostCreate(
    /* [in] */ IBundle* icicle)
{
    ActivityGroup::OnPostCreate(icicle);

    FAIL_RETURN(EnsureTabHost())

    Int32 curTab;
    mTabHost->GetCurrentTab(&curTab);
    if (curTab == -1) {
        mTabHost->SetCurrentTab(0);
    }
    return NOERROR;
}

ECode TabActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    ActivityGroup::OnSaveInstanceState(outState);
    String currentTabTag;
    mTabHost->GetCurrentTabTag(&currentTabTag);
    if (!currentTabTag.IsNull()) {
        outState->PutString(String("currentTab"), currentTabTag);
    }
    return NOERROR;
}

ECode TabActivity::OnContentChanged()
{
    ActivityGroup::OnContentChanged();
    AutoPtr<IView> view;
    FindViewById(R::id::tabhost, (IView**)&view);
    mTabHost = ITabHost::Probe(view);

    if (mTabHost == NULL) {
        // throw new RuntimeException(
        //         "Your content must have a TabHost whose id attribute is " +
        //         "'android.R.id.tabhost'");
        return E_RUNTIME_EXCEPTION;
    }
    AutoPtr<ILocalActivityManager> mgr;
    GetLocalActivityManager((ILocalActivityManager**)&mgr);
    mTabHost->Setup(mgr);
    return NOERROR;
}

ECode TabActivity::EnsureTabHost()
{
    if (mTabHost == NULL) {
        SetContentView(R::layout::tab_content);
    }
    return NOERROR;
}

ECode TabActivity::OnChildTitleChanged(
    /* [in] */ IActivity* childActivity,
    /* [in] */ ICharSequence* title)
{
    // Dorky implementation until we can have multiple activities running.
    AutoPtr<ILocalActivityManager> mgr;
    GetLocalActivityManager((ILocalActivityManager**)&mgr);
    AutoPtr<IActivity> activity;
    mgr->GetCurrentActivity((IActivity**)&activity);
    if (activity.Get() == childActivity) {
        AutoPtr<IView> tabView;
        mTabHost->GetCurrentTabView((IView**)&tabView);
        if (tabView != NULL && ITextView::Probe(tabView)) {
            ITextView::Probe(tabView)->SetText(title);
        }
    }
    return NOERROR;
}

ECode TabActivity::GetTabHost(
    /* [out] */ ITabHost** tab)
{
    FAIL_RETURN(EnsureTabHost())
    *tab = mTabHost;
    REFCOUNT_ADD(*tab)
    return NOERROR;
}

ECode TabActivity::GetTabWidget(
    /* [out] */ ITabWidget** tab)
{
    return mTabHost->GetTabWidget(tab);
}


} // namespace App
} // namespace Droid
} // namespace Elastos
