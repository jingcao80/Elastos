
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
    Int32 res;
    GetDefault()->BroadcastIntent(
        NULL, intent, String(NULL), NULL, IActivity::RESULT_OK, String(NULL), NULL,
        String(NULL) /*permission*/, IAppOpsManager::OP_NONE, FALSE, TRUE, userId, &res);
}

void ActivityManagerNative::NoteWakeupAlarm(
    /* [in] */ IPendingIntent* ps,
    /* [in] */ Int32 sourceUid,
    /* [in] */ const String& sourcePkg)
{
    AutoPtr<IIIntentSender> target;
    ps->GetTarget((IIIntentSender**)&target);
    GetDefault()->NoteWakeupAlarm(target, sourceUid, sourcePkg);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
