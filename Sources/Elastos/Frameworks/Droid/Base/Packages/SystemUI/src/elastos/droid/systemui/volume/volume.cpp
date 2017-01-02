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

#include "elastos/droid/systemui/volume/CSegmentedButtons.h"
#include "elastos/droid/systemui/volume/CVolumeController.h"
#include "elastos/droid/systemui/volume/CRemoteVolumeController.h"
#include "elastos/droid/systemui/volume/CZenModePanel.h"
#include "elastos/droid/systemui/volume/CVolumePanel.h"
#include "elastos/droid/systemui/volume/CVolumeDialog.h"
#include "elastos/droid/systemui/volume/CVolumePanelSafetyWarning.h"

namespace Elastos {
namespace Droid {
namespace SystemUI{
namespace Volume{

CAR_OBJECT_IMPL(CSegmentedButtons)
CAR_OBJECT_IMPL(CVolumeController)
CAR_OBJECT_IMPL(CRemoteVolumeController)
CAR_OBJECT_IMPL(CZenModePanel)
CAR_OBJECT_IMPL(CVolumePanel)
CAR_OBJECT_IMPL(CVolumeDialog)
CAR_OBJECT_IMPL(CVolumePanelSafetyWarning)


} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos