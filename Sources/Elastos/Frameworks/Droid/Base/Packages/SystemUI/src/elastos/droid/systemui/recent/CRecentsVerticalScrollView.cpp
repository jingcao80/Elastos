#include "elastos/droid/systemui/recent/CRecentsVerticalScrollView.h"
#include "elastos/droid/systemui/recent/RecentsVerticalScrollView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;
using Elastos::Droid::SystemUI::Recent::RecentsVerticalScrollView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

IVIEW_METHODS_IMPL(CRecentsVerticalScrollView, RecentsVerticalScrollView)
IVIEWGROUP_METHODS_IMPL(CRecentsVerticalScrollView, RecentsVerticalScrollView)
IVIEWPARENT_METHODS_IMPL(CRecentsVerticalScrollView, RecentsVerticalScrollView)
IVIEWMANAGER_METHODS_IMPL(CRecentsVerticalScrollView, RecentsVerticalScrollView)
IDRAWABLECALLBACK_METHODS_IMPL(CRecentsVerticalScrollView, RecentsVerticalScrollView)
IKEYEVENTCALLBACK_METHODS_IMPL(CRecentsVerticalScrollView, RecentsVerticalScrollView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CRecentsVerticalScrollView, RecentsVerticalScrollView)
IFRAMELAYOUT_METHODS_IMPL(CRecentsVerticalScrollView, RecentsVerticalScrollView)
ISCROLLVIEW_METHODS_IMPL(CRecentsVerticalScrollView, RecentsVerticalScrollView)

ECode CRecentsVerticalScrollView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return RecentsVerticalScrollView::Init(ctx, attrs);
}

PInterface CRecentsVerticalScrollView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CRecentsVerticalScrollView::Probe(riid);
}

ECode CRecentsVerticalScrollView::SetMinSwipeAlpha(
    /* [in] */ Float minAlpha)
{
    return RecentsVerticalScrollView::SetMinSwipeAlpha(minAlpha);
}

ECode CRecentsVerticalScrollView::FindViewForTask(
    /* [in] */ Int32 persistentTaskId,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return RecentsVerticalScrollView::FindViewForTask(persistentTaskId, view);
}

ECode CRecentsVerticalScrollView::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RecentsVerticalScrollView::CanChildBeDismissed(v);
    return NOERROR;
}

ECode CRecentsVerticalScrollView::DismissChild(
    /* [in] */ IView* v)
{
    return RecentsVerticalScrollView::DismissChild(v);
}

ECode CRecentsVerticalScrollView::OnChildDismissed(
    /* [in] */ IView* v)
{
    return RecentsVerticalScrollView::OnChildDismissed(v);
}

ECode CRecentsVerticalScrollView::OnBeginDrag(
    /* [in] */ IView* v)
{
    return RecentsVerticalScrollView::OnBeginDrag(v);
}

ECode CRecentsVerticalScrollView::OnDragCancelled(
    /* [in] */ IView* v)
{
    return RecentsVerticalScrollView::OnDragCancelled(v);
}

ECode CRecentsVerticalScrollView::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return RecentsVerticalScrollView::GetChildAtPosition(ev, view);
}

ECode CRecentsVerticalScrollView::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return RecentsVerticalScrollView::GetChildContentView(v, view);
}

ECode CRecentsVerticalScrollView::SetAdapter(
    /* [in] */ ITaskDescriptionAdapter* adapter)
{
    return RecentsVerticalScrollView::SetAdapter(adapter);
}

ECode CRecentsVerticalScrollView::NumItemsInOneScreenful(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = RecentsVerticalScrollView::NumItemsInOneScreenful();
    return NOERROR;
}

ECode CRecentsVerticalScrollView::SetCallback(
    /* [in] */ IRecentsCallback* callback)
{
    return RecentsVerticalScrollView::SetCallback(callback);
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
