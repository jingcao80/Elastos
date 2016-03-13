
#include "elastos/droid/launcher2/SpringLoadedDragController.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const Int64 SpringLoadedDragController::ENTER_SPRING_LOAD_HOVER_TIME = 500;
const Int64 SpringLoadedDragController::ENTER_SPRING_LOAD_CANCEL_HOVER_TIME = 950;
const Int64 SpringLoadedDragController::EXIT_SPRING_LOAD_HOVER_TIME = 200;

SpringLoadedDragController::SpringLoadedDragController(
    /* [in] */ ILauncher* launcher)
    : mLauncher(launcher)
{
    mAlarm = new Alarm();
    mAlarm->SetOnAlarmListener(this);
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
        mLauncher->GetWorkspace(()IWorkspace**&w);
        Int32 page;
        w->IndexOfChild(mScreen, &page);
        Int32 tmp;
        w->GetCurrentPage(&tmp);
        if (page != tmp) {
            return w->SnapToPage(page);
        }
    }
    else {
        AutoPtr<IDragController> controller;
        mLauncher->GetDragController((IDragController**)&controller);
        return CancelDrag();
    }
    return NOERROR;
}


} // namespace Launcher2
} // namespace Droid
} // namespace Elastos