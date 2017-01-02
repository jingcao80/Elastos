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

#include "elastos/droid/hardware/hdmi/HdmiPortInfo.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL_2(HdmiPortInfo, Object, IHdmiPortInfo, IParcelable)

HdmiPortInfo::HdmiPortInfo()
    : mId(0)
    , mType(0)
    , mAddress(0)
    , mCecSupported(FALSE)
    , mArcSupported(FALSE)
    , mMhlSupported(FALSE)
{
}

ECode HdmiPortInfo::constructor()
{
    return NOERROR;
}

ECode HdmiPortInfo::constructor(
    /* [in] */ Int32 id,
    /* [in] */ Int32 type,
    /* [in] */ Int32 address,
    /* [in] */ Boolean cec,
    /* [in] */ Boolean mhl,
    /* [in] */ Boolean arc)
{
    mId = id;
    mType = type;
    mAddress = address;
    mCecSupported = cec;
    mArcSupported = arc;
    mMhlSupported = mhl;
    return NOERROR;
}

ECode HdmiPortInfo::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mId;
    return NOERROR;
}

ECode HdmiPortInfo::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = mType;
    return NOERROR;
}

ECode HdmiPortInfo::GetAddress(
    /* [out] */ Int32* address)
{
    VALIDATE_NOT_NULL(address);

    *address = mAddress;
    return NOERROR;
}

ECode HdmiPortInfo::IsCecSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mCecSupported;
    return NOERROR;
}

ECode HdmiPortInfo::IsMhlSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mMhlSupported;
    return NOERROR;
}

ECode HdmiPortInfo::IsArcSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mArcSupported;
    return NOERROR;
}

ECode HdmiPortInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mId);
    source->ReadInt32(&mType);
    source->ReadInt32(&mAddress);
    source->ReadBoolean(&mCecSupported);
    source->ReadBoolean(&mArcSupported);
    source->ReadBoolean(&mMhlSupported);

    return NOERROR;
}

ECode HdmiPortInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mId);
    dest->WriteInt32(mType);
    dest->WriteInt32(mAddress);
    dest->WriteBoolean(mCecSupported);
    dest->WriteBoolean(mArcSupported);
    dest->WriteBoolean(mMhlSupported);

    return NOERROR;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
