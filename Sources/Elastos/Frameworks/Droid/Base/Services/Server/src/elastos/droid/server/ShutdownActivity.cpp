
#include "ShutdownActivity.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IIPowerManager;

namespace Elastos {
namespace Droid {
namespace Server {

const String ShutdownActivity::TAG("ShutdownActivity");

ECode ShutdownActivity::LocalThread::Run()
{
    AutoPtr<IInterface> service = ServiceManager::GetService(IContext::POWER_SERVICE);
    AutoPtr<IIPowerManager> pm = IIPowerManager::Probe(service);
//    try {
        if (mHost->mReboot) {
            FAIL_RETURN(pm->Reboot(mHost->mConfirm, String(NULL), FALSE));
        } else {
            FAIL_RETURN(pm->Shutdown(mHost->mConfirm, FALSE));
        }
//    } catch (RemoteException e) {
//    }
    return NOERROR;
}


ECode ShutdownActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    String action;
    intent->GetAction(&action);
    mReboot = IIntent::ACTION_REBOOT.Equals(action);
    intent->GetBooleanExtra(IIntent::EXTRA_KEY_CONFIRM, FALSE, &mConfirm);
    Slogger::I(TAG, "onCreate(): confirm=%d", mConfirm);

    AutoPtr<LocalThread> thr = new LocalThread(this, String("ShutdownActivity"));

    thr->Start();
    // Wait for us to tell the power manager to shutdown.
//    try {
    FAIL_RETURN(thr->Join());
//    } catch (InterruptedException e) {
//    }
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
