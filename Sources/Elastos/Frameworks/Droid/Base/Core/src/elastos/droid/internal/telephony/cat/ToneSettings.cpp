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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/ToneSettings.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                 ToneSettings::InnerParcelableCreator
////=====================================================================
//ToneSettings::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ ToneSettings* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode ToneSettings::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ ToneSettings** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new ToneSettings(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode ToneSettings::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ ToneSettings[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new ToneSettings[size];
//    assert(0);
//    return NOERROR;
//}
//
////=====================================================================
////                             ToneSettings
////=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<ToneSettings> > ToneSettings::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL_2(ToneSettings, Object, IToneSettings, IParcelable);

ToneSettings::ToneSettings()
{
}

ECode ToneSettings::constructor()
{
    return NOERROR;
}

ECode ToneSettings::constructor(
    /* [in] */ IDuration* duration,
    /* [in] */ Tone tone,
    /* [in] */ Boolean vibrate)
{
    mDuration = duration;
    mTone = tone;
    mVibrate = vibrate;
    return NOERROR;
}

//ECode ToneSettings::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode ToneSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    IParcelable::Probe(mDuration)->WriteToParcel(dest);
    dest->WriteInt32(mTone);
    dest->WriteInt32(mVibrate ? 1 : 0);
    return NOERROR;
}

ECode ToneSettings::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    IParcelable::Probe(mDuration)->ReadFromParcel(in);
    in->ReadInt32(&mTone);
    Int32 vibrate = 0;
    in->ReadInt32(&vibrate);
    mVibrate = vibrate == 1;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
