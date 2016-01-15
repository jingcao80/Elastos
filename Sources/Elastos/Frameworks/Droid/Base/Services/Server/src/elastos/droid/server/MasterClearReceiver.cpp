
#include "elastos/droid/server/MasterClearReceiver.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

MasterClearReceiver::RebootThread::RebootThread()
{
    Thread::constructor(String("Reboot"));
}

ECode MasterClearReceiver::RebootThread::Run()
{
    Slogger::E("MasterClearReceiver", "MasterClearReceiver::RebootThread::Run() E_NOT_IMPLEMENTED");
    // try {
    //     RecoverySystem.rebootWipeUserData(context);
        // Log.wtf(TAG, "Still running after master clear?!");
    // } catch (IOException e) {
    //     Slog.e(TAG, "Can't perform master clear/factory reset", e);
    // }
    return NOERROR;
}

ECode MasterClearReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_REMOTE_INTENT)) {
        String extra;
        intent->GetStringExtra(String("from"), &extra);
        if (!extra.Equals("google.com")) {
            Slogger::W("MasterClearReceiver", "Ignoring master clear request -- not from trusted server.");
            return NOERROR;
        }
    }

    Slogger::W("MasterClearReceiver", "!!! FACTORY RESET !!!");
    // The reboot call is blocking, so we need to do it on another thread.
    mRebootThread = new RebootThread();
    mRebootThread->Start();

    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos