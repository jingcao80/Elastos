
#include "elastos/droid/internal/app/DisableCarModeActivity.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::App::IUiModeManager;
using Elastos::Droid::App::IIUiModeManager;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

const String DisableCarModeActivity::TAG("DisableCarModeActivity");

CAR_INTERFACE_IMPL(DisableCarModeActivity, Activity, IDisableCarModeActivity)

ECode DisableCarModeActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);

    AutoPtr<IIUiModeManager> uiModeManager = IIUiModeManager::Probe(ServiceManager::GetService(String("uimode")));
    if (FAILED(uiModeManager->DisableCarMode(IUiModeManager::DISABLE_CAR_MODE_GO_HOME)))
        Logger::E(TAG, "Failed to disable car mode");
    Finish();
    return NOERROR;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
