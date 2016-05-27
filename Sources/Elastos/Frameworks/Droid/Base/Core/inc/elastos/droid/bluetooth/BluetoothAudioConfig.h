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
