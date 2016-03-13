
#include "elastos/droid/launcher2/ButtonDropTarget.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL_2(ButtonDropTarget, TextView, IDropTarget, IDragControllerDragListener);

ButtonDropTarget::ButtonDropTarget(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : mTransitionDuration(0)
    , mActive(FALSE)
    , mHoverColor(0)
    , mBottomDragPadding(0)
{
    ButtonDropTarget(context, attrs, 0);
}

ButtonDropTarget::ButtonDropTarget(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : mTransitionDuration(0)
    , mActive(FALSE)
    , mHoverColor(0)
    , mBottomDragPadding(0)
    , TextView(context, attrs, defStyle)
{
    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    r->GetInteger(Elastos::Droid::Launcher2::R::integer::
            config_dropTargetBgTransitionDuration, &mTransitionDuration);
    r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::
            drop_target_drag_padding, &mBottomDragPadding);
}

void ButtonDropTarget::SetLauncher(
    /* [in] */ ILauncher* launcher)
{
    mLauncher = launcher;
    return;
}

Boolean ButtonDropTarget::AcceptDrop(
    /* [in] */ DragObject* d)
{
    return FALSE;
}

void ButtonDropTarget::SetSearchDropTargetBar(
    /* [in] */ ISearchDropTargetBar* searchDropTargetBar)
{
    mSearchDropTargetBar = searchDropTargetBar;
    return;
}

AutoPtr<IDrawable> ButtonDropTarget::GetCurrentDrawable()
{
    Drawable[] drawables = getCompoundDrawablesRelative();
    for (int i = 0; i < drawables.length; ++i) {
        if (drawables[i] != null) {
            return drawables[i];
        }
    }
    return null;
}

void ButtonDropTarget::OnDrop(
    /* [in] */ DragObject* d)
{
    return;
}

void ButtonDropTarget::OnFlingToDelete(
    /* [in] */ DragObject* d,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPointF* vec)
{
    // Do nothing
    return;
}

void ButtonDropTarget::OnDragEnter(
    /* [in] */ DragObject* d)
{
    d->mDragView->SetColor(mHoverColor);
    return;
}

void ButtonDropTarget::OnDragOver(
    /* [in] */ DragObject* d)
{
    // Do nothing
    return;
}

void ButtonDropTarget::OnDragExit(
    /* [in] */ DragObject* d)
{
    d->mDragView->SetColor(0);
}

void ButtonDropTarget::OnDragStart(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info,
    /* [in] */ Int32 dragAction)
{
    // Do nothing
    return;
}

Boolean ButtonDropTarget::IsDropEnabled()
{
    return mActive;
}

void ButtonDropTarget::OnDragEnd()
{
    // Do nothing
    return;
}

void ButtonDropTarget::GetHitRect(
    /* [in] */ IRect* outRect)
{
    TextView::GetHitRect(outRect);
    outRect.bottom += mBottomDragPadding;

    Int32 bottom;
    outRect->GetBottom(&bottom);
    bottom += mBottomDragPadding;
    outRect->SetBottom(bottom);
    return;
}

Boolean ButtonDropTarget::IsRtl()
{
    return (GetLayoutDirection() == IView::LAYOUT_DIRECTION_RTL);
}

AutoPtr<IRect> ButtonDropTarget::GetIconRect(
    /* [in] */ Int32 viewWidth,
    /* [in] */ Int32 viewHeight,
    /* [in] */ Int32 drawableWidth,
    /* [in] */ Int32 drawableHeight)
{
    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);

    // Find the rect to animate to (the view is center aligned)
    AutoPtr<IRect> to;
    CRect::New((IRect**)&to);
    dragLayer->GetViewRectRelativeToSelf(this, to);

    const Int32 width = drawableWidth;
    const Int32 height = drawableHeight;

    Int32 left;
    Int32 right;

    if (IsRtl()) {
        Int32 tmp;
        to->GetRight(&tmp);
        right = tmp - GetPaddingRight();
        left = right - width;
    }
    else {
        Int32 tmp;
        to->GetLeft(&tmp);
        left = tmp + GetPaddingLeft();
        right = left + width;
    }

    Int32 tmp;
    to->GetTop(&tmp);
    const Int32 top = tmp + (GetMeasuredHeight() - height) / 2;
    const Int32 bottom = top +  height;

    to->Set(left, top, right, bottom);

    // Center the destination rect about the trash icon
    const Int32 xOffset = (Int32) -(viewWidth - width) / 2;
    const Int32 yOffset = (Int32) -(viewHeight - height) / 2;
    to->Offset(xOffset, yOffset);

    return to;
}

AutoPtr<DropTarget> ButtonDropTarget::GetDropTargetDelegate(
    /* [in] */ DragObject* d)
{
    return NULL;
}

void ButtonDropTarget::GetLocationInDragLayer(
    /* [in] */ ArrayOf<Int32>* loc)
{
    AutoPtr<IDragLayer> layer;
    mLauncher->GetDragLayer((IDragLayer**)&layer);
    layer->GetLocationInDragLayer(this, loc);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos