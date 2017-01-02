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

#ifndef __ELASTOS_DROID_DIALER_LIST_DRAGDROPCONTROLLER_H__
#define __ELASTOS_DROID_DIALER_LIST_DRAGDROPCONTROLLER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::View::IView;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * Class that handles and combines drag events generated from multiple views, and then fires
 * off events to any OnDragDropListeners that have registered for callbacks.
 */
class DragDropController
    : public Object
    , public IDragDropController
{
public:
    CAR_INTERFACE_DECL()

    DragDropController(
        /* [in] */ IDragItemContainer* dragItemContainer);

    /**
     * @return True if the drag is started, false if the drag is cancelled for some reason.
     */
    CARAPI_(Boolean) HandleDragStarted(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI HandleDragHovered(
        /* [in] */ IView* v,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI HandleDragFinished(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Boolean isRemoveView);

    CARAPI AddOnDragDropListener(
        /* [in] */ IOnDragDropListener* listener);

    CARAPI RemoveOnDragDropListener(
        /* [in] */ IOnDragDropListener* listener);

private:
    AutoPtr<IList> mOnDragDropListeners;
    AutoPtr<IDragItemContainer> mDragItemContainer;
    AutoPtr<ArrayOf<Int32> > mLocationOnScreen;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_DRAGDROPCONTROLLER_H__
