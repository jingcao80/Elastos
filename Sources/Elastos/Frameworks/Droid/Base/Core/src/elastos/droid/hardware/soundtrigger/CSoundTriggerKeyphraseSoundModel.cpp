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

#include "elastos/droid/hardware/soundtrigger/CSoundTriggerKeyphraseSoundModel.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL(CSoundTriggerKeyphraseSoundModel, SoundTriggerSoundModel, ISoundTriggerKeyphraseSoundModel)

CAR_OBJECT_IMPL(CSoundTriggerKeyphraseSoundModel)

CSoundTriggerKeyphraseSoundModel::CSoundTriggerKeyphraseSoundModel()
    : mKeyphrases(NULL)
{
}

ECode CSoundTriggerKeyphraseSoundModel::constructor()
{
    return NOERROR;
}

ECode CSoundTriggerKeyphraseSoundModel::constructor(
    /* [in] */ IUUID* uuid,
    /* [in] */ IUUID* vendorUuid,
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ ArrayOf<ISoundTriggerKeyphrase*>* keyphrases)
{
    FAIL_RETURN(SoundTriggerSoundModel::constructor(uuid, vendorUuid,
            ISoundTriggerSoundModel::TYPE_KEYPHRASE, data))
    mKeyphrases = keyphrases;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseSoundModel::FromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ ISoundTriggerKeyphraseSoundModel** result)
{
    VALIDATE_NOT_NULL(result);

    String tmp;
    AutoPtr<IUUID> uuid;
    FAIL_RETURN(source->ReadString(&tmp))
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    FAIL_RETURN(helper->FromString(tmp,(IUUID**)&uuid))

    AutoPtr<IUUID> vendorUuid;
    Int32 length;
    FAIL_RETURN(source->ReadInt32(&length))
    if (length >= 0) {
        String str;
        FAIL_RETURN(source->ReadString(&str))
        FAIL_RETURN(helper->FromString(str,(IUUID**)&vendorUuid))
    }
    AutoPtr<ArrayOf<Byte> > data;
    AutoPtr<ArrayOf<ISoundTriggerKeyphrase*> > keyphrases;
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&data))
    assert(0 && "TODO: Keyphrase.CREATOR");
    // FAIL_RETURN(source->ReadArrayOf((Handle32*)&mKeyphrases)) //Keyphrase[] keyphrases = in.createTypedArray(Keyphrase.CREATOR);

    AutoPtr<CSoundTriggerKeyphraseSoundModel> newResult;
    CSoundTriggerKeyphraseSoundModel::NewByFriend(uuid, vendorUuid, data,
            keyphrases, (CSoundTriggerKeyphraseSoundModel**)&newResult);
    *result = newResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSoundTriggerKeyphraseSoundModel::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    String tmp;
    FAIL_RETURN(mUuid->ToString(&tmp))
    FAIL_RETURN(dest->WriteString(tmp))
    if (mVendorUuid == NULL) {
        FAIL_RETURN(dest->WriteInt32(-1))
    } else {
        String str;
        FAIL_RETURN(mVendorUuid->ToString(&str))
        FAIL_RETURN(dest->WriteInt32(str.GetLength()))
        FAIL_RETURN(dest->WriteString(str))
    }
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mData.Get()))
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mKeyphrases.Get()))
    return NOERROR;
}

ECode CSoundTriggerKeyphraseSoundModel::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    String tmp;
    FAIL_RETURN(source->ReadString(&tmp))
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    FAIL_RETURN(helper->FromString(tmp,(IUUID**)&mUuid))

    Int32 len;
    FAIL_RETURN(source->ReadInt32(&len))
    if (len > 0) {
        String str;
        FAIL_RETURN(source->ReadString(&str))
        FAIL_RETURN(helper->FromString(str,(IUUID**)&mVendorUuid))
    }
    else {
        mVendorUuid = NULL;
    }
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mData))
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mKeyphrases))

    return NOERROR;
}

ECode CSoundTriggerKeyphraseSoundModel::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "KeyphraseSoundModel [keyphrases=";
    sb += Arrays::ToString(mKeyphrases);
    sb += ", uuid=";
    sb += mUuid;
    sb += ", vendorUuid=";
    sb += mVendorUuid;
    sb += ", type=";
    sb += mType;
    sb += ", data=";
    sb += (mData == NULL ? 0 : mData->GetLength());
    sb += "]";

    return sb.ToString(str);
}

ECode CSoundTriggerKeyphraseSoundModel::GetKeyphrases(
    /* [out, callee] */ ArrayOf<ISoundTriggerKeyphrase*>** keyphrases)
{
    VALIDATE_NOT_NULL(keyphrases);

    *keyphrases = mKeyphrases;
    REFCOUNT_ADD(*keyphrases);
    return NOERROR;
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos
