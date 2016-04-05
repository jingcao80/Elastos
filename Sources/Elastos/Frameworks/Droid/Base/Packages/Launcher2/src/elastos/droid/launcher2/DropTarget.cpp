
#include "elastos/droid/launcher2/DropTarget.h"
#include "Elastos.Droid.Service.h"
#include "R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(DragObject, Object, IDropTargetDragObject);

DragObject::DragObject()
    : mX(-1)
    , mY(-1)
    , mXOffset(-1)
    , mYOffset(-1)
    , mDragComplete(FALSE)
    , mCancelled(FALSE)
    , mDeferDragViewCleanupPostAnimation(FALSE)
{
}

CAR_INTERFACE_IMPL(DragEnforcer, Object, IDragControllerDragListener);

DragEnforcer::DragEnforcer()
    : mDragParity(0)
{
}

ECode DragEnforcer::constructor(
    /* [in] */ IContext* context)
{
    assert(0 && "need class Launcher");
    // AutoPtr<ILauncher> launcher = ILauncher::Probe(context);
    // AutoPtr<IDragController> controller;
    // launcher->GetDragController((IDragController**)&controller);
    // return controller->AddDragListener(this);
    return NOERROR;
}

ECode DragEnforcer::OnDragEnter()
{
    mDragParity++;
    if (mDragParity != 1) {
        Slogger::E(IDropTarget::TAG, "onDragEnter: Drag contract violated: %d", mDragParity);
    }
    return NOERROR;
}

ECode DragEnforcer::OnDragExit()
{
    mDragParity--;
    if (mDragParity != 0) {
        Slogger::E(IDropTarget::TAG, "onDragExit: Drag contract violated: %d", mDragParity);
    }
    return NOERROR;
}

ECode DragEnforcer::OnDragStart(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info,
    /* [in] */ Int32 dragAction)
{
    if (mDragParity != 0) {
        Slogger::E(IDropTarget::TAG, "onDragEnter: Drag contract violated: %d", mDragParity);
    }
    return NOERROR;
}

ECode DragEnforcer::OnDragEnd()
{
    if (mDragParity != 0) {
        Slogger::E(IDropTarget::TAG, "onDragExit: Drag contract violated: %d", mDragParity);
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos