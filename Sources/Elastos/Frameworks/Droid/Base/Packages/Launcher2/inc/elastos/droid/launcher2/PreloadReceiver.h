#ifndef  __ELASTOS_DROID_LAUNCHER2_PRELOADRECEIVER_H__
#define  __ELASTOS_DROID_LAUNCHER2_PRELOADRECEIVER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class PreloadReceiver
    : public BroadcastReceiver
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
        ILauncherProvider* mProvider;
        Int32 mWorkspaceResId;
        Boolean mOverridePrevious;
    };

public:
    CAR_INTERFACE_DECL();

    //@Override
    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    static const String TAG = "Launcher.PreloadReceiver";
    static const Boolean LOGD = FALSE;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PRELOADRECEIVER_H__