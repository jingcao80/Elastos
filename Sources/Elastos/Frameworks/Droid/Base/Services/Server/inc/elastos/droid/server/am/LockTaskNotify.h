
#ifndef __ELASTOS_DROID_SERVER_AM_LOCKTASKNOTIFY_H__
#define __ELASTOS_DROID_SERVER_AM_LOCKTASKNOTIFY_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::IToast;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class ActivityRecord;

class LockTaskNotify : public Object
{
private:
    class H : public Handler
    {
    public:
        TO_STRING_IMPL("LockTaskNotify::H")

        H(
            /* [in] */ LockTaskNotify* host)
            : mHost(host)
        {}

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 SHOW_TOAST = 3;

    private:
        LockTaskNotify* mHost;
    };

public:
    LockTaskNotify(
        /* [in] */ IContext* context);

    CARAPI_(void) ShowToast(
        /* [in] */ Boolean isLocked);

    CARAPI_(void) HandleShowToast(
        /* [in] */ Boolean isLocked);

    CARAPI_(void) Show(
        /* [in] */ Boolean starting);

private:
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<H> mHandler;
    AutoPtr<IWindowManagerPolicy> mPolicy;
    AutoPtr<IAccessibilityManager> mAccessibilityManager;
    AutoPtr<IToast> mLastToast;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_LOCKTASKNOTIFY_H__
