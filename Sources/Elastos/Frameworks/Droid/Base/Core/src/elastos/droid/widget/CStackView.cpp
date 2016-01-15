#include "elastos/droid/widget/CStackView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {

IVIEW_METHODS_IMPL(CStackView, StackView)
IVIEWGROUP_METHODS_IMPL(CStackView, StackView)
IVIEWPARENT_METHODS_IMPL(CStackView, StackView)
IVIEWMANAGER_METHODS_IMPL(CStackView, StackView)
IDRAWABLECALLBACK_METHODS_IMPL(CStackView, StackView)
IKEYEVENTCALLBACK_METHODS_IMPL(CStackView, StackView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CStackView, StackView)
IADAPTERVIEW_METHODS_IMPL(CStackView, StackView)

PInterface CStackView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CStackView::Probe(riid);
}

ECode CStackView::constructor(
    /* [in] */ IContext* context)
{
    return StackView::Init(context);
}

ECode CStackView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return StackView::Init(context, attrs);
}

ECode CStackView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return StackView::Init(context, attrs, defStyle);
}

ECode CStackView::SetDisplayedChild(
    /* [in] */ Int32 whichChild)
{
    return StackView::SetDisplayedChild(whichChild);
}

ECode CStackView::GetDisplayedChild(
    /* [out] */ Int32* child)
{
    VALIDATE_NOT_NULL(child);
    *child = StackView::GetDisplayedChild();
    return NOERROR;
}

ECode CStackView::ShowNext()
{
    return StackView::ShowNext();
}

ECode CStackView::ShowPrevious()
{
    return StackView::ShowPrevious();
}

ECode CStackView::GetCurrentView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IView> v = StackView::GetCurrentView();
    *view = v;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CStackView::GetInAnimation(
    /* [out] */ IObjectAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);
    AutoPtr<IObjectAnimator> oa = StackView::GetInAnimation();
    *animator = oa;
    REFCOUNT_ADD(*animator);
    return NOERROR;
}

ECode CStackView::SetInAnimation(
    /* [in] */ IObjectAnimator* inAnimation)
{
    return StackView::SetInAnimation(inAnimation);
}

ECode CStackView::GetOutAnimation(
    /* [out] */ IObjectAnimator** animator)
{
    VALIDATE_NOT_NULL(animator);
    AutoPtr<IObjectAnimator> oa = StackView::GetOutAnimation();
    *animator = oa;
    REFCOUNT_ADD(*animator);
    return NOERROR;
}

ECode CStackView::SetOutAnimation(
    /* [in] */ IObjectAnimator* outAnimation)
{
    return StackView::SetOutAnimation(outAnimation);
}

ECode CStackView::SetInAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceID)
{
     return StackView::SetInAnimation(context, resourceID);
}

ECode CStackView::SetOutAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceID)
{
    return StackView::SetOutAnimation(context, resourceID);
}

ECode CStackView::SetAnimateFirstView(
    /* [in] */ Boolean animate)
{
    return StackView::SetAnimateFirstView(animate);
}

ECode CStackView::SetRemoteViewsOnClickHandler(
    /* [in] */ IRemoteViewsOnClickHandler* handler)
{
    return StackView::SetRemoteViewsOnClickHandler(handler);
}

ECode CStackView::SetRemoteViewsAdapter(
    /* [in] */ IIntent* intent)
{
    return StackView::SetRemoteViewsAdapter(intent);
}

ECode CStackView::Advance()
{
    return StackView::Advance();
}

ECode CStackView::FyiWillBeAdvancedByHostKThx()
{
    return StackView::FyiWillBeAdvancedByHostKThx();
}

ECode CStackView::OnRemoteAdapterConnected(
        /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = StackView::OnRemoteAdapterConnected();
    return NOERROR;
}

ECode CStackView::OnRemoteAdapterDisconnected()
{
    return StackView::OnRemoteAdapterDisconnected();
}

ECode CStackView::DeferNotifyDataSetChanged()
{
    return StackView::DeferNotifyDataSetChanged();
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
