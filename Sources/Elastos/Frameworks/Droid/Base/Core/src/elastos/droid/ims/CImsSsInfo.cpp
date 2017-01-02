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

#include "elastos/droid/ims/CImsSsInfo.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL_2(CImsSsInfo, Object, IImsSsInfo, IParcelable)

CAR_OBJECT_IMPL(CImsSsInfo)

CImsSsInfo::CImsSsInfo()
    : mStatus(0)
{
}

CImsSsInfo::~CImsSsInfo()
{
}

ECode CImsSsInfo::constructor()
{
    return NOERROR;
}

ECode CImsSsInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mStatus);
    source->ReadString(&mIcbNum);
    return NOERROR;
}

ECode CImsSsInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mStatus);
    dest->WriteString(mIcbNum);
    return NOERROR;
}

ECode CImsSsInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    StringBuilder sb;
    sb.Append("{ Status=");
    sb.Append((mStatus == 0) ? "disabled" : "enabled");
    sb.Append(" }");

    return sb.ToString(result);
}

} // namespace Ims
} // namespace Droid
} // namespace Elastos