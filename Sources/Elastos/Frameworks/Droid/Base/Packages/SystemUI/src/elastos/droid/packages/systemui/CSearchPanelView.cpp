#include "elastos/droid/systemui/CSearchPanelView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {


IVIEW_METHODS_IMPL(CSearchPanelView, SearchPanelView)
IVIEWGROUP_METHODS_IMPL(CSearchPanelView, SearchPanelView)
IVIEWPARENT_METHODS_IMPL(CSearchPanelView, SearchPanelView)
IVIEWMANAGER_METHODS_IMPL(CSearchPanelView, SearchPanelView)
IDRAWABLECALLBACK_METHODS_IMPL(CSearchPanelView, SearchPanelView)
IKEYEVENTCALLBACK_METHODS_IMPL(CSearchPanelView, SearchPanelView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CSearchPanelView, SearchPanelView)
IFRAMELAYOUT_METHODS_IMPL(CSearchPanelView, SearchPanelView)

PInterface CSearchPanelView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CSearchPanelView::Probe(riid);
}

ECode CSearchPanelView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(context, attrs);
}

ECode CSearchPanelView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return Init(context, attrs);
}

ECode CSearchPanelView::OnAnimationStarted()
{
    return SearchPanelView::OnAnimationStarted();
}

ECode CSearchPanelView::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchPanelView::IsInContentArea(x, y);
    return NOERROR;
}

ECode CSearchPanelView::Show(
    /* [in] */ Boolean show,
    /* [in] */ Boolean animate)
{
    return SearchPanelView::Show(show, animate);
}

ECode CSearchPanelView::Hide(
    /* [in] */ Boolean animate)
{
    return SearchPanelView::Hide(animate);
}

ECode CSearchPanelView::IsShowing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchPanelView::IsShowing();
    return NOERROR;
}

ECode CSearchPanelView::SetBar(
    /* [in] */ IBaseStatusBar* bar)
{
    return SearchPanelView::SetBar(bar);
}

ECode CSearchPanelView::SetStatusBarView(
    /* [in] */ IView* statusBarView)
{
    return SearchPanelView::SetStatusBarView(statusBarView);
}

ECode CSearchPanelView::IsAssistantAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = SearchPanelView::IsAssistantAvailable();
    return NOERROR;
}


}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
