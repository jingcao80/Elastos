
#include "CLauncherHelper.h"
#include <cutils/properties.h>
#include <elautoptr.h>
#include "Elastos.Droid.Core.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::App::IIActivityManager;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

ECode CLauncherHelper::LaunchApp(
    /* [in] */ IIntent* intent)
{
    if (!intent) return E_INVALID_ARGUMENT;

    AutoPtr<IServiceManager> sm;
    AutoPtr<IIActivityManager> am;
    ECode ec = CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    if (FAILED(ec)) return ec;

    ec = sm->GetService(IContext::ACTIVITY_SERVICE, (IInterface**)&am);
    if (FAILED(ec)) return ec;

    Int32 status;
    return am->StartActivity(NULL, intent, String(NULL),
            NULL, String(NULL), 0, 0, String(NULL), NULL, NULL, &status);
}

ECode CLauncherHelper::CloseBootanimation()
{
    property_set("ctl.stop", "bootanim");

    return NOERROR;
}

ECode CLauncherHelper::StartVideoViewDemo(
    /* [in] */ const String& path)
{
    AutoPtr<IIntent> intent;
    ECode ec = CIntent::New((IIntent**)&intent);
    if (FAILED(ec)) return ec;
    intent->SetPackage(String("VideoViewDemo"));
    intent->SetAction(String("android.intent.action.MAIN"));
    intent->PutExtra(String(IIntent::EXTRA_STREAM), path);

    return LaunchApp(intent);
}

ECode CLauncherHelper::LaunchApp2(
    /* [in] */ const String& capsule,
    /* [in] */ const String& action)
{
    AutoPtr<IIntent> intent;
    ECode ec = CIntent::New((IIntent**)&intent);
    if (FAILED(ec)) return ec;
    intent->SetPackage(capsule);
    intent->SetAction(action);

    return LaunchApp(intent);
}

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
