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

#include "elastos/droid/internal/telephony/UUSInfo.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  UUSInfo::
//==============================================================
CAR_INTERFACE_IMPL(UUSInfo, Object, IUUSInfo)

UUSInfo::UUSInfo()
{
    mUusType = UUS_TYPE1_IMPLICIT;
    mUusDcs = UUS_DCS_IA5c;
    mUusData = NULL;
}

ECode UUSInfo::constructor(
    /* [in] */ Int32 uusType,
    /* [in] */ Int32 uusDcs,
    /* [in] */ ArrayOf<Byte>* uusData)
{
    mUusType = uusType;
    mUusDcs = uusDcs;
    mUusData = uusData;
    return NOERROR;
}

ECode UUSInfo::GetDcs(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUusDcs;
    return NOERROR;
}

ECode UUSInfo::SetDcs(
    /* [in] */ Int32 uusDcs)
{
    mUusDcs = uusDcs;
    return NOERROR;
}

ECode UUSInfo::GetType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUusType;
    return NOERROR;
}

ECode UUSInfo::SetType(
    /* [in] */ Int32 uusType)
{
    mUusType = uusType;
    return NOERROR;
}

ECode UUSInfo::GetUserData(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUusData;
    return NOERROR;
}

ECode UUSInfo::SetUserData(
    /* [in] */ ArrayOf<Byte>* uusData)
{
    mUusData = uusData;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
