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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEADSETCLIENTCALL_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEADSETCLIENTCALL_H__

#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * This class represents a single call, its state and properties.
  * It implements {@link Parcelable} for inter-process message passing.
  * @hide
  */
class BluetoothHeadsetClientCall
    : public Object
    , public IBluetoothHeadsetClientCall
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    BluetoothHeadsetClientCall();

    CARAPI constructor();

    /**
      * Creates BluetoothHeadsetClientCall instance.
      */
    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ Int32 state,
        /* [in] */ const String& number,
        /* [in] */ Boolean multiParty,
        /* [in] */ Boolean outgoing);

    /**
      * Sets call's state.
      *
      * <p>Note: This is an internal function and shouldn't be exposed</p>
      *
      * @param  state    new call state.
      */
    CARAPI SetState(
        /* [in] */ Int32 state);

    /**
      * Sets call's number.
      *
      * <p>Note: This is an internal function and shouldn't be exposed</p>
      *
      * @param number    String representing phone number.
      */
    CARAPI SetNumber(
        /* [in] */ const String& number);

    /**
      * Sets this call as multi party call.
      *
      * <p>Note: This is an internal function and shouldn't be exposed</p>
      *
      * @param multiParty    if <code>true</code> sets this call as a part
      *                      of multi party conference.
      */
    CARAPI SetMultiParty(
        /* [in] */ Boolean multiParty);

    /**
      * Gets call's Id.
      *
      * @return call id.
      */
    CARAPI GetId(
        /* [out] */ Int32* result);

    /**
      * Gets call's current state.
      *
      * @return state of this particular phone call.
      */
    CARAPI GetState(
        /* [out] */ Int32* result);

    /**
      * Gets call's number.
      *
      * @return string representing phone number.
      */
    CARAPI GetNumber(
        /* [out] */ String* result);

    /**
      * Checks if call is an active call in a conference mode (aka multi party).
      *
      * @return <code>true</code> if call is a multi party call,
      *         <code>false</code> otherwise.
      */
    CARAPI IsMultiParty(
        /* [out] */ Boolean* result);

    /**
      * Checks if this call is an outgoing call.
      *
      * @return <code>true</code> if its outgoing call,
      *         <code>false</code> otherwise.
      */
    CARAPI IsOutgoing(
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);
    //    /* [in] */ Int32 flags);

    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    Int32 mId;
    Int32 mState;
    String mNumber;
    Boolean mMultiParty;
    Boolean mOutgoing;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHHEADSETCLIENTCALL_H__

