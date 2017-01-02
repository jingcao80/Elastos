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

#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class PairedDevice
    : public Object
    , public IBluetoothControllerPairedDevice
{
public:
    CAR_INTERFACE_DECL()

    PairedDevice();

    CARAPI SetId(
        /* [in] */ const String& id);

    CARAPI GetId(
        /* [out] */ String* id);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetState(
        /* [in] */ Int32 state);

    CARAPI GetState(
        /* [out] */ Int32* state);

    CARAPI SetTag(
        /* [in] */ IInterface* tag);

    CARAPI GetTag(
        /* [out] */ IInterface** tag);

    CARAPI_(String) StateToString(
        /* [in] */ Int32 state);

public:
    String mId;
    String mName;
    Int32 mState;
    AutoPtr<IInterface> mTag;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_PAIREDDEVICE_H__
