#include "elastos/droid/server/location/ActivityRecognitionProxy.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Hardware::Location::IIActivityRecognitionHardware;
using Elastos::Droid::Hardware::Location::IIActivityRecognitionHardwareWatcher;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//====================================
// ActivityRecognitionProxy::MyRunnable
//====================================

ActivityRecognitionProxy::MyRunnable::MyRunnable(
    /* [in] */ ActivityRecognitionProxy* host)
    : mHost(host)
{
}

ECode ActivityRecognitionProxy::MyRunnable::Run()
{
    return mHost->BindProvider(mHost->mActivityRecognitionHardware);
}

ActivityRecognitionProxy::ActivityRecognitionProxy(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IActivityRecognitionHardware* activityRecognitionHardware,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNameResId)
{
    mActivityRecognitionHardware = activityRecognitionHardware;

    AutoPtr<MyRunnable> newServiceWork = new MyRunnable(this);

    // prepare the connection to the provider
    mServiceWatcher = new ServiceWatcher(
        context,
        TAG,
        String("com.android.location.service.ActivityRecognitionProvider"),
        overlaySwitchResId,
        defaultServicePackageNameResId,
        initialPackageNameResId,
        (IRunnable*)newServiceWork.Get(),
        handler);
}

//====================================
// ActivityRecognitionProxy
//====================================

const String ActivityRecognitionProxy::TAG("ActivityRecognitionProxy");

AutoPtr<ActivityRecognitionProxy> ActivityRecognitionProxy::CreateAndBind(
    /* [in] */ IContext* context,
    /* [in] */ IHandler* handler,
    /* [in] */ IActivityRecognitionHardware* activityRecognitionHardware,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNameResId)
{
    AutoPtr<ActivityRecognitionProxy> activityRecognitionProxy = new ActivityRecognitionProxy(
        context,
        handler,
        activityRecognitionHardware,
        overlaySwitchResId,
        defaultServicePackageNameResId,
        initialPackageNameResId);

    // try to bind the provider
    if (!activityRecognitionProxy->mServiceWatcher->Start()) {
        Logger::E(TAG, "ServiceWatcher could not start.");
        return NULL;
    }

    return activityRecognitionProxy;
}

ECode ActivityRecognitionProxy::BindProvider(
    /* [in] */ IActivityRecognitionHardware* activityRecognitionHardware)
{
    AutoPtr<IIActivityRecognitionHardwareWatcher> watcher = IIActivityRecognitionHardwareWatcher::Probe(mServiceWatcher->GetBinder());

    if (watcher == NULL) {
        Logger::E(TAG, "No provider instance found on connection.");
        return E_NULL_POINTER_EXCEPTION;
    }

    ECode ec = watcher->OnInstanceChanged(IIActivityRecognitionHardware::Probe(mActivityRecognitionHardware));
    if (FAILED(ec)) {
        Logger::E(TAG, "Error delivering hardware interface.%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos