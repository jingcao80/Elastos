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

#ifndef __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_POLICY_BATTERYCONTROLLER_H__
#define __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_POLICY_BATTERYCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IPowerManager;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class BatteryController
    : public Object
    , public IBatteryController
{
public:
    CAR_INTERFACE_DECL()

    BatteryController(
        /* [in] */ IContext* context);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI AddStateChangedCallback(
        /* [in] */ IBatteryStateChangeCallback* cb);

    CARAPI RemoveStateChangedCallback(
        /* [in] */ IBatteryStateChangeCallback* cb);

    CARAPI IsPowerSave(
        /* [out] */ Boolean* result);

    CARAPI_(void) SetPowerSave(
        /* [in] */ Boolean powerSave);

    CARAPI_(void) FireBatteryLevelChanged();

    CARAPI_(void) UpdatePowerSave();

private:
    CARAPI_(void) FirePowerSaveChanged();

public:
    Int32 mLevel;
    Boolean mCharging;
    Boolean mCharged;
    Boolean mPluggedIn;

private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IArrayList> mChangeCallbacks;  /*<BatteryStateChangeCallback*/
    AutoPtr<IPowerManager> mPowerManager;

    Boolean mPowerSave;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_POLICY_BATTERYCONTROLLER_H__
