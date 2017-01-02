//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
