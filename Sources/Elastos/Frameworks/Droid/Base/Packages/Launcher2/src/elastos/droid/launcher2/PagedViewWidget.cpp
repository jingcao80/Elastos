
#include "elastos/droid/launcher2/PagedViewWidget.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

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
    mShortPressTriggered = TRUE;
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
    /* [in] */ Int32* defStyle)
{
    LinearLayout::constructor(context, attrs, defStyle);

    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    r->GetString(Elastos::Droid::Launcher2::R::string::widget_dims_format, &mDimensionsFormatString);

    SetWillNotDraw(false);
    return SetClipToPadding(false);
}

ECode PagedViewWidget::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();

    AutoPtr<IView> view;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_preview, (IView**)&view);
    AutoPtr<IImageView> image = IImageView::Probe(view);

    Int32 left;
    image->GetPaddingLeft(&left);
    mOriginalImagePadding->SetLeft(left);

    Int32 top;
    image->GetPaddingTop(&top);
    mOriginalImagePadding->SetTop(top);

    Int32 right;
    image->GetPaddingRight(&right);
    mOriginalImagePadding->SetRight(right);

    Int32 bottom;
    image->GetPaddingBottom(&bottom);
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
            IDrawable::Probe(preview)->GetBitmap((IBitmap**)&bp);
            if (sRecyclePreviewsWhenDetachedFromWindow &&
                    mInfo != NULL && preview != NULL && map != NULL) {
                mWidgetPreviewLoader->RecycleBitmap(mInfo, map);
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
    String result;
    info->LoadLabel(packageManager, &result);
    name->SetText(result);

    AutoPtr<IView> view3;
    FindViewById(Elastos::Droid::Launcher2::R::id::widget_dims, (IView**)&view3);
    AutoPtr<ITextView> dims = ITextView::Probe(view3);
    if (dims != NULL) {
        Int32 x;
        LauncherModel::GetCellCountX(&x);
        Int32 hSpan = Math::Min((*cellSpan)[0], x);
        Int32 y;
        LauncherModel::GetCellCountY(&y);
        Int32 vSpan = Math::Min((*cellSpan)[1], y);
        dims->SetText(String.format(mDimensionsFormatString, hSpan, vSpan));
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
        dims->SetText(String.format(mDimensionsFormatString, 1, 1));
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
    i->GetWidth(&width);
    Int32 left;
    mOriginalImagePadding->GetLeft(&left);
    Int32 right;
    mOriginalImagePadding->GetRight(&right);
    (*maxSize)[0] = width - left - right;
    Int32 height;
    i->GetHeight(&height);
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
        image->SetImageDrawable(preview);
        if (mIsAppWidget) {
            // center horizontally
            AutoPtr<ArrayOf<Int32> > imageSize = GetPreviewSize();
            Int32 width;
            preview->GetIntrinsicWidth(&width);
            Int32 centerAmount = ((*imageSize)[0] - width) / 2;
            Int32 left;
            mOriginalImagePadding->GetLeft(&left);
            Int32 right;
            mOriginalImagePadding->GetRight(&right);
            Int32 top;
            mOriginalImagePadding->GetTop(&top);
            Int32 bottom;
            mOriginalImagePadding->GetBottom(&bottom);
            image->SetPadding(left + centerAmount,
                    top,
                    right,
                    bottom);
        }
        image->SetAlpha(1f);
        image->SetAllowRequestLayout(TRUE);
    }
    return NOERROR;
}

ECode PagedViewWidget::SetShortPressListener(
    /* [in] */ IShortPressListener* listener)
{
    mShortPressListener = listener;
    return NOERROR;
}

void PagedViewWidget::CheckForShortPress()
{
    if (sShortpressTarget != NULL) {
        return;
    }
    if (mPendingCheckForShortPress == NULL) {
        mPendingCheckForShortPress = new CheckForShortPress();
    }
    PostDelayed(mPendingCheckForShortPress, 120);
}

void PagedViewWidget::RemoveShortPressCallback()
{
    if (mPendingCheckForShortPress != NULL) {
        RemoveCallbacks(mPendingCheckForShortPress);
    }
}

void PagedViewWidget::CleanUpShortPress()
{
    RemoveShortPressCallback();
    if (mShortPressTriggered) {
        if (mShortPressListener != NULL) {
            mShortPressListener->CleanUpShortPress(PagedViewWidget.this);
        }
        mShortPressTriggered = FALSE;
    }
}

ECode PagedViewWidget::ResetShortPressTarget()
{
    sShortpressTarget = NULL;
}

ECode PagedViewWidget::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    LinearLayout::OnTouchEvent(event);

    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case MotionEvent::ACTION_UP:
            CleanUpShortPress();
            break;
        case MotionEvent::ACTION_DOWN:
            CheckForShortPress();
            break;
        case MotionEvent::ACTION_CANCEL:
            CleanUpShortPress();
            break;
        case MotionEvent::ACTION_MOVE:
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