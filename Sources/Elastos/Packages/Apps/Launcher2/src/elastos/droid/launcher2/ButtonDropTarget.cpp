//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/launcher2/ButtonDropTarget.h"
#include "elastos/droid/launcher2/DropTarget.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

using Elastos::Droid::Launcher2::DragObject;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CRect;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL_3(ButtonDropTarget, TextView, IButtonDropTarget,
        IDropTarget, IDragControllerDragListener);

ButtonDropTarget::ButtonDropTarget()
    : mTransitionDuration(0)
    , mSearchDropTargetBar(NULL)
    , mActive(FALSE)
    , mHoverColor(0)
    , mBottomDragPadding(0)
{
}

ECode ButtonDropTarget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode ButtonDropTarget::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    TextView::constructor(context, attrs, defStyle);
    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    r->GetInteger(Elastos::Droid::Launcher2::R::integer::
            config_dropTargetBgTransitionDuration, &mTransitionDuration);
    return r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::
            drop_target_drag_padding, &mBottomDragPadding);
}

ECode ButtonDropTarget::SetLauncher(
    /* [in] */ ILauncher* launcher)
{
    mLauncher = launcher;
    return NOERROR;
}

ECode ButtonDropTarget::AcceptDrop(
    /* [in] */ IDropTargetDragObject* d,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    return  NOERROR;
}

ECode ButtonDropTarget::SetSearchDropTargetBar(
    /* [in] */ ISearchDropTargetBar* searchDropTargetBar)
{
    mSearchDropTargetBar = searchDropTargetBar;
    return NOERROR;
}

ECode ButtonDropTarget::GetCurrentDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);

    AutoPtr<ArrayOf<IDrawable*> > drawables;
    GetCompoundDrawablesRelative((ArrayOf<IDrawable*>**)&drawables);
    for (Int32 i = 0; i < drawables->GetLength(); ++i) {
        if ((*drawables)[i] != NULL) {
            *drawable = (*drawables)[i];
            REFCOUNT_ADD(*drawable);
            return NOERROR;
        }
    }
    *drawable = NULL;
    return NOERROR;
}

ECode ButtonDropTarget::OnDrop(
    /* [in] */ IDropTargetDragObject* d)
{
    return NOERROR;
}

ECode ButtonDropTarget::OnFlingToDelete(
    /* [in] */ IDropTargetDragObject* d,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPointF* vec)
{
    // Do nothing
    return NOERROR;
}

ECode ButtonDropTarget::OnDragEnter(
    /* [in] */ IDropTargetDragObject* d)
{
    DragObject* _d = (DragObject*)d;
    return _d->mDragView->SetColor(mHoverColor);
}

ECode ButtonDropTarget::OnDragOver(
    /* [in] */ IDropTargetDragObject* d)
{
    // Do nothing
    return NOERROR;
}

ECode ButtonDropTarget::OnDragExit(
    /* [in] */ IDropTargetDragObject* d)
{
    DragObject* _d = (DragObject*)d;
    return _d->mDragView->SetColor(0);
}

ECode ButtonDropTarget::OnDragStart(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info,
    /* [in] */ Int32 dragAction)
{
    // Do nothing
    return NOERROR;
}

ECode ButtonDropTarget::IsDropEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mActive;
    return NOERROR;
}

ECode ButtonDropTarget::OnDragEnd()
{
    // Do nothing
    return NOERROR;
}

ECode ButtonDropTarget::GetHitRect(
    /* [in] */ IRect* outRect)
{
    TextView::GetHitRect(outRect);

    Int32 bottom;
    outRect->GetBottom(&bottom);
    bottom += mBottomDragPadding;
    outRect->SetBottom(bottom);
    return NOERROR;
}

Boolean ButtonDropTarget::IsRtl()
{
    Int32 tmp;
    GetLayoutDirection(&tmp);
    return tmp == IView::LAYOUT_DIRECTION_RTL;
}

ECode ButtonDropTarget::GetIconRect(
    /* [in] */ Int32 viewWidth,
    /* [in] */ Int32 viewHeight,
    /* [in] */ Int32 drawableWidth,
    /* [in] */ Int32 drawableHeight,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

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
        Int32 res;
        GetPaddingRight(&res);
        right = tmp - res;
        left = right - width;
    }
    else {
        Int32 tmp;
        to->GetLeft(&tmp);
        Int32 res;
        GetPaddingLeft(&res);
        left = tmp + res;
        right = left + width;
    }

    Int32 tmp;
    to->GetTop(&tmp);
    Int32 res;
    GetMeasuredHeight(&res);
    const Int32 top = tmp + (res - height) / 2;
    const Int32 bottom = top +  height;

    to->Set(left, top, right, bottom);

    // Center the destination rect about the trash icon
    const Int32 xOffset = (Int32) -(viewWidth - width) / 2;
    const Int32 yOffset = (Int32) -(viewHeight - height) / 2;
    to->Offset(xOffset, yOffset);

    *rect = to;
    REFCOUNT_ADD(*rect);
    return NOERROR;
}

ECode ButtonDropTarget::GetDropTargetDelegate(
    /* [in] */ IDropTargetDragObject* d,
    /* [out] */ IDropTarget** target)
{
    VALIDATE_NOT_NULL(target);;

    *target = NULL;
    return NOERROR;
}

ECode ButtonDropTarget::GetLocationInDragLayer(
    /* [in] */ ArrayOf<Int32>* loc)
{
    AutoPtr<IDragLayer> layer;
    mLauncher->GetDragLayer((IDragLayer**)&layer);
    Float tmp;
    return layer->GetLocationInDragLayer(IView::Probe(this), loc, &tmp);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos