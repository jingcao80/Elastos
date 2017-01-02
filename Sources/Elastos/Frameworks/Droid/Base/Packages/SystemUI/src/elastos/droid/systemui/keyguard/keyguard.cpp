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

#include "elastos/droid/systemui/keyguard/CKeyguardService.h"
#include "elastos/droid/systemui/keyguard/CKeyguardSecurityViewFlipperLayoutParams.h"
#include "elastos/droid/systemui/keyguard/CKeyguardUpdateMonitorContentObserver.h"
#include "elastos/droid/systemui/keyguard/CKeyguardUpdateMonitorBroadcastReceiver1.h"
#include "elastos/droid/systemui/keyguard/CKeyguardUpdateMonitorBroadcastReceiver2.h"
#include "elastos/droid/systemui/keyguard/CKeyguardUpdateMonitorUserSwitchObserver.h"
#include "elastos/droid/systemui/keyguard/CAlphaOptimizedLinearLayout.h"
#include "elastos/droid/systemui/keyguard/CKeyguardWidgetFrame.h"
#include "elastos/droid/systemui/keyguard/CKeyguardWidgetPager.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_OBJECT_IMPL(CKeyguardService)
CAR_OBJECT_IMPL(CKeyguardSecurityViewFlipperLayoutParams)
CAR_OBJECT_IMPL(CKeyguardUpdateMonitorContentObserver)
CAR_OBJECT_IMPL(CKeyguardUpdateMonitorBroadcastReceiver1)
CAR_OBJECT_IMPL(CKeyguardUpdateMonitorBroadcastReceiver2)
CAR_OBJECT_IMPL(CKeyguardUpdateMonitorUserSwitchObserver)
CAR_OBJECT_IMPL(CAlphaOptimizedLinearLayout)
CAR_OBJECT_IMPL(CKeyguardWidgetFrame)
CAR_OBJECT_IMPL(CKeyguardWidgetPager)

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos