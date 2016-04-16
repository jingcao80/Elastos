#ifndef  __ELASTOS_DROID_LAUNCHER2_CPRELOADRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CPRELOADRECEIVER_H__

#include "_Elastos_Droid_Launcher2_CPreloadReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CPreloadReceiver)
    , public BroadcastReceiver
    , public IPreloadReceiver
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ ILauncherProvider* provider,
            /* [in] */ Int32 workspaceResId,
            /* [in] */ Boolean overridePrevious);

        CARAPI Run();

    private:
        AutoPtr<ILauncherProvider> mProvider;
        Int32 mWorkspaceResId;
        Boolean mOverridePrevious;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static const String TAG;
    static const Boolean LOGD;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CPRELOADRECEIVER_H__