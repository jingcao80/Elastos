
#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/tv/TvInputHardwareManager.h"
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/core/StringUtils.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/os/ServiceManager.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::CInteger32;
using Elastos::Core::CObject;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Droid::Hardware::Hdmi::EIID_IIHdmiDeviceEventListener;
using Elastos::Droid::Hardware::Hdmi::EIID_IIHdmiHotplugEventListener;
using Elastos::Droid::Hardware::Hdmi::EIID_IIHdmiSystemAudioModeChangeListener;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
using Elastos::Droid::Internal::Os::CSomeArgsHelper;
using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Os::ISomeArgsHelper;
using Elastos::Droid::Media::EIID_IAudioManagerOnAudioPortUpdateListener;
using Elastos::Droid::Media::IAudioDevicePortConfig;
using Elastos::Droid::Media::IAudioFormat;
using Elastos::Droid::Media::IAudioGain;
using Elastos::Droid::Media::IAudioGainConfig;
using Elastos::Droid::Media::IAudioPortConfig;
using Elastos::Droid::Media::Tv::EIID_IITvInputHardware;
using Elastos::Droid::Media::Tv::ITvInputManager;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Server::SystemService;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CSparseBooleanArray;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Tv {

//=============================================================================
// TvInputHardwareManager::Connection
//=============================================================================
CAR_INTERFACE_IMPL(TvInputHardwareManager::Connection, Object, IProxyDeathRecipient)

TvInputHardwareManager::Connection::Connection(
    /* [in] */ TvInputHardwareManager* host)
    : mHost(host)
{}

ECode TvInputHardwareManager::Connection::constructor(
    /* [in] */ ITvInputHardwareInfo* hardwareInfo)
{
    mHardwareInfo = hardwareInfo;
    return NOERROR;
}

ECode TvInputHardwareManager::Connection::ResetLocked(
    /* [in] */ TvInputHardwareImpl* hardware,
    /* [in] */ IITvInputHardwareCallback* callback,
    /* [in] */ ITvInputInfo* info,
    /* [in] */ IInteger32* callingUid,
    /* [in] */ IInteger32* resolvedUserId)
{
    if (mHardware != NULL) {
        // try {
        ECode ec = mCallback->OnReleased();
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in Connection::resetLocked %d", ec);
            }
            else
                return ec;
        }
        // }
        mHardware->Release();
    }
    mHardware = hardware;
    mCallback = callback;
    mInfo = info;
    mCallingUid = callingUid;
    mResolvedUserId = resolvedUserId;
    mOnFirstFrameCaptured = NULL;
    if (mHardware != NULL && mCallback != NULL) {
        // try {
        ECode ec;
        AutoPtr<ArrayOf<ITvStreamConfig*> > config;
        do {
            ec = GetConfigsLocked((ArrayOf<ITvStreamConfig*>**)&config);
            if (FAILED(ec)) break;
            ec = mCallback->OnStreamConfigChanged(config);
        } while(FALSE);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in Connection::resetLocked %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputHardwareManager::Connection::UpdateConfigsLocked(
    /* [in] */ ArrayOf<ITvStreamConfig*>* configs)
{
    mConfigs = configs;
    return NOERROR;
}

ECode TvInputHardwareManager::Connection::GetHardwareInfoLocked(
    /* [out] */ ITvInputHardwareInfo** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mHardwareInfo)
}

ECode TvInputHardwareManager::Connection::GetInfoLocked(
    /* [out] */ ITvInputInfo** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mInfo)
}

ECode TvInputHardwareManager::Connection::GetHardwareLocked(
    /* [out] */ IITvInputHardware** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mHardware)
}

ECode TvInputHardwareManager::Connection::GetHardwareImplLocked(
    /* [out] */ TvInputHardwareManager::TvInputHardwareImpl** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mHardware)
}

ECode TvInputHardwareManager::Connection::GetCallbackLocked(
    /* [out] */ IITvInputHardwareCallback** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mCallback)
}

ECode TvInputHardwareManager::Connection::GetConfigsLocked(
    /* [out, callee] */ ArrayOf<ITvStreamConfig*>** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mConfigs)
}

ECode TvInputHardwareManager::Connection::GetCallingUidLocked(
    /* [out] */ IInteger32** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mCallingUid)
}

ECode TvInputHardwareManager::Connection::GetResolvedUserIdLocked(
    /* [out] */ IInteger32** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mResolvedUserId)
}

ECode TvInputHardwareManager::Connection::SetOnFirstFrameCapturedLocked(
    /* [in] */ IRunnable* runnable)
{
    mOnFirstFrameCaptured = runnable;
    return NOERROR;
}

ECode TvInputHardwareManager::Connection::GetOnFirstFrameCapturedLocked(
    /* [out] */ IRunnable** result)
{
    VALIDATE_NOT_NULL(result)

    FUNC_RETURN(mOnFirstFrameCaptured)
}

ECode TvInputHardwareManager::Connection::ProxyDied()
{
    synchronized(mHost->mLock) {
        return ResetLocked(NULL, NULL, NULL, NULL, NULL);
    }
    return NOERROR;
}

//=============================================================================
// TvInputHardwareManager::TvInputHardwareImpl::InnerSub_AudioManagerOnAudioPortUpdateListener
//=============================================================================
CAR_INTERFACE_IMPL(TvInputHardwareManager::TvInputHardwareImpl::InnerSub_AudioManagerOnAudioPortUpdateListener, Object, IAudioManagerOnAudioPortUpdateListener)

TvInputHardwareManager::TvInputHardwareImpl::InnerSub_AudioManagerOnAudioPortUpdateListener::InnerSub_AudioManagerOnAudioPortUpdateListener(
    /* [in] */ TvInputHardwareImpl* host)
    : mHost(host)
{}

ECode TvInputHardwareManager::TvInputHardwareImpl::InnerSub_AudioManagerOnAudioPortUpdateListener::OnAudioPortListUpdate(
    /* [in] */ ArrayOf<IAudioPort*>* portList)
{
    synchronized(mHost->mImplLock) {
        return mHost->UpdateAudioConfigLocked();
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::InnerSub_AudioManagerOnAudioPortUpdateListener::OnAudioPatchListUpdate(
    /* [in] */ ArrayOf<IAudioPatch*>* patchList)
{
    // No-op
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::InnerSub_AudioManagerOnAudioPortUpdateListener::OnServiceDied()
{
    synchronized(mHost->mImplLock) {
        mHost->mAudioSource = NULL;
        mHost->mAudioSink = NULL;
        mHost->mAudioPatch = NULL;
    }
    return NOERROR;
}

//=============================================================================
// TvInputHardwareManager::TvInputHardwareImpl
//=============================================================================
CAR_INTERFACE_IMPL_2(TvInputHardwareManager::TvInputHardwareImpl, Object, IITvInputHardware, IBinder)

TvInputHardwareManager::TvInputHardwareImpl::TvInputHardwareImpl(
    /* [in] */ TvInputHardwareManager* host)
    : mHost(host)
    , mReleased(FALSE)
    , mOverrideAudioType(IAudioManager::DEVICE_NONE)
    , mOverrideAudioAddress("")
    , mCommittedVolume(0.0f)
    , mVolume(0.0f)
    , mDesiredSamplingRate(0)
    , mDesiredChannelMask(IAudioFormat::CHANNEL_OUT_DEFAULT)
    , mDesiredFormat(IAudioFormat::ENCODING_DEFAULT)
{
    Elastos::Core::CObject::New((IObject**)&mImplLock);
    mAudioListener = new InnerSub_AudioManagerOnAudioPortUpdateListener(this);
}

ECode TvInputHardwareManager::TvInputHardwareImpl::constructor(
    /* [in] */ ITvInputHardwareInfo* info)
{
    mInfo = info;
    mHost->mAudioManager->RegisterAudioPortUpdateListener(mAudioListener);
    if (Ptr(mInfo)->Func(mInfo->GetAudioType) != IAudioManager::DEVICE_NONE) {
        FindAudioDevicePort(Ptr(mInfo)->Func(mInfo->GetAudioType), Ptr(mInfo)->Func(mInfo->GetAudioAddress), (IAudioDevicePort**)&mAudioSource);
        FindAudioSinkFromAudioPolicy((IAudioDevicePort**)&mAudioSink);
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::FindAudioSinkFromAudioPolicy(
    /* [out] */ IAudioDevicePort** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> devicePorts;
    CArrayList::New((IArrayList**)&devicePorts);
    Int32 status;
    mHost->mAudioManager->ListAudioDevicePorts(devicePorts, &status);
    if (status == IAudioManager::SUCCESS) {
        Int32 sinkDevice;
        mHost->mAudioManager->GetDevicesForStream(IAudioManager::STREAM_MUSIC, &sinkDevice);
        FOR_EACH(iter, devicePorts) {
            AutoPtr<IAudioPort> port = IAudioPort::Probe(Ptr(iter)->Func(iter->GetNext));
            AutoPtr<IAudioDevicePort> devicePort = IAudioDevicePort::Probe(port);
            if ((Ptr(devicePort)->Func(devicePort->Type) & sinkDevice) != 0) {
                FUNC_RETURN(devicePort)
            }
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::FindAudioDevicePort(
    /* [in] */ Int32 type,
    /* [in] */ const String& address,
    /* [out] */ IAudioDevicePort** result)
{
    VALIDATE_NOT_NULL(result)

    if (type == IAudioManager::DEVICE_NONE) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IArrayList> devicePorts;
    CArrayList::New((IArrayList**)&devicePorts);
    Int32 status;
    mHost->mAudioManager->ListAudioDevicePorts(devicePorts, &status);
    if (status != IAudioManager::SUCCESS) {
        *result = NULL;
        return NOERROR;
    }
    FOR_EACH(iter, devicePorts) {
        AutoPtr<IAudioPort> port = IAudioPort::Probe(Ptr(iter)->Func(iter->GetNext));
        AutoPtr<IAudioDevicePort> devicePort = IAudioDevicePort::Probe(port);
        if (Ptr(devicePort)->Func(devicePort->Type) == type && Ptr(devicePort)->Func(devicePort->Address).Equals(address)) {
            FUNC_RETURN(devicePort)
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::ReleaseRes()
{
    synchronized(mImplLock) {
        mHost->mAudioManager->UnregisterAudioPortUpdateListener(mAudioListener);
        if (mAudioPatch != NULL) {
            Int32 iNoUse;
            mHost->mAudioManager->ReleaseAudioPatch(mAudioPatch, &iNoUse);
            mAudioPatch = NULL;
        }
        mReleased = TRUE;
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::SetSurface(
    /* [in] */ ISurface* surface,
    /* [in] */ ITvStreamConfig* config,
    /* [out] */ Boolean* rev)
{
    VALIDATE_NOT_NULL(rev)
    *rev = FALSE;

    synchronized(mImplLock) {
        if (mReleased) {
            Logger::E(TAG, "Device already released.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        if (surface != NULL && config == NULL) {
            *rev = FALSE;
            return NOERROR;
        }
        if (surface == NULL && mActiveConfig == NULL) {
            *rev = FALSE;
            return NOERROR;
        }
        Int32 result = TvInputHal::ERROR_UNKNOWN;
        if (surface == NULL) {
            mHost->mHal->RemoveStream(Ptr(mInfo)->Func(mInfo->GetDeviceId), mActiveConfig, &result);
            mActiveConfig = NULL;
        }
        else {
            if (config != mActiveConfig && mActiveConfig != NULL) {
                mHost->mHal->RemoveStream(Ptr(mInfo)->Func(mInfo->GetDeviceId), mActiveConfig, &result);
                if (result != TvInputHal::SUCCESS) {
                    mActiveConfig = NULL;
                    *rev = FALSE;
                    return NOERROR;
                }
            }
            mHost->mHal->AddStream(Ptr(mInfo)->Func(mInfo->GetDeviceId), surface, config, &result);
            if (result == TvInputHal::SUCCESS) {
                mActiveConfig = config;
            }
        }
        UpdateAudioConfigLocked();
        *rev = result == TvInputHal::SUCCESS;
        return NOERROR;
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::UpdateAudioConfigLocked()
{
    Boolean sinkUpdated;
    UpdateAudioSinkLocked(&sinkUpdated);
    Boolean sourceUpdated;
    UpdateAudioSourceLocked(&sourceUpdated);
    // We can't do updated = UpdateAudioSinkLocked() || UpdateAudioSourceLocked() here
    // because Java won't evaluate the latter if the former is true.
    if (mAudioSource == NULL || mAudioSink == NULL || mActiveConfig == NULL) {
        if (mAudioPatch != NULL) {
            Int32 iNoUse;
            mHost->mAudioManager->ReleaseAudioPatch(mAudioPatch, &iNoUse);
            mAudioPatch = NULL;
        }
        return NOERROR;
    }
    AutoPtr<IAudioGainConfig> sourceGainConfig;
    AutoPtr<ArrayOf<IAudioGain*> > gains;
    IAudioPort::Probe(mAudioSource)->Gains((ArrayOf<IAudioGain*>**)&gains);
    if (gains->GetLength() > 0 && mVolume != mCommittedVolume) {
        AutoPtr<IAudioGain> sourceGain;
        for (Int32 i = 0; i < gains->GetLength(); ++i) {
            AutoPtr<IAudioGain> gain = (*gains)[i];
            if ((Ptr(gain)->Func(gain->Mode) & IAudioGain::MODE_JOINT) != 0) {
                sourceGain = gain;
                break;
            }
        }
        // NOTE: we only change the source gain in MODE_JOINT here.
        if (sourceGain != NULL) {
            Int32 steps = (Ptr(sourceGain)->Func(sourceGain->MaxValue) - Ptr(sourceGain)->Func(sourceGain->MinValue))
                    / Ptr(sourceGain)->Func(sourceGain->StepValue);
            Int32 gainValue;
            sourceGain->MinValue(&gainValue);
            if (mVolume < 1.0f) {
                gainValue += Ptr(sourceGain)->Func(sourceGain->StepValue) * (Int32) (mVolume * steps + 0.5);
            } else {
                sourceGain->MaxValue(&gainValue);
            }
            Int32 numChannels = 0;
            for (Int32 mask = Ptr(sourceGain)->Func(sourceGain->ChannelMask); mask > 0; mask >>= 1) {
                numChannels += (mask & 1);
            }
            AutoPtr<ArrayOf<Int32> > gainValues = ArrayOf<Int32>::Alloc(numChannels);
            Arrays::Fill(gainValues, gainValue);
            sourceGain->BuildConfig(IAudioGain::MODE_JOINT,
                    Ptr(sourceGain)->Func(sourceGain->ChannelMask), gainValues, 0, (IAudioGainConfig**)&sourceGainConfig);
        } else {
            Slogger::W(TAG, "No audio source gain with MODE_JOINT support exists.");
        }
    }
    AutoPtr<IAudioPortConfig> sourceConfig;
    IAudioPort::Probe(mAudioSource)->ActiveConfig((IAudioPortConfig**)&sourceConfig);
    AutoPtr<IAudioPortConfig> sinkConfig;
    IAudioPort::Probe(mAudioSink)->ActiveConfig((IAudioPortConfig**)&sinkConfig);
    AutoPtr<ArrayOf<IAudioPatch*> > audioPatchArray = ArrayOf<IAudioPatch*>::Alloc(1);
    audioPatchArray->Set(0, mAudioPatch);
    Boolean shouldRecreateAudioPatch = sourceUpdated || sinkUpdated;
    if (sinkConfig == NULL
            || (mDesiredSamplingRate != 0
                    && Ptr(sinkConfig)->Func(sinkConfig->SamplingRate) != mDesiredSamplingRate)
            || (mDesiredChannelMask != IAudioFormat::CHANNEL_OUT_DEFAULT
                    && Ptr(sinkConfig)->Func(sinkConfig->ChannelMask) != mDesiredChannelMask)
            || (mDesiredFormat != IAudioFormat::ENCODING_DEFAULT
                    && Ptr(sinkConfig)->Func(sinkConfig->Format) != mDesiredFormat)) {
        AutoPtr<IAudioDevicePortConfig> tmp;
        mAudioSource->BuildConfig(mDesiredSamplingRate, mDesiredChannelMask,
                mDesiredFormat, NULL, (IAudioDevicePortConfig**)&tmp);
        sinkConfig = IAudioPortConfig::Probe(tmp);
        shouldRecreateAudioPatch = TRUE;
    }
    if (sourceConfig == NULL || sourceGainConfig != NULL) {
        AutoPtr<IAudioDevicePortConfig> tmp;
        mAudioSource->BuildConfig(Ptr(sinkConfig)->Func(sinkConfig->SamplingRate),
                Ptr(sinkConfig)->Func(sinkConfig->ChannelMask), Ptr(sinkConfig)->Func(sinkConfig->Format), sourceGainConfig, (IAudioDevicePortConfig**)&tmp);
        sourceConfig = IAudioPortConfig::Probe(tmp);
        shouldRecreateAudioPatch = TRUE;
    }
    if (shouldRecreateAudioPatch) {
        mCommittedVolume = mVolume;
        AutoPtr<ArrayOf<IAudioPortConfig*> > source = ArrayOf<IAudioPortConfig*>::Alloc(1);
        source->Set(0, sourceConfig);
        AutoPtr<ArrayOf<IAudioPortConfig*> > sink = ArrayOf<IAudioPortConfig*>::Alloc(1);
        sink->Set(0, sinkConfig);
        Int32 iNoUse;
        mHost->mAudioManager->CreateAudioPatch(audioPatchArray, source, sink, &iNoUse);
        mAudioPatch = (*audioPatchArray)[0];
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::SetStreamVolume(
    /* [in] */ Float volume)
{
    synchronized(mImplLock) {
        if (mReleased) {
            Logger::E(TAG, "Device already released.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        mVolume = volume;
        UpdateAudioConfigLocked();
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::DispatchKeyEventToHdmi(
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    synchronized(mImplLock) {
        if (mReleased) {
            Logger::E(TAG, "Device already released.");
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    if (Ptr(mInfo)->Func(mInfo->GetType) != ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI) {
        *result = FALSE;
        return NOERROR;
    }
    // TODO(hdmi): mHdmiClient->SendKeyEvent(event);
    *result = FALSE;
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::StartCapture(
    /* [in] */ ISurface* surface,
    /* [in] */ ITvStreamConfig* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    synchronized(mImplLock) {
        if (mReleased) {
            *result = FALSE;
            return NOERROR;
        }
        if (surface == NULL || config == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        if (Ptr(config)->Func(config->GetType) != ITvStreamConfig::STREAM_TYPE_BUFFER_PRODUCER) {
            *result = FALSE;
            return NOERROR;
        }
        Int32 status;
        mHost->mHal->AddStream(Ptr(mInfo)->Func(mInfo->GetDeviceId), surface, config, &status);
        *result = status == TvInputHal::SUCCESS;
        return NOERROR;
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::StopCapture(
    /* [in] */ ITvStreamConfig* config,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    synchronized(mImplLock) {
        if (mReleased) {
            *result = FALSE;
            return NOERROR;
        }
        if (config == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        Int32 status;
        mHost->mHal->RemoveStream(Ptr(mInfo)->Func(mInfo->GetDeviceId), config, &status);
        return status == TvInputHal::SUCCESS;
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::UpdateAudioSourceLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (Ptr(mInfo)->Func(mInfo->GetAudioType) == IAudioManager::DEVICE_NONE) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IAudioDevicePort> previousSource = mAudioSource;
    FindAudioDevicePort(Ptr(mInfo)->Func(mInfo->GetAudioType), Ptr(mInfo)->Func(mInfo->GetAudioAddress), (IAudioDevicePort**)&mAudioSource);
    if (mAudioSource == NULL)
        *result = previousSource != NULL;
    else {
        IObject::Probe(mAudioSource)->Equals(previousSource, result);
        *result = !(*result);
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::UpdateAudioSinkLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (Ptr(mInfo)->Func(mInfo->GetAudioType) == IAudioManager::DEVICE_NONE) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IAudioDevicePort> previousSink = mAudioSink;
    if (mOverrideAudioType == IAudioManager::DEVICE_NONE) {
        FindAudioSinkFromAudioPolicy((IAudioDevicePort**)&mAudioSink);
    } else {
        AutoPtr<IAudioDevicePort> audioSink;
        FindAudioDevicePort(mOverrideAudioType, mOverrideAudioAddress, (IAudioDevicePort**)&audioSink);
        if (audioSink != NULL) {
            mAudioSink = audioSink;
        }
    }
    if (mAudioSink == NULL)
        *result = previousSink != NULL;
    else
    {
        IObject::Probe(mAudioSink)->Equals(previousSink, result);
        *result = !(*result);
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::HandleAudioSinkUpdated()
{
    synchronized(mImplLock) {
        UpdateAudioConfigLocked();
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::OverrideAudioSink(
    /* [in] */ Int32 audioType,
    /* [in] */ const String& audioAddress,
    /* [in] */ Int32 samplingRate,
    /* [in] */ Int32 channelMask,
    /* [in] */ Int32 format)
{
    synchronized(mImplLock) {
        mOverrideAudioType = audioType;
        mOverrideAudioAddress = audioAddress;
        mDesiredSamplingRate = samplingRate;
        mDesiredChannelMask = channelMask;
        mDesiredFormat = format;
        UpdateAudioConfigLocked();
    }
    return NOERROR;
}

ECode TvInputHardwareManager::TvInputHardwareImpl::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// TvInputHardwareManager::ListenerHandler
//=============================================================================
const Int32 TvInputHardwareManager::ListenerHandler::STATE_CHANGED = 1;
const Int32 TvInputHardwareManager::ListenerHandler::HARDWARE_DEVICE_ADDED = 2;
const Int32 TvInputHardwareManager::ListenerHandler::HARDWARE_DEVICE_REMOVED = 3;
const Int32 TvInputHardwareManager::ListenerHandler::HDMI_DEVICE_ADDED = 4;
const Int32 TvInputHardwareManager::ListenerHandler::HDMI_DEVICE_REMOVED = 5;
const Int32 TvInputHardwareManager::ListenerHandler::HDMI_DEVICE_UPDATED = 6;

TvInputHardwareManager::ListenerHandler::ListenerHandler(
    /* [in] */ TvInputHardwareManager* host)
    : mHost(host)
{}

ECode TvInputHardwareManager::ListenerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 arg1;
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    switch (Ptr(msg)->Func(msg->GetWhat)) {
        case STATE_CHANGED: {
            String inputId;
            IObject::Probe(obj)->ToString(&inputId);
            Int32 state = arg1;
            mHost->mListener->OnStateChanged(inputId, state);
            break;
        }
        case HARDWARE_DEVICE_ADDED: {
            AutoPtr<ITvInputHardwareInfo> info = ITvInputHardwareInfo::Probe(obj);
            mHost->mListener->OnHardwareDeviceAdded(info);
            break;
        }
        case HARDWARE_DEVICE_REMOVED: {
            AutoPtr<ITvInputHardwareInfo> info = ITvInputHardwareInfo::Probe(obj);
            mHost->mListener->OnHardwareDeviceRemoved(info);
            break;
        }
        case HDMI_DEVICE_ADDED: {
            AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
            mHost->mListener->OnHdmiDeviceAdded(info);
            break;
        }
        case HDMI_DEVICE_REMOVED: {
            AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(obj);
            mHost->mListener->OnHdmiDeviceRemoved(info);
            break;
        }
        case HDMI_DEVICE_UPDATED: {
            AutoPtr<ISomeArgs> args = ISomeArgs::Probe(obj);
            AutoPtr<IInterface> objArg;
            args->GetObjectArg(1, (IInterface**)&objArg);
            String inputId;
            IObject::Probe(objArg)->ToString(&inputId);
            objArg = NULL;
            args->GetObjectArg(2, (IInterface**)&objArg);
            AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(objArg);
            args->Recycle();
            mHost->mListener->OnHdmiDeviceUpdated(inputId, info);
        }
        default: {
            Slogger::W(TAG, "Unhandled message: %s", TO_CSTR(msg));
            break;
        }
    }
    return NOERROR;
}

//=============================================================================
// TvInputHardwareManager::HdmiHotplugEventListener
//=============================================================================
CAR_INTERFACE_IMPL_2(TvInputHardwareManager::HdmiHotplugEventListener, Object, IIHdmiHotplugEventListener, IBinder)

TvInputHardwareManager::HdmiHotplugEventListener::HdmiHotplugEventListener(
    /* [in] */ TvInputHardwareManager* host)
    : mHost(host)
{}

ECode TvInputHardwareManager::HdmiHotplugEventListener::OnReceived(
    /* [in] */ IHdmiHotplugEvent* event)
{
    synchronized(mHost->mLock) {
        mHost->mHdmiStateMap->Put(Ptr(event)->Func(event->GetPort), Ptr(event)->Func(event->IsConnected));
        AutoPtr<ITvInputHardwareInfo> hardwareInfo;
        mHost->FindHardwareInfoForHdmiPortLocked(Ptr(event)->Func(event->GetPort), (ITvInputHardwareInfo**)&hardwareInfo);
        if (hardwareInfo == NULL) {
            return NOERROR;
        }
        AutoPtr<ICharSequence> inputId;
        AutoPtr<IInterface> obj;
        mHost->mHardwareInputIdMap->Get(Ptr(hardwareInfo)->Func(hardwareInfo->GetDeviceId), (IInterface**)&obj);
        inputId = ICharSequence::Probe(obj);
        if (inputId == NULL) {
            return NOERROR;
        }
        Int32 state;
        mHost->ConvertConnectedToState(Ptr(event)->Func(event->IsConnected), &state);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(ListenerHandler::STATE_CHANGED, state, 0, inputId, (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode TvInputHardwareManager::HdmiHotplugEventListener::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// TvInputHardwareManager::HdmiDeviceEventListener
//=============================================================================
CAR_INTERFACE_IMPL_2(TvInputHardwareManager::HdmiDeviceEventListener, Object, IIHdmiDeviceEventListener, IBinder)

TvInputHardwareManager::HdmiDeviceEventListener::HdmiDeviceEventListener(
    /* [in] */ TvInputHardwareManager* host)
    : mHost(host)
{}

ECode TvInputHardwareManager::HdmiDeviceEventListener::OnStatusChanged(
    /* [in] */ IHdmiDeviceInfo* deviceInfo,
    /* [in] */ Int32 status)
{
    synchronized(mHost->mLock) {
        Int32 messageType = 0;
        AutoPtr<IObject> obj;
        switch (status) {
            case IHdmiControlManager::DEVICE_EVENT_ADD_DEVICE: {
                AutoPtr<IHdmiDeviceInfo> info;
                FindHdmiDeviceInfo(Ptr(deviceInfo)->Func(deviceInfo->GetId), (IHdmiDeviceInfo**)&info);
                if (info == NULL) {
                    mHost->mHdmiDeviceList->Add(deviceInfo);
                } else {
                    Slogger::W(TAG, "The list already contains %s; ignoring.", TO_CSTR(deviceInfo));
                    return NOERROR;
                }
                messageType = ListenerHandler::HDMI_DEVICE_ADDED;
                obj = IObject::Probe(deviceInfo);
                break;
            }
            case IHdmiControlManager::DEVICE_EVENT_REMOVE_DEVICE: {
                AutoPtr<IHdmiDeviceInfo> originalDeviceInfo;
                FindHdmiDeviceInfo(Ptr(deviceInfo)->Func(deviceInfo->GetId), (IHdmiDeviceInfo**)&originalDeviceInfo);
                Boolean b;
                mHost->mHdmiDeviceList->Remove(originalDeviceInfo, &b);
                if (!b) {
                    Slogger::W(TAG, "The list doesn't contain %s; ignoring.", TO_CSTR(deviceInfo));
                    return NOERROR;
                }
                messageType = ListenerHandler::HDMI_DEVICE_REMOVED;
                obj = IObject::Probe(deviceInfo);
                break;
            }
            case IHdmiControlManager::DEVICE_EVENT_UPDATE_DEVICE: {
                AutoPtr<IHdmiDeviceInfo> originalDeviceInfo;
                FindHdmiDeviceInfo(Ptr(deviceInfo)->Func(deviceInfo->GetId), (IHdmiDeviceInfo**)&originalDeviceInfo);
                Boolean b;
                mHost->mHdmiDeviceList->Remove(originalDeviceInfo, &b);
                if (!b) {
                    Slogger::W(TAG, "The list doesn't contain %s; ignoring.", TO_CSTR(deviceInfo));
                    return NOERROR;
                }
                mHost->mHdmiDeviceList->Add(deviceInfo);
                messageType = ListenerHandler::HDMI_DEVICE_UPDATED;
                AutoPtr<IInterface> obj;
                mHost->mHdmiInputIdMap->Get(Ptr(deviceInfo)->Func(deviceInfo->GetId), (IInterface**)&obj);
                AutoPtr<ICharSequence> inputId = ICharSequence::Probe(obj);
                AutoPtr<ISomeArgsHelper> helper;
                CSomeArgsHelper::AcquireSingleton((ISomeArgsHelper**)&helper);
                AutoPtr<ISomeArgs> args;
                helper->Obtain((ISomeArgs**)&args);
                args->SetObjectArg(1, inputId);
                args->SetObjectArg(2, deviceInfo);
                obj = IObject::Probe(args);
                break;
            }
        }
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(messageType, 0, 0, obj, (IMessage**)&msg);
        AutoPtr<ITvInputHardwareInfo> info;
        mHost->FindHardwareInfoForHdmiPortLocked(Ptr(deviceInfo)->Func(deviceInfo->GetPortId), (ITvInputHardwareInfo**)&info);
        if (info != NULL) {
            msg->SendToTarget();
        } else {
            mHost->mPendingHdmiDeviceEvents->Add(msg);
        }
    }
    return NOERROR;
}

ECode TvInputHardwareManager::HdmiDeviceEventListener::FindHdmiDeviceInfo(
    /* [in] */ Int32 id,
    /* [out] */ IHdmiDeviceInfo** result)
{
    FOR_EACH(iter, mHost->mHdmiDeviceList) {
        AutoPtr<IHdmiDeviceInfo> info = IHdmiDeviceInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        if (Ptr(info)->Func(info->GetId) == id) {
            FUNC_RETURN(info);
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode TvInputHardwareManager::HdmiDeviceEventListener::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// TvInputHardwareManager::HdmiSystemAudioModeChangeListener
//=============================================================================
CAR_INTERFACE_IMPL_2(TvInputHardwareManager::HdmiSystemAudioModeChangeListener, Object, IIHdmiSystemAudioModeChangeListener, IBinder)

TvInputHardwareManager::HdmiSystemAudioModeChangeListener::HdmiSystemAudioModeChangeListener(
    /* [in] */ TvInputHardwareManager* host)
    : mHost(host)
{}

ECode TvInputHardwareManager::HdmiSystemAudioModeChangeListener::OnStatusChanged(
    /* [in] */ Boolean enabled)
{
    AutoLock lock(mHost->mLock);
    for (Int32 i = 0; i < Ptr(mHost->mConnections)->Func(ISparseArray::GetSize); ++i) {
        AutoPtr<IInterface> obj;
        mHost->mConnections->ValueAt(i, (IInterface**)&obj);
        AutoPtr<TvInputHardwareImpl> impl;
        ((Connection*)IProxyDeathRecipient::Probe(obj))->GetHardwareImplLocked((TvInputHardwareImpl**)&impl);
        if (impl != NULL) {
            impl->HandleAudioSinkUpdated();
        }
    }
    return NOERROR;
}

ECode TvInputHardwareManager::HdmiSystemAudioModeChangeListener::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// TvInputHardwareManager::InnerSub_Runnable
//=============================================================================
CAR_INTERFACE_IMPL(TvInputHardwareManager::InnerSub_Runnable, Object, IRunnable)

TvInputHardwareManager::InnerSub_Runnable::InnerSub_Runnable(
    /* [in] */ TvInputHardwareImpl* hardwareImpl,
    /* [in] */ ITvStreamConfig* config)
    : mHardwareImpl(hardwareImpl)
    , mConfig(config)
{}

ECode TvInputHardwareManager::InnerSub_Runnable::Run()
{
    Boolean b;
    return mHardwareImpl->StopCapture(mConfig, &b);
}

//=============================================================================
// TvInputHardwareManager
//=============================================================================
const String TvInputHardwareManager::TAG("TvInputHardwareManager");

CAR_INTERFACE_IMPL(TvInputHardwareManager, Object, IProxyDeathRecipient)

TvInputHardwareManager::TvInputHardwareManager()
{
    mHal = new TvInputHal();
    mHal->constructor(this);
    CSparseArray::New((ISparseArray**)&mConnections);
    CArrayList::New((IList**)&mHardwareList);
    CLinkedList::New((IList**)&mHdmiDeviceList);
    CSparseArray::New((ISparseArray**)&mHardwareInputIdMap);
    CSparseArray::New((ISparseArray**)&mHdmiInputIdMap);
    CArrayMap::New((IMap**)&mInputMap);
    mHdmiHotplugEventListener = new HdmiHotplugEventListener(this);
    mHdmiDeviceEventListener = new HdmiDeviceEventListener(this);
    mHdmiSystemAudioModeChangeListener = new HdmiSystemAudioModeChangeListener(this);
    CSparseBooleanArray::New((ISparseBooleanArray**)&mHdmiStateMap);
    CLinkedList::New((IList**)&mPendingHdmiDeviceEvents);
    mHandler = new ListenerHandler(this);
    Elastos::Core::CObject::New((IObject**)&mLock);
}

ECode TvInputHardwareManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ITvInputHardwareManagerListener* listener)
{
    mListener = listener;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);
    mHal->Init();
    return NOERROR;
}

ECode TvInputHardwareManager::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
        AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::HDMI_CONTROL_SERVICE);
        mHdmiControlService = IIHdmiControlService::Probe(obj);
        if (mHdmiControlService != NULL) {
            // try {
            ECode ec;
            do {
                ec = mHdmiControlService->AddHotplugEventListener(mHdmiHotplugEventListener);
                if (FAILED(ec)) break;
                ec = mHdmiControlService->AddDeviceEventListener(mHdmiDeviceEventListener);
                if (FAILED(ec)) break;
                ec = mHdmiControlService->AddSystemAudioModeChangeListener(mHdmiSystemAudioModeChangeListener);
                if (FAILED(ec)) break;
                ec = mHdmiDeviceList->AddAll(ICollection::Probe(Ptr(mHdmiControlService)->Func(mHdmiControlService->GetInputDevices)));
            } while(FALSE);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    Slogger::W(TAG, "Error registering listeners to HdmiControlService:%d", ec);
                }
            }
            // }
        }
    }
    return NOERROR;
}

ECode TvInputHardwareManager::OnDeviceAvailable(
    /* [in] */ ITvInputHardwareInfo* info,
    /* [in] */ ArrayOf<ITvStreamConfig*>* configs)
{
    synchronized(mLock) {
        AutoPtr<Connection> connection = new Connection(this);
        connection->constructor(info);
        connection->UpdateConfigsLocked(configs);
        mConnections->Put(Ptr(info)->Func(info->GetDeviceId), TO_IINTERFACE(connection));
        BuildHardwareListLocked();
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(
                ListenerHandler::HARDWARE_DEVICE_ADDED, 0, 0, info, (IMessage**)&msg);
        msg->SendToTarget();
        if (Ptr(info)->Func(info->GetType) == ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI) {
            ProcessPendingHdmiDeviceEventsLocked();
        }
    }
    return NOERROR;
}

ECode TvInputHardwareManager::BuildHardwareListLocked()
{
    mHardwareList->Clear();
    for (Int32 i = 0; i < Ptr(mConnections)->Func(mConnections->GetSize); ++i) {
        AutoPtr<IInterface> obj;
        mConnections->ValueAt(i, (IInterface**)&obj);
        AutoPtr<ITvInputHardwareInfo> info;
        ((Connection*)IProxyDeathRecipient::Probe(obj))->GetHardwareInfoLocked((ITvInputHardwareInfo**)&info);
        mHardwareList->Add(info);
    }
    return NOERROR;
}

ECode TvInputHardwareManager::OnDeviceUnavailable(
    /* [in] */ Int32 deviceId)
{
    synchronized(mLock) {
        AutoPtr<IInterface> obj;
        mConnections->Get(deviceId, (IInterface**)&obj);
        AutoPtr<Connection> connection = (Connection*)IProxyDeathRecipient::Probe(obj);
        if (connection == NULL) {
            Slogger::E(TAG, "onDeviceUnavailable: Cannot find a connection with %d", deviceId);
            return NOERROR;
        }
        connection->ResetLocked(NULL, NULL, NULL, NULL, NULL);
        mConnections->Remove(deviceId);
        BuildHardwareListLocked();
        AutoPtr<ITvInputHardwareInfo> info;
        connection->GetHardwareInfoLocked((ITvInputHardwareInfo**)&info);
        if (Ptr(info)->Func(info->GetType) == ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI) {
            // Remove HDMI devices linked with this hardware.
            FOR_EACH(it,mHdmiDeviceList) {
                AutoPtr<IHdmiDeviceInfo> deviceInfo = IHdmiDeviceInfo::Probe(Ptr(it)->Func(it->GetNext));
                if (Ptr(deviceInfo)->Func(deviceInfo->GetPortId) == Ptr(info)->Func(info->GetHdmiPortId)) {
                    AutoPtr<IMessage> msg;
                    mHandler->ObtainMessage(ListenerHandler::HDMI_DEVICE_REMOVED, 0, 0,
                            deviceInfo, (IMessage**)&msg);
                    msg->SendToTarget();
                    it->Remove();
                }
            }
        }
        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(
                ListenerHandler::HARDWARE_DEVICE_REMOVED, 0, 0, info, (IMessage**)&msg);
        msg->SendToTarget();
    }
    return NOERROR;
}

ECode TvInputHardwareManager::OnStreamConfigurationChanged(
    /* [in] */ Int32 deviceId,
    /* [in] */ ArrayOf<ITvStreamConfig*>* configs)
{
    synchronized(mLock) {
        AutoPtr<IInterface> obj;
        mConnections->Get(deviceId, (IInterface**)&obj);
        AutoPtr<Connection> connection = (Connection*) IProxyDeathRecipient::Probe(obj);
        if (connection == NULL) {
            Slogger::E(TAG, "StreamConfigurationChanged: Cannot find a connection with %d", deviceId);
            return NOERROR;
        }
        connection->UpdateConfigsLocked(configs);
        // try {
        ECode ec = Ptr(connection)->Func(connection->GetCallbackLocked)->OnStreamConfigChanged(configs);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode)E_REMOTE_EXCEPTION == ec) {
                Slogger::E(TAG, "error in onStreamConfigurationChanged %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode TvInputHardwareManager::OnFirstFrameCaptured(
    /* [in] */ Int32 deviceId,
    /* [in] */ Int32 streamId)
{
    synchronized(mLock) {
        AutoPtr<IInterface> obj;
        mConnections->Get(deviceId, (IInterface**)&obj);
        AutoPtr<Connection> connection = (Connection*)IProxyDeathRecipient::Probe(obj);
        if (connection == NULL) {
            Slogger::E(TAG, "FirstFrameCaptured: Cannot find a connection with %d", deviceId);
            return NOERROR;
        }
        AutoPtr<IRunnable> runnable;
        connection->GetOnFirstFrameCapturedLocked((IRunnable**)&runnable);
        if (runnable != NULL) {
            runnable->Run();
            connection->SetOnFirstFrameCapturedLocked(NULL);
        }
    }
    return NOERROR;
}

ECode TvInputHardwareManager::GetHardwareList(
    /* [out] */ IList** result)
{
    synchronized(mLock) {
        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        return helper->UnmodifiableList(mHardwareList, result);
    }
    return NOERROR;
}

ECode TvInputHardwareManager::GetHdmiDeviceList(
    /* [out] */ IList** result)
{
    synchronized(mLock) {
        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        return helper->UnmodifiableList(mHdmiDeviceList, result);
    }
    return NOERROR;
}

Boolean TvInputHardwareManager::CheckUidChangedLocked(
    /* [in] */ Connection* connection,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 resolvedUserId)
{

    AutoPtr<IInteger32> connectionCallingUid;
    connection->GetCallingUidLocked((IInteger32**)&connectionCallingUid);
    AutoPtr<IInteger32> connectionResolvedUserId;
    connection->GetResolvedUserIdLocked((IInteger32**)&connectionResolvedUserId);
    if (connectionCallingUid == NULL || connectionResolvedUserId == NULL) {
        return TRUE;
    }
    if (Ptr(connectionCallingUid)->Func(IInteger32::GetValue) != callingUid || Ptr(connectionResolvedUserId)->Func(IInteger32::GetValue) != resolvedUserId) {
        return TRUE;
    }
    return FALSE;
}

ECode TvInputHardwareManager::ConvertConnectedToState(
    /* [in] */ Boolean connected,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (connected) {
        *result = ITvInputManager::INPUT_STATE_CONNECTED;
        return NOERROR;
    } else {
        *result = ITvInputManager::INPUT_STATE_DISCONNECTED;
        return NOERROR;
    }
    return NOERROR;
}

ECode TvInputHardwareManager::AddHardwareTvInput(
    /* [in] */ Int32 deviceId,
    /* [in] */ ITvInputInfo* info)
{
    synchronized(mLock) {
        AutoPtr<IInterface> obj;
        mHardwareInputIdMap->Get(deviceId, (IInterface**)&obj);
        AutoPtr<ICharSequence> oldInputId = ICharSequence::Probe(obj);
        if (oldInputId != NULL) {
            AutoPtr<IInterface> obj;
            mInputMap->Get(oldInputId, (IInterface**)&obj);
            Slogger::W(TAG, "Trying to override previous registration: old = %s:%d, new = %s:%d",
                    TO_CSTR(obj), deviceId, TO_CSTR(info), deviceId);
        }
        String s;
        info->GetId(&s);
        mHardwareInputIdMap->Put(deviceId, StringUtils::ParseCharSequence(s));
        mInputMap->Put(StringUtils::ParseCharSequence(s), info);
        for (Int32 i = 0; i < Ptr(mHdmiStateMap)->Func(mHdmiStateMap->GetSize); ++i) {
            Int32 key;
            mHdmiStateMap->KeyAt(i, &key);
            AutoPtr<ITvInputHardwareInfo> hardwareInfo;
            FindHardwareInfoForHdmiPortLocked(key, (ITvInputHardwareInfo**)&hardwareInfo);
            if (hardwareInfo == NULL) {
                continue;
            }
            AutoPtr<IInterface> obj;
            mHardwareInputIdMap->Get(Ptr(hardwareInfo)->Func(hardwareInfo->GetDeviceId), (IInterface**)&obj);
            String inputId;
            IObject::Probe(obj)->ToString(&inputId);
            if (inputId != NULL && inputId.Equals(Ptr(info)->Func(info->GetId))) {
                Boolean value;
                mHdmiStateMap->ValueAt(i, &value);
                Int32 state;
                ConvertConnectedToState(value, &state);
                AutoPtr<IMessage> msg;
                mHandler->ObtainMessage(ListenerHandler::STATE_CHANGED, state, 0, StringUtils::ParseCharSequence(inputId), (IMessage**)&msg);
                msg->SendToTarget();
            }
        }
    }
    return NOERROR;
}

ECode TvInputHardwareManager::IndexOfEqualValue(
    /* [in] */ ISparseArray* map,
    /* [in] */ IInterface* value,
    /* [out] */ Int32* result)
{
    for (Int32 i = 0; i < Ptr(map)->Func(map->GetSize); ++i) {
        AutoPtr<IInterface> obj;
        map->ValueAt(i, (IInterface**)&obj);
        Boolean isEquals;
        IObject::Probe(obj)->Equals(value, &isEquals);
        if (isEquals) {
            *result = i;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode TvInputHardwareManager::AddHdmiTvInput(
    /* [in] */ Int32 id,
    /* [in] */ ITvInputInfo* info)
{
    if (Ptr(info)->Func(info->GetType) != ITvInputInfo::TYPE_HDMI) {
        Logger::E(TAG, "info (%s) has non-HDMI type.", TO_CSTR(info));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    synchronized(mLock) {
        String parentId;
        info->GetParentId(&parentId);
        Int32 parentIndex;
        IndexOfEqualValue(mHardwareInputIdMap, StringUtils::ParseCharSequence(parentId), &parentIndex);
        if (parentIndex < 0) {
            Logger::E(TAG, "info (%s) has invalid parentId.", TO_CSTR(info));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IInterface> obj;
        mHdmiInputIdMap->Get(id, (IInterface**)&obj);
        AutoPtr<ICharSequence> oldInputId = ICharSequence::Probe(obj);
        if (oldInputId != NULL) {
            obj = NULL;
            mInputMap->Get(oldInputId, (IInterface**)&obj);
            Slogger::W(TAG, "Trying to override previous registration: old = %s:%d, new = %s:%d",
                    TO_CSTR(obj), id, TO_CSTR(info), id);
        }
        AutoPtr<IInteger32> i32;
        CInteger32::New(id, (IInteger32**)&i32);
        mHdmiInputIdMap->Put(Ptr(i32)->Func(IInteger32::GetValue), StringUtils::ParseCharSequence(Ptr(info)->Func(info->GetId)));
        mInputMap->Put(StringUtils::ParseCharSequence(Ptr(info)->Func(info->GetId)), info);
    }
    return NOERROR;
}

ECode TvInputHardwareManager::RemoveTvInput(
    /* [in] */ const String& inputId)
{
    synchronized(mLock) {
        mInputMap->Remove(StringUtils::ParseCharSequence(inputId));
        Int32 hardwareIndex;
        IndexOfEqualValue(mHardwareInputIdMap, StringUtils::ParseCharSequence(inputId), &hardwareIndex);
        if (hardwareIndex >= 0) {
            mHardwareInputIdMap->RemoveAt(hardwareIndex);
        }
        Int32 deviceIndex;
        IndexOfEqualValue(mHdmiInputIdMap, StringUtils::ParseCharSequence(inputId), &deviceIndex);
        if (deviceIndex >= 0) {
            mHdmiInputIdMap->RemoveAt(deviceIndex);
        }
    }
    return NOERROR;
}

ECode TvInputHardwareManager::AcquireHardware(
    /* [in] */ Int32 deviceId,
    /* [in] */ IITvInputHardwareCallback* callback,
    /* [in] */ ITvInputInfo* info,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 resolvedUserId,
    /* [out] */ IITvInputHardware** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (callback == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    synchronized(mLock) {
        AutoPtr<IInterface> obj;
        mConnections->Get(deviceId, (IInterface**)&obj);
        AutoPtr<Connection> connection = (Connection*)IProxyDeathRecipient::Probe(obj);
        if (connection == NULL) {
            Slogger::E(TAG, "Invalid deviceId : %d", deviceId);
            *result = NULL;
            return NOERROR;
        }
        if (CheckUidChangedLocked(connection, callingUid, resolvedUserId)) {
            AutoPtr<TvInputHardwareImpl> hardware =
                    new TvInputHardwareImpl((TvInputHardwareManager*)ITvInputHalCallback::Probe(Ptr(connection)->Func(connection->GetHardwareInfoLocked)));
            // try {
            AutoPtr<IProxy> proxy = (IProxy*) callback->Probe(EIID_IProxy);
            ECode ec = proxy->LinkToDeath(connection, 0);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode)E_REMOTE_EXCEPTION == ec) {
                    hardware->Release();
                    *result = NULL;
                    return NOERROR;
                }
                return ec;
            }
            // }
            AutoPtr<IInteger32> i32_calling, i32_user;
            CInteger32::New(callingUid, (IInteger32**)&i32_calling);
            CInteger32::New(resolvedUserId, (IInteger32**)&i32_user);
            connection->ResetLocked(hardware, callback, info, i32_calling, i32_user);
        }
        return connection->GetHardwareLocked(result);
    }
    return NOERROR;
}

ECode TvInputHardwareManager::ReleaseHardware(
    /* [in] */ Int32 deviceId,
    /* [in] */ IITvInputHardware* hardware,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 resolvedUserId)
{
    synchronized(mLock) {
        AutoPtr<IInterface> obj;
        mConnections->Get(deviceId, (IInterface**)&obj);
        AutoPtr<Connection> connection = (Connection*) IObject::Probe(obj);
        if (connection == NULL) {
            Slogger::E(TAG, "Invalid deviceId : %d", deviceId);
            return NOERROR;
        }
        if (Ptr(connection)->Func(connection->GetHardwareLocked).Get() != hardware
                || CheckUidChangedLocked(connection, callingUid, resolvedUserId)) {
            return NOERROR;
        }
        connection->ResetLocked(NULL, NULL, NULL, NULL, NULL);
    }
    return NOERROR;
}

ECode TvInputHardwareManager::FindHardwareInfoForHdmiPortLocked(
    /* [in] */ Int32 port,
    /* [out] */ ITvInputHardwareInfo** result)
{
    VALIDATE_NOT_NULL(result)

    FOR_EACH(iter, mHardwareList) {
        AutoPtr<ITvInputHardwareInfo> hardwareInfo = ITvInputHardwareInfo::Probe(Ptr(iter)->Func(iter->GetNext));
        if (Ptr(hardwareInfo)->Func(hardwareInfo->GetType) == ITvInputHardwareInfo::TV_INPUT_TYPE_HDMI
                && Ptr(hardwareInfo)->Func(hardwareInfo->GetHdmiPortId) == port) {
            FUNC_RETURN(hardwareInfo);
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode TvInputHardwareManager::FindDeviceIdForInputIdLocked(
    /* [in] */ const String& inputId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    for (Int32 i = 0; i < Ptr(mConnections)->Func(mConnections->GetSize); ++i) {
        AutoPtr<IInterface> obj;
        mConnections->Get(i, (IInterface**)&obj);
        AutoPtr<Connection> connection = (Connection*) IObject::Probe(obj);
        AutoPtr<ITvInputInfo> info;
        connection->GetInfoLocked((ITvInputInfo**)&info);
        if (Ptr(info)->Func(ITvInputInfo::GetId).Equals(inputId)) {
            *result = i;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode TvInputHardwareManager::GetAvailableTvStreamConfigList(
    /* [in] */ const String& inputId,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 resolvedUserId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> configsList;
    CArrayList::New((IList**)&configsList);
    synchronized(mLock) {
        Int32 deviceId;
        FindDeviceIdForInputIdLocked(inputId, &deviceId);
        if (deviceId < 0) {
            Slogger::E(TAG, "Invalid inputId : %s", inputId.string());
            FUNC_RETURN(configsList)
        }
        AutoPtr<IInterface> obj;
        mConnections->Get(deviceId, (IInterface**)&obj);
        AutoPtr<Connection> connection = (Connection*) IObject::Probe(obj);
        AutoPtr<ArrayOf<ITvStreamConfig*> > array;
        for (Int32 i = 0; i < array->GetLength(); ++i) {
            AutoPtr<ITvStreamConfig> config = (*array)[i];
            if (Ptr(config)->Func(config->GetType) == ITvStreamConfig::STREAM_TYPE_BUFFER_PRODUCER) {
                configsList->Add(config);
            }
        }
    }
    FUNC_RETURN(configsList)
}

ECode TvInputHardwareManager::CaptureFrame(
    /* [in] */ const String& inputId,
    /* [in] */ ISurface* surface,
    /* [in] */ ITvStreamConfig* config,
    /* [in] */ Int32 callingUid,
    /* [in] */ Int32 resolvedUserId,
    /* [out] */ Boolean* rev)
{
    VALIDATE_NOT_NULL(rev)

    synchronized(mLock) {
        Int32 deviceId;
        FindDeviceIdForInputIdLocked(inputId, &deviceId);
        if (deviceId < 0) {
            Slogger::E(TAG, "Invalid inputId : %s", inputId.string());
            *rev = FALSE;
            return NOERROR;
        }
        AutoPtr<IInterface> obj;
        mConnections->Get(deviceId, (IInterface**)&obj);
        AutoPtr<Connection> connection = (Connection*) IObject::Probe(obj);
        AutoPtr<TvInputHardwareImpl> hardwareImpl;
        connection->GetHardwareImplLocked((TvInputHardwareImpl**)&hardwareImpl);
        if (hardwareImpl != NULL) {
            // Stop previous capture.
            AutoPtr<IRunnable> runnable;
            connection->GetOnFirstFrameCapturedLocked((IRunnable**)&runnable);
            if (runnable != NULL) {
                runnable->Run();
                connection->SetOnFirstFrameCapturedLocked(NULL);
            }
            Boolean result;
            hardwareImpl->StartCapture(surface, config, &result);
            if (result) {
                connection->SetOnFirstFrameCapturedLocked(new InnerSub_Runnable(hardwareImpl, config));
            }
            *rev = result;
            return NOERROR;
        }
    }
    *rev = FALSE;
    return NOERROR;
}

ECode TvInputHardwareManager::ProcessPendingHdmiDeviceEventsLocked()
{
    FOR_EACH(it,mPendingHdmiDeviceEvents) {
        AutoPtr<IMessage> msg = IMessage::Probe(Ptr(it)->Func(it->GetNext));
        AutoPtr<IHdmiDeviceInfo> deviceInfo = IHdmiDeviceInfo::Probe(Ptr(msg)->Func(msg->GetObj));
        AutoPtr<ITvInputHardwareInfo> hardwareInfo;
        FindHardwareInfoForHdmiPortLocked(Ptr(deviceInfo)->Func(deviceInfo->GetPortId), (ITvInputHardwareInfo**)&hardwareInfo);
        if (hardwareInfo != NULL) {
            msg->SendToTarget();
            it->Remove();
        }
    }
    return NOERROR;
}

} // namespace Tv
} // namespace Server
} // namespace Droid
} // namespace Elastos
