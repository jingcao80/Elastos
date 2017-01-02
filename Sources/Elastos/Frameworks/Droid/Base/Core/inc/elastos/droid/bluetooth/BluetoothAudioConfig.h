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

#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHAUDIOCONFIG_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHAUDIOCONFIG_H__

#include "Elastos.Droid.Bluetooth.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {

/**
  * Represents the audio configuration for a Bluetooth A2DP source device.
  *
  * {@see BluetoothA2dpSink}
  *
  * {@hide}
  */
class BluetoothAudioConfig
    : public Object
    , public IBluetoothAudioConfig
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    BluetoothAudioConfig();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 sampleRate,
        /* [in] */ Int32 channelConfig,
        /* [in] */ Int32 audioFormat);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    // @Override
    CARAPI ToString(
        /* [out] */ String* info);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);
    //    /* [in] */ Int32 flags);

    // @Override
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    /**
      * Returns the sample rate in samples per second
      * @return sample rate
      */
    CARAPI GetSampleRate(
        /* [out] */ Int32* result);

    /**
      * Returns the channel configuration (either {@link android.media.AudioFormat#CHANNEL_IN_MONO}
      * or {@link android.media.AudioFormat#CHANNEL_IN_STEREO})
      * @return channel configuration
      */
    CARAPI GetChannelConfig(
        /* [out] */ Int32* result);

    /**
      * Returns the channel audio format (either {@link android.media.AudioFormat#ENCODING_PCM_16BIT}
      * or {@link android.media.AudioFormat#ENCODING_PCM_8BIT}
      * @return audio format
      */
    CARAPI GetAudioFormat(
        /* [out] */ Int32* result);

private:
    Int32 mSampleRate;
    Int32 mChannelConfig;
    Int32 mAudioFormat;
};

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHAUDIOCONFIG_H__
