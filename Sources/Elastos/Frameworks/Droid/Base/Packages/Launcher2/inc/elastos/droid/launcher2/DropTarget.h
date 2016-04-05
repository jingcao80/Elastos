#ifndef  __ELASTOS_DROID_LAUNCHER2_DROPTARGET_H__
#define  __ELASTOS_DROID_LAUNCHER2_DROPTARGET_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class DragObject
    : public Object
    , public IDropTargetDragObject
{
public:
    CAR_INTERFACE_DECL();

    DragObject();

public:
    Int32 mX;
    Int32 mY;

    /** X offset from the upper-left corner of the cell to where we touched.  */
    Int32 mXOffset;

    /** Y offset from the upper-left corner of the cell to where we touched.  */
    Int32 mYOffset;

    /** This indicates whether a drag is in final stages, either drop or cancel. It
     * differentiates onDragExit, since this is called when the drag is ending, above
     * the current drag target, or when the drag moves off the current drag object.
     */
    Boolean mDragComplete;

    /** The view that moves around while you drag.  */
    AutoPtr<IDragView> mDragView;

    /** The data associated with the object being dragged */
    AutoPtr<IInterface> mDragInfo;

    /** Where the drag originated */
    AutoPtr<IDragSource> mDragSource;

    /** Post drag animation runnable */
    AutoPtr<IRunnable> mPostAnimationRunnable;

    /** Indicates that the drag operation was cancelled */
    Boolean mCancelled;

    /** Defers removing the DragView from the DragLayer until after the drop animation. */
    Boolean mDeferDragViewCleanupPostAnimation;
};

class DragEnforcer
    : public Object
    , public IDragControllerDragListener
{
public:
    CAR_INTERFACE_DECL();

    DragEnforcer();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnDragEnter();

    CARAPI OnDragExit();

    //@Override
    CARAPI OnDragStart(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info,
        /* [in] */ Int32 dragAction);

    //@Override
    CARAPI OnDragEnd();

public:
    Int32 mDragParity;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_DROPTARGET_H__