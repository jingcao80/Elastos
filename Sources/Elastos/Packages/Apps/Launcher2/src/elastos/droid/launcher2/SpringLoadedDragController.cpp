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

#include "elastos/droid/launcher2/SpringLoadedDragController.h"
#include "elastos/droid/launcher2/Alarm.h"
#include "elastos/droid/launcher2/Workspace.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(SpringLoadedDragController::InnerListener, Object, IAlarmOnAlarmListener);


SpringLoadedDragController::InnerListener::InnerListener(
    /* [in] */ SpringLoadedDragController* host)
    : mHost(host)
{}

ECode SpringLoadedDragController::InnerListener::OnAlarm(
    /* [in] */ IAlarm* alarm)
{
    return mHost->OnAlarm(alarm);
}

SpringLoadedDragController::SpringLoadedDragController(
    /* [in] */ ILauncher* launcher)
    : ENTER_SPRING_LOAD_HOVER_TIME(500)
    , ENTER_SPRING_LOAD_CANCEL_HOVER_TIME(950)
    , EXIT_SPRING_LOAD_HOVER_TIME(200)
    , mLauncher(launcher)
{
    AutoPtr<InnerListener> listener = new InnerListener(this);
    mAlarm = new Alarm();
    mAlarm->SetOnAlarmListener(listener);
}

ECode SpringLoadedDragController::Cancel()
{
    return mAlarm->CancelAlarm();
}

// Set a new alarm to expire for the screen that we are hovering over now
ECode SpringLoadedDragController::SetAlarm(
    /* [in] */ ICellLayout* cl)
{
    mAlarm->CancelAlarm();
    mAlarm->SetAlarm((cl == NULL) ? ENTER_SPRING_LOAD_CANCEL_HOVER_TIME :
        ENTER_SPRING_LOAD_HOVER_TIME);
    mScreen = cl;
    return NOERROR;
}

// this is called when our timer runs out
ECode SpringLoadedDragController::OnAlarm(
    /* [in] */ IAlarm* alarm)
{
    if (mScreen != NULL) {
        // Snap to the screen that we are hovering over now
        AutoPtr<IWorkspace> w;
        mLauncher->GetWorkspace((IWorkspace**)&w);
        Int32 page;
        IViewGroup::Probe(w)->IndexOfChild(IView::Probe(mScreen), &page);
        Int32 tmp;
        IPagedView::Probe(w)->GetCurrentPage(&tmp);
        if (page != tmp) {
            return ((Workspace*)w.Get())->SnapToPage(page);
        }
    }
    else {
        AutoPtr<IDragController> controller;
        mLauncher->GetDragController((IDragController**)&controller);
        return controller->CancelDrag();
    }
    return NOERROR;
}


} // namespace Launcher2
} // namespace Droid
} // namespace Elastos