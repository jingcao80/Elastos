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

#include "elastos/droid/telephony/SmsCbLocation.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Telephony {

//=====================================================================
//                            SmsCbLocation
//=====================================================================
CAR_INTERFACE_IMPL_2(SmsCbLocation, Object, ISmsCbLocation, IParcelable);

SmsCbLocation::SmsCbLocation()
    : mLac(-1)
    , mCid(-1)
{
    mPlmn = String("");
}

ECode SmsCbLocation::constructor(
    /* [in*/ const String& plmn)
{
    mPlmn = plmn;
    mLac = -1;
    mCid = -1;
    return NOERROR;
}

ECode SmsCbLocation::constructor(
    /* [in*/ const String& plmn,
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid)
{
    mPlmn = plmn;
    mLac = lac;
    mCid = cid;
    return NOERROR;
}

ECode SmsCbLocation::constructor()
{
    return NOERROR;
}

ECode SmsCbLocation::GetPlmn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPlmn;
    return NOERROR;
}

ECode SmsCbLocation::GetLac(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLac;
    return NOERROR;
}

ECode SmsCbLocation::GetCid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCid;
    return NOERROR;
}

ECode SmsCbLocation::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 hash = mPlmn.GetHashCode();
    hash = hash * 31 + mLac;
    hash = hash * 31 + mCid;
    *result = hash;
    return NOERROR;
}

ECode SmsCbLocation::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (o == TO_IINTERFACE(this)) {
        *result = TRUE;
        return NOERROR;
    }
    if (o == NULL || (ISmsCbLocation::Probe(o) == NULL)) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<SmsCbLocation> other = (SmsCbLocation*)(IObject*)o;
    *result = mPlmn.Equals(other->mPlmn) && mLac == other->mLac && mCid == other->mCid;
    return NOERROR;
}

ECode SmsCbLocation::ToString(
    /* [out] */ String* result)
{
    *result = String("[") + mPlmn + "," + StringUtils::ToString(mLac)
            + "," + StringUtils::ToString(mCid) + "]";
    return NOERROR;
}

ECode SmsCbLocation::IsInLocationArea(
    /* [in] */ ISmsCbLocation* _area,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<SmsCbLocation> area = (SmsCbLocation*)_area;
    if (mCid != -1 && mCid != area->mCid) {
        *result = FALSE;
        return NOERROR;
    }
    if (mLac != -1 && mLac != area->mLac) {
        *result = FALSE;
        return NOERROR;
    }
    *result = mPlmn.Equals(area->mPlmn);
    return NOERROR;
}

ECode SmsCbLocation::IsInLocationArea(
    /* [in*/ const String& plmn,
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mPlmn.Equals(plmn)) {
        *result = FALSE;
        return NOERROR;
    }

    if (mLac != -1 && mLac != lac) {
        *result = FALSE;
        return NOERROR;
    }

    if (mCid != -1 && mCid != cid) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode SmsCbLocation::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mPlmn);
    dest->WriteInt32(mLac);
    dest->WriteInt32(mCid);
    return NOERROR;
}

ECode SmsCbLocation::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mPlmn);
    source->ReadInt32(&mLac);
    source->ReadInt32(&mCid);
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos


