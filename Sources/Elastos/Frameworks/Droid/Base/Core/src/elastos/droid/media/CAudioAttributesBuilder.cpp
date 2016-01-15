#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/media/CAudioAttributes.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioAttributesBuilder, Object, IAudioAttributesBuilder)

CAR_OBJECT_IMPL(CAudioAttributesBuilder)

CAudioAttributesBuilder::CAudioAttributesBuilder()
    : mUsage(IAudioAttributes::USAGE_UNKNOWN)
    , mContentType(IAudioAttributes::CONTENT_TYPE_UNKNOWN)
    , mSource(IMediaRecorderAudioSource::AUDIO_SOURCE_INVALID)
    , mFlags(0)
{
    CHashSet::New((IHashSet**)&mTags);
}

CAudioAttributesBuilder::~CAudioAttributesBuilder()
{
}

ECode CAudioAttributesBuilder::constructor()
{
    return NOERROR;
}

ECode CAudioAttributesBuilder::constructor(
    /* [in] */ IAudioAttributes* aa)
{
    AutoPtr<CAudioAttributes> caa = (CAudioAttributes*)aa;
    mUsage = caa->mUsage;
    mContentType = caa->mContentType;
    mFlags = caa->mFlags;
    CHashSet::New(ICollection::Probe(caa->mTags), (IHashSet**)&mTags);
    return NOERROR;
}

ECode CAudioAttributesBuilder::Build(
    /* [out] */ IAudioAttributes** result)
{
    AutoPtr<CAudioAttributes> aa;
    CAudioAttributes::NewByFriend((CAudioAttributes**)&aa);
    aa->mContentType = mContentType;
    aa->mUsage = mUsage;
    aa->mSource = mSource;
    aa->mFlags = mFlags;
    CHashSet::New(ICollection::Probe(mTags), (IHashSet**)&aa->mTags);
    AutoPtr<ICharSequence> delimiter;
    CString::New(String(";"), (ICharSequence**)&delimiter);
    aa->mFormattedTags = TextUtils::Join(delimiter, IIterable::Probe(mTags));
    *result = IAudioAttributes::Probe(aa);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CAudioAttributesBuilder::SetUsage(
    /* [in] */ Int32 usage)
{
    switch (usage) {
        case IAudioAttributes::USAGE_UNKNOWN:
        case IAudioAttributes::USAGE_MEDIA:
        case IAudioAttributes::USAGE_VOICE_COMMUNICATION:
        case IAudioAttributes::USAGE_VOICE_COMMUNICATION_SIGNALLING:
        case IAudioAttributes::USAGE_ALARM:
        case IAudioAttributes::USAGE_NOTIFICATION:
        case IAudioAttributes::USAGE_NOTIFICATION_RINGTONE:
        case IAudioAttributes::USAGE_NOTIFICATION_COMMUNICATION_REQUEST:
        case IAudioAttributes::USAGE_NOTIFICATION_COMMUNICATION_INSTANT:
        case IAudioAttributes::USAGE_NOTIFICATION_COMMUNICATION_DELAYED:
        case IAudioAttributes::USAGE_NOTIFICATION_EVENT:
        case IAudioAttributes::USAGE_ASSISTANCE_ACCESSIBILITY:
        case IAudioAttributes::USAGE_ASSISTANCE_NAVIGATION_GUIDANCE:
        case IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION:
        case IAudioAttributes::USAGE_GAME:
             mUsage = usage;
             break;
        default:
             mUsage = IAudioAttributes::USAGE_UNKNOWN;
    }
    return NOERROR;
}

ECode CAudioAttributesBuilder::SetContentType(
    /* [in] */ Int32 contentType)
{
    switch (contentType) {
        case IAudioAttributes::CONTENT_TYPE_UNKNOWN:
        case IAudioAttributes::CONTENT_TYPE_MOVIE:
        case IAudioAttributes::CONTENT_TYPE_MUSIC:
        case IAudioAttributes::CONTENT_TYPE_SONIFICATION:
        case IAudioAttributes::CONTENT_TYPE_SPEECH:
             mContentType = contentType;
             break;
        default:
             mUsage = IAudioAttributes::CONTENT_TYPE_UNKNOWN;
    }
    return NOERROR;
}

ECode CAudioAttributesBuilder::SetFlags(
    /* [in] */ Int32 _flags)
{
    Int32 flags = _flags;
    flags &= CAudioAttributes::FLAG_ALL;
    mFlags |= flags;
    return NOERROR;
}

ECode CAudioAttributesBuilder::AddTag(
    /* [in] */ const String& tag)
{
    AutoPtr<ICharSequence> cs;
    CString::New(tag, (ICharSequence**)&cs);
    return mTags->Add(cs);
}

ECode CAudioAttributesBuilder::SetLegacyStreamType(
    /* [in] */ Int32 streamType)
{
    return SetInternalLegacyStreamType(streamType);
}

ECode CAudioAttributesBuilder::SetInternalLegacyStreamType(
    /* [in] */ Int32 streamType)
{
    switch(streamType) {
        case IAudioSystem::STREAM_VOICE_CALL:
            mContentType = IAudioAttributes::CONTENT_TYPE_SPEECH;
            break;
        case IAudioSystem::STREAM_SYSTEM_ENFORCED:
            mFlags |= IAudioAttributes::FLAG_AUDIBILITY_ENFORCED;
            // intended fall through, attributes in common with STREAM_SYSTEM
        case IAudioSystem::STREAM_SYSTEM:
            mContentType = IAudioAttributes::CONTENT_TYPE_SONIFICATION;
            break;
        case IAudioSystem::STREAM_RING:
            mContentType = IAudioAttributes::CONTENT_TYPE_SONIFICATION;
            break;
        case IAudioSystem::STREAM_MUSIC:
            mContentType = IAudioAttributes::CONTENT_TYPE_MUSIC;
            break;
        case IAudioSystem::STREAM_ALARM:
            mContentType = IAudioAttributes::CONTENT_TYPE_SONIFICATION;
            break;
        case IAudioSystem::STREAM_NOTIFICATION:
            mContentType = IAudioAttributes::CONTENT_TYPE_SONIFICATION;
            break;
        case IAudioSystem::STREAM_BLUETOOTH_SCO:
            mContentType = IAudioAttributes::CONTENT_TYPE_SPEECH;
            mFlags |= IAudioAttributes::FLAG_SCO;
            break;
        case IAudioSystem::STREAM_DTMF:
            mContentType = IAudioAttributes::CONTENT_TYPE_SONIFICATION;
            break;
        case IAudioSystem::STREAM_TTS:
            mContentType = IAudioAttributes::CONTENT_TYPE_SPEECH;
            break;
        default:
            Logger::E(CAudioAttributes::TAG, "Invalid stream type %d for AudioAttributes", streamType);
    }
    return CAudioAttributes::UsageForLegacyStreamType(streamType, &mUsage);
}

ECode CAudioAttributesBuilder::SetCapturePreset(
    /* [in] */ Int32 preset)
{
    switch (preset) {
        case IMediaRecorderAudioSource::DEFAULT:
        case IMediaRecorderAudioSource::MIC:
        case IMediaRecorderAudioSource::CAMCORDER:
        case IMediaRecorderAudioSource::VOICE_RECOGNITION:
        case IMediaRecorderAudioSource::VOICE_COMMUNICATION:
            mSource = preset;
            break;
        default:
            Logger::E(CAudioAttributes::TAG, "Invalid capture preset %d for AudioAttributes", preset);
    }
    return NOERROR;
}

ECode CAudioAttributesBuilder::SetInternalCapturePreset(
    /* [in] */ Int32 preset)
{
    if ((preset == IMediaRecorderAudioSource::HOTWORD)
            || (preset == IMediaRecorderAudioSource::REMOTE_SUBMIX)) {
        mSource = preset;
    } else {
        SetCapturePreset(preset);
    }
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
