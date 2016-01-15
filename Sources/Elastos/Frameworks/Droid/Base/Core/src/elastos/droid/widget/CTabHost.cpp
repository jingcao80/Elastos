#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CTabHost.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {


IVIEW_METHODS_IMPL(CTabHost, TabHost)
IVIEWGROUP_METHODS_IMPL(CTabHost, TabHost)
IVIEWPARENT_METHODS_IMPL(CTabHost, TabHost)
IVIEWMANAGER_METHODS_IMPL(CTabHost, TabHost)
IDRAWABLECALLBACK_METHODS_IMPL(CTabHost, TabHost)
IKEYEVENTCALLBACK_METHODS_IMPL(CTabHost, TabHost)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CTabHost, TabHost)
IFRAMELAYOUT_METHODS_IMPL(CTabHost, TabHost)

PInterface CTabHost::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CTabHost::Probe(riid);
}

ECode CTabHost::constructor(
    /* [in] */ IContext* context)
{
    return TabHost::Init(context);
}

ECode CTabHost::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TabHost::Init(context, attrs);
}

ECode CTabHost::NewTabSpec(
    /* [in] */ const String& tag,
    /* [out] */ ITabSpec** tabspec)
{
    VALIDATE_NOT_NULL(tabspec);
    AutoPtr<ITabSpec> ts = TabHost::NewTabSpec(tag);
    *tabspec = ts;
    REFCOUNT_ADD(*tabspec);
    return NOERROR;
}

ECode CTabHost::Setup()
{
    return TabHost::Setup();
}

ECode CTabHost::Setup(
    /* [in] */ ILocalActivityManager* activityGroup)
{
    return TabHost::Setup(activityGroup);
}

ECode CTabHost::AddTab(
    /* [in] */ ITabSpec* tabSpec)
{
    return TabHost::AddTab(tabSpec);
}

ECode CTabHost::ClearAllTabs()
{
    return TabHost::ClearAllTabs();
}

ECode CTabHost::GetTabWidget(
    /* [out] */ ITabWidget** tabWidget)
{
    VALIDATE_NOT_NULL(tabWidget);
    AutoPtr<ITabWidget> tw = TabHost::GetTabWidget();
    *tabWidget = tw.Get();
    REFCOUNT_ADD(*tabWidget);
    return NOERROR;
}

ECode CTabHost::GetCurrentTab(
    /* [out] */ Int32* tab)
{
    VALIDATE_NOT_NULL(tab);
    *tab = TabHost::GetCurrentTab();
    return NOERROR;
}

ECode CTabHost::GetCurrentTabTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag);
    *tag = TabHost::GetCurrentTabTag();
    return NOERROR;
}

ECode CTabHost::GetCurrentTabView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> tw = TabHost::GetCurrentTabView();
    *view = tw.Get();
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CTabHost::GetCurrentView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> tw = TabHost::GetCurrentView();
    *view = tw.Get();
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CTabHost::SetCurrentTabByTag(
    /* [in] */ const String& tag)
{
    return TabHost::SetCurrentTabByTag(tag);
}

ECode CTabHost::GetTabContentView(
    /* [out] */ IFrameLayout** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IFrameLayout> tw = TabHost::GetTabContentView();
    *view = tw.Get();
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CTabHost::SetCurrentTab(
    /* [in] */ Int32 index)
{
    return TabHost::SetCurrentTab(index);
}

ECode CTabHost::SetOnTabChangedListener(
    /* [in] */ ITabHostOnTabChangeListener* l)
{
    return TabHost::SetOnTabChangedListener(l);
}

ECode CTabHost::OnTouchModeChanged(
    /* [in] */ Boolean isInTouchMode)
{
    return TabHost::OnTouchModeChanged(isInTouchMode);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
