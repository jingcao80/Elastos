
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/AudioSystem.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>
#include <media/AudioSystem.h>
#include <utils/String8.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

static const Boolean DBG = TRUE;

const Int32 AudioSystem::NUM_STREAM_TYPES = 10;
AutoPtr<IAudioSystemErrorCallback> AudioSystem::sErrorCallback;
const Int32 AudioSystem::NUM_DEVICE_STATES = 1;
const Int32 AudioSystem::NUM_FORCE_CONFIG = 13;
const Int32 AudioSystem::NUM_FORCE_USE = 6;
Object AudioSystem::sLock;
AudioSystem::StaticInitializer AudioSystem::sInitializer;

static void elastos_media_AudioSystem_error_callback(android::status_t err)
{
    Int32 error = IAudioSystem::AUDIO_STATUS_ERROR;
    switch (err) {
        case android::DEAD_OBJECT:
            error = IAudioSystem::AUDIO_STATUS_SERVER_DIED;
            break;
        case android::NO_ERROR:
            error = IAudioSystem::AUDIO_STATUS_OK;
            break;
        default:
            error = IAudioSystem::AUDIO_STATUS_ERROR;
            break;
    }

    AudioSystem::ErrorCallbackFromNative(error);
}

AudioSystem::StaticInitializer::StaticInitializer()
{
    android::AudioSystem::setErrorCallback(elastos_media_AudioSystem_error_callback);
}

ECode AudioSystem::GetNumStreamTypes(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NUM_STREAM_TYPES;
    return NOERROR;
}

ECode AudioSystem::MuteMicrophone(
    /* [in] */ Boolean on)
{
    return CheckAudioSystemCommand(android::AudioSystem::muteMicrophone(on));
}

ECode AudioSystem::IsMicrophoneMuted(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    bool state = FALSE;
    android::AudioSystem::isMicrophoneMuted(&state);
    *result = (Boolean)state;
    return NOERROR;
}

ECode AudioSystem::IsStreamActive(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 inPastMs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    bool state = FALSE;
    android::AudioSystem::isStreamActive((audio_stream_type_t)stream, &state, inPastMs);
    *result = (Boolean)state;
    return NOERROR;
}

ECode AudioSystem::IsStreamActiveRemotely(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 inPastMs,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    bool state = FALSE;
    android::AudioSystem::isStreamActiveRemotely((audio_stream_type_t)stream, &state, inPastMs);
    *result = (Boolean)state;
    return NOERROR;
}

ECode AudioSystem::IsSourceActive(
    /* [in] */ Int32 source,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    bool state = FALSE;
    android::AudioSystem::isSourceActive((audio_source_t)source, &state);
    *result = (Boolean)state;
    return NOERROR;
}

ECode AudioSystem::NewAudioSessionId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = android::AudioSystem::newAudioUniqueId();
    return NOERROR;
}

ECode AudioSystem::SetParameters(
    /* [in] */ const String& keyValuePairs)
{
    const char* c_keyValuePairs = keyValuePairs.string();
    android::String8 c_keyValuePairs8;
    if (keyValuePairs) {
        c_keyValuePairs8 = android::String8(c_keyValuePairs);
    }
    return CheckAudioSystemCommand(
        android::AudioSystem::setParameters(0, c_keyValuePairs8));
}

ECode AudioSystem::GetParameters(
    /* [in] */ const String& keys,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    const char* c_keys = keys.string();
    android::String8 c_keys8;
    if (keys) {
        c_keys8 = android::String8(c_keys);
    }
    *result = android::AudioSystem::getParameters(0, c_keys8).string();
    return NOERROR;
}

ECode AudioSystem::SetErrorCallback(
    /* [in] */ IAudioSystemErrorCallback* cb)
{
    synchronized(sLock) {
        sErrorCallback = cb;
        if (cb != NULL) {
            Int32 val;
            CheckAudioFlinger(&val);
            cb->OnError(val);
        }
    }
    return NOERROR;
}

ECode AudioSystem::GetOutputDeviceName(
    /* [in] */ Int32 device,
    /* [out] */ String* result)
{
    switch(device) {
    case IAudioSystem::DEVICE_OUT_EARPIECE:
        *result = IAudioSystem::DEVICE_OUT_EARPIECE_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_SPEAKER:
        *result = IAudioSystem::DEVICE_OUT_SPEAKER_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_WIRED_HEADSET:
        *result = IAudioSystem::DEVICE_OUT_WIRED_HEADSET_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE:
        *result = IAudioSystem::DEVICE_OUT_WIRED_HEADPHONE_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO:
        *result = IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET:
        *result = IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT:
        *result = IAudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP:
        *result = IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES:
        *result = IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_HEADPHONES_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER:
        *result = IAudioSystem::DEVICE_OUT_BLUETOOTH_A2DP_SPEAKER_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_HDMI:
        *result = IAudioSystem::DEVICE_OUT_HDMI_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET:
        *result = IAudioSystem::DEVICE_OUT_ANLG_DOCK_HEADSET_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET:
        *result = IAudioSystem::DEVICE_OUT_DGTL_DOCK_HEADSET_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_USB_ACCESSORY:
        *result = IAudioSystem::DEVICE_OUT_USB_ACCESSORY_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_USB_DEVICE:
        *result = IAudioSystem::DEVICE_OUT_USB_DEVICE_NAME;
        return NOERROR;

    case IAudioSystem::DEVICE_OUT_REMOTE_SUBMIX:
        *result = IAudioSystem::DEVICE_OUT_REMOTE_SUBMIX_NAME;
        return NOERROR;
    case IAudioSystem::DEVICE_OUT_TELEPHONY_TX:
        *result = IAudioSystem::DEVICE_OUT_TELEPHONY_TX_NAME;
        return NOERROR;
    case IAudioSystem::DEVICE_OUT_LINE:
        *result = IAudioSystem::DEVICE_OUT_LINE_NAME;
        return NOERROR;
    case IAudioSystem::DEVICE_OUT_HDMI_ARC:
        *result = IAudioSystem::DEVICE_OUT_HDMI_ARC_NAME;
        return NOERROR;
    case IAudioSystem::DEVICE_OUT_SPDIF:
        *result = IAudioSystem::DEVICE_OUT_SPDIF_NAME;
        return NOERROR;
    case IAudioSystem::DEVICE_OUT_FM:
        *result = IAudioSystem::DEVICE_OUT_FM_NAME;
        return NOERROR;
    case IAudioSystem::DEVICE_OUT_AUX_LINE:
        *result = IAudioSystem::DEVICE_OUT_AUX_LINE_NAME;
        return NOERROR;
    case IAudioSystem::DEVICE_OUT_DEFAULT:
    default:
        *result = "";
        return NOERROR;
    }
}

ECode AudioSystem::SetDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ Int32 state,
    /* [in] */ const String& device_address)
{
    const char *c_address = device_address.string();
    return CheckAudioSystemCommand(
        android::AudioSystem::setDeviceConnectionState(
            static_cast<audio_devices_t>(device),
            static_cast<audio_policy_dev_state_t>(state),
            c_address));
}

ECode AudioSystem::GetDeviceConnectionState(
    /* [in] */ Int32 device,
    /* [in] */ const String& device_address,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    const char *c_address = device_address.string();
    *result = static_cast<Int32>(
        android::AudioSystem::getDeviceConnectionState(
            static_cast<audio_devices_t>(device),
            c_address));
    return NOERROR;
}

ECode AudioSystem::SetPhoneState(
    /* [in] */ Int32 state,
    /* [out] */ Int32* result)
{
    *result = CheckAudioSystemCommand(
        android::AudioSystem::setPhoneState((audio_mode_t)state));
    return NOERROR;
}

ECode AudioSystem::SetForceUse(
    /* [in] */ Int32 usage,
    /* [in] */ Int32 config)
{
    return CheckAudioSystemCommand(
        android::AudioSystem::setForceUse(
            static_cast<audio_policy_force_use_t>(usage),
            static_cast<audio_policy_forced_cfg_t>(config)));
}

ECode AudioSystem::GetForceUse(
    /* [in] */ Int32 usage,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = static_cast<Int32>(
        android::AudioSystem::getForceUse(
            static_cast<audio_policy_force_use_t>(usage)));
    return NOERROR;
}

ECode AudioSystem::InitStreamVolume(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 indexMin,
    /* [in] */ Int32 indexMax)
{
    return CheckAudioSystemCommand(
        android::AudioSystem::initStreamVolume(
            static_cast<audio_stream_type_t>(stream),
            indexMin, indexMax));
}

ECode AudioSystem::SetStreamVolumeIndex(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 index,
    /* [in] */ Int32 device)
{
    ECode ec = CheckAudioSystemCommand(
        android::AudioSystem::setStreamVolumeIndex(
            static_cast<audio_stream_type_t>(stream),
            index, (audio_devices_t)device));
    if (FAILED(ec)) {
        Logger::E("AudioSystem",
            "Failed to SetStreamVolumeIndex stream %d, index %d, device %d",
            stream, index, device);
    }

    return ec;
}

ECode AudioSystem::GetStreamVolumeIndex(
    /* [in] */ Int32 stream,
    /* [in] */ Int32 device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 index = 0;
    if (android::AudioSystem::getStreamVolumeIndex(
       static_cast<audio_stream_type_t>(stream),
       &index, (audio_devices_t)device) != android::NO_ERROR)
    {
        index = -1;
    }
    *result = index;
    return NOERROR;
}

ECode AudioSystem::SetMasterVolume(
    /* [in] */ Float value)
{
    return CheckAudioSystemCommand(
        android::AudioSystem::setMasterVolume(value));
}

ECode AudioSystem::GetMasterVolume(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);

    Float value = 0.0;
    if (android::AudioSystem::getMasterVolume(&value) != android::NO_ERROR) {
        value = -1.0;
    }
    *result = value;
    return NOERROR;
}

ECode AudioSystem::SetMasterMute(
    /* [in] */ Boolean mute)
{
    return CheckAudioSystemCommand(
        android::AudioSystem::setMasterMute(mute));
}

ECode AudioSystem::GetMasterMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    bool mute = FALSE;
    if (android::AudioSystem::getMasterMute(&mute) != android::NO_ERROR) {
        mute = FALSE;
    }
    *result = (Boolean)mute;
    return NOERROR;
}

ECode AudioSystem::GetDevicesForStream(
    /* [in] */ Int32 stream,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (Int32)android::AudioSystem::getDevicesForStream(
        static_cast<audio_stream_type_t>(stream));
    return NOERROR;
}

ECode AudioSystem::GetPrimaryOutputSamplingRate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (Int32)android::AudioSystem::getPrimaryOutputSamplingRate();
    return NOERROR;
}

ECode AudioSystem::GetPrimaryOutputFrameCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (Int32)android::AudioSystem::getPrimaryOutputFrameCount();
    return NOERROR;
}

ECode AudioSystem::GetOutputLatency(
    /* [in] */ Int32 stream,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    uint32_t afLatency;
    if (android::AudioSystem::getOutputLatency(&afLatency,
        static_cast <audio_stream_type_t>(stream)) != android::NO_ERROR) {
        afLatency = -1;
    }
    *result = (Int32)afLatency;
    return NOERROR;
}

ECode AudioSystem::SetLowRamDevice(
    /* [in] */ Boolean isLowRamDevice,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (Int32)android::AudioSystem::setLowRamDevice((bool)isLowRamDevice);
    return NOERROR;
}

ECode AudioSystem::CheckAudioFlinger(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (Int32)CheckAudioSystemCommand(android::AudioSystem::checkAudioFlinger());
    return NOERROR;
}

//TODO: Need JNI
ECode AudioSystem::ListAudioPorts(
    /* [in] */ IArrayList* ports,
    /* [in] */ ArrayOf<Int32>* generation,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
//     ALOGV("listAudioPorts");

//     if (jPorts == NULL) {
//         ALOGE("listAudioPorts NULL AudioPort ArrayList");
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }
//     if (!env->IsInstanceOf(jPorts, gArrayListClass)) {
//         ALOGE("listAudioPorts not an arraylist");
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }

//     if (jGeneration == NULL || env->GetArrayLength(jGeneration) != 1) {
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }

//     status_t status;
//     unsigned int generation1;
//     unsigned int generation;
//     unsigned int numPorts;
//     jint *nGeneration;
//     struct audio_port *nPorts = NULL;
//     int attempts = MAX_PORT_GENERATION_SYNC_ATTEMPTS;

//     // get the port count and all the ports until they both return the same generation
//     do {
//         if (attempts-- < 0) {
//             status = TIMED_OUT;
//             break;
//         }

//         numPorts = 0;
//         status = AudioSystem::listAudioPorts(AUDIO_PORT_ROLE_NONE,
//                                              AUDIO_PORT_TYPE_NONE,
//                                                       &numPorts,
//                                                       NULL,
//                                                       &generation1);
//         if (status != NO_ERROR || numPorts == 0) {
//             ALOGE_IF(status != NO_ERROR, "AudioSystem::listAudioPorts error %d", status);
//             break;
//         }
//         nPorts = (struct audio_port *)realloc(nPorts, numPorts * sizeof(struct audio_port));

//         status = AudioSystem::listAudioPorts(AUDIO_PORT_ROLE_NONE,
//                                              AUDIO_PORT_TYPE_NONE,
//                                                       &numPorts,
//                                                       nPorts,
//                                                       &generation);
//         ALOGV("listAudioPorts AudioSystem::listAudioPorts numPorts %d generation %d generation1 %d",
//               numPorts, generation, generation1);
//     } while (generation1 != generation && status == NO_ERROR);

//     jint jStatus = nativeToJavaStatus(status);
//     if (jStatus != AUDIO_JAVA_SUCCESS) {
//         goto exit;
//     }

//     nGeneration = env->GetIntArrayElements(jGeneration, NULL);
//     if (nGeneration == NULL) {
//         jStatus = (jint)AUDIO_JAVA_ERROR;
//         goto exit;
//     }
//     nGeneration[0] = generation1;
//     env->ReleaseIntArrayElements(jGeneration, nGeneration, 0);

//     for (size_t i = 0; i < numPorts; i++) {
//         jobject jAudioPort;
//         jStatus = convertAudioPortFromNative(env, &jAudioPort, &nPorts[i]);
//         if (jStatus != AUDIO_JAVA_SUCCESS) {
//             goto exit;
//         }
//         env->CallBooleanMethod(jPorts, gArrayListMethods.add, jAudioPort);
//     }

// exit:
//     free(nPorts);
//     return jStatus;
    return NOERROR;
}

ECode AudioSystem::CreateAudioPatch(
    /* [in] */ ArrayOf<IAudioPatch*>* patch,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sources,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sinks,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
//     status_t status;
//     jint jStatus;

//     ALOGV("createAudioPatch");
//     if (jPatches == NULL || jSources == NULL || jSinks == NULL) {
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }

//     if (env->GetArrayLength(jPatches) != 1) {
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }
//     jint numSources = env->GetArrayLength(jSources);
//     if (numSources == 0 || numSources > AUDIO_PATCH_PORTS_MAX) {
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }

//     jint numSinks = env->GetArrayLength(jSinks);
//     if (numSinks == 0 || numSinks > AUDIO_PATCH_PORTS_MAX) {
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }

//     audio_patch_handle_t handle = (audio_patch_handle_t)0;
//     jobject jPatch = env->GetObjectArrayElement(jPatches, 0);
//     jobject jPatchHandle = NULL;
//     if (jPatch != NULL) {
//         if (!env->IsInstanceOf(jPatch, gAudioPatchClass)) {
//             return (jint)AUDIO_JAVA_BAD_VALUE;
//         }
//         jPatchHandle = env->GetObjectField(jPatch, gAudioPatchFields.mHandle);
//         handle = (audio_patch_handle_t)env->GetIntField(jPatchHandle, gAudioHandleFields.mId);
//     }

//     struct audio_patch nPatch;

//     nPatch.id = handle;
//     nPatch.num_sources = 0;
//     nPatch.num_sinks = 0;
//     jobject jSource = NULL;
//     jobject jSink = NULL;

//     for (jint i = 0; i < numSources; i++) {
//         jSource = env->GetObjectArrayElement(jSources, i);
//         if (!env->IsInstanceOf(jSource, gAudioPortConfigClass)) {
//             jStatus = (jint)AUDIO_JAVA_BAD_VALUE;
//             goto exit;
//         }
//         jStatus = convertAudioPortConfigToNative(env, &nPatch.sources[i], jSource);
//         env->DeleteLocalRef(jSource);
//         jSource = NULL;
//         if (jStatus != AUDIO_JAVA_SUCCESS) {
//             goto exit;
//         }
//         nPatch.num_sources++;
//     }

//     for (jint i = 0; i < numSinks; i++) {
//         jSink = env->GetObjectArrayElement(jSinks, i);
//         if (!env->IsInstanceOf(jSink, gAudioPortConfigClass)) {
//             jStatus = (jint)AUDIO_JAVA_BAD_VALUE;
//             goto exit;
//         }
//         jStatus = convertAudioPortConfigToNative(env, &nPatch.sinks[i], jSink);
//         env->DeleteLocalRef(jSink);
//         jSink = NULL;
//         if (jStatus != AUDIO_JAVA_SUCCESS) {
//             goto exit;
//         }
//         nPatch.num_sinks++;
//     }

//     ALOGV("AudioSystem::createAudioPatch");
//     status = AudioSystem::createAudioPatch(&nPatch, &handle);
//     ALOGV("AudioSystem::createAudioPatch() returned %d hande %d", status, handle);

//     jStatus = nativeToJavaStatus(status);
//     if (jStatus != AUDIO_JAVA_SUCCESS) {
//         goto exit;
//     }

//     if (jPatchHandle == NULL) {
//         jPatchHandle = env->NewObject(gAudioHandleClass, gAudioHandleCstor,
//                                            handle);
//         if (jPatchHandle == NULL) {
//             jStatus = (jint)AUDIO_JAVA_ERROR;
//             goto exit;
//         }
//         jPatch = env->NewObject(gAudioPatchClass, gAudioPatchCstor, jPatchHandle, jSources, jSinks);
//         if (jPatch == NULL) {
//             jStatus = (jint)AUDIO_JAVA_ERROR;
//             goto exit;
//         }
//         env->SetObjectArrayElement(jPatches, 0, jPatch);
//     } else {
//         env->SetIntField(jPatchHandle, gAudioHandleFields.mId, handle);
//     }

// exit:
//     if (jPatchHandle != NULL) {
//         env->DeleteLocalRef(jPatchHandle);
//     }
//     if (jPatch != NULL) {
//         env->DeleteLocalRef(jPatch);
//     }
//     if (jSource != NULL) {
//         env->DeleteLocalRef(jSource);
//     }
//     if (jSink != NULL) {
//         env->DeleteLocalRef(jSink);
//     }
//     return jStatus;
    return NOERROR;
}

ECode AudioSystem::ReleaseAudioPatch(
    /* [in] */ IAudioPatch* patch,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // ALOGV("releaseAudioPatch");
    // if (jPatch == NULL) {
    //     return (jint)AUDIO_JAVA_BAD_VALUE;
    // }

    // audio_patch_handle_t handle = (audio_patch_handle_t)0;
    // jobject jPatchHandle = NULL;
    // if (!env->IsInstanceOf(jPatch, gAudioPatchClass)) {
    //     return (jint)AUDIO_JAVA_BAD_VALUE;
    // }
    // jPatchHandle = env->GetObjectField(jPatch, gAudioPatchFields.mHandle);
    // handle = (audio_patch_handle_t)env->GetIntField(jPatchHandle, gAudioHandleFields.mId);
    // env->DeleteLocalRef(jPatchHandle);

    // ALOGV("AudioSystem::releaseAudioPatch");
    // status_t status = AudioSystem::releaseAudioPatch(handle);
    // ALOGV("AudioSystem::releaseAudioPatch() returned %d", status);
    // jint jStatus = nativeToJavaStatus(status);
    // return status;
    return NOERROR;
}

ECode AudioSystem::ListAudioPatches(
    /* [in] */ IArrayList* patches,
    /* [in] */ ArrayOf<Int32>* generation,
    /* [out] */ Int32* result)
{
//     VALIDATE_NOT_NULL(result)
//     ALOGV("listAudioPatches");
//     if (jPatches == NULL) {
//         ALOGE("listAudioPatches NULL AudioPatch ArrayList");
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }
//     if (!env->IsInstanceOf(jPatches, gArrayListClass)) {
//         ALOGE("listAudioPatches not an arraylist");
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }

//     if (jGeneration == NULL || env->GetArrayLength(jGeneration) != 1) {
//         return (jint)AUDIO_JAVA_BAD_VALUE;
//     }

//     status_t status;
//     unsigned int generation1;
//     unsigned int generation;
//     unsigned int numPatches;
//     jint *nGeneration;
//     struct audio_patch *nPatches = NULL;
//     jobjectArray jSources = NULL;
//     jobject jSource = NULL;
//     jobjectArray jSinks = NULL;
//     jobject jSink = NULL;
//     jobject jPatch = NULL;
//     int attempts = MAX_PORT_GENERATION_SYNC_ATTEMPTS;

//     // get the patch count and all the patches until they both return the same generation
//     do {
//         if (attempts-- < 0) {
//             status = TIMED_OUT;
//             break;
//         }

//         numPatches = 0;
//         status = AudioSystem::listAudioPatches(&numPatches,
//                                                NULL,
//                                                &generation1);
//         if (status != NO_ERROR || numPatches == 0) {
//             ALOGE_IF(status != NO_ERROR, "listAudioPatches AudioSystem::listAudioPatches error %d",
//                                       status);
//             break;
//         }
//         nPatches = (struct audio_patch *)realloc(nPatches, numPatches * sizeof(struct audio_patch));

//         status = AudioSystem::listAudioPatches(&numPatches,
//                                                nPatches,
//                                                &generation);
//         ALOGV("listAudioPatches AudioSystem::listAudioPatches numPatches %d generation %d generation1 %d",
//               numPatches, generation, generation1);

//     } while (generation1 != generation && status == NO_ERROR);

//     jint jStatus = nativeToJavaStatus(status);
//     if (jStatus != AUDIO_JAVA_SUCCESS) {
//         goto exit;
//     }

//     nGeneration = env->GetIntArrayElements(jGeneration, NULL);
//     if (nGeneration == NULL) {
//         jStatus = AUDIO_JAVA_ERROR;
//         goto exit;
//     }
//     nGeneration[0] = generation1;
//     env->ReleaseIntArrayElements(jGeneration, nGeneration, 0);

//     for (size_t i = 0; i < numPatches; i++) {
//         jobject patchHandle = env->NewObject(gAudioHandleClass, gAudioHandleCstor,
//                                                  nPatches[i].id);
//         if (patchHandle == NULL) {
//             jStatus = AUDIO_JAVA_ERROR;
//             goto exit;
//         }
//         ALOGV("listAudioPatches patch %d num_sources %d num_sinks %d",
//               i, nPatches[i].num_sources, nPatches[i].num_sinks);

//         env->SetIntField(patchHandle, gAudioHandleFields.mId, nPatches[i].id);

//         // load sources
//         jSources = env->NewObjectArray(nPatches[i].num_sources,
//                                        gAudioPortConfigClass, NULL);
//         if (jSources == NULL) {
//             jStatus = AUDIO_JAVA_ERROR;
//             goto exit;
//         }

//         for (size_t j = 0; j < nPatches[i].num_sources; j++) {
//             jStatus = convertAudioPortConfigFromNative(env,
//                                                       NULL,
//                                                       &jSource,
//                                                       &nPatches[i].sources[j]);
//             if (jStatus != AUDIO_JAVA_SUCCESS) {
//                 goto exit;
//             }
//             env->SetObjectArrayElement(jSources, j, jSource);
//             env->DeleteLocalRef(jSource);
//             jSource = NULL;
//             ALOGV("listAudioPatches patch %d source %d is a %s handle %d",
//                   i, j,
//                   nPatches[i].sources[j].type == AUDIO_PORT_TYPE_DEVICE ? "device" : "mix",
//                   nPatches[i].sources[j].id);
//         }
//         // load sinks
//         jSinks = env->NewObjectArray(nPatches[i].num_sinks,
//                                      gAudioPortConfigClass, NULL);
//         if (jSinks == NULL) {
//             jStatus = AUDIO_JAVA_ERROR;
//             goto exit;
//         }

//         for (size_t j = 0; j < nPatches[i].num_sinks; j++) {
//             jStatus = convertAudioPortConfigFromNative(env,
//                                                       NULL,
//                                                       &jSink,
//                                                       &nPatches[i].sinks[j]);

//             if (jStatus != AUDIO_JAVA_SUCCESS) {
//                 goto exit;
//             }
//             env->SetObjectArrayElement(jSinks, j, jSink);
//             env->DeleteLocalRef(jSink);
//             jSink = NULL;
//             ALOGV("listAudioPatches patch %d sink %d is a %s handle %d",
//                   i, j,
//                   nPatches[i].sinks[j].type == AUDIO_PORT_TYPE_DEVICE ? "device" : "mix",
//                   nPatches[i].sinks[j].id);
//         }

//         jPatch = env->NewObject(gAudioPatchClass, gAudioPatchCstor,
//                                        patchHandle, jSources, jSinks);
//         env->DeleteLocalRef(jSources);
//         jSources = NULL;
//         env->DeleteLocalRef(jSinks);
//         jSinks = NULL;
//         if (jPatch == NULL) {
//             jStatus = AUDIO_JAVA_ERROR;
//             goto exit;
//         }
//         env->CallBooleanMethod(jPatches, gArrayListMethods.add, jPatch);
//         env->DeleteLocalRef(jPatch);
//         jPatch = NULL;
//     }

// exit:
//     if (jSources != NULL) {
//         env->DeleteLocalRef(jSources);
//     }
//     if (jSource != NULL) {
//         env->DeleteLocalRef(jSource);
//     }
//     if (jSinks != NULL) {
//         env->DeleteLocalRef(jSinks);
//     }
//     if (jSink != NULL) {
//         env->DeleteLocalRef(jSink);
//     }
//     if (jPatch != NULL) {
//         env->DeleteLocalRef(jPatch);
//     }
//     free(nPatches);
//     return jStatus;
    return NOERROR;
}

ECode AudioSystem::SetAudioPortConfig(
    /* [in] */ IAudioPortConfig* config,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // ALOGV("setAudioPortConfig");
    // if (jAudioPortConfig == NULL) {
    //     return AUDIO_JAVA_BAD_VALUE;
    // }
    // if (!env->IsInstanceOf(jAudioPortConfig, gAudioPortConfigClass)) {
    //     return AUDIO_JAVA_BAD_VALUE;
    // }
    // struct audio_port_config nAudioPortConfig;
    // jint jStatus = convertAudioPortConfigToNative(env, &nAudioPortConfig, jAudioPortConfig);
    // if (jStatus != AUDIO_JAVA_SUCCESS) {
    //     return jStatus;
    // }
    // status_t status = AudioSystem::setAudioPortConfig(&nAudioPortConfig);
    // ALOGV("AudioSystem::setAudioPortConfig() returned %d", status);
    // jStatus = nativeToJavaStatus(status);
    // return jStatus;
    return NOERROR;
}

ECode AudioSystem::GetAudioHwSyncForSession(
    /* [in] */ Int32 sessionId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (Int32)android::AudioSystem::getAudioHwSyncForSession((audio_session_t)sessionId);
    return NOERROR;
}

void AudioSystem::ErrorCallbackFromNative(
    /* [in] */ Int32 error)
{
    if (DBG) Logger::E("AudioSystem", "ErrorCallbackFromNative error: %d", error);
    AutoPtr<IAudioSystemErrorCallback> errorCallback;
    synchronized(sLock) {
        if (sErrorCallback != NULL) {
            errorCallback = sErrorCallback;
        }
    }
    if (errorCallback != NULL) {
        errorCallback->OnError(error);
    }
}

ECode AudioSystem::CheckAudioSystemCommand(Int32 status)
{
    if (status == android::NO_ERROR) {
        return NOERROR;
    } else {
        Logger::E("AudioSystem", "AudioSystem operator failed status %d", status);
        return E_FAIL;
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
