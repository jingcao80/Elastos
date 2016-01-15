
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/ActivityManagerNative.h"
// #include "elastos/droid/app/CAppOpsManager.h"
#include "elastos/droid/os/ServiceManager.h"

using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIIntentSender;

namespace Elastos {
namespace Droid {
namespace App {

Boolean ActivityManagerNative::sSystemReady = FALSE;

AutoPtr<IIActivityManager> ActivityManagerNative::GetDefault()
{
    AutoPtr<IIActivityManager> service =
            (IIActivityManager*)ServiceManager::GetService(IContext::ACTIVITY_SERVICE).Get();
    assert(service != NULL);
    return service;
}

Boolean ActivityManagerNative::IsSystemReady()
{
    if (!sSystemReady) {
        GetDefault()->TestIsSystemReady(&sSystemReady);
    }
    return sSystemReady;
}

void ActivityManagerNative::BroadcastStickyIntent(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& permission,
    /* [in] */ Int32 userId)
{
    assert(0 && "TODO");
    Int32 res;
    String nullStr;
    // GetDefault()->BroadcastIntent(
    //     NULL, intent, nullStr, NULL, IActivity::RESULT_OK, nullStr, NULL,
    //     nullStr /*permission*/, 1/*CAppOpsManager::OP_NONE*/, FALSE, TRUE, userId, &res);
}

void ActivityManagerNative::NoteWakeupAlarm(
    /* [in] */ IPendingIntent* ps)
{
    AutoPtr<IIIntentSender> target;
    ps->GetTarget((IIIntentSender**)&target);
    GetDefault()->NoteWakeupAlarm(target);
}



} // namespace App
} // namespace Droid
} // namespace Elastos
