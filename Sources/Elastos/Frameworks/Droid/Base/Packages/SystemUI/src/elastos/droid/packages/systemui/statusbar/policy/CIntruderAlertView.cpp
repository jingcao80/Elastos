#include "elastos/droid/systemui/statusbar/policy/CIntruderAlertView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

IVIEW_METHODS_IMPL(CIntruderAlertView, IntruderAlertView)
IVIEWGROUP_METHODS_IMPL(CIntruderAlertView, IntruderAlertView)
IVIEWPARENT_METHODS_IMPL(CIntruderAlertView, IntruderAlertView)
IVIEWMANAGER_METHODS_IMPL(CIntruderAlertView, IntruderAlertView)
IDRAWABLECALLBACK_METHODS_IMPL(CIntruderAlertView, IntruderAlertView)
IKEYEVENTCALLBACK_METHODS_IMPL(CIntruderAlertView, IntruderAlertView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CIntruderAlertView, IntruderAlertView)
ILINEARLAYOUT_METHODS_IMPL(CIntruderAlertView, IntruderAlertView)

PInterface CIntruderAlertView::Probe(
    /*  [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CIntruderAlertView::Probe(riid);
}

ECode CIntruderAlertView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return IntruderAlertView::Init(context, attrs);
}

ECode CIntruderAlertView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return IntruderAlertView::Init(context, attrs, defStyle);
}

ECode CIntruderAlertView::SetBar(
    /* [in] */ IBaseStatusBar* bar)
{
    return IntruderAlertView::SetBar(bar);
}

ECode CIntruderAlertView::ApplyIntruderContent(
    /* [in] */ IRemoteViews* intruderView,
    /* [in] */ IViewOnClickListener* listener)
{
    return IntruderAlertView::ApplyIntruderContent(intruderView, listener);
}

ECode CIntruderAlertView::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> tmpView = IntruderAlertView::GetChildAtPosition(ev);
    *result = tmpView.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CIntruderAlertView::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IView> tmpView = IntruderAlertView::GetChildContentView(v);
    *result = tmpView.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CIntruderAlertView::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IntruderAlertView::CanChildBeDismissed(v);
    return NOERROR;
}

ECode CIntruderAlertView::OnBeginDrag(
    /* [in] */ IView* v)
{
    return IntruderAlertView::OnBeginDrag(v);
}

ECode CIntruderAlertView::OnChildDismissed(
    /* [in] */ IView* v)
{
    return IntruderAlertView::OnChildDismissed(v);
}

ECode CIntruderAlertView::OnDragCancelled(
    /* [in] */ IView* v)
{
    return IntruderAlertView::OnDragCancelled(v);
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
