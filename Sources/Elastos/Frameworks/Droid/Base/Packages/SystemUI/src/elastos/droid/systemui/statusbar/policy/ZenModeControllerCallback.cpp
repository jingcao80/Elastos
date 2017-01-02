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

#include "elastos/droid/systemui/statusbar/policy/ZenModeControllerCallback.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(ZenModeControllerCallback, Object, IZenModeControllerCallback)

ECode ZenModeControllerCallback::OnZenChanged(
    /* [in] */ Int32 zen)
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnExitConditionChanged(
    /* [in] */ ICondition* exitCondition)
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnConditionsChanged(
    /* [in] */ ArrayOf<ICondition*>* conditions)
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnNextAlarmChanged()
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnZenAvailableChanged(
    /* [in] */ Boolean available)
{
    return NOERROR;
}

ECode ZenModeControllerCallback::OnEffectsSupressorChanged()
{
    return NOERROR;
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
