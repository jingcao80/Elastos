
#include "elastos/droid/systemui/statusbar/CNotificationContentView.h"
#include "R.h"
#include <elastos/droid/R.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Graphics::CColorFilter;
using Elastos::Droid::Graphics::CColorMatrix;
using Elastos::Droid::Graphics::CColorMatrixColorFilter;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::IColorMatrixColorFilter;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::PorterDuffMode_ADD;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::View::Animation::CLinearInterpolator;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::Widget::IImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CNotificationContentView::ViewRunnable::ViewRunnable(
    /* [in] */ CNotificationContentView* host,
    /* [in] */ Boolean showContractedChild)
    : mHost(host)
    , mShowContractedChild(showContractedChild)
{}

ECode CNotificationContentView::ViewRunnable::Run()
{
    mHost->mContractedChild->SetLayerType(LAYER_TYPE_NONE, NULL);
    mHost->mExpandedChild->SetLayerType(LAYER_TYPE_NONE, NULL);
    mHost->SetLayerType(LAYER_TYPE_NONE, NULL);
    mHost->mContractedChild->SetVisibility(mShowContractedChild
            ? IView::VISIBLE
            : IView::INVISIBLE);
    mHost->mExpandedChild->SetVisibility(mShowContractedChild
            ? IView::INVISIBLE
            : IView::VISIBLE);
    return NOERROR;
}

AutoPtr<IColorFilter> InitNO_COLOR_FILTER()
{
    AutoPtr<IColorFilter> filter;
    CColorFilter::New((IColorFilter**)&filter);
    return filter;
}

const Int64 CNotificationContentView::ANIMATION_DURATION_LENGTH = 170;
AutoPtr<IPaint> CNotificationContentView::INVERT_PAINT = CreateInvertPaint();
AutoPtr<IColorFilter> CNotificationContentView::NO_COLOR_FILTER = InitNO_COLOR_FILTER();

CAR_OBJECT_IMPL(CNotificationContentView)

CAR_INTERFACE_IMPL(CNotificationContentView, FrameLayout, INotificationContentView)

CNotificationContentView::CNotificationContentView()
    : mSmallHeight(0)
    , mClipTopAmount(0)
    , mActualHeight(0)
    , mContractedVisible(TRUE)
    , mDark(FALSE)
{
    CRect::New((IRect**)&mClipBounds);
    CLinearInterpolator::New((IInterpolator**)&mLinearInterpolator);
    CPaint::New((IPaint**)&mFadePaint);
}

ECode CNotificationContentView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);
    AutoPtr<IXfermode> mode;
    CPorterDuffXfermode::New(PorterDuffMode_ADD, (IXfermode**)&mode);
    mFadePaint->SetXfermode(mode);
    Reset();
    return NOERROR;
}

ECode CNotificationContentView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    FrameLayout::OnLayout(changed, left, top, right, bottom);
    UpdateClipping();
    return NOERROR;
}

ECode CNotificationContentView::Reset()
{
    if (mContractedChild != NULL) {
        AutoPtr<IViewPropertyAnimator> anim;
        mContractedChild->Animate((IViewPropertyAnimator**)&anim);
        anim->Cancel();
    }
    if (mExpandedChild != NULL) {
        AutoPtr<IViewPropertyAnimator> anim;
        mExpandedChild->Animate((IViewPropertyAnimator**)&anim);
        anim->Cancel();
    }
    RemoveAllViews();
    mContractedChild = NULL;
    mExpandedChild = NULL;
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::notification_min_height, &mSmallHeight);
    mActualHeight = mSmallHeight;
    mContractedVisible = TRUE;
    return NOERROR;
}

ECode CNotificationContentView::GetContractedChild(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mContractedChild;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotificationContentView::GetExpandedChild(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mExpandedChild;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotificationContentView::SetContractedChild(
    /* [in] */ IView* child)
{
    if (mContractedChild != NULL) {
        AutoPtr<IViewPropertyAnimator> anim;
        mContractedChild->Animate((IViewPropertyAnimator**)&anim);
        anim->Cancel();
        RemoveView(mContractedChild);
    }
    SanitizeContractedLayoutParams(child);
    AddView(child);
    mContractedChild = child;
    SelectLayout(FALSE /* animate */, TRUE /* force */);
    return NOERROR;
}

ECode CNotificationContentView::SetExpandedChild(
    /* [in] */ IView* child)
{
    if (mExpandedChild != NULL) {
        AutoPtr<IViewPropertyAnimator> anim;
        mExpandedChild->Animate((IViewPropertyAnimator**)&anim);
        anim->Cancel();
        RemoveView(mExpandedChild);
    }
    AddView(child);
    mExpandedChild = child;
    SelectLayout(FALSE /* animate */, TRUE /* force */);
    return NOERROR;
}

ECode CNotificationContentView::SetActualHeight(
    /* [in] */ Int32 actualHeight)
{
    mActualHeight = actualHeight;
    SelectLayout(TRUE /* animate */, FALSE /* force */);
    UpdateClipping();
    return NOERROR;
}

ECode CNotificationContentView::GetMaxHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    // The maximum height is just the laid out height.
    return GetHeight(height);
}

ECode CNotificationContentView::GetMinHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mSmallHeight;
    return NOERROR;
}

ECode CNotificationContentView::SetClipTopAmount(
    /* [in] */ Int32 clipTopAmount)
{
    mClipTopAmount = clipTopAmount;
    UpdateClipping();
    return NOERROR;
}

void CNotificationContentView::UpdateClipping()
{
    Int32 width = 0;
    GetWidth(&width);
    mClipBounds->Set(0, mClipTopAmount, width, mActualHeight);
    SetClipBounds(mClipBounds);
}

void CNotificationContentView::SanitizeContractedLayoutParams(
    /* [in] */ IView* contractedChild)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    contractedChild->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    lp->SetHeight(mSmallHeight);
    contractedChild->SetLayoutParams(lp);
}

void CNotificationContentView::SelectLayout(
    /* [in] */ Boolean animate,
    /* [in] */ Boolean force)
{
    if (mContractedChild == NULL) {
        return;
    }
    Boolean showContractedChild = ShowContractedChild();
    if (showContractedChild != mContractedVisible || force) {
        if (animate && mExpandedChild != NULL) {
            RunSwitchAnimation(showContractedChild);
        }
        else if (mExpandedChild != NULL) {
            mContractedChild->SetVisibility(showContractedChild ? IView::VISIBLE : IView::INVISIBLE);
            mContractedChild->SetAlpha(showContractedChild ? 1.f : 0.f);
            mExpandedChild->SetVisibility(showContractedChild ? IView::INVISIBLE : IView::VISIBLE);
            mExpandedChild->SetAlpha(showContractedChild ? 0.f : 1.f);
        }
    }
    mContractedVisible = showContractedChild;
}

void CNotificationContentView::RunSwitchAnimation(
    /* [in] */ Boolean showContractedChild)
{
    mContractedChild->SetVisibility(IView::VISIBLE);
    mExpandedChild->SetVisibility(IView::VISIBLE);
    mContractedChild->SetLayerType(LAYER_TYPE_HARDWARE, mFadePaint);
    mExpandedChild->SetLayerType(LAYER_TYPE_HARDWARE, mFadePaint);
    SetLayerType(LAYER_TYPE_HARDWARE, NULL);
    AutoPtr<IViewPropertyAnimator> anim;
    mContractedChild->Animate((IViewPropertyAnimator**)&anim);
    anim->Alpha(showContractedChild ? 1.f : 0.f);
    anim->SetDuration(ANIMATION_DURATION_LENGTH);
    anim->SetInterpolator(ITimeInterpolator::Probe(mLinearInterpolator));
    anim = NULL;
    mExpandedChild->Animate((IViewPropertyAnimator**)&anim);
    anim->Alpha(showContractedChild ? 0.f : 1.f);
    anim->SetDuration(ANIMATION_DURATION_LENGTH);
    anim->SetInterpolator(ITimeInterpolator::Probe(mLinearInterpolator));
    AutoPtr<ViewRunnable> runnable = new ViewRunnable(this, showContractedChild);
    anim->WithEndAction(runnable);
}

Boolean CNotificationContentView::ShowContractedChild()
{
    return mActualHeight <= mSmallHeight || mExpandedChild == NULL;
}

ECode CNotificationContentView::NotifyContentUpdated()
{
    SelectLayout(FALSE /* animate */, TRUE /* force */);
    return NOERROR;
}

ECode CNotificationContentView::IsContentExpandable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mExpandedChild != NULL;
    return NOERROR;
}

ECode CNotificationContentView::SetDark(
    /* [in] */ Boolean dark,
    /* [in] */ Boolean fade)
{
    if (mDark == dark || mContractedChild == NULL) return NOERROR;
    mDark = dark;
    SetImageViewDark(dark, fade, Elastos::Droid::R::id::right_icon);
    SetImageViewDark(dark, fade, Elastos::Droid::R::id::icon);
    return NOERROR;
}

void CNotificationContentView::SetImageViewDark(
    /* [in] */ Boolean dark,
    /* [in] */ Boolean fade,
    /* [in] */ Int32 imageViewId)
{
    // TODO: implement fade
    AutoPtr<IView> v;
    mContractedChild->FindViewById(imageViewId, (IView**)&v);
    if (v == NULL) return;
    AutoPtr<IDrawable> d;
    v->GetBackground((IDrawable**)&d);
    if (dark) {
        v->SetLayerType(LAYER_TYPE_HARDWARE, INVERT_PAINT);
        if (d != NULL) {
            AutoPtr<IColorFilter> filter;
            d->GetColorFilter((IColorFilter**)&filter);
            v->SetTag(R::id::doze_saved_filter_tag, filter != NULL ? filter
                    : NO_COLOR_FILTER);

            AutoPtr<IResources> res;
            GetResources((IResources**)&res);
            Int32 value = 0;
            res->GetColor(R::color::doze_small_icon_background_color, &value);
            d->SetColorFilter(value, PorterDuffMode_SRC_ATOP);
            res->GetInteger(R::integer::doze_small_icon_alpha, &value);
            IImageView::Probe(v)->SetImageAlpha(value);
        }
    }
    else {
        v->SetLayerType(LAYER_TYPE_NONE, NULL);
        if (d != NULL)  {
            AutoPtr<IInterface> obj;
            v->GetTag(R::id::doze_saved_filter_tag, (IInterface**)&obj);
            AutoPtr<IColorFilter> filter = IColorFilter::Probe(obj);
            if (filter != NULL) {
                d->SetColorFilter(filter == NO_COLOR_FILTER ? NULL : filter);
                v->SetTag(R::id::doze_saved_filter_tag, NULL);
            }
            IImageView::Probe(v)->SetImageAlpha(0xff);
        }
    }
}

ECode CNotificationContentView::HasOverlappingRendering(
    /* [out] */ Boolean* has)
{
    VALIDATE_NOT_NULL(has);
    // This is not really TRUE, but good enough when fading from the contracted to the expanded
    // layout, and saves us some layers.
    *has = FALSE;
    return NOERROR;
}

AutoPtr<IPaint> CNotificationContentView::CreateInvertPaint()
{
    AutoPtr<IPaint> p;
    CPaint::New((IPaint**)&p);
    Float invert[] = {
        -1.f,  0.f,  0.f, 1.f, 1.f,
         0.f, -1.f,  0.f, 1.f, 1.f,
         0.f,  0.f, -1.f, 1.f, 1.f,
         0.f,  0.f,  0.f, 1.f, 0.f
    };

    Int32 N = sizeof(invert) / sizeof(invert[0]);
    AutoPtr<ArrayOf<Float> > floats = ArrayOf<Float>::Alloc(N);
    floats->Copy(invert, N);
    AutoPtr<IColorMatrix> cm;
    CColorMatrix::New(*floats, (IColorMatrix**)&cm);
    AutoPtr<IColorFilter> ccf;
    CColorMatrixColorFilter::New(cm, (IColorFilter**)&ccf);
    p->SetColorFilter(ccf);
    return p;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
