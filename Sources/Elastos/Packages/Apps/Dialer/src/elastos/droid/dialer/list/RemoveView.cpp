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

#include "elastos/droid/dialer/list/RemoveView.h"
#include "R.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CAR_INTERFACE_IMPL(RemoveView, FrameLayout, IRemoveView);

RemoveView::RemoveView()
    : mUnhighlightedColor(0)
    , mHighlightedColor(0)
{}

ECode RemoveView::constructor(
    /* [in] */ IContext* context)
{
    return FrameLayout::constructor(context);
}

ECode RemoveView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, -1);
}

ECode RemoveView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return FrameLayout::constructor(context, attrs, defStyle);
}

ECode RemoveView::OnFinishInflate()
{
    AutoPtr<IView> view;
    FindViewById(R::id::remove_view_text, (IView**)&view);
    mRemoveText = ITextView::Probe(view);
    view = NULL;
    FindViewById(R::id::remove_view_icon, (IView**)&view);
    mRemoveIcon = IImageView::Probe(view);
    AutoPtr<IResources> r;
    GetResources((IResources**)&r);
    r->GetColor(R::color::remove_text_color, &mUnhighlightedColor);
    r->GetColor(R::color::remove_highlighted_text_color, &mHighlightedColor);
    r->GetDrawable(R::drawable::ic_remove, (IDrawable**)&mRemoveDrawable);
    return NOERROR;
}

ECode RemoveView::SetDragDropController(
    /* [in] */ IDragDropController* controller)
{
    mDragDropController = controller;
    return NOERROR;
}

ECode RemoveView::OnDragEvent(
    /* [in] */ IDragEvent* event,
    /* [out] */ Boolean* result)
{
    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case IDragEvent::ACTION_DRAG_ENTERED:
            SetAppearanceHighlighted();
            break;
        case IDragEvent::ACTION_DRAG_EXITED:
            SetAppearanceNormal();
            break;
        case IDragEvent::ACTION_DRAG_LOCATION:
            if (mDragDropController != NULL) {
                Float x, y;
                event->GetX(&x);
                event->GetY(&y);
                mDragDropController->HandleDragHovered(this, (Int32)x, (Int32)y);
            }
            break;
        case IDragEvent::ACTION_DROP:
            if (mDragDropController != NULL) {
                Float x, y;
                event->GetX(&x);
                event->GetY(&y);
                mDragDropController->HandleDragFinished((Int32)x, (Int32)y, TRUE);
            }
            SetAppearanceNormal();
            break;
    }
    *result = TRUE;
    return NOERROR;
}

void RemoveView::SetAppearanceNormal()
{
    mRemoveText->SetTextColor(mUnhighlightedColor);
    mRemoveIcon->SetColorFilter(mUnhighlightedColor);
    Invalidate();
}

void RemoveView::SetAppearanceHighlighted()
{
    mRemoveText->SetTextColor(mHighlightedColor);
    mRemoveIcon->SetColorFilter(mHighlightedColor);
    Invalidate();
}

} // List
} // Dialer
} // Droid
} // Elastos
