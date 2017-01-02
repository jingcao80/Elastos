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

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerModuleProperties.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL_2(CSoundTriggerModuleProperties, Object, ISoundTriggerModuleProperties, IParcelable)
CAR_OBJECT_IMPL(CSoundTriggerModuleProperties)

CSoundTriggerModuleProperties::CSoundTriggerModuleProperties()
    : mId(0)
    , mImplementor(NULL)
    , mDescription(NULL)
    , mUuid(NULL)
    , mVersion(0)
    , mMaxSoundModels(0)
    , mMaxKeyphrases(0)
    , mMaxUsers(0)
    , mRecognitionModes(0)
    , mSupportsCaptureTransition(FALSE)
    , mMaxBufferMs(0)
    , mSupportsConcurrentCapture(FALSE)
    , mPowerConsumptionMw(0)
    , mReturnsTriggerInEvent(FALSE)
{
}

ECode CSoundTriggerModuleProperties::constructor()
{
    return NOERROR;
}

ECode CSoundTriggerModuleProperties::constructor(
    /* [in] */ Int32 id,
    /* [in] */ const String& implementor,
    /* [in] */ const String& description,
    /* [in] */ const String& uuid,
    /* [in] */ Int32 version,
    /* [in] */ Int32 maxSoundModels,
    /* [in] */ Int32 maxKeyphrases,
    /* [in] */ Int32 maxUsers,
    /* [in] */ Int32 recognitionModes,
    /* [in] */ Boolean supportsCaptureTransition,
    /* [in] */ Int32 maxBufferMs,
    /* [in] */ Boolean supportsConcurrentCapture,
    /* [in] */ Int32 powerConsumptionMw,
    /* [in] */ Boolean returnsTriggerInEvent)
{
    mId = id;
    mImplementor = implementor;
    mDescription = description;

    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    FAIL_RETURN(helper->FromString(uuid,(IUUID**)&mUuid))

    mVersion = version;
    mMaxSoundModels = maxSoundModels;
    mMaxKeyphrases = maxKeyphrases;
    mMaxUsers = maxUsers;
    mRecognitionModes = recognitionModes;
    mSupportsCaptureTransition = supportsCaptureTransition;
    mMaxBufferMs = maxBufferMs;
    mSupportsConcurrentCapture = supportsConcurrentCapture;
    mPowerConsumptionMw = powerConsumptionMw;
    mReturnsTriggerInEvent = returnsTriggerInEvent;
    return NOERROR;
}

ECode CSoundTriggerModuleProperties::FromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ ISoundTriggerModuleProperties** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    FAIL_RETURN(source->ReadInt32(&id))
    String implementor;
    FAIL_RETURN(source->ReadString(&implementor))
    String description;
    FAIL_RETURN(source->ReadString(&description))
    String uuid;
    FAIL_RETURN(source->ReadString(&uuid))
    Int32 version;
    FAIL_RETURN(source->ReadInt32(&version))
    Int32 maxSoundModels;
    FAIL_RETURN(source->ReadInt32(&maxSoundModels))
    Int32 maxKeyphrases;
    FAIL_RETURN(source->ReadInt32(&maxKeyphrases))
    Int32 maxUsers;
    FAIL_RETURN(source->ReadInt32(&maxUsers))
    Int32 recognitionModes;
    FAIL_RETURN(source->ReadInt32(&recognitionModes))
    Boolean supportsCaptureTransition;
    FAIL_RETURN(source->ReadBoolean(&supportsCaptureTransition))
    Int32 maxBufferMs;
    FAIL_RETURN(source->ReadInt32(&maxBufferMs))
    Boolean supportsConcurrentCapture;
    FAIL_RETURN(source->ReadBoolean(&supportsConcurrentCapture))
    Int32 powerConsumptionMw;
    FAIL_RETURN(source->ReadInt32(&powerConsumptionMw))
    Boolean returnsTriggerInEvent;
    FAIL_RETURN(source->ReadBoolean(&returnsTriggerInEvent))

    AutoPtr<CSoundTriggerModuleProperties> newResult;
    CSoundTriggerModuleProperties::NewByFriend(id, implementor, description, uuid, version,
            maxSoundModels, maxKeyphrases, maxUsers, recognitionModes,
            supportsCaptureTransition, maxBufferMs, supportsConcurrentCapture,
            powerConsumptionMw, returnsTriggerInEvent, (CSoundTriggerModuleProperties**)&newResult);
    *result = newResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSoundTriggerModuleProperties::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteInt32(mId))
    FAIL_RETURN(dest->WriteString(mImplementor))
    FAIL_RETURN(dest->WriteString(mDescription))
    String tmp;
    FAIL_RETURN(mUuid->ToString(&tmp))
    FAIL_RETURN(dest->WriteString(tmp))
    FAIL_RETURN(dest->WriteInt32(mVersion))
    FAIL_RETURN(dest->WriteInt32(mMaxSoundModels))
    FAIL_RETURN(dest->WriteInt32(mMaxKeyphrases))
    FAIL_RETURN(dest->WriteInt32(mMaxUsers))
    FAIL_RETURN(dest->WriteInt32(mRecognitionModes))
    FAIL_RETURN(dest->WriteBoolean(mSupportsCaptureTransition))
    FAIL_RETURN(dest->WriteInt32(mMaxBufferMs))
    FAIL_RETURN(dest->WriteBoolean(mSupportsConcurrentCapture))
    FAIL_RETURN(dest->WriteInt32(mPowerConsumptionMw))
    FAIL_RETURN(dest->WriteBoolean(mReturnsTriggerInEvent))
    return NOERROR;
}

ECode CSoundTriggerModuleProperties::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mId))
    FAIL_RETURN(source->ReadString(&mImplementor))
    FAIL_RETURN(source->ReadString(&mDescription))
    String tmp;
    FAIL_RETURN(source->ReadString(&tmp))
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    FAIL_RETURN(helper->FromString(tmp,(IUUID**)&mUuid))
    FAIL_RETURN(source->ReadInt32(&mVersion))
    FAIL_RETURN(source->ReadInt32(&mMaxSoundModels))
    FAIL_RETURN(source->ReadInt32(&mMaxKeyphrases))
    FAIL_RETURN(source->ReadInt32(&mMaxUsers))
    FAIL_RETURN(source->ReadInt32(&mRecognitionModes))
    FAIL_RETURN(source->ReadBoolean(&mSupportsCaptureTransition))
    FAIL_RETURN(source->ReadInt32(&mMaxBufferMs))
    FAIL_RETURN(source->ReadBoolean(&mSupportsConcurrentCapture))
    FAIL_RETURN(source->ReadInt32(&mPowerConsumptionMw))
    FAIL_RETURN(source->ReadBoolean(&mReturnsTriggerInEvent))
    return NOERROR;
}

//@Override
ECode CSoundTriggerModuleProperties::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "ModuleProperties [id=";
    sb += mId;
    sb += ", implementor=";
    sb += mImplementor;
    sb += ", description=";
    sb += mDescription;
    sb += ", uuid=";
    String tmp;
    FAIL_RETURN(mUuid->ToString(&tmp))
    sb += tmp;
    sb += ", version=";
    sb += mVersion;
    sb += ", maxSoundModels=";
    sb += mMaxSoundModels;
    sb += ", maxKeyphrases=";
    sb += mMaxKeyphrases;
    sb += ", maxUsers=";
    sb += mMaxUsers;
    sb += ", recognitionModes=";
    sb += mRecognitionModes;
    sb += ", supportsCaptureTransition=";
    sb += mSupportsCaptureTransition;
    sb += ", maxBufferMs=";
    sb += mMaxBufferMs;
    sb += ", supportsConcurrentCapture=";
    sb += mSupportsConcurrentCapture;
    sb += ", powerConsumptionMw=";
    sb += mPowerConsumptionMw;
    sb += ", returnsTriggerInEvent=";
    sb += mReturnsTriggerInEvent;
    sb += "]";

    return sb.ToString(str);
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos