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

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/TextMessage.h"
#include "elastos/droid/internal/telephony/cat/CDuration.h"
#include "elastos/droid/graphics/CBitmap.h"

using Elastos::Droid::Graphics::CBitmap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                 TextMessage::InnerParcelableCreator
////=====================================================================
//TextMessage::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ TextMessage* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode TextMessage::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ TextMessage** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new TextMessage(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode TextMessage::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ TextMessage[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new TextMessage[size];
//    assert(0);
//    return NOERROR;
//}
//
//=====================================================================
//                             TextMessage
//=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<TextMessage> > TextMessage::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL_2(TextMessage, Object, ITextMessage, IParcelable);

TextMessage::TextMessage()
{
}

ECode TextMessage::constructor()
{
    return NOERROR;
}

//ECode TextMessage::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode TextMessage::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    dest->WriteString(mTitle);
    dest->WriteString(mText);
    if (mIcon) {
        dest->WriteInt32(1);
        IParcelable::Probe(mIcon)->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }
    dest->WriteInt32(mIconSelfExplanatory ? 1 : 0);
    dest->WriteInt32(mIsHighPriority ? 1 : 0);
    dest->WriteInt32(mResponseNeeded ? 1 : 0);
    dest->WriteInt32(mUserClear ? 1 : 0);
    if (mDuration) {
        dest->WriteInt32(1);
        IParcelable::Probe(mDuration)->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }
    return NOERROR;
}

ECode TextMessage::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mTitle);
    in->ReadString(&mText);
    Int32 value;

    mIcon = NULL;
    in->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IBitmap> bitmap;
        CBitmap::New((IBitmap**)&bitmap);
        IParcelable* parcel = IParcelable::Probe(bitmap);
        parcel->ReadFromParcel(in);
        mIcon = bitmap;
    }

    Int32 iconSelfExplanatory = 0;
    in->ReadInt32(&iconSelfExplanatory);
    mIconSelfExplanatory = iconSelfExplanatory == 1 ? TRUE : FALSE;
    Int32 isHighPriority = 0;
    in->ReadInt32(&isHighPriority);
    mIsHighPriority = isHighPriority == 1 ? TRUE : FALSE;
    Int32 responseNeeded = 0;
    in->ReadInt32(&responseNeeded);
    mResponseNeeded = responseNeeded == 1 ? TRUE : FALSE;
    Int32 userClear = 0;
    in->ReadInt32(&userClear);
    mUserClear = userClear == 1 ? TRUE : FALSE;

    mDuration = NULL;
    in->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IDuration> duration;
        CDuration::New((IDuration**)&duration);
        IParcelable* parcel = IParcelable::Probe(duration);
        parcel->ReadFromParcel(in);
        mDuration = duration;
    }

    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
