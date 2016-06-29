
#include "elastos/apps/dialer/list/RemoveView.h"
#include "R.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

CAR_INTERFACE_IMPL(RemoveView, FrameLayout, IRemoveView);

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
    r->GetDrawable(R::drawable::ic_remove, &mRemoveDrawable);

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
            if (mDragDropController != null) {
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
} // Apps
} // Elastos
