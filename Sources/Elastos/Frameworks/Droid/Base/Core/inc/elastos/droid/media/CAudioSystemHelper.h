
#ifndef __ELASTOS_DROID_MEDIA_CAUDIOSYSTEMHELPER_H__
#define __ELASTOS_DROID_MEDIA_CAUDIOSYSTEMHELPER_H__

#include "_Elastos_Droid_Media_CAudioSystemHelper.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CAudioSystemHelper)
{
public:
    CARAPI GetNumStreamTypes(
        /* [out] */ Int32* result);

    /*
     * Sets the microphone mute on or off.
     *
     * @param on set <var>true</var> to mute the microphone;
     *           <var>false</var> to turn mute off
     * @return command completion status see AUDIO_STATUS_OK, see AUDIO_STATUS_ERROR
     */
    CARAPI MuteMicrophone(
        /* [in] */ Boolean on);

    /*
     * Checks whether the microphone mute is on or off.
     *
     * @return true if microphone is muted, false if it's not
     */
    CARAPI IsMicrophoneMuted(
        /* [out] */ Boolean* result);

    /*
     * Checks whether the specified stream type is active.
     *
     * return true if any track playing on this stream is active.
     */
    CARAPI IsStreamActive(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 inPastMs,
        /* [out] */ Boolean* result);

    /*
     * Checks whether the specified audio source is active.
     *
     * return true if any recorder using this source is currently recording
     */
    CARAPI IsSourceActive(
        /* [in] */ Int32 source,
        /* [out] */ Boolean* result);

    /*
     * Sets a group generic audio configuration parameters. The use of these parameters
     * are platform dependent, see libaudio
     *
     * param keyValuePairs  list of parameters key value pairs in the form:
     *    key1=value1;key2=value2;...
     */
    CARAPI SetParameters(
        /* [in] */ const String& keyValuePairs);

    /*
     * Gets a group generic audio configuration parameters. The use of these parameters
     * are platform dependent, see libaudio
     *
     * param keys  list of parameters
     * return value: list of parameters key value pairs in the form:
     *    key1=value1;key2=value2;...
     */
    CARAPI GetParameters(
        /* [in] */ const String& keys,
        /* [out] */ String* result);

    /*
     * Registers a callback to be invoked when an error occurs.
     * @param cb the callback to run
     */
    CARAPI SetErrorCallback(
        /* [in] */ IAudioSystemErrorCallback* cb);

    CARAPI GetDeviceName(
        /* [in] */ Int32 device,
        /* [out] */ String* result);

    CARAPI SetDeviceConnectionState(
        /* [in] */ Int32 device,
        /* [in] */ Int32 state,
        /* [in] */ const String& device_address);

    CARAPI GetDeviceConnectionState(
        /* [in] */ Int32 device,
        /* [in] */ const String& device_address,
        /* [out] */ Int32* result);

    CARAPI SetPhoneState(
        /* [in] */ Int32 state);

    CARAPI SetForceUse(
        /* [in] */ Int32 usage,
        /* [in] */ Int32 config);

    CARAPI GetForceUse(
        /* [in] */ Int32 usage,
        /* [out] */ Int32* result);

    CARAPI InitStreamVolume(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 indexMin,
        /* [in] */ Int32 indexMax);

    CARAPI SetStreamVolumeIndex(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 index,
        /* [in] */ Int32 device);

    CARAPI GetStreamVolumeIndex(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 device,
        /* [out] */ Int32* result);

    CARAPI SetMasterVolume(
        /* [in] */ Float value);

    CARAPI GetMasterVolume(
        /* [out] */ Float* result);

    CARAPI SetMasterMute(
        /* [in] */ Boolean mute);

    CARAPI GetMasterMute(
        /* [out] */ Boolean* result);

    CARAPI GetDevicesForStream(
        /* [in] */ Int32 stream,
        /* [out] */ Int32* result);

    // helpers for android.media.AudioManager.getProperty(), see description there for meaning
    CARAPI GetPrimaryOutputSamplingRate(
        /* [out] */ Int32* result);

    CARAPI GetPrimaryOutputFrameCount(
        /* [out] */ Int32* result);
};

} // namespace Media
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CAUDIOSYSTEMHELPER_H__
