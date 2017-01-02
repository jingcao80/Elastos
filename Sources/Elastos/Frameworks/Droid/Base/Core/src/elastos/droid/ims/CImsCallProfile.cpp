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

#include "elastos/droid/ims/CImsCallProfile.h"
#include "elastos/droid/ims/CImsStreamMediaProfile.h"
#include "elastos/droid/os/CBundle.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Telecom::IVideoProfileVideoState;
using Elastos::Core::ICloneable;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL_2(CImsCallProfile, Object, IImsCallProfile, IParcelable)

CAR_OBJECT_IMPL(CImsCallProfile)

CImsCallProfile::CImsCallProfile()
    : mServiceType(SERVICE_TYPE_NORMAL)
    , mCallType(CALL_TYPE_VOICE_N_VIDEO)
{
}

CImsCallProfile::~CImsCallProfile()
{
}

ECode CImsCallProfile::constructor()
{
    CBundle::New((IBundle**)&mCallExtras);
    CImsStreamMediaProfile::New((IImsStreamMediaProfile**)&mMediaProfile);
    return NOERROR;
}

ECode CImsCallProfile::constructor(
    /* [in] */ Int32 serviceType,
    /* [in] */ Int32 callType)
{
    mServiceType = serviceType;
    mCallType = callType;
    CBundle::New((IBundle**)&mCallExtras);
    CImsStreamMediaProfile::New((IImsStreamMediaProfile**)&mMediaProfile);
    return NOERROR;
}

ECode CImsCallProfile::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mServiceType);
    source->ReadInt32(&mCallType);

    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mCallExtras = IBundle::Probe(obj);

    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mMediaProfile = IImsStreamMediaProfile::Probe(obj);

    return NOERROR;
}

ECode CImsCallProfile::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mServiceType);
    dest->WriteInt32(mCallType);
    dest->WriteInterfacePtr(mCallExtras);
    dest->WriteInterfacePtr(mMediaProfile);
    return NOERROR;
}

ECode CImsCallProfile::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    sb.Append("{ serviceType=");
    sb.Append(mServiceType);
    sb.Append(", callType=");
    sb.Append(mCallType);

    String str;
    sb.Append(", callExtras=");
    IObject::Probe(mCallExtras)->ToString(&str);
    sb.Append(str);
    sb.Append(", mediaProfile=");
    IObject::Probe(mMediaProfile)->ToString(&str);
    sb.Append(str);
    sb.Append(" }");

    return sb.ToString(result);
}

ECode CImsCallProfile::GetCallExtra(
    /* [in] */ const String& name,
    /* [out] */ String* result)
{
    return GetCallExtra(name, String(""), result);
}

ECode CImsCallProfile::GetCallExtra(
    /* [in] */ const String& name,
    /* [in] */ const String& defaultValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    if (mCallExtras == NULL) {
        *result = defaultValue;
        return NOERROR;
    }

    return mCallExtras->GetString(name, defaultValue, result);
}

ECode CImsCallProfile::GetCallExtraBoolean(
    /* [in] */ const String& name,
    /* [out] */ Boolean* result)
{
    return GetCallExtraBoolean(name, FALSE, result);
}

ECode CImsCallProfile::GetCallExtraBoolean(
    /* [in] */ const String& name,
    /* [in] */ Boolean defaultValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (mCallExtras == NULL) {
        *result = defaultValue;
        return NOERROR;
    }

    return mCallExtras->GetBoolean(name, defaultValue, result);
}

ECode CImsCallProfile::GetCallExtraInt32(
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    return GetCallExtraInt32(name, -1, result);
}

ECode CImsCallProfile::GetCallExtraInt32(
    /* [in] */ const String& name,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (mCallExtras == NULL) {
        *result = defaultValue;
        return NOERROR;
    }

    return mCallExtras->GetInt32(name, defaultValue, result);
}

ECode CImsCallProfile::SetCallExtra(
    /* [in] */ const String& name,
    /* [in] */ const String& value)
{
    if (mCallExtras != NULL) {
        mCallExtras->PutString(name, value);
    }
    return NOERROR;
}

ECode CImsCallProfile::SetCallExtraBoolean(
    /* [in] */ const String& name,
    /* [in] */ Boolean value)
{
    if (mCallExtras != NULL) {
        mCallExtras->PutBoolean(name, value);
    }
    return NOERROR;
}

ECode CImsCallProfile::SetCallExtraInt32(
    /* [in] */ const String& name,
    /* [in] */ Int32 value)
{
    if (mCallExtras != NULL) {
        mCallExtras->PutInt32(name, value);
    }
    return NOERROR;
}

ECode CImsCallProfile::UpdateCallType(
    /* [in] */ IImsCallProfile* profile)
{
    mCallType = ((CImsCallProfile*)profile)->mCallType;
    return NOERROR;
}

ECode CImsCallProfile::UpdateCallExtras(
    /* [in] */ IImsCallProfile* profile)
{
    mCallExtras->Clear();
    AutoPtr<CImsCallProfile> pro = (CImsCallProfile*)profile;
    AutoPtr<IInterface> obj;
    ICloneable::Probe(pro->mCallExtras)->Clone((IInterface**)&obj);
    mCallExtras = IBundle::Probe(obj);
    return NOERROR;
}

ECode CImsCallProfile::GetVideoStateFromCallType(
    /* [in] */ Int32 callType,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    switch (callType) {
        case CALL_TYPE_VT_NODIR:
            *result = IVideoProfileVideoState::PAUSED |
                    IVideoProfileVideoState::BIDIRECTIONAL;
            return NOERROR;
        case CALL_TYPE_VT_TX:
            *result = IVideoProfileVideoState::TX_ENABLED;
            return NOERROR;
        case CALL_TYPE_VT_RX:
            *result = IVideoProfileVideoState::RX_ENABLED;
            return NOERROR;
        case CALL_TYPE_VT:
            *result = IVideoProfileVideoState::BIDIRECTIONAL;
            return NOERROR;
        case CALL_TYPE_VOICE:
            *result = IVideoProfileVideoState::AUDIO_ONLY;
            return NOERROR;
        default:
            *result = IVideoProfileVideoState::AUDIO_ONLY;
    }

    return NOERROR;
}

ECode CImsCallProfile::GetCallTypeFromVideoState(
    /* [in] */ Int32 videoState,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean videoTx = IsVideoStateSet(videoState, IVideoProfileVideoState::TX_ENABLED);
    Boolean videoRx = IsVideoStateSet(videoState, IVideoProfileVideoState::RX_ENABLED);
    Boolean isPaused = IsVideoStateSet(videoState, IVideoProfileVideoState::PAUSED);

    if (isPaused) {
        *result = IImsCallProfile::CALL_TYPE_VT_NODIR;
        return NOERROR;
    }
    else if (videoTx && !videoRx) {
        *result = IImsCallProfile::CALL_TYPE_VT_TX;
        return NOERROR;
    }
    else if (!videoTx && videoRx) {
        *result = IImsCallProfile::CALL_TYPE_VT_RX;
        return NOERROR;
    }
    else if (videoTx && videoRx) {
        *result = IImsCallProfile::CALL_TYPE_VT;
        return NOERROR;
    }
    *result = IImsCallProfile::CALL_TYPE_VOICE;
    return NOERROR;

}

ECode CImsCallProfile::PresentationToOIR(
    /* [in] */ Int32 presentation,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    switch (presentation) {
        case IPhoneConstants::PRESENTATION_RESTRICTED:
            *result = IImsCallProfile::OIR_PRESENTATION_RESTRICTED;
            return NOERROR;
        case IPhoneConstants::PRESENTATION_ALLOWED:
            *result = IImsCallProfile::OIR_PRESENTATION_NOT_RESTRICTED;
            return NOERROR;
        default:
            *result = IImsCallProfile::OIR_DEFAULT;
    }

    return NOERROR;
}

ECode CImsCallProfile::OIRToPresentation(
    /* [in] */ Int32 oir,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    switch(oir) {
        case IImsCallProfile::OIR_PRESENTATION_RESTRICTED:
            *result = IPhoneConstants::PRESENTATION_RESTRICTED;
            return NOERROR;
        case IImsCallProfile::OIR_PRESENTATION_NOT_RESTRICTED:
            *result = IPhoneConstants::PRESENTATION_ALLOWED;
            return NOERROR;
        default:
            *result = IPhoneConstants::PRESENTATION_UNKNOWN;
    }

    return NOERROR;
}

Boolean CImsCallProfile::IsVideoStateSet(
    /* [in] */ Int32 videoState,
    /* [in] */ Int32 videoStateToCheck)
{
    return (videoState & videoStateToCheck) == videoStateToCheck;
}

} // namespace Ims
} // namespace Droid
} // namespace Elastos