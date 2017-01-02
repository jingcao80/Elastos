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

#include "elastos/droid/settings/notification/CManagedServiceSettingsScaryWarningDialogFragment.h"
#include "elastos/droid/settings/notification/CRedactionInterstitialFragment.h"
#include "elastos/droid/settings/notification/CZenModeAutomaticConditionSelectionConditionListener.h"
#include "elastos/droid/settings/notification/CZenModeConditionSelectionConditionListener.h"
#include "elastos/droid/settings/notification/CZenModeSettingsDowntimeDaysSelection.h"
#include "elastos/droid/settings/notification/CZenModeSettingsTimePickerFragment.h"
#include "elastos/droid/settings/notification/CZenModeSettingsTimePickerPreference.h"

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

CAR_OBJECT_IMPL(CManagedServiceSettingsScaryWarningDialogFragment);
CAR_OBJECT_IMPL(CRedactionInterstitialFragment);
CAR_OBJECT_IMPL(CZenModeAutomaticConditionSelectionConditionListener);
CAR_OBJECT_IMPL(CZenModeConditionSelectionConditionListener);
CAR_OBJECT_IMPL(CZenModeSettingsDowntimeDaysSelection);
CAR_OBJECT_IMPL(CZenModeSettingsTimePickerFragment);
CAR_OBJECT_IMPL(CZenModeSettingsTimePickerPreference);

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos