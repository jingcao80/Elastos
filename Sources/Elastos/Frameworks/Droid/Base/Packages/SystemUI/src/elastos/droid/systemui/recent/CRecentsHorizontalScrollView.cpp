#include "elastos/droid/systemui/recent/CRecentsHorizontalScrollView.h"
#include "elastos/droid/systemui/recent/RecentsHorizontalScrollView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;
using Elastos::Droid::SystemUI::Recent::RecentsHorizontalScrollView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

IVIEW_METHODS_IMPL(CRecentsHorizontalScrollView, RecentsHorizontalScrollView)
IVIEWGROUP_METHODS_IMPL(CRecentsHorizontalScrollView, RecentsHorizontalScrollView)
IVIEWPARENT_METHODS_IMPL(CRecentsHorizontalScrollView, RecentsHorizontalScrollView)
IVIEWMANAGER_METHODS_IMPL(CRecentsHorizontalScrollView, RecentsHorizontalScrollView)
IDRAWABLECALLBACK_METHODS_IMPL(CRecentsHorizontalScrollView, RecentsHorizontalScrollView)
IKEYEVENTCALLBACK_METHODS_IMPL(CRecentsHorizontalScrollView, RecentsHorizontalScrollView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CRecentsHorizontalScrollView, RecentsHorizontalScrollView)
IFRAMELAYOUT_METHODS_IMPL(CRecentsHorizontalScrollView, RecentsHorizontalScrollView)
IHORIZONTALSCROLLVIEW_METHODS_IMPL(CRecentsHorizontalScrollView, RecentsHorizontalScrollView)

ECode CRecentsHorizontalScrollView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return RecentsHorizontalScrollView::Init(ctx, attrs);
}

PInterface CRecentsHorizontalScrollView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CRecentsHorizontalScrollView::Probe(riid);
}

ECode CRecentsHorizontalScrollView::SetMinSwipeAlpha(
    /* [in] */ Float minAlpha)
{
    return RecentsHorizontalScrollView::SetMinSwipeAlpha(minAlpha);
}

ECode CRecentsHorizontalScrollView::FindViewForTask(
    /* [in] */ Int32 persistentTaskId,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return RecentsHorizontalScrollView::FindViewForTask(persistentTaskId, view);
}

ECode CRecentsHorizontalScrollView::CanChildBeDismissed(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RecentsHorizontalScrollView::CanChildBeDismissed(v);
    return NOERROR;
}

ECode CRecentsHorizontalScrollView::DismissChild(
    /* [in] */ IView* v)
{
    return RecentsHorizontalScrollView::DismissChild(v);
}

ECode CRecentsHorizontalScrollView::OnChildDismissed(
    /* [in] */ IView* v)
{
    return RecentsHorizontalScrollView::OnChildDismissed(v);
}

ECode CRecentsHorizontalScrollView::OnBeginDrag(
    /* [in] */ IView* v)
{
    return RecentsHorizontalScrollView::OnBeginDrag(v);
}

ECode CRecentsHorizontalScrollView::OnDragCancelled(
    /* [in] */ IView* v)
{
    return RecentsHorizontalScrollView::OnDragCancelled(v);
}

ECode CRecentsHorizontalScrollView::GetChildAtPosition(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return RecentsHorizontalScrollView::GetChildAtPosition(ev, view);
}

ECode CRecentsHorizontalScrollView::GetChildContentView(
    /* [in] */ IView* v,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return RecentsHorizontalScrollView::GetChildContentView(v, view);
}

ECode CRecentsHorizontalScrollView::SetAdapter(
    /* [in] */ ITaskDescriptionAdapter* adapter)
{
    return RecentsHorizontalScrollView::SetAdapter(adapter);
}

ECode CRecentsHorizontalScrollView::NumItemsInOneScreenful(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = RecentsHorizontalScrollView::NumItemsInOneScreenful();
    return NOERROR;
}

ECode CRecentsHorizontalScrollView::SetCallback(
    /* [in] */ IRecentsCallback* callback)
{
    return RecentsHorizontalScrollView::SetCallback(callback);
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
