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

#include "elastos/droid/dialer/list/DragDropController.h"

using Elastos::Droid::Dialer::List::EIID_IDragDropController;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

CAR_INTERFACE_IMPL(DragDropController, Object, IDragDropController)

DragDropController::DragDropController(
    /* [in] */ IDragItemContainer* dragItemContainer)
    : mDragItemContainer(dragItemContainer)
{
    CArrayList::New((IList**)&mOnDragDropListeners);
    mLocationOnScreen = ArrayOf<Int32>::Alloc(2);
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
