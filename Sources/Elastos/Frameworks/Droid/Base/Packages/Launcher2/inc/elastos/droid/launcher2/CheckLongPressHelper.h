#ifndef  __ELASTOS_DROID_LAUNCHER2_CHECKLONGPRESSHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CHECKLONGPRESSHELPER_H__

#include "elastos/droid/ext/frameworkext.h"

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
        CheckLongPressHelper* mHost;
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