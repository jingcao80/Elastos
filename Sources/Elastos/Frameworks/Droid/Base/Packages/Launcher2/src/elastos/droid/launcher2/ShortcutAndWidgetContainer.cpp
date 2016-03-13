
#include "elastos/droid/launcher2/ShortcutAndWidgetContainer.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String ShortcutAndWidgetContainer::TAG("CellLayoutChildren");

CAR_INTERFACE_IMPL(ShortcutAndWidgetContainer, ViewGroup,
        IShortcutAndWidgetContainer);

ShortcutAndWidgetContainer::ShortcutAndWidgetContainer(
        /* [in] */ IContext* context)
    : mCellWidth(0)
    , mCellHeight(0)
    , mWidthGap(0)
    , mHeightGap(0)
    , mCountX(0)
    , mInvertIfRtl(FALSE)
{
    mTmpCellXY = ArrayOf<Int32>::Alloc(2);
}

ECode ShortcutAndWidgetContainer::constructor(
        /* [in] */ IContext* context)
{
    ViewGroup::constructor(context);
    return WallpaperManager::GetInstance(context, (IWallpaperManager**)&mWallpaperManager);
}

ECode ShortcutAndWidgetContainer::SetCellDimensions(
    /* [in] */ Int32 cellWidth,
    /* [in] */ Int32 cellHeight,
    /* [in] */ Int32 widthGap,
    /* [in] */ Int32 heightGap,
    /* [in] */ Int32 countX)
{
    mCellWidth = cellWidth;
    mCellHeight = cellHeight;
    mWidthGap = widthGap;
    mHeightGap = heightGap;
    mCountX = countX;
    return NOERROR;
}

AutoPtr<IView> ShortcutAndWidgetContainer::GetChildAt(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        AutoPtr<IViewGroupLayoutParams> params;
        child->GetLayoutParams((IViewGroupLayoutParams**)&params);
        AutoPtr<CellLayout::LayoutParams> lp = (CellLayout::LayoutParams*)IObject::Prober(params);

        if ((lp->mCellX <= x) && (x < lp->mCellX + lp->mCellHSpan) &&
                (lp->mCellY <= y) && (y < lp->mCellY + lp->mCellVSpan)) {
            return child;
        }
    }
    return NULL;
}

ECode ShortcutAndWidgetContainer::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    //@SuppressWarnings("all") // suppress dead code warning
    const Boolean debug = FALSE;
    if (debug) {
        // Debug drawing for hit space
        AutoPtr<IPaint> p;
        CPaint::New((IPaint**)&p);
        p->SetColor(0x6600FF00);
        Int32 count;
        GetChildCount(&count);
        for (Int32 i = count - 1; i >= 0; i--) {
            AutoPtr<IView> child;
            GetChildAt(i, (IView**)&child);
            AutoPtr<IViewGroupLayoutParams> params;
            child->GetLayoutParams((IViewGroupLayoutParams**)&params);
            AutoPtr<CellLayout::LayoutParams> lp = (CellLayout::LayoutParams*)IObject::Prober(params);

            canvas->DrawRect(lp->mX, lp->mY, lp->mX + lp->mWidth, lp->mY + lp->mHeight, p);
        }
    }
    return ViewGroup::DispatchDraw(canvas);
}

ECode ShortcutAndWidgetContainer::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        MeasureChild(child);
    }
    Int32 widthSpecSize;
    MeasureSpec::GetSize(widthMeasureSpec, &widthSpecSize);
    Int32 heightSpecSize;
    MeasureSpec::getSize(heightMeasureSpec, &heightSpecSize);
    return SetMeasuredDimension(widthSpecSize, heightSpecSize);
}

ECode ShortcutAndWidgetContainer::SetupLp(
    /* [in] */ CellLayout::LayoutParams* lp)
{
    return lp->Setup(mCellWidth, mCellHeight, mWidthGap, mHeightGap, InvertLayoutHorizontally(),
            mCountX);
}

ECode ShortcutAndWidgetContainer::SetInvertIfRtl(
    /* [in] */ Boolean invert)
{
    mInvertIfRtl = invert;
    return NOERROR;
}

ECode ShortcutAndWidgetContainer::MeasureChild(
    /* [in] */ IView* child)
{
    const Int32 cellWidth = mCellWidth;
    const Int32 cellHeight = mCellHeight;
    AutoPtr<IViewGroupLayoutParams> params;
    child->GetLayoutParams((IViewGroupLayoutParams**)&params);
    AutoPtr<CellLayout::LayoutParams> lp = (CellLayout::LayoutParams*)IObject::Prober(params);

    lp->Setup(cellWidth, cellHeight, mWidthGap, mHeightGap, InvertLayoutHorizontally(), mCountX);
    Int32 childWidthMeasureSpec;
    MeasureSpec::MakeMeasureSpec(lp->mWidth, MeasureSpec::EXACTLY, &childWidthMeasureSpec);
    Int32 childheightMeasureSpec;
    MeasureSpec->MakeMeasureSpec(lp->mHeight, MeasureSpec::EXACTLY, &childheightMeasureSpec);
    return child->Measure(childWidthMeasureSpec, childheightMeasureSpec);
}

Boolean ShortcutAndWidgetContainer::InvertLayoutHorizontally()
{
    return mInvertIfRtl && IsLayoutRtl();
}

Boolean ShortcutAndWidgetContainer::IsLayoutRtl()
{
    Int32 direction;
    GetLayoutDirection(&direction);
    return direction == LAYOUT_DIRECTION_RTL;
}

ECode ShortcutAndWidgetContainer::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        Int32 visibility;
        child->GetVisibility(&visibility);
        if (visibility != GONE) {
            AutoPtr<IViewGroupLayoutParams> params;
            child->GetLayoutParams((IViewGroupLayoutParams**)&params);
            AutoPtr<CellLayout::LayoutParams> lp = (CellLayout::LayoutParams*)IObject::Prober(params);

            Int32 childLeft = lp->mX;
            Int32 childTop = lp->mY;
            child->Layout(childLeft, childTop, childLeft + lp->mWidth, childTop + lp->mHeight);

            if (lp->mDropped) {
                lp->mDropped = FALSE;

                AutoPtr<ArrayOf<Int32> > cellXY = mTmpCellXY;
                GetLocationOnScreen(cellXY);
                AutoPTR<IBinder> token;
                getWindowToken((IBinder**)&token);
                mWallpaperManager->SendWallpaperCommand(token,
                        WallpaperManager::COMMAND_DROP,
                        (*cellXY)[0] + childLeft + lp->mWidth / 2,
                        (*cellXY)[1] + childTop + lp->mHeight / 2, 0, NULL);
            }
        }
    }
    return NOERROR;
}

Boolean ShortcutAndWidgetContainer::ShouldDelayChildPressedState()
{
    return FALSE;
}

ECode ShortcutAndWidgetContainer::RequestChildFocus(
    /* [in] */ IView* child,
    /* [in] */ IView* focused)
{
    ViewGroup::RequestChildFocus(child, focused);
    if (child != NULL) {
        AutoPtr<IRect> r;
        CRect::New((IRect**)&r);
        child->GetDrawingRect(r);
        return RequestRectangleOnScreen(r);
    }
    return NOERROR;
}

ECode ShortcutAndWidgetContainer::CancelLongPress()
{
    ViewGroup::CancelLongPress();

    // Cancel long press for all children
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        return child->CancelLongPress();
    }
    return NOERROR;
}

ECode ShortcutAndWidgetContainer::SetChildrenDrawingCacheEnabled(
    /* [in] */ Boolean enabled)
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> view;
        getChildAt(i, (IView**)&view);
        view->SetDrawingCacheEnabled(enabled);
        // Update the drawing caches
        Boolean res;
        view->IsHardwareAccelerated(&res);
        if (!res && enabled) {
            return view->BuildDrawingCache(TRUE);
        }
    }
    return NOERROR;
}

ECode ShortcutAndWidgetContainer::SetChildrenDrawnWithCacheEnabled(
    /* [in] */ Boolean enabled)
{
    return ViewGroup::SetChildrenDrawnWithCacheEnabled(enabled);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos