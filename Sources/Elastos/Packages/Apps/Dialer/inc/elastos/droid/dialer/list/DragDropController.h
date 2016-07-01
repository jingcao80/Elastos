#ifndef __ELASTOS_DROID_DIALER_LIST_DRAGDROPCONTROLLER_H__
#define __ELASTOS_DROID_DIALER_LIST_DRAGDROPCONTROLLER_H__

#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

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
    CAR_INTERFACE_DECL();

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
