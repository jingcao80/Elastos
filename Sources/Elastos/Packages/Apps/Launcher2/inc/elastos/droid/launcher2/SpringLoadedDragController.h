#ifndef  __ELASTOS_DROID_LAUNCHER2_SPRINGLOADEDDRAGCONTROLLER_H__
#define  __ELASTOS_DROID_LAUNCHER2_SPRINGLOADEDDRAGCONTROLLER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class SpringLoadedDragController
    : public Object
{
private:
    class InnerListener
        : public Object
        , public IAlarmOnAlarmListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ SpringLoadedDragController* host);

        CARAPI OnAlarm(
            /* [in] */ IAlarm* alarm);

    private:
        SpringLoadedDragController* mHost;
    };

public:

    SpringLoadedDragController(
        /* [in] */ ILauncher* launcher);

    CARAPI Cancel();

    // Set a new alarm to expire for the screen that we are hovering over now
    CARAPI SetAlarm(
        /* [in] */ ICellLayout* cl);

    // this is called when our timer runs out
    virtual CARAPI OnAlarm(
        /* [in] */ IAlarm* alarm);

public:
    // how long the user must hover over a mini-screen before it unshrinks
    const Int64 ENTER_SPRING_LOAD_HOVER_TIME;
    const Int64 ENTER_SPRING_LOAD_CANCEL_HOVER_TIME;
    const Int64 EXIT_SPRING_LOAD_HOVER_TIME;

    AutoPtr<IAlarm> mAlarm;

private:
    // the screen the user is currently hovering over, if any
    AutoPtr<ICellLayout> mScreen;
    ILauncher* mLauncher;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_SPRINGLOADEDDRAGCONTROLLER_H__