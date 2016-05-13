
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

CAR_INTERFACE_IMPL(SpringLoadedDragController, Object, IAlarmOnAlarmListener);

SpringLoadedDragController::SpringLoadedDragController(
    /* [in] */ ILauncher* launcher)
    : ENTER_SPRING_LOAD_HOVER_TIME(500)
    , ENTER_SPRING_LOAD_CANCEL_HOVER_TIME(950)
    , EXIT_SPRING_LOAD_HOVER_TIME(200)
    , mLauncher(launcher)
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