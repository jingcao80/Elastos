#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/content/CComponentName.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/service/dreams/Sandman.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Service::Dreams::IIDreamManager;
using Elastos::Droid::R;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

static AutoPtr<IComponentName> InitStatic()
{
    AutoPtr<CComponentName> cc;
    CComponentName::NewByFriend(String("com.android.systemui"), String("com.android.systemui.Somnambulator"), (CComponentName**)&cc);
    return (IComponentName*)(cc.Get());
}

const String Sandman::TAG("Sandman");
AutoPtr<IComponentName> Sandman::SOMNAMBULATOR_COMPONENT = InitStatic();

Sandman::Sandman()
{
}

ECode Sandman::ShouldStartDockApp(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IComponentName> name;
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    intent->ResolveActivity(pm, (IComponentName**)&name);
    Boolean isEqual = FALSE;
    IObject::Probe(name)->Equals(SOMNAMBULATOR_COMPONENT, &isEqual);
    *result = name != NULL && !isEqual;
    return NOERROR;
}

ECode Sandman::StartDreamByUserRequest(
    /* [in] */ IContext* context)
{
    StartDream(context, FALSE);
    return NOERROR;
}

ECode Sandman::StartDreamWhenDockedIfAppropriate(
    /* [in] */ IContext* context)
{
    if (!IsScreenSaverEnabled(context) || !IsScreenSaverActivatedOnDock(context)) {
        Slogger::I(TAG, "Dreams currently disabled for docks.");
        return E_NULL_POINTER_EXCEPTION;
    }

    StartDream(context, TRUE);
    return NOERROR;
}

ECode Sandman::StartDream(
    /* [in] */ IContext* context,
    /* [in] */ Boolean docked)
{
    AutoPtr<IInterface> obj = ServiceManager::GetService(IDreamService::DREAM_SERVICE);
    AutoPtr<IIDreamManager> dreamManagerService = IIDreamManager::Probe(obj);
    Boolean isDreaming;
    dreamManagerService->IsDreaming(&isDreaming);
    if (dreamManagerService != NULL && !isDreaming) {
        if (docked) {
            Slogger::I(TAG, "Activating dream while docked.");

            // Wake up.
            // The power manager will wake up the system automatically when it starts
            // receiving power from a dock but there is a race between that happening
            // and the UI mode manager starting a dream.  We want the system to already
            // be awake by the time this happens.  Otherwise the dream may not start.
            AutoPtr<IInterface> obj2;
            context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj2);
            AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(obj2);
            ECode ec = powerManager->WakeUp(SystemClock::GetUptimeMillis());
            if (FAILED(ec)) {
                Slogger::E(TAG, "Could not start dream when docked.%08x", ec);
                return E_REMOTE_EXCEPTION;
            }
        }
        else {
            Slogger::I(TAG, "Activating dream by user request.");
        }

        // Dream.
        dreamManagerService->Dream();
    }
    return NOERROR;
}

Boolean Sandman::IsScreenSaverEnabled(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean def;
    res->GetBoolean(R::bool_::config_dreamsEnabledByDefault ? 1 : 0, &def);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int32 i;
    Settings::Secure::GetInt32ForUser(resolver, ISettingsSecure::SCREENSAVER_ENABLED, def, IUserHandle::USER_CURRENT, &i);
    return i != 0;
}

Boolean Sandman::IsScreenSaverActivatedOnDock(
    /* [in] */ IContext* context)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Boolean def;
    res->GetBoolean(R::bool_::config_dreamsActivatedOnDockByDefault ? 1 : 0, &def);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    Int32 i;
    Settings::Secure::GetInt32ForUser(resolver, ISettingsSecure::SCREENSAVER_ACTIVATE_ON_DOCK, def, IUserHandle::USER_CURRENT, &i);
    return i != 0;
}

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos
