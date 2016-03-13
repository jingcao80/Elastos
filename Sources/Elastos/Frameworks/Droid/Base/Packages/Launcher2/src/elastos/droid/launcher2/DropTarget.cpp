
#include "elastos/droid/launcher2/DropTarget.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(DragObject, Object, IDragObject);

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

ECode DragEnforcer::DragEnforcer(
    /* [in] */ IContext* context)
    : mDragParity(0)
{
    AutoPtr<ILauncher> launcher = ILauncher::Probe(context);
    AutoPtr<IDragController> controller;
    launcher->GetDragController((IDragController**)&controller);
    controller->AddDragListener(this);
}

ECode DragEnforcer::OnDragEnter()
{
    mDragParity++;
    if (mDragParity != 1) {
        Slogger::E(TAG, "onDragEnter: Drag contract violated: %d", mDragParity);
    }
}

ECode DragEnforcer::OnDragExit()
{
    mDragParity--;
    if (mDragParity != 0) {
        Slogger::E(TAG, "onDragExit: Drag contract violated: %d", mDragParity);
    }
}

ECode DragEnforcer::OnDragStart(
    /* [in] */ IDragSource* source,
    /* [in] */ IInterface* info,
    /* [in] */ Int32 dragAction)
{
    if (mDragParity != 0) {
        Slogger::E(TAG, "onDragEnter: Drag contract violated: %d", mDragParity);
    }
    return NOERROR;
}

ECode DragEnforcer::OnDragEnd()
{
    if (mDragParity != 0) {
        Slogger::E(TAG, "onDragExit: Drag contract violated: %d", mDragParity);
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos