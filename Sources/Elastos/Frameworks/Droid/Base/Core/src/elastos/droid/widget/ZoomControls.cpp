
#include "elastos/droid/widget/ZoomControls.h"
#include "elastos/droid/view/animation/CAlphaAnimation.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Animation::CAlphaAnimation;
using Elastos::Droid::View::Animation::IAlphaAnimation;
using Elastos::Droid::View::Animation::IAnimation;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(ZoomControls, LinearLayout, IZoomControls);
ZoomControls::ZoomControls()
{}

ECode ZoomControls::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode ZoomControls::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);
    SetFocusable(FALSE);

    AutoPtr<ILayoutInflater> inflater;
    AutoPtr<IInterface> svTemp;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&svTemp);
    inflater = ILayoutInflater::Probe(svTemp);
    AutoPtr<IView> v;
    inflater->Inflate(R::layout::zoom_controls, this, // we are the parent
            TRUE, (IView**)&v);

    AutoPtr<IView> view;
    FindViewById(R::id::zoomIn, (IView**)&view);
    mZoomIn = IZoomButton::Probe(view);

    view = NULL;
    FindViewById(R::id::zoomOut, (IView**)&view);
    mZoomOut = IZoomButton::Probe(view);
    return NOERROR;
}

ECode ZoomControls::SetOnZoomInClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    return IView::Probe(mZoomIn)->SetOnClickListener(listener);
}

ECode ZoomControls::SetOnZoomOutClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    return IView::Probe(mZoomOut)->SetOnClickListener(listener);
}

ECode ZoomControls::SetZoomSpeed(
    /* [in] */ Int64 speed)
{
    mZoomIn->SetZoomSpeed(speed);
    mZoomOut->SetZoomSpeed(speed);
    return NOERROR;
}

ECode ZoomControls::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode ZoomControls::Show()
{
    return Fade(IView::VISIBLE, 0.0f, 1.0f);
}

ECode ZoomControls::Hide()
{
    return Fade(IView::GONE, 1.0f, 0.0f);
}

ECode ZoomControls::SetIsZoomInEnabled(
    /* [in] */ Boolean isEnabled)
{
    return IView::Probe(mZoomIn)->SetEnabled(isEnabled);
}

ECode ZoomControls::SetIsZoomOutEnabled(
    /* [in] */ Boolean isEnabled)
{
    return IView::Probe(mZoomOut)->SetEnabled(isEnabled);
}

ECode ZoomControls::HasFocus(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    Boolean in = FALSE, out = FALSE;
    IView::Probe(mZoomIn)->HasFocus(&in);
    IView::Probe(mZoomOut)->HasFocus(&out);
    *has = in || out;
    return NOERROR;
}

ECode ZoomControls::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    LinearLayout::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> cs;
    CString::New(String("CZoomControls"), (ICharSequence**)&cs);
    IAccessibilityRecord::Probe(event)->SetClassName(cs);
    return NOERROR;
}

ECode ZoomControls::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    LinearLayout::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> cs;
    CString::New(String("CZoomControls"), (ICharSequence**)&cs);
    info->SetClassName(cs);
    return NOERROR;
}

ECode ZoomControls::Fade(
    /* [in] */ Int32 visibility,
    /* [in] */ Float startAlpha,
    /* [in] */ Float endAlpha)
{
    AutoPtr<IAlphaAnimation> anim;
    CAlphaAnimation::New(startAlpha, endAlpha, (IAlphaAnimation**)&anim);
    IAnimation::Probe(anim)->SetDuration(500);
    StartAnimation(IAnimation::Probe(anim));
    SetVisibility(visibility);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
