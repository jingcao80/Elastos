#include "elastos/droid/systemui/statusbar/tablet/CTabletStatusBarView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


IVIEW_METHODS_IMPL(CTabletStatusBarView, TabletStatusBarView)
IVIEWGROUP_METHODS_IMPL(CTabletStatusBarView, TabletStatusBarView)
IVIEWPARENT_METHODS_IMPL(CTabletStatusBarView, TabletStatusBarView)
IVIEWMANAGER_METHODS_IMPL(CTabletStatusBarView, TabletStatusBarView)
IDRAWABLECALLBACK_METHODS_IMPL(CTabletStatusBarView, TabletStatusBarView)
IKEYEVENTCALLBACK_METHODS_IMPL(CTabletStatusBarView, TabletStatusBarView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CTabletStatusBarView, TabletStatusBarView)
IFRAMELAYOUT_METHODS_IMPL(CTabletStatusBarView, TabletStatusBarView)


PInterface CTabletStatusBarView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CTabletStatusBarView::Probe(riid);
}

ECode CTabletStatusBarView::constructor(
    /* [in] */ IContext* context)
{
    return TabletStatusBarView::Init(context);
}

ECode CTabletStatusBarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TabletStatusBarView::Init(context, attrs);
}

ECode CTabletStatusBarView::SetDelegateView(
    /* [in] */ IView* view)
{
    return TabletStatusBarView::SetDelegateView(view);
}

ECode CTabletStatusBarView::SetBar(
    /* [in] */ IBaseStatusBar* bar)
{
    return TabletStatusBarView::SetBar(bar);
}

ECode CTabletStatusBarView::SetHandler(
    /* [in] */ IHandler* h)
{
    return TabletStatusBarView::SetHandler(h);
}

ECode CTabletStatusBarView::SetIgnoreChildren(
    /* [in] */ Int32 index,
    /* [in] */ IView* ignore,
    /* [in] */ IView* panel)
{
    return TabletStatusBarView::SetIgnoreChildren(index, ignore, panel);
}

ECode CTabletStatusBarView::SetShowVolume(
        /* [in] */ Boolean show,
        /* [in] */ IContext* context)
{
    return TabletStatusBarView::SetShowVolume(show, context);
}

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
