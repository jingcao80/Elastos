
#include "elastos/droid/dialer/list/DragDropController.h"

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CAR_INTERFACE_IMPL(DragDropController, Object, IDragDropController);

DragDropController::DragDropController()
{
    CArrayList::New((IList**)&mOnDragDropListeners);
    mLocationOnScreen = ArrayOf<Int32>::Alloc(2);
}

ECode DragDropController::constructor(
    /* [in] */ IDragItemContainer* dragItemContainer)
{
    mDragItemContainer = dragItemContainer;
    return NOERROR;
}

Boolean DragDropController::HandleDragStarted(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoPtr<IPhoneFavoriteSquareTileView> tileView;
    mDragItemContainer->GetViewForLocation(x, y, (IPhoneFavoriteSquareTileView**)&tileView);
    if (tileView == NULL) {
        return FALSE;
    }

    Int32 size;
    mOnDragDropListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        mOnDragDropListeners->Get(i, (IInterface**)&item);
        IOnDragDropListener::Probe(item)->OnDragStarted(x, y, tileView);
    }

    return TRUE;
}

ECode DragDropController::HandleDragHovered(
    /* [in] */ IView* v,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    v->GetLocationOnScreen(mLocationOnScreen);
    Int32 screenX = x + (*mLocationOnScreen)[0];
    Int32 screenY = y + (*mLocationOnScreen)[1];
    AutoPtr<IPhoneFavoriteSquareTileView> view;
    mDragItemContainer->GetViewForLocation(
            screenX, screenY, (IPhoneFavoriteSquareTileView**)&view);

    Int32 size;
    mOnDragDropListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        mOnDragDropListeners->Get(i, (IInterface**)&item);
        IOnDragDropListener::Probe(item)->OnDragHovered(screenX, screenY, view);
    }

    return NOERROR;
}

ECode DragDropController::HandleDragFinished(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Boolean isRemoveView)
{
    Int32 size;
    mOnDragDropListeners->GetSize(&size);
    if (isRemoveView) {
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> item;
            mOnDragDropListeners->Get(i, (IInterface**)&item);
            IOnDragDropListener::Probe(item)->OnDroppedOnRemove();
        }
    }

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        mOnDragDropListeners->Get(i, (IInterface**)&item);
        IOnDragDropListener::Probe(item)->OnDragFinished(x, y);
    }

    return NOERROR;
}

ECode DragDropController::AddOnDragDropListener(
    /* [in] */ IOnDragDropListener* listener)
{
    Boolean contains;
    if (mOnDragDropListeners->Contains(listener, &contains), !contains) {
        mOnDragDropListeners->Add(listener);
    }

    return NOERROR;
}

ECode DragDropController::RemoveOnDragDropListener(
    /* [in] */ IOnDragDropListener* listener)
{
    Boolean contains;
    if (mOnDragDropListeners->Contains(listener, &contains), contains) {
        mOnDragDropListeners->Remove(listener);
    }

    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
