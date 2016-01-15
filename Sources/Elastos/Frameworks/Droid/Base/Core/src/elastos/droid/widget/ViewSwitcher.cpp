
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/ViewSwitcher.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"

using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(ViewSwitcher, ViewAnimator, IViewSwitcher);
ECode ViewSwitcher::constructor(
    /* [in] */ IContext* context)
{
    return ViewAnimator::constructor(context);
}

ECode ViewSwitcher::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ViewAnimator::constructor(context, attrs);
}

ECode ViewSwitcher::AddView(
    /* [in] */ IView* child,
    /* [in] */ Int32 index,
    /* [in] */ IViewGroupLayoutParams* params)
{
    Int32 count = 0;
    if ((GetChildCount(&count), count) >= 2) {
//        throw new IllegalStateException("Can't add more than 2 views to a ViewSwitcher");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return ViewAnimator::AddView(child, index, params);
}

ECode ViewSwitcher::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewAnimator::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> cs;
    CString::New(String("CViewSwitcher"), (ICharSequence**)&cs);
    IAccessibilityRecord::Probe(event)->SetClassName(cs);
    return NOERROR;
}

ECode ViewSwitcher::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewAnimator::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> cs;
    CString::New(String("CViewSwitcher"), (ICharSequence**)&cs);
    info->SetClassName(cs);
    return NOERROR;
}

ECode ViewSwitcher::GetNextView(
    /* [out] */ IView** v)
{
    VALIDATE_NOT_NULL(v);
    Int32 which = mWhichChild == 0 ? 1 : 0;
    return GetChildAt(which, v);
}

ECode ViewSwitcher::SetFactory(
    /* [in] */ IViewFactory* factory)
{
    mFactory = factory;
    AutoPtr<IView> temp;
    ObtainView((IView**)&temp);
    temp = NULL;
    ObtainView((IView**)&temp);
    return NOERROR;
}

ECode ViewSwitcher::Reset()
{
    mFirstTime = TRUE;
    AutoPtr<IView> v;
    GetChildAt(0, (IView**)&v);
    if (v != NULL) {
        v->SetVisibility(IView::GONE);
    }
    v = NULL;
    GetChildAt(1, ((IView**)&v));
    if (v != NULL) {
        v->SetVisibility(IView::GONE);
    }
    return NOERROR;
}

ECode ViewSwitcher::ObtainView(
    /* [out] */ IView** v)
{
    AutoPtr<IView> child;
    mFactory->MakeView((IView**)&child);
    AutoPtr<IFrameLayoutLayoutParams> lp;
    AutoPtr<IViewGroupLayoutParams> temp;
    child->GetLayoutParams((IViewGroupLayoutParams**)&temp);
    if (temp) {
        lp = (IFrameLayoutLayoutParams*)temp->Probe(EIID_IFrameLayoutLayoutParams);
    }
    if (lp == NULL) {
        CFrameLayoutLayoutParams::New(IViewGroupLayoutParams::MATCH_PARENT,
                IViewGroupLayoutParams::WRAP_CONTENT, (IFrameLayoutLayoutParams**)&lp);
    }
    AddView(child, IViewGroupLayoutParams::Probe(lp));
    *v = child;
    REFCOUNT_ADD(*v);
    return NOERROR;
}

}// namespace Elastos
}// namespace Droid
}// namespace Widget
