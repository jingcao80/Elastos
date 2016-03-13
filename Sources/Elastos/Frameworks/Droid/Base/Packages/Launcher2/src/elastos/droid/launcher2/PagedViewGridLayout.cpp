
#include "elastos/droid/launcher2/PagedViewGridLayout.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

PagedViewGridLayout::LayoutParams::LayoutParams(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
    : FrameLayout::LayoutParams(width, height);
{
}

const String PagedViewGridLayout::TAG("PagedViewGridLayout");

CAR_INTERFACE_IMPL(PagedViewGridLayout, GridLayout, IPage);

PagedViewGridLayout::PagedViewGridLayout(
    /* [in] */ IContext* context,
    /* [in] */ Int32 cellCountX,
    /* [in] */ Int32 cellCountY)
    : GridLayout(context, NULL, 0);
    , mCellCountX(cellCountX)
    , mCellCountY(cellCountY)
{
}

Int32 PagedViewGridLayout::GetCellCountX()
{
    return mCellCountX;
}

Int32 PagedViewGridLayout::GetCellCountY()
{
    return mCellCountY;
}

ECode PagedViewGridLayout::ResetChildrenOnKeyListeners()
{
    Int32 childCount;
    GetChildCount(&childCount);
    for (Int32 j = 0; j < childCount; ++j) {
        AutoPtr<IView> view;
        GetChildAt(j, (IView**)&view);
        view->SetOnKeyListener(NULL);
    }
    return NOERROR;
}

ECode PagedViewGridLayout::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // PagedView currently has issues with different-sized pages since it calculates the
    // offset of each page to scroll to before it updates the actual size of each page
    // (which can change depending on the content if the contents aren't a fixed size).
    // We work around this by having a minimum size on each widget page).
    Int32 width;
    GetSuggestedMinimumWidth(&width);
    Int32 size;
    MeasureSpec::GetSize(widthMeasureSpec, &size);
    Int32 widthSpecSize = Math::Min(width, size);
    Int32 widthSpecMode = MeasureSpec::EXACTLY;
    return GridLayout::OnMeasure(MeasureSpec::MakeMeasureSpec(widthSpecSize, widthSpecMode),
            heightMeasureSpec);
}

ECode PagedViewGridLayout::OnDetachedFromWindow()
{
    GridLayout::OnDetachedFromWindow();
    mOnLayoutListener = NULL;
    return NOERROR;
}

ECode PagedViewGridLayout::SetOnLayoutListener(
    /* [in] */ IRunnable* r)
{
    mOnLayoutListener = r;
    return NOERROR;
}

ECode PagedViewGridLayout::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    GridLayout::OnLayout(changed, left, top, right, bottom);
    if (mOnLayoutListener != NULL) {
        mOnLayoutListener->Run();
    }
    return NOERROR;
}

ECode PagedViewGridLayout::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    Boolean _result;
    GridLayout::OnTouchEvent(event, &_result);
    Int32 count;
    GetPageChildCount(&count);
    if (count > 0) {
        // We only intercept the touch if we are tapping in empty space after the final row
        AutoPtr<IView> child;
        GetChildOnPageAt(count - 1, (IView**)&child);
        Int32 bottom;
        child->GetBottom(&bottom);
        Int32 y;
        event->GetY(&y);
        _result = _result || (y < bottom);
    }
    *result = _result;
    return NOERROR;
}

ECode PagedViewGridLayout::RemoveAllViewsOnPage()
{
    RemoveAllViews();
    mOnLayoutListener = NULL;
    return SetLayerType(LAYER_TYPE_NONE, NULL);
}

ECode PagedViewGridLayout::RemoveViewOnPageAt(
    /* [in] */ Int32 index)
{
    return RemoveViewAt(index);
}

ECode PagedViewGridLayout::GetPageChildCount(
    /* [out] */ Int32* count)
{
    return GetChildCount(count);
}

ECode PagedViewGridLayout::GetChildOnPageAt(
    /* [in] */ Int32 i,
    /* [out] */ IView** view)
{
    return GetChildAt(i, view);
}

ECode PagedViewGridLayout::IndexOfChildOnPage(
    /* [in] */ IView* v,
    /* [out] */ Int32* index)
{
    return IndexOfChild(v, index);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos