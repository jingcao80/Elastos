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

#ifndef __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISECALLBACK_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISECALLBACK_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * Bluetooth LE advertising callbacks, used to deliver advertising operation status.
  */
class AdvertiseCallback
    : public Object
    , public IAdvertiseCallback
{
public:

    CAR_INTERFACE_DECL()

    AdvertiseCallback();

    virtual ~AdvertiseCallback() = 0;

    /**
      * Callback triggered in response to {@link BluetoothLeAdvertiser#startAdvertising} indicating
      * that the advertising has been started successfully.
      *
      * @param settingsInEffect The actual settings used for advertising, which may be different from
      *            what has been requested.
      */
    CARAPI OnStartSuccess(
        /* [in] */ IAdvertiseSettings* settingsInEffect);

    /**
      * Callback when advertising could not be started.
      *
      * @param errorCode Error code (see ADVERTISE_FAILED_* constants) for advertising start
      *            failures.
      */
    CARAPI OnStartFailure(
        /* [in] */ Int32 errorCode);
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_ADVERTISECALLBACK_H__

