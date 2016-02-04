
#include "elastos/droid/bluetooth/BluetoothAudioConfig.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//                         BluetoothAudioConfig
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothAudioConfig, Object, IBluetoothAudioConfig, IParcelable);

BluetoothAudioConfig::BluetoothAudioConfig()
{
}

BluetoothAudioConfig::BluetoothAudioConfig(
    /* [in] */ Int32 sampleRate,
    /* [in] */ Int32 channelConfig,
    /* [in] */ Int32 audioFormat)
{
    // ==================before translated======================
    // mSampleRate = sampleRate;
    // mChannelConfig = channelConfig;
    // mAudioFormat = audioFormat;
}

ECode BluetoothAudioConfig::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (o instanceof BluetoothAudioConfig) {
    //     BluetoothAudioConfig bac = (BluetoothAudioConfig)o;
    //     return (bac.mSampleRate == mSampleRate &&
    //             bac.mChannelConfig == mChannelConfig &&
    //             bac.mAudioFormat == mAudioFormat);
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothAudioConfig::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    // ==================before translated======================
    // return mSampleRate | (mChannelConfig << 24) | (mAudioFormat << 28);
    assert(0);
    return NOERROR;
}

ECode BluetoothAudioConfig::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    // ==================before translated======================
    // return "{mSampleRate:" + mSampleRate + ",mChannelConfig:" + mChannelConfig
    //         + ",mAudioFormat:" + mAudioFormat + "}";
    assert(0);
    return NOERROR;
}

ECode BluetoothAudioConfig::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int32 flags)
{
    VALIDATE_NOT_NULL(out);
    // ==================before translated======================
    // out.writeInt(mSampleRate);
    // out.writeInt(mChannelConfig);
    // out.writeInt(mAudioFormat);
    assert(0);
    return NOERROR;
}

CARAPI BluetoothAudioConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return NOERROR;
}

ECode BluetoothAudioConfig::GetSampleRate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSampleRate;
    assert(0);
    return NOERROR;
}

ECode BluetoothAudioConfig::GetChannelConfig(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mChannelConfig;
    assert(0);
    return NOERROR;
}

ECode BluetoothAudioConfig::GetAudioFormat(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mAudioFormat;
    assert(0);
    return NOERROR;
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
