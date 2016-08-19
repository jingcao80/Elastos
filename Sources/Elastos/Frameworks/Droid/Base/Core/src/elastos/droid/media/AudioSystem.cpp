
#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/_AudioErrors.h"
#include "elastos/droid/media/_AudioFormat.h"
#include "elastos/droid/media/AudioSystem.h"
#include "elastos/droid/media/CAudioDevicePort.h"
#include "elastos/droid/media/CAudioDevicePortConfig.h"
#include "elastos/droid/media/CAudioGain.h"
#include "elastos/droid/media/CAudioGainConfig.h"
#include "elastos/droid/media/CAudioHandle.h"
#include "elastos/droid/media/CAudioMixPort.h"
#include "elastos/droid/media/CAudioMixPortConfig.h"
#include "elastos/droid/media/CAudioPatch.h"
#include "elastos/droid/media/CAudioPort.h"
#include "elastos/droid/media/CAudioPortConfig.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <media/AudioSystem.h>
#include <system/audio.h>
#include <system/audio_policy.h>
#include <utils/Errors.h>
#include <utils/String8.h>

using Elastos::Core::AutoLock;
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

enum AudioError {
    kAudioStatusOk = 0,
    kAudioStatusError = 1,
    kAudioStatusMediaServerDied = 100
};

#define MAX_PORT_GENERATION_SYNC_ATTEMPTS 5

static int check_AudioSystem_Command(android::status_t status)
{
    switch (status) {
    case android::DEAD_OBJECT:
        return kAudioStatusMediaServerDied;
    case android::NO_ERROR:
        return kAudioStatusOk;
    default:
        break;
    }
    return kAudioStatusError;
}

ECode AudioSystem::CheckAudioSystemCommand(Int32 status)
{
    if (status == android::NO_ERROR) {
        return NOERROR;
    }
    else if (status == android::DEAD_OBJECT) {
        Logger::E("AudioSystem", "error: Media Server died.");
    }
    return E_FAIL;
}

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

    // TODO: varible error is unused, is that ok?
    int val = check_AudioSystem_Command(err);
    AudioSystem::ErrorCallbackFromNative(error);
}

AudioSystem::StaticInitializer::StaticInitializer()
{
    android::AudioSystem::setErrorCallback(elastos_media_AudioSystem_error_callback);
}

static bool useInChannelMask(audio_port_type_t type, audio_port_role_t role)
{
    return ((type == AUDIO_PORT_TYPE_DEVICE) && (role == AUDIO_PORT_ROLE_SOURCE)) ||
                ((type == AUDIO_PORT_TYPE_MIX) && (role == AUDIO_PORT_ROLE_SINK));
}

static void convertAudioGainConfigToNative(
   struct audio_gain_config *nAudioGainConfig,
   IAudioGainConfig* audioGainConfig,
   bool useInMask)
{
    audioGainConfig->Index(&nAudioGainConfig->index);
    Int32 value;
    audioGainConfig->Mode(&value);
    nAudioGainConfig->mode = value;
    ALOGV("convertAudioGainConfigToNative got gain index %d", nAudioGainConfig->index);
    Int32 mask;
    audioGainConfig->ChannelMask(&mask);

    audio_channel_mask_t nMask;
    if (useInMask) {
        nMask = InChannelMaskToNative(mask);
        ALOGV("convertAudioGainConfigToNative IN mask java %x native %x", mask, nMask);
    }
    else {
        nMask = OutChannelMaskToNative(mask);
        ALOGV("convertAudioGainConfigToNative OUT mask java %x native %x", mask, nMask);
    }
    nAudioGainConfig->channel_mask = nMask;
    audioGainConfig->RampDurationMs(&value);
    nAudioGainConfig->ramp_duration_ms = value;
    AutoPtr<ArrayOf<Int32> > values;
    audioGainConfig->Values((ArrayOf<Int32>**)&values);

    int *nValues = values->GetPayload();
    size_t size = values->GetLength();
    memcpy(nAudioGainConfig->values, nValues, size * sizeof(int));
}

static Int32 convertAudioPortConfigToNative(
    struct audio_port_config *nAudioPortConfig,
    IAudioPortConfig* audioPortConfig)
{
    AutoPtr<IAudioPort> audioPort;
    audioPortConfig->Port((IAudioPort**)&audioPort);
    AutoPtr<IAudioHandle> handle;
    audioPort->Handle((IAudioHandle**)&handle);
    ((CAudioHandle*)handle.Get())->Id(&nAudioPortConfig->id);
    Int32 role;
    audioPort->Role(&role);
    nAudioPortConfig->role = (audio_port_role_t)role;

    if (IAudioDevicePort::Probe(audioPort) != NULL) {
        nAudioPortConfig->type = AUDIO_PORT_TYPE_DEVICE;
    }
    else if (IAudioMixPort::Probe(audioPort) != NULL) {
        nAudioPortConfig->type = AUDIO_PORT_TYPE_MIX;
    }
    else {
        return IAudioSystem::ERROR;
    }

    ALOGV("convertAudioPortConfigToNative handle %d role %d type %d",
          nAudioPortConfig->id, nAudioPortConfig->role, nAudioPortConfig->type);

    Int32 value;
    audioPortConfig->SamplingRate(&value);
    nAudioPortConfig->sample_rate = value;

    bool useInMask = useInChannelMask(nAudioPortConfig->type, nAudioPortConfig->role);
    audio_channel_mask_t nMask;
    Int32 mask;
    audioPortConfig->ChannelMask(&mask);

    if (useInMask) {
        nMask = InChannelMaskToNative(mask);
        ALOGV("convertAudioPortConfigToNative IN mask java %x native %x", mask, nMask);
    }
    else {
        nMask = OutChannelMaskToNative(mask);
        ALOGV("convertAudioPortConfigToNative OUT mask java %x native %x", mask, nMask);
    }
    nAudioPortConfig->channel_mask = nMask;

    Int32 format;
    audioPortConfig->Format(&format);
    audio_format_t nFormat = AudioFormatToNative(format);
    ALOGV("convertAudioPortConfigToNative format %d native %d", format, nFormat);
    nAudioPortConfig->format = nFormat;
    AutoPtr<IAudioGainConfig> gain;
    audioPortConfig->Gain((IAudioGainConfig**)&gain);
    if (gain != NULL) {
        convertAudioGainConfigToNative(&nAudioPortConfig->gain, gain, useInMask);
    }
    else {
        ALOGV("convertAudioPortConfigToNative no gain");
        nAudioPortConfig->gain.index = -1;
    }

    nAudioPortConfig->config_mask = ((CAudioPortConfig*)audioPortConfig)->mConfigMask;

    return IAudioSystem::SUCCESS;
}

static Int32 convertAudioPortConfigFromNative(
    IAudioPort* _jAudioPort,
    struct audio_port_config *nAudioPortConfig,
    IAudioPortConfig **result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 jStatus = IAudioSystem::SUCCESS;
    AutoPtr<IAudioGainConfig> jAudioGainConfig;
    AutoPtr<IAudioGain> jAudioGain;
    AutoPtr<ArrayOf<Int32> > jGainValues;
    bool audioportCreated = false;

    ALOGV("convertAudioPortConfigFromNative jAudioPort %p", _jAudioPort);

    AutoPtr<IAudioPort> jAudioPort = _jAudioPort;
    if (jAudioPort == NULL) {
        AutoPtr<IAudioHandle> jHandle;
        CAudioHandle::New(nAudioPortConfig->id, (IAudioHandle**)&jHandle);

        ALOGV("convertAudioPortConfigFromNative handle %d is a %s", nAudioPortConfig->id,
              nAudioPortConfig->type == AUDIO_PORT_TYPE_DEVICE ? "device" : "mix");

        if (jHandle == NULL) {
            return IAudioSystem::ERROR;
        }
        // create dummy port and port config objects with just the correct handle
        // and configuration data. The actual AudioPortConfig objects will be
        // constructed by java code with correct class type (device, mix etc...)
        // and reference to AudioPort instance in this client
        CAudioPort::New(jHandle, 0, NULL, NULL, NULL, NULL, (IAudioPort**)&jAudioPort);

        if (jAudioPort == NULL) {
            return IAudioSystem::ERROR;
        }
        ALOGV("convertAudioPortConfigFromNative jAudioPort created for handle %d",
              nAudioPortConfig->id);

        audioportCreated = true;
    }

    bool useInMask = useInChannelMask(nAudioPortConfig->type, nAudioPortConfig->role);

    audio_channel_mask_t nMask;
    Int32 jMask;

    int gainIndex = nAudioPortConfig->gain.index;
    if (gainIndex >= 0) {
        ALOGV("convertAudioPortConfigFromNative gain found with index %d mode %x",
              gainIndex, nAudioPortConfig->gain.mode);
        if (audioportCreated) {
            ALOGV("convertAudioPortConfigFromNative creating gain");
            CAudioGain::New(gainIndex, 0, 0, 0, 0, 0, 0, 0, 0, (IAudioGain**)&jAudioGain);

            if (jAudioGain == NULL) {
                ALOGV("convertAudioPortConfigFromNative creating gain FAILED");
                jStatus = IAudioSystem::ERROR;
                return jStatus;
            }
        }
        else {
            ALOGV("convertAudioPortConfigFromNative reading gain from port");
            AutoPtr<ArrayOf<IAudioGain*> > jGains;
            jAudioPort->Gains((ArrayOf<IAudioGain*>**)&jGains);

            if (jGains == NULL) {
                ALOGV("convertAudioPortConfigFromNative could not get gains from port");
                jStatus = IAudioSystem::ERROR;
                return jStatus;
            }
            jAudioGain = (*jGains)[gainIndex];
            if (jAudioGain == NULL) {
                ALOGV("convertAudioPortConfigFromNative could not get gain at index %d", gainIndex);
                jStatus = IAudioSystem::ERROR;
                return jStatus;
            }
        }
        int numValues;
        if (useInMask) {
            numValues = audio_channel_count_from_in_mask(nAudioPortConfig->gain.channel_mask);
        }
        else {
            numValues = audio_channel_count_from_out_mask(nAudioPortConfig->gain.channel_mask);
        }
        jGainValues = ArrayOf<Int32>::Alloc(numValues);

        if (jGainValues == NULL) {
            ALOGV("convertAudioPortConfigFromNative could not create gain values %d", numValues);
            jStatus = IAudioSystem::ERROR;
            return jStatus;
        }

        for (Int32 i = 0; i < numValues; ++i) {
            jGainValues->Set(i, nAudioPortConfig->gain.values[i]);
        }

        nMask = nAudioPortConfig->gain.channel_mask;
        if (useInMask) {
            jMask = InChannelMaskFromNative(nMask);
            ALOGV("convertAudioPortConfigFromNative IN mask java %x native %x", jMask, nMask);
        }
        else {
            jMask = OutChannelMaskFromNative(nMask);
            ALOGV("convertAudioPortConfigFromNative OUT mask java %x native %x", jMask, nMask);
        }

        CAudioGainConfig::New(gainIndex,
                jAudioGain,
                nAudioPortConfig->gain.mode,
                jMask,
                jGainValues,
                nAudioPortConfig->gain.ramp_duration_ms,
                (IAudioGainConfig**)&jAudioGainConfig);

        if (jAudioGainConfig == NULL) {
            ALOGV("convertAudioPortConfigFromNative could not create gain config");
            jStatus = IAudioSystem::ERROR;
            return jStatus;
        }
    }

    Int32 clazz;
    if (audioportCreated) {
        clazz = 0; //gAudioPortConfigClass;
        ALOGV("convertAudioPortConfigFromNative building a generic port config");
    }
    else {
        if (IAudioDevicePort::Probe(jAudioPort) != NULL) {
            clazz = 1; //gAudioDevicePortConfigClass;
            ALOGV("convertAudioPortConfigFromNative building a device config");
        }
        else if (IAudioMixPort::Probe(jAudioPort) != NULL) {
            clazz = 2; //gAudioMixPortConfigClass;
            ALOGV("convertAudioPortConfigFromNative building a mix config");
        }
        else {
            jStatus = IAudioSystem::ERROR;
            return jStatus;
        }
    }
    nMask = nAudioPortConfig->channel_mask;
    if (useInMask) {
        jMask = InChannelMaskFromNative(nMask);
        ALOGV("convertAudioPortConfigFromNative IN mask java %x native %x", jMask, nMask);
    }
    else {
        jMask = OutChannelMaskFromNative(nMask);
        ALOGV("convertAudioPortConfigFromNative OUT mask java %x native %x", jMask, nMask);
    }

    switch (clazz) {
        case 0 : {
            AutoPtr<IAudioPortConfig> ap;
            CAudioPortConfig::New(jAudioPort,
                    nAudioPortConfig->sample_rate,
                    jMask,
                    AudioFormatFromNative(nAudioPortConfig->format),
                    jAudioGainConfig,
                    (IAudioPortConfig**)&ap);
            *result = ap;
            REFCOUNT_ADD(*result)
            break;
        }
        case 1: {
            AutoPtr<IAudioDevicePortConfig> adp;
            CAudioDevicePortConfig::New(IAudioDevicePort::Probe(jAudioPort),
                    nAudioPortConfig->sample_rate,
                    jMask,
                    AudioFormatFromNative(nAudioPortConfig->format),
                    jAudioGainConfig,
                    (IAudioDevicePortConfig**)&adp);
            *result = IAudioPortConfig::Probe(adp);
            REFCOUNT_ADD(*result)
            break;
        }

        case 2: {
            AutoPtr<IAudioMixPortConfig> amp;
            CAudioMixPortConfig::New(IAudioMixPort::Probe(jAudioPort),
                    nAudioPortConfig->sample_rate,
                    jMask,
                    AudioFormatFromNative(nAudioPortConfig->format),
                    jAudioGainConfig,
                    (IAudioMixPortConfig**)&amp);
            *result = IAudioPortConfig::Probe(amp);
            REFCOUNT_ADD(*result)
            break;
        }
    }

    if (*result == NULL) {
        ALOGV("convertAudioPortConfigFromNative could not create new port config");
        jStatus = IAudioSystem::ERROR;
    }
    else {
        ALOGV("convertAudioPortConfigFromNative OK");
    }

    return jStatus;
}

static Int32 convertAudioPortFromNative(
    struct audio_port *nAudioPort,
    IAudioPort **result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Int32 jStatus = IAudioSystem::SUCCESS;
    AutoPtr<ArrayOf<Int32> > jSamplingRates;
    AutoPtr<ArrayOf<Int32> > jChannelMasks;
    AutoPtr<ArrayOf<Int32> > jFormats;
    AutoPtr<ArrayOf<IAudioGain*> > jGains;
    AutoPtr<IAudioHandle> jHandle;
    bool useInMask;

    ALOGV("convertAudioPortFromNative id %d role %d type %d",
                                  nAudioPort->id, nAudioPort->role, nAudioPort->type);

    jSamplingRates = ArrayOf<Int32>::Alloc(nAudioPort->num_sample_rates);
    if (jSamplingRates == NULL) {
        jStatus = IAudioSystem::ERROR;
        return jStatus;
    }
    if (nAudioPort->num_sample_rates) {
        for (Int32 i = 0; i < (Int32)nAudioPort->num_sample_rates; i++) {
            jSamplingRates->Set(i, nAudioPort->sample_rates[i]);
        }
    }

    jChannelMasks = ArrayOf<Int32>::Alloc(nAudioPort->num_channel_masks);
    if (jChannelMasks == NULL) {
        jStatus = IAudioSystem::ERROR;
        return jStatus;
    }
    useInMask = useInChannelMask(nAudioPort->type, nAudioPort->role);

    Int32 jMask;
    for (size_t j = 0; j < nAudioPort->num_channel_masks; j++) {
        if (useInMask) {
            jMask = InChannelMaskFromNative(nAudioPort->channel_masks[j]);
        }
        else {
            jMask = OutChannelMaskFromNative(nAudioPort->channel_masks[j]);
        }
        jChannelMasks->Set(j, jMask);
    }

    jFormats = ArrayOf<Int32>::Alloc(nAudioPort->num_formats);
    if (jFormats == NULL) {
        jStatus = IAudioSystem::ERROR;
        return jStatus;
    }
    for (size_t j = 0; j < nAudioPort->num_formats; j++) {
        Int32 jFormat = AudioFormatFromNative(nAudioPort->formats[j]);
        jFormats->Set(j, jFormat);
    }

    jGains = ArrayOf<IAudioGain*>::Alloc(nAudioPort->num_gains);
    if (jGains == NULL) {
        jStatus = IAudioSystem::ERROR;
        return jStatus;
    }
    for (size_t j = 0; j < nAudioPort->num_gains; j++) {
        audio_channel_mask_t nMask = nAudioPort->gains[j].channel_mask;
        if (useInMask) {
            jMask = InChannelMaskFromNative(nMask);
            ALOGV("convertAudioPortConfigFromNative IN mask java %x native %x", jMask, nMask);
        }
        else {
            jMask = OutChannelMaskFromNative(nMask);
            ALOGV("convertAudioPortConfigFromNative OUT mask java %x native %x", jMask, nMask);
        }

        AutoPtr<IAudioGain> jGain;
        CAudioGain::New(j,
            nAudioPort->gains[j].mode,
            jMask,
            nAudioPort->gains[j].min_value,
            nAudioPort->gains[j].max_value,
            nAudioPort->gains[j].default_value,
            nAudioPort->gains[j].step_value,
            nAudioPort->gains[j].min_ramp_ms,
            nAudioPort->gains[j].max_ramp_ms,
            (IAudioGain**)&jGain);

        if (jGain == NULL) {
            jStatus = IAudioSystem::ERROR;
            return jStatus;
        }
        jGains->Set(j, jGain);
    }

    CAudioHandle::New(nAudioPort->id, (IAudioHandle**)&jHandle);
    if (jHandle == NULL) {
        jStatus = IAudioSystem::ERROR;
        return jStatus;
    }

    AutoPtr<IAudioPort> ap;
    if (nAudioPort->type == AUDIO_PORT_TYPE_DEVICE) {
        ALOGV("convertAudioPortFromNative is a device %08x", nAudioPort->ext.device.type);
        AutoPtr<IAudioDevicePort> adp;
        CAudioDevicePort::New(jHandle, jSamplingRates, jChannelMasks, jFormats, jGains,
                nAudioPort->ext.device.type, String(nAudioPort->ext.device.address), (IAudioDevicePort**)&adp);
        ap = IAudioPort::Probe(adp);
    }
    else if (nAudioPort->type == AUDIO_PORT_TYPE_MIX) {
        ALOGV("convertAudioPortFromNative is a mix");
        AutoPtr<IAudioMixPort> amp;
        CAudioMixPort::New(jHandle, nAudioPort->role, jSamplingRates, jChannelMasks,
                jFormats, jGains, (IAudioMixPort**)&amp);
    ap = IAudioPort::Probe(amp);
    }
    else {
        ALOGE("convertAudioPortFromNative unknown nAudioPort type %d", nAudioPort->type);
        jStatus = IAudioSystem::ERROR;
        return jStatus;
    }
    if (ap == NULL) {
        jStatus = IAudioSystem::ERROR;
        return jStatus;
    }

    AutoPtr<IAudioPortConfig> jAudioPortConfig;
    jStatus = convertAudioPortConfigFromNative(ap,
            &nAudioPort->active_config,
            (IAudioPortConfig**)&jAudioPortConfig);
    if (jStatus != IAudioSystem::SUCCESS) {
        return jStatus;
    }

    ap->SetActiveConfig(jAudioPortConfig);
    *result = ap;
    REFCOUNT_ADD(*result)
    return jStatus;
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
    ECode ec = CheckAudioSystemCommand(android::AudioSystem::muteMicrophone(on));
    if (FAILED(ec)) {
        Logger::E("AudioSystem", "failed to MuteMicrophone:%d", on);
    }
    return ec;
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
    ECode ec = CheckAudioSystemCommand(
        android::AudioSystem::setParameters(0, c_keyValuePairs8));
    if (FAILED(ec)) {
        Logger::E("AudioSystem", "failed to SetParameters: %s", keyValuePairs.string());
    }
    return ec;
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
    {    AutoLock syncLock(sLock);
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
    case IAudioSystem::DEVICE_OUT_FM_TX:
        *result = IAudioSystem::DEVICE_OUT_FM_TX_NAME;
        return NOERROR;
    case IAudioSystem::DEVICE_OUT_PROXY:
        *result = IAudioSystem::DEVICE_OUT_PROXY_NAME;
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
    ECode ec = CheckAudioSystemCommand(
        android::AudioSystem::setDeviceConnectionState(
            static_cast<audio_devices_t>(device),
            static_cast<audio_policy_dev_state_t>(state),
            c_address));
    if (FAILED(ec)) {
        Logger::E("AudioSystem", "Failed to setDeviceConnectionState:%d, state:%d, address:%s",
            device, state, c_address);
    }
    return ec;
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
    VALIDATE_NOT_NULL(result)
    *result = check_AudioSystem_Command(
        android::AudioSystem::setPhoneState((audio_mode_t)state));
    return NOERROR;
}

ECode AudioSystem::SetForceUse(
    /* [in] */ Int32 usage,
    /* [in] */ Int32 config)
{
    ECode ec = CheckAudioSystemCommand(
        android::AudioSystem::setForceUse(
            static_cast<audio_policy_force_use_t>(usage),
            static_cast<audio_policy_forced_cfg_t>(config)));
    if (FAILED(ec)) {
        Logger::E("AudioSystem", "Failed to SetForceUse: usage:%d, config:%d", usage, config);
    }
    return ec;
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
    ECode ec = CheckAudioSystemCommand(
        android::AudioSystem::initStreamVolume(
            static_cast<audio_stream_type_t>(stream),
            indexMin, indexMax));
    if (FAILED(ec)) {
        Logger::E("AudioSystem", "Failed to InitStreamVolume: stream:%08x, indexMin:%d, indexMax:%d",
            stream, indexMin, indexMax);
    }
    return ec;
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
            "Failed to SetStreamVolumeIndex stream %08x, index %d, device %d",
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
    ECode ec= CheckAudioSystemCommand(
        android::AudioSystem::setMasterVolume(value));
    if (FAILED(ec)) {
        Logger::E("AudioSystem", "Failed to setMasterVolume: %.2f", value);
    }
    return ec;
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
    ECode ec = CheckAudioSystemCommand(
        android::AudioSystem::setMasterMute(mute));
    if (FAILED(ec)) {
        Logger::E("AudioSystem", "Failed to SetMasterMute: %d", mute);
    }
    return ec;
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
    *result = (Int32)check_AudioSystem_Command(android::AudioSystem::checkAudioFlinger());
    return NOERROR;
}

ECode AudioSystem::ListAudioPorts(
    /* [in] */ IArrayList* ports,
    /* [in] */ ArrayOf<Int32>* generationArray,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    ALOGV("listAudioPorts");

    if (ports == NULL) {
        ALOGE("listAudioPorts NULL AudioPort ArrayList");
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }
    if (IArrayList::Probe(ports) != NULL) {
        ALOGE("listAudioPorts not an arraylist");
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }

    if (generationArray == NULL || generationArray->GetLength() != 1) {
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }

    android::status_t status;
    unsigned int generation1;
    unsigned int generation;
    unsigned int numPorts;
    Int32 *nGeneration;
    struct audio_port *nPorts = NULL;
    int attempts = MAX_PORT_GENERATION_SYNC_ATTEMPTS;

    // get the port count and all the ports until they both return the same generation
    do {
        if (attempts-- < 0) {
            status = android::TIMED_OUT;
            break;
        }

        numPorts = 0;
        status = android::AudioSystem::listAudioPorts(AUDIO_PORT_ROLE_NONE,
                                             AUDIO_PORT_TYPE_NONE,
                                                      &numPorts,
                                                      NULL,
                                                      &generation1);
        if (status != android::NO_ERROR || numPorts == 0) {
            ALOGE_IF(status != android::NO_ERROR, "AudioSystem::listAudioPorts error %d", status);
            break;
        }
        nPorts = (struct audio_port *)realloc(nPorts, numPorts * sizeof(struct audio_port));

        status = android::AudioSystem::listAudioPorts(AUDIO_PORT_ROLE_NONE,
                                             AUDIO_PORT_TYPE_NONE,
                                                      &numPorts,
                                                      nPorts,
                                                      &generation);
        ALOGV("listAudioPorts AudioSystem::listAudioPorts numPorts %d generation %d generation1 %d",
              numPorts, generation, generation1);
    } while (generation1 != generation && status == android::NO_ERROR);

    Int32 jStatus = NativeToElastosStatus(status);
    if (jStatus != IAudioSystem::SUCCESS) {
        goto exit;
    }

    nGeneration = generationArray->GetPayload();
    if (nGeneration == NULL) {
        jStatus = IAudioSystem::ERROR;
        goto exit;
    }
    nGeneration[0] = generation1;

    for (size_t i = 0; i < numPorts; i++) {
        AutoPtr<IAudioPort> audioPort;
        jStatus = convertAudioPortFromNative(&nPorts[i], (IAudioPort**)&audioPort);
        if (jStatus != IAudioSystem::SUCCESS) {
            goto exit;
        }
        ports->Add(audioPort);
    }

exit:
    free(nPorts);
    *result = jStatus;
    return NOERROR;
}

ECode AudioSystem::CreateAudioPatch(
    /* [in] */ ArrayOf<IAudioPatch*>* patches,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sources,
    /* [in] */ ArrayOf<IAudioPortConfig*>* sinks,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    android::status_t status;
    Int32 jStatus;

    ALOGV("createAudioPatch");
    if (patches == NULL || sources == NULL || sinks == NULL) {
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }

    if (patches->GetLength() != 1) {
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }
    Int32 numSources = sources->GetLength();
    if (numSources == 0 || numSources > AUDIO_PATCH_PORTS_MAX) {
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }

    Int32 numSinks = sinks->GetLength();
    if (numSinks == 0 || numSinks > AUDIO_PATCH_PORTS_MAX) {
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }

    audio_patch_handle_t handle = (audio_patch_handle_t)0;
    AutoPtr<IAudioPatch> jPatch = (*patches)[0];
    AutoPtr<IAudioHandle> jPatchHandle;
    if (jPatch != NULL) {
        if (IAudioPatch::Probe(jPatch) == NULL) {
            *result = IAudioSystem::BAD_VALUE;
            return NOERROR;
        }
        jPatchHandle = ((CAudioPatch*)jPatch.Get())->mHandle;
        Int32 id;
        ((CAudioHandle*)jPatchHandle.Get())->Id(&id);
        handle = (audio_patch_handle_t)id;
    }

    struct audio_patch nPatch;

    nPatch.id = handle;
    nPatch.num_sources = 0;
    nPatch.num_sinks = 0;
    AutoPtr<IAudioPortConfig> jSource;
    AutoPtr<IAudioPortConfig> jSink;

    for (Int32 i = 0; i < numSources; i++) {
        jSource = (*sources)[i];
        if (IAudioPortConfig::Probe(jSource) == NULL) {
            jStatus = IAudioSystem::BAD_VALUE;
            *result = jStatus;
            return NOERROR;
        }
        jStatus = convertAudioPortConfigToNative(&nPatch.sources[i], jSource);
        if (jStatus != IAudioSystem::SUCCESS) {
            *result = jStatus;
            return NOERROR;
        }
        nPatch.num_sources++;
    }

    for (Int32 i = 0; i < numSinks; i++) {
        jSink = (*sinks)[i];
        if (IAudioPortConfig::Probe(jSink) == NULL) {
            jStatus = IAudioSystem::BAD_VALUE;
            *result = jStatus;
            return NOERROR;
        }
        jStatus = convertAudioPortConfigToNative(&nPatch.sinks[i], jSink);
        if (jStatus != IAudioSystem::SUCCESS) {
            *result = jStatus;
            return NOERROR;
        }
        nPatch.num_sinks++;
    }

    ALOGV("AudioSystem::createAudioPatch");
    status = android::AudioSystem::createAudioPatch(&nPatch, &handle);
    ALOGV("AudioSystem::createAudioPatch() returned %d hande %d", status, handle);

    jStatus = NativeToElastosStatus(status);
    if (jStatus != IAudioSystem::SUCCESS) {
        *result = jStatus;
        return NOERROR;
    }

    if (jPatchHandle == NULL) {
        CAudioHandle::New(handle, (IAudioHandle**)&jPatchHandle);

        if (jPatchHandle == NULL) {
            jStatus = IAudioSystem::ERROR;
            *result = jStatus;
            return NOERROR;
        }

        CAudioPatch::New(jPatchHandle, sources, sinks, (IAudioPatch**)&jPatch);
        if (jPatch == NULL) {
            jStatus = IAudioSystem::ERROR;
            *result = jStatus;
            return NOERROR;
        }
        patches->Set(0, jPatch);
    }
    else {
        ((CAudioHandle*)jPatchHandle.Get())->mId = handle;
    }

    *result = jStatus;
    return NOERROR;
}

ECode AudioSystem::ReleaseAudioPatch(
    /* [in] */ IAudioPatch* patch,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    ALOGV("releaseAudioPatch");
    if (patch == NULL) {
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }

    audio_patch_handle_t handle = (audio_patch_handle_t)0;
    AutoPtr<IAudioHandle> patchHandle = ((CAudioPatch*)patch)->mHandle;
    Int32 id;
    ((CAudioHandle*)patchHandle.Get())->Id(&id);
    handle = (audio_patch_handle_t)id;

    ALOGV("AudioSystem::releaseAudioPatch");
    android::status_t status = android::AudioSystem::releaseAudioPatch(handle);
    ALOGV("AudioSystem::releaseAudioPatch() returned %d", status);
    Int32 jStatus = NativeToElastosStatus(status);
    *result = jStatus;
    return NOERROR;
}

ECode AudioSystem::ListAudioPatches(
    /* [in] */ IArrayList* patches,
    /* [in] */ ArrayOf<Int32>* jGeneration,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    ALOGV("listAudioPatches");
    if (patches == NULL) {
        ALOGE("listAudioPatches NULL AudioPatch ArrayList");
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }
    if (IArrayList::Probe(patches) == NULL) {
        ALOGE("listAudioPatches not an arraylist");
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }

    if (jGeneration == NULL || jGeneration->GetLength() != 1) {
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }

    android::status_t status;
    unsigned int generation1;
    unsigned int generation;
    unsigned int numPatches;
    Int32 *nGeneration;
    struct audio_patch *nPatches = NULL;
    AutoPtr<ArrayOf<IAudioPortConfig*> > jSources;
    AutoPtr<IAudioPortConfig> jSource;
    AutoPtr<ArrayOf<IAudioPortConfig*> > jSinks;
    AutoPtr<IAudioPortConfig> jSink;
    AutoPtr<IAudioPatch> jPatch;
    int attempts = MAX_PORT_GENERATION_SYNC_ATTEMPTS;

    // get the patch count and all the patches until they both return the same generation
    do {
        if (attempts-- < 0) {
            status = android::TIMED_OUT;
            break;
        }

        numPatches = 0;
        status = android::AudioSystem::listAudioPatches(&numPatches,
                                               NULL,
                                               &generation1);
        if (status != android::NO_ERROR || numPatches == 0) {
            ALOGE_IF(status != android::NO_ERROR, "listAudioPatches AudioSystem::listAudioPatches error %d",
                                      status);
            break;
        }
        nPatches = (struct audio_patch *)realloc(nPatches, numPatches * sizeof(struct audio_patch));

        status = android::AudioSystem::listAudioPatches(&numPatches,
                                               nPatches,
                                               &generation);
        ALOGV("listAudioPatches AudioSystem::listAudioPatches numPatches %d generation %d generation1 %d",
              numPatches, generation, generation1);

    } while (generation1 != generation && status == android::NO_ERROR);

    Int32 jStatus = NativeToElastosStatus(status);
    if (jStatus != IAudioSystem::SUCCESS) {
        goto exit;
    }

    nGeneration = jGeneration->GetPayload();
    if (nGeneration == NULL) {
        jStatus = IAudioSystem::ERROR;
        goto exit;
    }
    nGeneration[0] = generation1;

    for (size_t i = 0; i < numPatches; i++) {
        AutoPtr<IAudioHandle> patchHandle;
        CAudioHandle::New(nPatches[i].id, (IAudioHandle**)&patchHandle);
        if (patchHandle == NULL) {
            jStatus = IAudioSystem::ERROR;
            goto exit;
        }
        ALOGV("listAudioPatches patch %d num_sources %d num_sinks %d",
              i, nPatches[i].num_sources, nPatches[i].num_sinks);

        ((CAudioHandle*)patchHandle.Get())->mId = nPatches[i].id;

        // load sources
        jSources = ArrayOf<IAudioPortConfig*>::Alloc(nPatches[i].num_sources);
        if (jSources == NULL) {
            jStatus = IAudioSystem::ERROR;
            goto exit;
        }

        for (size_t j = 0; j < nPatches[i].num_sources; j++) {
            jStatus = convertAudioPortConfigFromNative(NULL,
                                                      &nPatches[i].sources[j],
                                                      (IAudioPortConfig**)&jSource);
            if (jStatus != IAudioSystem::SUCCESS) {
                goto exit;
            }
            jSources->Set(i, jSource);
            ALOGV("listAudioPatches patch %d source %d is a %s handle %d",
                  i, j,
                  nPatches[i].sources[j].type == AUDIO_PORT_TYPE_DEVICE ? "device" : "mix",
                  nPatches[i].sources[j].id);
        }
        // load sinks
        jSinks = ArrayOf<IAudioPortConfig*>::Alloc(nPatches[i].num_sinks);
        if (jSinks == NULL) {
            jStatus = IAudioSystem::ERROR;
            goto exit;
        }

        for (size_t j = 0; j < nPatches[i].num_sinks; j++) {
            jStatus = convertAudioPortConfigFromNative(NULL,
                                                      &nPatches[i].sinks[j],
                                                      (IAudioPortConfig**)&jSink);

            if (jStatus != IAudioSystem::SUCCESS) {
                goto exit;
            }
            jSinks->Set(j, jSink);
            ALOGV("listAudioPatches patch %d sink %d is a %s handle %d",
                  i, j,
                  nPatches[i].sinks[j].type == AUDIO_PORT_TYPE_DEVICE ? "device" : "mix",
                  nPatches[i].sinks[j].id);
        }

        CAudioPatch::New(patchHandle, jSources, jSinks, (IAudioPatch**)&jPatch);
        if (jPatch == NULL) {
            jStatus = IAudioSystem::ERROR;
            goto exit;
        }
        patches->Add(jPatch);
    }

exit:
    free(nPatches);
    *result = jStatus;
    return NOERROR;
}

ECode AudioSystem::SetAudioPortConfig(
    /* [in] */ IAudioPortConfig* config,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    ALOGV("setAudioPortConfig");
    if (config == NULL) {
        *result = IAudioSystem::BAD_VALUE;
        return NOERROR;
    }
    struct audio_port_config nAudioPortConfig;
    Int32 jStatus = convertAudioPortConfigToNative(&nAudioPortConfig, config);
    if (jStatus != IAudioSystem::SUCCESS) {
        *result = jStatus;
        return NOERROR;
    }
    android::status_t status = android::AudioSystem::setAudioPortConfig(&nAudioPortConfig);
    ALOGV("AudioSystem::setAudioPortConfig() returned %d", status);
    jStatus = NativeToElastosStatus(status);
    *result = jStatus;
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
    {
        AutoLock syncLock(sLock);
        if (sErrorCallback != NULL) {
            errorCallback = sErrorCallback;
        }
    }
    if (errorCallback != NULL) {
        errorCallback->OnError(error);
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
