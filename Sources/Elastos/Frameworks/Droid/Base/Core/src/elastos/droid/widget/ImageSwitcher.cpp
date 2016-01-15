
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/ImageSwitcher.h"

using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

const String ImageSwitcher::IMAGESWITCHER_NAME("ImageSwitcher");
CAR_INTERFACE_IMPL(ImageSwitcher, ViewSwitcher, IImageSwitcher);
ECode ImageSwitcher::constructor(
        /* [in] */ IContext* context)
{
    return ViewSwitcher::constructor(context);
}

ECode ImageSwitcher::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs)
{
    return ViewSwitcher::constructor(context, attrs);
}

ECode ImageSwitcher::SetImageResource(
    /* [in] */ Int32 resid)
{
    AutoPtr<IView> view;
    GetNextView((IView**)&view);
    AutoPtr<IImageView> image = IImageView::Probe(view);
    image->SetImageResource(resid);
    ShowNext();
    return NOERROR;
}

ECode ImageSwitcher::SetImageURI(
    /* [in] */ IUri* uri)
{
    AutoPtr<IView> view;
    GetNextView((IView**)&view);
    AutoPtr<IImageView> image = IImageView::Probe(view);
    image->SetImageURI(uri);
    ShowNext();
    return NOERROR;
}

ECode ImageSwitcher::SetImageDrawable(
    /* [in] */ IDrawable* drawable)
{
    AutoPtr<IView> view;
    GetNextView((IView**)&view);
    AutoPtr<IImageView> image = IImageView::Probe(view);
    image->SetImageDrawable(drawable);
    ShowNext();
    return NOERROR;
}

ECode ImageSwitcher::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    ViewSwitcher::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(IMAGESWITCHER_NAME, (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode ImageSwitcher::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    ViewSwitcher::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(IMAGESWITCHER_NAME, (ICharSequence**)&seq));
    info->SetClassName(seq);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos