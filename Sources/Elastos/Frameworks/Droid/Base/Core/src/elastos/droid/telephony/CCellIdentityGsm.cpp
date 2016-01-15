
#include "CCellIdentityGsm.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Telephony {

#define INTEGER_MAX_VALUE 0x7FFFFFFF;
const String CCellIdentityGsm::LOG_TAG("CCellIdentityGsm");

ECode CCellIdentityGsm::constructor()
{
    mMcc = INTEGER_MAX_VALUE;
    mMnc = INTEGER_MAX_VALUE;
    mLac = INTEGER_MAX_VALUE;
    mCid = INTEGER_MAX_VALUE;
    mPsc = INTEGER_MAX_VALUE;
    return NOERROR;
}

ECode CCellIdentityGsm::constructor(
    /* [in] */ Int32 mcc,
    /* [in] */ Int32 mnc,
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid,
    /* [in] */ Int32 psc)
{
    mMcc = mcc;
    mMnc = mnc;
    mLac = lac;
    mCid = cid;
    mPsc = psc;
    return NOERROR;
}

ECode CCellIdentityGsm::constructor(
    /* [in] */ ICellIdentityGsm* cid)
{
    cid->GetMcc(&mMcc);
    cid->GetMnc(&mMnc);
    cid->GetLac(&mLac);
    cid->GetCid(&mCid);
    cid->GetPsc(&mPsc);
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
    source->ReadInt32(&mPsc);
    return NOERROR;
}

ECode CCellIdentityGsm::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (DBG) {
        String str;
        ToString(&str);
        Slogger::I(LOG_TAG, "writeToParcel(Parcel, int): %s", str.string());
    }
    dest->WriteInt32(mMcc);
    dest->WriteInt32(mMnc);
    dest->WriteInt32(mLac);
    dest->WriteInt32(mCid);
    dest->WriteInt32(mPsc);
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
    *psc = mPsc;
    return NOERROR;
}

ECode CCellIdentityGsm::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    int primeNum = 31;
    *hashCode = ((mMcc * primeNum) + (mMnc * primeNum) + (mLac * primeNum) + (mCid * primeNum) +
            (mPsc * primeNum));
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
        mCid == cid &&
        mPsc == psc);
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
    StringBuilder sb;
    sb.Append("super.toString()");
    sb.Append(" mMcc=");
    sb.Append(mMcc);
    sb.Append(" mMnc=");
    sb.Append(mMcc);
    sb.Append(" mLac=");
    sb.Append(mLac);
    sb.Append(" mCid=");
    sb.Append(mCid);
    sb.Append(" mPsc=");
    sb.Append(mPsc);
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
