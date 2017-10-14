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

#ifndef __ELASTOS_DROID_MEDIA_CAUDIODEVICEPORT_H__
#define __ELASTOS_DROID_MEDIA_CAUDIODEVICEPORT_H__

#include "_Elastos_Droid_Media_CAudioDevicePort.h"
#include "elastos/droid/media/AudioPort.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The AudioDevicePort is a specialized type of AudioPort
 * describing an input (e.g microphone) or output device (e.g speaker)
 * of the system.
 * An AudioDevicePort is an AudioPort controlled by the audio HAL, almost always a physical
 * device at the boundary of the audio system.
 * In addition to base audio port attributes, the device descriptor contains:
 * - the device type (e.g AudioManager.DEVICE_OUT_SPEAKER)
 * - the device address (e.g MAC adddress for AD2P sink).
 * @see AudioPort
 * @hide
 */
CarClass(CAudioDevicePort)
    , public AudioPort
    , public IAudioDevicePort
{
public:
    CAR_INTERFACE_DECL()

    virtual ~CAudioDevicePort();

    CAR_OBJECT_DECL()

    CAudioDevicePort();

    CARAPI constructor(
        /* [in] */ IAudioHandle* handle,
        /* [in] */ ArrayOf<Int32>* samplingRates,
        /* [in] */ ArrayOf<Int32>* channelMasks,
        /* [in] */ ArrayOf<Int32>* formats,
        /* [in] */ ArrayOf<IAudioGain*>* gains,
        /* [in] */ Int32 type,
        /* [in] */ const String& address);

    /**
     * Get the device type (e.g AudioManager.DEVICE_OUT_SPEAKER)
     */
    CARAPI Type(
        /* [out] */ Int32* result);

    /**
     * Get the device address. Address format varies with the device type.
     * - USB devices ({@link AudioManager#DEVICE_OUT_USB_DEVICE},
     * {@link AudioManager#DEVICE_IN_USB_DEVICE}) use an address composed of the ALSA card number
     * and device number: "card=2;device=1"
     * - Bluetooth devices ({@link AudioManager#DEVICE_OUT_BLUETOOTH_SCO},
     * {@link AudioManager#DEVICE_OUT_BLUETOOTH_SCO}, {@link AudioManager#DEVICE_OUT_BLUETOOTH_A2DP})
     * use the MAC address of the bluetooth device in the form "00:11:22:AA:BB:CC" as reported by
     * {@link BluetoothDevice#getAddress()}.
     * - Deivces that do not have an address will indicate an empty string "".
     */
    CARAPI Address(
        /* [out] */ String* result);

    using AudioPort::BuildConfig;

    /**
     * Build a specific configuration of this audio device port for use by methods
     * like AudioManager.connectAudioPatch().
     */
    CARAPI BuildConfig(
        /* [in] */ Int32 samplingRate,
        /* [in] */ Int32 channelMask,
        /* [in] */ Int32 format,
        /* [in] */ IAudioGainConfig* gain,
        /* [out] */ IAudioDevicePortConfig** result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    Int32 mType;
    String mAddress;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIODEVICEPORT_H__
