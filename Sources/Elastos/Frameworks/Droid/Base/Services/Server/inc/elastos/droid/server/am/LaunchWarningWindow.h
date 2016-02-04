
#ifndef __ELASTOS_DROID_SERVER_AM_LAUNCHWARNINTWINDOW_H__
#define __ELASTOS_DROID_SERVER_AM_LAUNCHWARNINTWINDOW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/ActivityRecord.h"
#include "elastos/droid/app/Dialog.h"

using Elastos::Droid::App::Dialog;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class LaunchWarningWindow
    : public Dialog
{
public:

    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyLongPress(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnKeyMultiple(
        /* [in] */ Int32 keyCode,
        /* [in] */ Int32 count,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    LaunchWarningWindow(
        /* [in] */ IContext* context,
        /* [in] */ ActivityRecord* cur,
        /* [in] */ ActivityRecord* next);

    CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuInfo* menuInfo);

};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_LAUNCHWARNINTWINDOW_H__
