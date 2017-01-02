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

#ifndef __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHCALLBACKWRAPPER_H__
#define __ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHCALLBACKWRAPPER_H__

#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Bluetooth_CBluetoothHealthCallbackWrapper.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

CarClass(CBluetoothHealthCallbackWrapper)
    , public Object
    , public IIBluetoothHealthCallback
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBluetoothHealthCallbackWrapper();

    CARAPI OnHealthAppConfigurationStatusChange(
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [in] */ Int32 status);

    CARAPI OnHealthChannelStateChange(
        /* [in] */ IBluetoothHealthAppConfiguration* config,
        /* [in] */ IBluetoothDevice* device,
        /* [in] */ Int32 prevState,
        /* [in] */ Int32 newState,
        /* [in] */ IParcelFileDescriptor* fd,
        /* [in] */ Int32 channelId);

    CARAPI constructor(
        /* [in] */ IBluetoothHealthCallback* cb);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
private:
    AutoPtr<IBluetoothHealthCallback> mCallback;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_BLUETOOTH_CBLUETOOTHHEALTHCALLBACKWRAPPER_H__
