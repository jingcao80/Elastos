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

#include "elastos/droid/internal/app/CChooserActivity.h"
#include "elastos/droid/internal/app/CDisableCarModeActivity.h"
#include "elastos/droid/internal/app/CExternalMediaFormatActivity.h"
#include "elastos/droid/internal/app/CHeavyWeightSwitcherActivity.h"
#include "elastos/droid/internal/app/CIntentForwarderActivity.h"
#include "elastos/droid/internal/app/CLocalePicker.h"
#include "elastos/droid/internal/app/CMediaRouteChooserDialog.h"
#include "elastos/droid/internal/app/CMediaRouteChooserDialogFragment.h"
#include "elastos/droid/internal/app/CMediaRouteControllerDialog.h"
#include "elastos/droid/internal/app/CMediaRouteControllerDialogFragment.h"
#include "elastos/droid/internal/app/CNavItemSelectedListener.h"
#include "elastos/droid/internal/app/CNetInitiatedActivity.h"
#include "elastos/droid/internal/app/CPlatLogoActivity.h"
#include "elastos/droid/internal/app/CProcessMap.h"
#include "elastos/droid/internal/app/CAlertControllerRecycleListView.h"
#include "elastos/droid/internal/app/CResolverActivity.h"
#include "elastos/droid/internal/app/CRestrictionsPinActivity.h"
#include "elastos/droid/internal/app/CToolbarActionBar.h"
#include "elastos/droid/internal/app/CWindowDecorActionBar.h"
#include "elastos/droid/internal/app/CProcessStats.h"
#include "elastos/droid/internal/app/CProcessStateHolder.h"
#include "elastos/droid/internal/app/CProcessDataCollection.h"
#include "elastos/droid/internal/app/CTotalMemoryUseCollection.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_OBJECT_IMPL(CChooserActivity)

CAR_OBJECT_IMPL(CDisableCarModeActivity)

CAR_OBJECT_IMPL(CExternalMediaFormatActivity)

CAR_OBJECT_IMPL(CHeavyWeightSwitcherActivity)

CAR_OBJECT_IMPL(CIntentForwarderActivity)

CAR_OBJECT_IMPL(CLocalePicker)

CAR_OBJECT_IMPL(CNavItemSelectedListener)

CAR_OBJECT_IMPL(CNetInitiatedActivity)

CAR_OBJECT_IMPL(CPlatLogoActivity)

CAR_OBJECT_IMPL(CProcessMap)

CAR_OBJECT_IMPL(CAlertControllerRecycleListView)

CAR_OBJECT_IMPL(CResolverActivity)

CAR_OBJECT_IMPL(CRestrictionsPinActivity)

CAR_OBJECT_IMPL(CToolbarActionBar)

CAR_OBJECT_IMPL(CWindowDecorActionBar)

CAR_OBJECT_IMPL(CProcessStats)

CAR_OBJECT_IMPL(CProcessStateHolder)

CAR_OBJECT_IMPL(CProcessDataCollection)

CAR_OBJECT_IMPL(CTotalMemoryUseCollection)

CAR_OBJECT_IMPL(CMediaRouteChooserDialog)

CAR_OBJECT_IMPL(CMediaRouteChooserDialogFragment)

CAR_OBJECT_IMPL(CMediaRouteControllerDialog)

CAR_OBJECT_IMPL(CMediaRouteControllerDialogFragment)

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
