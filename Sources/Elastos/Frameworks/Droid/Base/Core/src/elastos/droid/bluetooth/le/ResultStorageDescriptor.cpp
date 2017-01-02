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

#include "elastos/droid/bluetooth/le/ResultStorageDescriptor.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                       ResultStorageDescriptor
//=====================================================================
CAR_INTERFACE_IMPL_2(ResultStorageDescriptor, Object, IResultStorageDescriptor, IParcelable);

ResultStorageDescriptor::ResultStorageDescriptor()
{
    mType = 0;
    mOffset = 0;
    mLength = 0;
}

ECode ResultStorageDescriptor::constructor()
{
    return NOERROR;
}

ECode ResultStorageDescriptor::constructor(
    /* [in] */ Int32 type,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    mType = type;
    mOffset = offset;
    mLength = length;
    return NOERROR;
}

ECode ResultStorageDescriptor::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mType;
    return NOERROR;
}

ECode ResultStorageDescriptor::GetOffset(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOffset;
    return NOERROR;
}

ECode ResultStorageDescriptor::GetLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLength;
    return NOERROR;
}

ECode ResultStorageDescriptor::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mType);
    dest->WriteInt32(mOffset);
    dest->WriteInt32(mLength);
    return NOERROR;
}

ECode ResultStorageDescriptor::constructor(
    /* [in] */ IParcel* in)
{
    ReadFromParcel(in);
    return NOERROR;
}

ECode ResultStorageDescriptor::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mType);
    in->ReadInt32(&mOffset);
    in->ReadInt32(&mLength);
    return NOERROR;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
