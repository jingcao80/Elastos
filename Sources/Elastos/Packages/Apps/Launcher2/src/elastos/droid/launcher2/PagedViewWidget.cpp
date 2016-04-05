
#include "elastos/droid/launcher2/PagedViewWidget.h"
#include "Elastos.Droid.Service.h"
#include "R.h"
#include <elastos/core/Math.h>
 #include <elastos/core/CoreUtils.h>

using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

PagedViewWidget::CheckForShortPress::CheckForShortPress(
    /* [in] */ PagedViewWidget* host)
    : mHost(host)
{
}

ECode PagedViewWidget::CheckForShortPress::Run()
{
    if (mHost->sShortpressTarget != NULL) {
        return NOERROR;
    }
    if (mHost->mShortPressListener != NULL) {
        mHost->mShortPressListener->OnShortPress(mHost);
        mHost->sShortpressTarget = mHost;
    }
    mHost->mShortPressTriggered = TRUE;
    return NOERROR;
}

const String PagedViewWidget::TAG("PagedViewWidgetLayout");
AutoPtr<IPagedViewWidget> PagedViewWidget::sShortpressTarget;
Boolean PagedViewWidget::sDeletePreviewsWhenDetachedFromWindow = TRUE;
Boolean PagedViewWidget::sRecyclePreviewsWhenDetachedFromWindow = TRUE;

CAR_INTERFACE_IMPL(PagedViewWidget, LinearLayout, IPagedViewWidget);

PagedViewWidget::PagedViewWidget()
    : mShortPressTriggered(FALSE)
    , mIsAppWidget(FALSE)
    , mDimensionsFormatString(NULL)
{
    CRect::New((IRect**)&mOriginalImagePadding);
}

ECode PagedViewWidget::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PagedViewWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode PagedViewWidget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::constructor(context, attrs, defStyle);

    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    r->GetString(Elastos::Droid::Launcher2::R::string::widget_dims_format, &mDimensionsFormatString);

    SetWillNotDraw(FALSE);
    return SetClipToPadding(FALSE);
}

ECode PagedViewWidget::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_preview, (IView**)&view);
    AutoPtr<IImageView> image = IImageView::Probe(view);

    Int32 left;
    IView::Probe(image)->GetPaddingLeft(&left);
    mOriginalImagePadding->SetLeft(left);

    Int32 top;
    IView::Probe(image)->GetPaddingTop(&top);
    mOriginalImagePadding->SetTop(top);

    Int32 right;
    IView::Probe(image)->GetPaddingRight(&right);
    mOriginalImagePadding->SetRight(right);

    Int32 bottom;
    IView::Probe(image)->GetPaddingBottom(&bottom);
    return mOriginalImagePadding->SetBottom(bottom);
}

ECode PagedViewWidget::SetDeletePreviewsWhenDetachedFromWindow(
    /* [in] */ Boolean value)
{
    sDeletePreviewsWhenDetachedFromWindow = value;
    return NOERROR;
}

ECode PagedViewWidget::SetRecyclePreviewsWhenDetachedFromWindow(
    /* [in] */ Boolean value)
{
    sRecyclePreviewsWhenDetachedFromWindow = value;
    return NOERROR;
}

ECode PagedViewWidget::OnDetachedFromWindow()
{
    LinearLayout::OnDetachedFromWindow();

    if (sDeletePreviewsWhenDetachedFromWindow) {
        AutoPtr<IView> view;
        FindViewById(Elastos::Droid::Launcher2::R::id::widget_preview, (IView**)&view);
        AutoPtr<IImageView> image = IImageView::Probe(view);
        if (image != NULL) {
            AutoPtr<IDrawable> drawable;
            image->GetDrawable((IDrawable**)&drawable);
            AutoPtr<IFastBitmapDrawable> preview = IFastBitmapDrawable::Probe(drawable);

            AutoPtr<IBitmap> map;
            preview->GetBitmap((IBitmap**)&map);
            if (sRecyclePreviewsWhenDetachedFromWindow &&
                    mInfo != NULL && preview != NULL && map != NULL) {
                assert(0 && "need class IWidgetPreviewLoader");
                //mWidgetPreviewLoader->RecycleBitmap(mInfo, map);
            }
            image->SetImageDrawable(NULL);
        }
    }
    return NOERROR;
}

ECode PagedViewWidget::ApplyFromAppWidgetProviderInfo(
    /* [in] */ IAppWidgetProviderInfo* info,
    /* [in] */ Int32 maxWidth,
    /* [in] */ ArrayOf<Int32>* cellSpan,
    /* [in] */ IWidgetPreviewLoader* loader)
{
    mIsAppWidget = TRUE;
    mInfo = info;
    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_preview, (IView**)&view);
    AutoPtr<IImageView> image = IImageView::Probe(view);

    if (maxWidth > -1) {
        image->SetMaxWidth(maxWidth);
    }

    AutoPtr<IView> view2;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_name, (IView**)&view2);
    AutoPtr<ITextView> name = ITextView::Probe(view2);
    AutoPtr<IContext> res;
    GetContext((IContext**)&res);
    AutoPtr<IPackageManager> packageManager;
    res->GetPackageManager((IPackageManager**)&packageManager);
    String label;
    info->LoadLabel(packageManager, &label);
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(label);
    name->SetText(cchar);

    AutoPtr<IView> view3;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_dims, (IView**)&view3);
    AutoPtr<ITextView> dims = ITextView::Probe(view3);
    if (dims != NULL) {
        Int32 x;
        assert(0 && "need class LauncherModel");
        //LauncherModel::GetCellCountX(&x);
        Int32 hSpan = Elastos::Core::Math::Min((*cellSpan)[0], x);
        Int32 y;
        assert(0 && "need class LauncherModel");
        //LauncherModel::GetCellCountY(&y);
        Int32 vSpan = Elastos::Core::Math::Min((*cellSpan)[1], y);
        String tmpStr;
        tmpStr.AppendFormat(mDimensionsFormatString, hSpan, vSpan);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(tmpStr);
        dims->SetText(cchar);
    }
    mWidgetPreviewLoader = loader;
    return NOERROR;
}

ECode PagedViewWidget::ApplyFromResolveInfo(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IResolveInfo* info,
    /* [in] */ IWidgetPreviewLoader* loader)
{
    mIsAppWidget = FALSE;
    mInfo = info;
    AutoPtr<ICharSequence> label;
    info->LoadLabel(pm, (ICharSequence**)&label);

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_name, (IView**)&view);
    AutoPtr<ITextView> name = ITextView::Probe(view);
    name->SetText(label);

    AutoPtr<IView> view2;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_dims, (IView**)&view2);
    AutoPtr<ITextView> dims = ITextView::Probe(view2);
    if (dims != NULL) {
        String tmpStr;
        tmpStr.AppendFormat(mDimensionsFormatString, 1, 1);
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(tmpStr);
        dims->SetText(cchar);
    }
    mWidgetPreviewLoader = loader;
    return NOERROR;
}

ECode PagedViewWidget::GetPreviewSize(
    /* [out, callee] */ ArrayOf<Int32>** size)
{
    VALIDATE_NOT_NULL(size);

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_preview, (IView**)&view);
    AutoPtr<IImageView> i = IImageView::Probe(view);
    AutoPtr<ArrayOf<Int32> > maxSize = ArrayOf<Int32>::Alloc(2);
    Int32 width;
    IView::Probe(i)->GetWidth(&width);
    Int32 left;
    mOriginalImagePadding->GetLeft(&left);
    Int32 right;
    mOriginalImagePadding->GetRight(&right);
    (*maxSize)[0] = width - left - right;
    Int32 height;
    IView::Probe(i)->GetHeight(&height);
    Int32 top;
    mOriginalImagePadding->GetTop(&top);
    (*maxSize)[1] = height - top;
    *size = maxSize;
    REFCOUNT_ADD(*size);
    return NOERROR;
}

ECode PagedViewWidget::ApplyPreview(
    /* [in] */ IFastBitmapDrawable* preview,
    /* [in] */ Int32 index)
{
    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_preview, (IView**)&view);
    AutoPtr<IPagedViewWidgetImageView> image = IPagedViewWidgetImageView::Probe(view);
    if (preview != NULL) {
        image->SetAllowRequestLayout(FALSE);
        IImageView::Probe(image)->SetImageDrawable(IDrawable::Probe(preview));
        if (mIsAppWidget) {
            // center horizontally
            AutoPtr<ArrayOf<Int32> > imageSize;
            GetPreviewSize((ArrayOf<Int32>**)&imageSize);
            Int32 width;
            IDrawable::Probe(preview)->GetIntrinsicWidth(&width);
            Int32 centerAmount = ((*imageSize)[0] - width) / 2;
            Int32 left;
            mOriginalImagePadding->GetLeft(&left);
            Int32 right;
            mOriginalImagePadding->GetRight(&right);
            Int32 top;
            mOriginalImagePadding->GetTop(&top);
            Int32 bottom;
            mOriginalImagePadding->GetBottom(&bottom);
            IView::Probe(image)->SetPadding(left + centerAmount,
                    top,
                    right,
                    bottom);
        }
        IImageView::Probe(image)->SetAlpha(1.0f);
        image->SetAllowRequestLayout(TRUE);
    }
    return NOERROR;
}

ECode PagedViewWidget::SetShortPressListener(
    /* [in] */ IPagedViewWidgetShortPressListener* listener)
{
    mShortPressListener = listener;
    return NOERROR;
}

void PagedViewWidget::MyCheckForShortPress()
{
    if (sShortpressTarget != NULL) {
        return;
    }
    if (mPendingCheckForShortPress == NULL) {
        mPendingCheckForShortPress = new CheckForShortPress(this);
    }
    Boolean tmp;
    PostDelayed(mPendingCheckForShortPress, 120, &tmp);
}

void PagedViewWidget::RemoveShortPressCallback()
{
    if (mPendingCheckForShortPress != NULL) {
        Boolean res;
        RemoveCallbacks(mPendingCheckForShortPress, &res);
    }
}

void PagedViewWidget::CleanUpShortPress()
{
    RemoveShortPressCallback();
    if (mShortPressTriggered) {
        if (mShortPressListener != NULL) {
            mShortPressListener->CleanUpShortPress(this);
        }
        mShortPressTriggered = FALSE;
    }
}

ECode PagedViewWidget::ResetShortPressTarget()
{
    sShortpressTarget = NULL;
    return NOERROR;
}

ECode PagedViewWidget::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean tmp;
    LinearLayout::OnTouchEvent(event, &tmp);

    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_UP:
            CleanUpShortPress();
            break;
        case IMotionEvent::ACTION_DOWN:
            MyCheckForShortPress();
            break;
        case IMotionEvent::ACTION_CANCEL:
            CleanUpShortPress();
            break;
        case IMotionEvent::ACTION_MOVE:
            break;
    }

    // We eat up the touch events here, since the PagedView (which uses the same swiping
    // touch code as Workspace previously) uses onInterceptTouchEvent() to determine when
    // the user is scrolling between pages.  This means that if the pages themselves don't
    // handle touch events, it gets forwarded up to PagedView itself, and it's own
    // onTouchEvent() handling will prevent further intercept touch events from being called
    // (it's the same view in that case).  This is not ideal, but to prevent more changes,
    // we just always mark the touch event as handled.
    *res = TRUE;
    return NOERROR;
}



} // namespace Launcher2
} // namespace Droid
} // namespace Elastos