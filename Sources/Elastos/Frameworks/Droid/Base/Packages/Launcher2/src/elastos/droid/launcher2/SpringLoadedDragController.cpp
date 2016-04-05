
#include "elastos/droid/launcher2/SpringLoadedDragController.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/launcher2/Alarm.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

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
        assert(0 && "need class mLauncher");
        //mLauncher->GetWorkspace((IWorkspace**)&w);
        Int32 page;
        assert(0 && "need class IWorkspace");
        //w->IndexOfChild(mScreen, &page);
        Int32 tmp;
        assert(0 && "need class IWorkspace");
        //w->GetCurrentPage(&tmp);
        if (page != tmp) {
            assert(0 && "need class IWorkspace");
            //return w->SnapToPage(page);
            return NOERROR;
        }
    }
    else {
        AutoPtr<IDragController> controller;
        assert(0 && "need class mLauncher");
        //mLauncher->GetDragController((IDragController**)&controller);
        return controller->CancelDrag();
    }
    return NOERROR;
}


} // namespace Launcher2
} // namespace Droid
} // namespace Elastos