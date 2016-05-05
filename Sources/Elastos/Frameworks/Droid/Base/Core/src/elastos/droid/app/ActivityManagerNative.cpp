
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIIntentSender;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

Boolean ActivityManagerNative::sSystemReady = FALSE;

AutoPtr<IIActivityManager> ActivityManagerNative::GetDefault()
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::ACTIVITY_SERVICE);
    AutoPtr<IIActivityManager> service = IIActivityManager::Probe(obj);
    if (service == NULL) {
        Logger::W("ActivityManagerNative", "GetDefault(): %s service not ready yet.", IContext::ACTIVITY_SERVICE.string());
    }
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
    String nullStr;
    GetDefault()->BroadcastIntent(
        NULL, intent, nullStr, NULL, IActivity::RESULT_OK, nullStr, NULL,
        nullStr /*permission*/, IAppOpsManager::OP_NONE, FALSE, TRUE, userId, &res);
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
