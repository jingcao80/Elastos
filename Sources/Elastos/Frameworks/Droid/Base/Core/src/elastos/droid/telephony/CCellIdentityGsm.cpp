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

#include "elastos/droid/telephony/CCellIdentityGsm.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellIdentityGsm::TAG("CCellIdentityGsm");
const Boolean CCellIdentityGsm::DBG;

CAR_INTERFACE_IMPL_2(CCellIdentityGsm, Object, ICellIdentityGsm, IParcelable)

CAR_OBJECT_IMPL(CCellIdentityGsm)

CCellIdentityGsm::CCellIdentityGsm()
    : mMcc(Elastos::Core::Math::INT32_MAX_VALUE)
    , mMnc(Elastos::Core::Math::INT32_MAX_VALUE)
    , mLac(Elastos::Core::Math::INT32_MAX_VALUE)
    , mCid(Elastos::Core::Math::INT32_MAX_VALUE)
{
}

CCellIdentityGsm::~CCellIdentityGsm()
{
}

ECode CCellIdentityGsm::constructor()
{
    return NOERROR;
}

ECode CCellIdentityGsm::constructor(
    /* [in] */ Int32 mcc,
    /* [in] */ Int32 mnc,
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid)
{
    mMcc = mcc;
    mMnc = mnc;
    mLac = lac;
    mCid = cid;
    return NOERROR;
}

ECode CCellIdentityGsm::constructor(
    /* [in] */ ICellIdentityGsm* cid)
{
    cid->GetMcc(&mMcc);
    cid->GetMnc(&mMnc);
    cid->GetLac(&mLac);
    cid->GetCid(&mCid);
    return NOERROR;
}

ECode CCellIdentityGsm::Copy(
    /* [out] */ ICellIdentityGsm** cid)
{
    VALIDATE_NOT_NULL(cid);
    CCellIdentityGsm::New(this, cid);
    return NOERROR;
}

ECode CCellIdentityGsm::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mMcc);
    source->ReadInt32(&mMnc);
    source->ReadInt32(&mLac);
    source->ReadInt32(&mCid);
    return NOERROR;
}

ECode CCellIdentityGsm::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (DBG) {
        String str;
        ToString(&str);
        Logger::I(TAG, "writeToParcel(Parcel, int): %s", str.string());
    }
    dest->WriteInt32(mMcc);
    dest->WriteInt32(mMnc);
    dest->WriteInt32(mLac);
    dest->WriteInt32(mCid);
    return NOERROR;
}

ECode CCellIdentityGsm::GetMcc(
    /* [out] */ Int32* mcc)
{
    VALIDATE_NOT_NULL(mcc);
    *mcc = mMcc;
    return NOERROR;
}

ECode CCellIdentityGsm::GetMnc(
    /* [out] */ Int32* mnc)
{
    VALIDATE_NOT_NULL(mnc);
    *mnc = mMnc;
    return NOERROR;
}

ECode CCellIdentityGsm::GetLac(
    /* [out] */ Int32* lac)
{
    VALIDATE_NOT_NULL(lac);
    *lac = mLac;
    return NOERROR;
}

ECode CCellIdentityGsm::GetCid(
    /* [out] */ Int32* cid)
{
    VALIDATE_NOT_NULL(cid);
    *cid = mCid;
    return NOERROR;
}

ECode CCellIdentityGsm::GetPsc(
    /* [out] */ Int32* psc)
{
    VALIDATE_NOT_NULL(psc);
    *psc = Elastos::Core::Math::INT32_MAX_VALUE;
    return NOERROR;
}

ECode CCellIdentityGsm::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    Int32 primeNum = 31;
    *hashCode = ((mMcc * primeNum) + (mMnc * primeNum) +
            (mLac * primeNum) + (mCid * primeNum));
    return NOERROR;
}

ECode CCellIdentityGsm::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    //if (super.equals(other)) {
    if(ICellIdentityGsm::Probe(other) == NULL) {
        *res = FALSE;
        return NOERROR;
    }
    //try {
    Int32 mcc, mnc, lac, cid, psc;
    AutoPtr<ICellIdentityGsm> o = ICellIdentityGsm::Probe(other);
    o->GetMcc(&mcc);
    o->GetMnc(&mnc);
    o->GetLac(&lac);
    o->GetCid(&cid);
    o->GetPsc(&psc);
    *res = (mMcc == mcc &&
        mMnc == mnc &&
        mLac == lac &&
        mCid == cid);
    // } catch (ClassCastException e) {
    //     return false;
    // }
    // } else {
    //     return false;
    // }
    return NOERROR;
}

ECode CCellIdentityGsm::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("CellIdentityGsm:{");
    sb.Append(" mMcc=");
    sb.Append(mMcc);
    sb.Append(" mMnc=");
    sb.Append(mMnc);
    sb.Append(" mLac=");
    sb.Append(mLac);
    sb.Append(" mCid=");
    sb.Append(mCid);
    sb.Append("}");

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
