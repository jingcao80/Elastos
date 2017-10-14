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

#include "elastos/droid/media/CAudioAttributes.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

String CAudioAttributes::TAG("AudioAttributes");
Int32 CAudioAttributes::FLAG_ALL = IAudioAttributes::IAudioAttributes::FLAG_AUDIBILITY_ENFORCED | IAudioAttributes::FLAG_SECURE | IAudioAttributes::FLAG_SCO | IAudioAttributes::FLAG_BEACON | IAudioAttributes::FLAG_HW_AV_SYNC | IAudioAttributes::FLAG_HW_HOTWORD;
Int32 CAudioAttributes::FLAG_ALL_PUBLIC = IAudioAttributes::FLAG_AUDIBILITY_ENFORCED | IAudioAttributes::FLAG_HW_AV_SYNC;
Int32 CAudioAttributes::ALL_PARCEL_FLAGS = IAudioAttributes::FLATTEN_TAGS;

CAR_INTERFACE_IMPL_2(CAudioAttributes, Object, IAudioAttributes, IParcelable)

CAR_OBJECT_IMPL(CAudioAttributes)

CAudioAttributes::CAudioAttributes()
    : mUsage(IAudioAttributes::USAGE_UNKNOWN)
    , mContentType(IAudioAttributes::CONTENT_TYPE_UNKNOWN)
    , mSource(IMediaRecorderAudioSource::AUDIO_SOURCE_INVALID)
    , mFlags(0)
{
}

CAudioAttributes::~CAudioAttributes()
{
}

ECode CAudioAttributes::constructor()
{
    return NOERROR;
}

ECode CAudioAttributes::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mUsage);
    source->ReadInt32(&mContentType);
    source->ReadInt32(&mSource);
    source->ReadInt32(&mFlags);
    source->ReadString(&mFormattedTags);
    AutoPtr<ArrayOf<String> > tagsArray;
    source->ReadArrayOfString((ArrayOf<String>**)&tagsArray);
    mTags = NULL;
    CHashSet::New((IHashSet**)&mTags);
    for (Int32 i = 0; i < tagsArray->GetLength(); i++) {
        mTags->Add(CoreUtils::Convert((*tagsArray)[i]));
    }
    return NOERROR;
}

ECode CAudioAttributes::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mUsage);
    dest->WriteInt32(mContentType);
    dest->WriteInt32(mSource);
    dest->WriteInt32(mFlags);
    dest->WriteString(mFormattedTags);
    AutoPtr<ArrayOf<IInterface*> > array;
    mTags->ToArray((ArrayOf<IInterface*>**)&array);
    Int32 size = array->GetLength();
    AutoPtr<ArrayOf<String> > tagsArray = ArrayOf<String>::Alloc(size);
    for (Int32 i = 0; i < size; i++)
        tagsArray->Set(i, TO_STR((*array)[i]));
    dest->WriteArrayOfString(tagsArray);
    return NOERROR;
}

ECode CAudioAttributes::GetContentType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mContentType;
    return NOERROR;
}

ECode CAudioAttributes::GetUsage(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUsage;
    return NOERROR;
}

ECode CAudioAttributes::GetCapturePreset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSource;
    return NOERROR;
}

ECode CAudioAttributes::GetFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mFlags & (FLAG_ALL_PUBLIC));
    return NOERROR;
}

ECode CAudioAttributes::GetAllFlags(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mFlags & FLAG_ALL);
    return NOERROR;
}

ECode CAudioAttributes::GetTags(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);
    c->UnmodifiableSet(ISet::Probe(mTags), result);
    return NOERROR;
}

ECode CAudioAttributes::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal)

    if (TO_IINTERFACE(this) == o) {
        *equal = TRUE;
        return NOERROR;
    }
    if (o == NULL) { // || getClass() != o.getClass()) {
        *equal = FALSE;
        return NOERROR;
    }

    AutoPtr<CAudioAttributes> that = (CAudioAttributes*)(IObject*)o;

    *equal = ((mContentType == that->mContentType)
            && (mFlags == that->mFlags)
            && (mSource == that->mSource)
            && (mUsage == that->mUsage)
            //mFormattedTags is never null due to assignment in Builder or unmarshalling
            && (mFormattedTags.Equals(that->mFormattedTags)));
    return NOERROR;
}

ECode CAudioAttributes::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = 0;
//TODO: Need Objects.hash
    // return Objects.hash(mContentType, mFlags, mSource, mUsage, mFormattedTags);
    return NOERROR;
}

ECode CAudioAttributes::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder b("AudioAttributes:");
    b.Append(" usage=");
    b.Append(mUsage);
    b.Append(" content=");
    b.Append(mContentType);
    b.Append(" flags=0x");
    String str = StringUtils::ToHexString(mFlags).ToUpperCase();
    b.Append(str);
    b.Append(" tags=");
    b.Append(mFormattedTags);
    return b.ToString(result);
}

ECode CAudioAttributes::UsageToString(
    /* [out] */ String* result)
{
    return UsageToString(mUsage, result);
}

ECode CAudioAttributes::UsageToString(
    /* [in] */ Int32 usage,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    switch(usage) {
        case IAudioAttributes::USAGE_UNKNOWN:
            *result = "USAGE_UNKNOWN";
            return NOERROR;
        case IAudioAttributes::USAGE_MEDIA:
            *result = "USAGE_MEDIA";
            return NOERROR;
        case IAudioAttributes::USAGE_VOICE_COMMUNICATION:
            *result = "USAGE_VOICE_COMMUNICATION";
            return NOERROR;
        case IAudioAttributes::USAGE_VOICE_COMMUNICATION_SIGNALLING:
            *result = "USAGE_VOICE_COMMUNICATION";
            return NOERROR;
        case IAudioAttributes::USAGE_ALARM:
            *result = "USAGE_ALARM";
            return NOERROR;
        case IAudioAttributes::USAGE_NOTIFICATION:
            *result = "USAGE_NOTIFICATION";
            return NOERROR;
        case IAudioAttributes::USAGE_NOTIFICATION_RINGTONE:
            *result = "USAGE_NOTIFICATION";
            return NOERROR;
        case IAudioAttributes::USAGE_NOTIFICATION_COMMUNICATION_REQUEST:
            *result = "USAGE_NOTIFICATION";
            return NOERROR;
        case IAudioAttributes::USAGE_NOTIFICATION_COMMUNICATION_INSTANT:
            *result = "USAGE_NOTIFICATION_COMMUNICATION_INSTANT";
            return NOERROR;
        case IAudioAttributes::USAGE_NOTIFICATION_COMMUNICATION_DELAYED:
            *result = "USAGE_NOTIFICATION_COMMUNICATION_DELAYED";
            return NOERROR;
        case IAudioAttributes::USAGE_NOTIFICATION_EVENT:
            *result = "USAGE_NOTIFICATION_EVENT";
            return NOERROR;
        case IAudioAttributes::USAGE_ASSISTANCE_ACCESSIBILITY:
            *result = "USAGE_ASSISTANCE_ACCESSIBILITY";
            return NOERROR;
        case IAudioAttributes::USAGE_ASSISTANCE_NAVIGATION_GUIDANCE:
            *result = "USAGE_ASSISTANCE_NAVIGATION_GUIDANCE";
            return NOERROR;
        case IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION:
            *result = "USAGE_ASSISTANCE_SONIFICATION";
            return NOERROR;
        case IAudioAttributes::USAGE_GAME:
            *result = "USAGE_GAME";
            return NOERROR;
        default:
            *result = "unknown usage ";
            result->AppendFormat("%d", usage);
            return NOERROR;
    }
}

ECode CAudioAttributes::UsageForLegacyStreamType(
    /* [in] */ Int32 streamType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    switch(streamType) {
        case IAudioSystem::STREAM_VOICE_CALL:
            *result = IAudioAttributes::USAGE_VOICE_COMMUNICATION;
            return NOERROR;
        case IAudioSystem::STREAM_SYSTEM_ENFORCED:
        case IAudioSystem::STREAM_SYSTEM:
            *result = IAudioAttributes::USAGE_ASSISTANCE_SONIFICATION;
            return NOERROR;
        case IAudioSystem::STREAM_RING:
            *result = IAudioAttributes::USAGE_NOTIFICATION_RINGTONE;
            return NOERROR;
        case IAudioSystem::STREAM_MUSIC:
            *result = IAudioAttributes::USAGE_MEDIA;
            return NOERROR;
        case IAudioSystem::STREAM_ALARM:
            *result = IAudioAttributes::USAGE_ALARM;
            return NOERROR;
        case IAudioSystem::STREAM_NOTIFICATION:
            *result = IAudioAttributes::USAGE_NOTIFICATION;
            return NOERROR;
        case IAudioSystem::STREAM_BLUETOOTH_SCO:
            *result = IAudioAttributes::USAGE_VOICE_COMMUNICATION;
            return NOERROR;
        case IAudioSystem::STREAM_DTMF:
            *result = IAudioAttributes::USAGE_VOICE_COMMUNICATION_SIGNALLING;
            return NOERROR;
        case IAudioSystem::STREAM_TTS:
            *result = IAudioAttributes::USAGE_ASSISTANCE_ACCESSIBILITY;
            return NOERROR;
        default:
            *result = IAudioAttributes::USAGE_UNKNOWN;
            return NOERROR;
    }
}

ECode CAudioAttributes::ToLegacyStreamType(
    /* [in] */ IAudioAttributes* aa,
    /* [out] */ Int32* result)
{
    // flags to stream type mapping
    Int32 flags;
    aa->GetFlags(&flags);
    if ((flags & IAudioAttributes::FLAG_AUDIBILITY_ENFORCED) == IAudioAttributes::FLAG_AUDIBILITY_ENFORCED) {
        *result = IAudioSystem::STREAM_SYSTEM_ENFORCED;
        return NOERROR;
    }
    if ((flags & IAudioAttributes::FLAG_SCO) == IAudioAttributes::FLAG_SCO) {
        *result = IAudioSystem::STREAM_BLUETOOTH_SCO;
        return NOERROR;
    }

    // usage to stream type mapping
    Int32 usage;
    aa->GetUsage(&usage);
    switch (usage) {
        case USAGE_MEDIA:
        case USAGE_GAME:
        case USAGE_ASSISTANCE_ACCESSIBILITY:
        case USAGE_ASSISTANCE_NAVIGATION_GUIDANCE:
            *result = IAudioSystem::STREAM_MUSIC;
            return NOERROR;
        case USAGE_ASSISTANCE_SONIFICATION:
            *result = IAudioSystem::STREAM_SYSTEM;
            return NOERROR;
        case USAGE_VOICE_COMMUNICATION:
            *result = IAudioSystem::STREAM_VOICE_CALL;
            return NOERROR;
        case USAGE_VOICE_COMMUNICATION_SIGNALLING:
            *result = IAudioSystem::STREAM_DTMF;
            return NOERROR;
        case USAGE_ALARM:
            *result = IAudioSystem::STREAM_ALARM;
            return NOERROR;
        case USAGE_NOTIFICATION_RINGTONE:
            *result = IAudioSystem::STREAM_RING;
            return NOERROR;
        case USAGE_NOTIFICATION:
        case USAGE_NOTIFICATION_COMMUNICATION_REQUEST:
        case USAGE_NOTIFICATION_COMMUNICATION_INSTANT:
        case USAGE_NOTIFICATION_COMMUNICATION_DELAYED:
        case USAGE_NOTIFICATION_EVENT:
            *result = IAudioSystem::STREAM_NOTIFICATION;
            return NOERROR;
        case USAGE_UNKNOWN:
        default:
            *result = IAudioSystem::STREAM_MUSIC;
            return NOERROR;
    }
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
