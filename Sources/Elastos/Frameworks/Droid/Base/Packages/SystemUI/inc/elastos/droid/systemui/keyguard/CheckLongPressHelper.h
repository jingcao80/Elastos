
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CHECKLONGPRESSHELPER_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CHECKLONGPRESSHELPER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

using Elastos::Core::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class CheckLongPressHelper
    : public Object
{
private:
    class CheckForLongPress
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CheckLongPressHelper::CheckForLongPress")

        CheckForLongPress(
            /* [in] */ CheckLongPressHelper* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        CheckLongPressHelper* mHost;
    };

public:
    TO_STRING_IMPL("CheckLongPressHelper")

    CheckLongPressHelper(
        /* [in] */ IView* v);

    CARAPI PostCheckForLongPress(
        /* [in] */ IMotionEvent* ev);

    CARAPI OnMove(
        /* [in] */ IMotionEvent* ev);

    CARAPI CancelLongPress();

    CARAPI HasPerformedLongPress(
        /* [out] */ Boolean* result);

private:
    AutoPtr<IView> mView;
    Boolean mHasPerformedLongPress;
    AutoPtr<CheckForLongPress> mPendingCheckForLongPress;
    Float mDownX, mDownY;
    Int32 mLongPressTimeout;
    Int32 mScaledTouchSlop;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_CHECKLONGPRESSHELPER_H__