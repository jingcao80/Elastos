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

#include "elastos/droid/hardware/soundtrigger/CSoundTriggerRecognitionConfig.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL_2(CSoundTriggerRecognitionConfig, Object, ISoundTriggerRecognitionConfig, IParcelable)

CAR_OBJECT_IMPL(CSoundTriggerRecognitionConfig)

CSoundTriggerRecognitionConfig::CSoundTriggerRecognitionConfig()
    : mCaptureRequested(FALSE)
    , mAllowMultipleTriggers(FALSE)
    , mKeyphrases(NULL)
    , mData(NULL)
{
}

ECode CSoundTriggerRecognitionConfig::constructor()
{
    return NOERROR;
}

ECode CSoundTriggerRecognitionConfig::constructor(
    /* [in] */ Boolean captureRequested,
    /* [in] */ Boolean allowMultipleTriggers,
    /* [in] */ ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*>* keyphrases,
    /* [in] */ ArrayOf<Byte>* data)
{
    mCaptureRequested = captureRequested;
    mAllowMultipleTriggers = allowMultipleTriggers;
    mKeyphrases = keyphrases;
    mData = data;
    return NOERROR;
}

ECode CSoundTriggerRecognitionConfig::FromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ ISoundTriggerRecognitionConfig** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean captureRequested ;
    FAIL_RETURN(source->ReadBoolean(&captureRequested))
    Boolean allowMultipleTriggers;
    FAIL_RETURN(source->ReadBoolean(&allowMultipleTriggers))

    AutoPtr<ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*> > keyphrases;
    assert(0 && "TODO: createTypedArray");
    //in.createTypedArray(KeyphraseRecognitionExtra.CREATOR);
    AutoPtr<ArrayOf<Byte> > data;
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&data))

    AutoPtr<CSoundTriggerRecognitionConfig> newResult;
    CSoundTriggerRecognitionConfig::NewByFriend(captureRequested, allowMultipleTriggers,
            keyphrases, data, (CSoundTriggerRecognitionConfig**)&newResult);
    *result = newResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSoundTriggerRecognitionConfig::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteBoolean(mCaptureRequested))
    FAIL_RETURN(dest->WriteBoolean(mAllowMultipleTriggers))
    assert(0 && "TODO: writeTypedArray");
    //dest->writeTypedArray(keyphrases, flags);
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mData.Get()))

    return NOERROR;
}

ECode CSoundTriggerRecognitionConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadBoolean(&mCaptureRequested))
    FAIL_RETURN(source->ReadBoolean(&mAllowMultipleTriggers))

    assert(0 && "TODO: createTypedArray");
    //in.createTypedArray(KeyphraseRecognitionExtra.CREATOR);
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mData))

    return NOERROR;
}

ECode CSoundTriggerRecognitionConfig::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "RecognitionConfig [captureRequested=";
    sb += mCaptureRequested;
    sb += ", allowMultipleTriggers=";
    sb += mAllowMultipleTriggers;
    sb += ", keyphrases=";
    sb += Arrays::ToString(mKeyphrases);
    sb += ", data=";
    sb += Arrays::ToString(mData);
    sb += "]";

    return sb.ToString(str);
}

ECode CSoundTriggerRecognitionConfig::GetCaptureRequested(
    /* [out] */ Boolean* requested)
{
    VALIDATE_NOT_NULL(requested);

    *requested = mCaptureRequested;
    return NOERROR;
}

ECode CSoundTriggerRecognitionConfig::GetAllowMultipleTriggers(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mAllowMultipleTriggers;
    return NOERROR;
}

ECode CSoundTriggerRecognitionConfig::GetKeyphrases(
    /* [out, callee] */ ArrayOf<ISoundTriggerKeyphraseRecognitionExtra*>** phrases)
{
    VALIDATE_NOT_NULL(phrases);

    *phrases = mKeyphrases;
    REFCOUNT_ADD(*phrases);
    return NOERROR;
}

ECode CSoundTriggerRecognitionConfig::GetData(
    /* [out, callee] */ ArrayOf<Byte>** data)
{
    VALIDATE_NOT_NULL(data);

    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos
