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

#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CKEYGUARDFACEUNLOCKVIEWROTATIONWATCHER_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CKEYGUARDFACEUNLOCKVIEWROTATIONWATCHER_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CKeyguardFaceUnlockViewRotationWatcher.h"
#include "elastos/droid/systemui/keyguard/CKeyguardFaceUnlockView.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::IRotationWatcher;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CKeyguardFaceUnlockViewRotationWatcher)
    , public Object
    , public IBinder
    , public IRotationWatcher
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IKeyguardFaceUnlockView* host);

    // @Override
    CARAPI OnRotationChanged(
        /* [in] */ Int32 rotation);

    CARAPI ToString(
        /* [out] */ String* info);

private:
    CKeyguardFaceUnlockView* mHost;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_CKEYGUARDFACEUNLOCKVIEWROTATIONWATCHER_H__
