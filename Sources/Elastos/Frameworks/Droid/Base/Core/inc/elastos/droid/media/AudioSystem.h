
#ifndef __ELASTOS_DROID_MEDIA_AUDIOSYSTEM_H__
#define __ELASTOS_DROID_MEDIA_AUDIOSYSTEM_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Media {

class AudioSystem
{
private:
    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    static CARAPI GetNumStreamTypes(
        /* [out] */ Int32* result);

    /*
     * Sets the microphone mute on or off.
     *
     * @param on set <var>true</var> to mute the microphone;
     *           <var>false</var> to turn mute off
     * @return command completion status see AUDIO_STATUS_OK, see AUDIO_STATUS_ERROR
     */
    static CARAPI MuteMicrophone(
        /* [in] */ Boolean on);

    /*
     * Checks whether the microphone mute is on or off.
     *
     * @return true if microphone is muted, false if it's not
     */
    static CARAPI IsMicrophoneMuted(
        /* [out] */ Boolean* result);

    /*
     * Checks whether the specified stream type is active.
     *
     * return true if any track playing on this stream is active.
     */
    static CARAPI IsStreamActive(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 inPastMs,
        /* [out] */ Boolean* result);

    /*
     * Checks whether the specified stream type is active on a remotely connected device. The notion
     * of what constitutes a remote device is enforced by the audio policy manager of the platform.
     *
     * return true if any track playing on this stream is active on a remote device.
     */
    static CARAPI IsStreamActiveRemotely(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 inPastMs,
        /* [out] */ Boolean * result);

    /*
     * Checks whether the specified audio source is active.
     *
     * return true if any recorder using this source is currently recording
     */
    static CARAPI IsSourceActive(
        /* [in] */ Int32 source,
        /* [out] */ Boolean* result);

    /*
     * Returns a new unused audio session ID
     */
    static CARAPI NewAudioSessionId(
        /* [out] */ Int32* result);

    /*
     * Sets a group generic audio configuration parameters. The use of these parameters
     * are platform dependent, see libaudio
     *
     * param keyValuePairs  list of parameters key value pairs in the form:
     *    key1=value1;key2=value2;...
     */
    static CARAPI SetParameters(
        /* [in] */ const String& keyValuePairs);

    /*
     * Gets a group generic audio configuration parameters. The use of these parameters
     * are platform dependent, see libaudio
     *
     * param keys  list of parameters
     * return value: list of parameters key value pairs in the form:
     *    key1=value1;key2=value2;...
     */
    static CARAPI GetParameters(
        /* [in] */ const String& keys,
        /* [out] */ String* result);

    /*
     * Registers a callback to be invoked when an error occurs.
     * @param cb the callback to run
     */
    static CARAPI SetErrorCallback(
        /* [in] */ IAudioSystemErrorCallback* cb);

    static CARAPI GetOutputDeviceName(
        /* [in] */ Int32 device,
        /* [out] */ String* result);

    static CARAPI SetDeviceConnectionState(
        /* [in] */ Int32 device,
        /* [in] */ Int32 state,
        /* [in] */ const String& device_address);

    static CARAPI GetDeviceConnectionState(
        /* [in] */ Int32 device,
        /* [in] */ const String& device_address,
        /* [out] */ Int32* result);

    static CARAPI SetPhoneState(
        /* [in] */ Int32 state,
        /* [out] */ Int32* result);

    static CARAPI SetForceUse(
        /* [in] */ Int32 usage,
        /* [in] */ Int32 config);

    static CARAPI GetForceUse(
        /* [in] */ Int32 usage,
        /* [out] */ Int32* result);

    static CARAPI InitStreamVolume(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 indexMin,
        /* [in] */ Int32 indexMax);

    static CARAPI SetStreamVolumeIndex(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 index,
        /* [in] */ Int32 device);

    static CARAPI GetStreamVolumeIndex(
        /* [in] */ Int32 stream,
        /* [in] */ Int32 device,
        /* [out] */ Int32* result);

    static CARAPI SetMasterVolume(
        /* [in] */ Float value);

    static CARAPI GetMasterVolume(
        /* [out] */ Float* result);

    static CARAPI SetMasterMute(
        /* [in] */ Boolean mute);

    static CARAPI GetMasterMute(
        /* [out] */ Boolean* result);

    static CARAPI GetDevicesForStream(
        /* [in] */ Int32 stream,
        /* [out] */ Int32* result);

    // helpers for android.media.AudioManager.getProperty(), see description there for meaning
    static CARAPI GetPrimaryOutputSamplingRate(
        /* [out] */ Int32* result);

    static CARAPI GetPrimaryOutputFrameCount(
        /* [out] */ Int32* result);

    static CARAPI GetOutputLatency(
        /* [in] */ Int32 stream,
        /* [out] */ Int32* result);

    static CARAPI SetLowRamDevice(
        /* [in] */ Boolean isLowRamDevice,
        /* [out] */ Int32* result);

    static CARAPI CheckAudioFlinger(
        /* [out] */ Int32* result);

    static CARAPI ListAudioPorts(
        /* [in] */ IArrayList* ports,
        /* [in] */ ArrayOf<Int32>* generation,
        /* [out] */ Int32* result);

    static CARAPI CreateAudioPatch(
        /* [in] */ ArrayOf<IAudioPatch*>* patch,
        /* [in] */ ArrayOf<IAudioPortConfig*>* sources,
        /* [in] */ ArrayOf<IAudioPortConfig*>* sinks,
        /* [out] */ Int32* result);

    static CARAPI ReleaseAudioPatch(
        /* [in] */ IAudioPatch* patch,
        /* [out] */ Int32* result);

    static CARAPI ListAudioPatches(
        /* [in] */ IArrayList* patches,
        /* [in] */ ArrayOf<Int32>* generation,
        /* [out] */ Int32* result);

    static CARAPI SetAudioPortConfig(
        /* [in] */ IAudioPortConfig* config,
        /* [out] */ Int32* result);

    static CARAPI GetAudioHwSyncForSession(
        /* [in] */ Int32 sessionId,
        /* [out] */ Int32* result);

    static CARAPI_(void) ErrorCallbackFromNative(
        /* [in] */ Int32 error);

    static CARAPI CheckAudioSystemCommand(
        /* [in] */ Int32 status);

private:
    // Expose only the getter method publicly so we can change it in the future
    static const Int32 NUM_STREAM_TYPES;

    static AutoPtr<IAudioSystemErrorCallback> sErrorCallback;

    static const Int32 NUM_DEVICE_STATES;

    static const Int32 NUM_FORCE_CONFIG;

    static const Int32 NUM_FORCE_USE;

    static Object sLock;
    static StaticInitializer sInitializer;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_AUDIOSYSTEM_H__
