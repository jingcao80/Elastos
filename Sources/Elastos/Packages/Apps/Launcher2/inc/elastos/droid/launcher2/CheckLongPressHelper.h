#ifndef  __ELASTOS_DROID_LAUNCHER2_CHECKLONGPRESSHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CHECKLONGPRESSHELPER_H__

#include "_Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class CheckLongPressHelper
    : public Object
{
public:
    class CheckForLongPress
        : public Runnable
    {
    public:
        CheckForLongPress(
            /* [in] */ CheckLongPressHelper* host);

        CARAPI Run();

    private:
        AutoPtr<CheckLongPressHelper> mHost;
    };

public:
    CheckLongPressHelper(
        /* [in] */ IView* v);

    CARAPI PostCheckForLongPress();

    CARAPI_(void) CancelLongPress();

    CARAPI_(Boolean) HasPerformedLongPress();

private:
    AutoPtr<IView> mView;
    Boolean mHasPerformedLongPress;
    AutoPtr<CheckForLongPress> mPendingCheckForLongPress;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CHECKLONGPRESSHELPER_H__