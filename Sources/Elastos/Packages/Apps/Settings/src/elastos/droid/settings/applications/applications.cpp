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

#include "elastos/droid/settings/applications/CApplicationsStatePackageStatsObserver.h"
#include "elastos/droid/settings/applications/CInstalledAppDetailsAlertDialogFragment.h"
#include "elastos/droid/settings/applications/CInstalledAppDetailsClearUserDataObserver.h"
#include "elastos/droid/settings/applications/CInstalledAppDetailsClearCacheObserver.h"
#include "elastos/droid/settings/applications/CInstalledAppDetailsPackageMoveObserver.h"
#include "elastos/droid/settings/applications/CManageApplications.h"
#include "elastos/droid/settings/applications/CRunningServiceDetailsAlertDialogFragment.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CAR_OBJECT_IMPL(CApplicationsStatePackageStatsObserver);
CAR_OBJECT_IMPL(CInstalledAppDetailsAlertDialogFragment);
CAR_OBJECT_IMPL(CInstalledAppDetailsClearUserDataObserver);
CAR_OBJECT_IMPL(CInstalledAppDetailsClearCacheObserver);
CAR_OBJECT_IMPL(CInstalledAppDetailsPackageMoveObserver);
CAR_OBJECT_IMPL(CManageApplications);
CAR_OBJECT_IMPL(CRunningServiceDetailsAlertDialogFragment);

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos