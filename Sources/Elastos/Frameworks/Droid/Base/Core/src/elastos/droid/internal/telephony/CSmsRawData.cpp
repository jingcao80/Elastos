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

#include "elastos/droid/internal/telephony/CSmsRawData.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CAR_INTERFACE_IMPL_2(CSmsRawData, Object, ISmsRawData, IParcelable)

CAR_OBJECT_IMPL(CSmsRawData)

CSmsRawData::CSmsRawData()
{
}

CSmsRawData::~CSmsRawData()
{
}

ECode CSmsRawData::constructor()
{
    return NOERROR;
}

ECode CSmsRawData::constructor(
    /* [in] */ ArrayOf<Byte>* data)
{
    mData = data;
    return NOERROR;
}

ECode CSmsRawData::GetBytes(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = mData;
    REFCOUNT_ADD(*bytes)
    return NOERROR;
}

ECode CSmsRawData::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadArrayOf((HANDLE*)(&mData));
    return NOERROR;
}

ECode CSmsRawData::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteArrayOf((HANDLE)mData.Get());
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos