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

#include "elastos/droid/launcher2/DropTarget.h"
#include "Elastos.Droid.Service.h"
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
    , mDeferDragViewCleanupPostAnimation(TRUE)
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
    AutoPtr<ILauncher> launcher = ILauncher::Probe(context);
    AutoPtr<IDragController> controller;
    launcher->GetDragController((IDragController**)&controller);
    return controller->AddDragListener(this);
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