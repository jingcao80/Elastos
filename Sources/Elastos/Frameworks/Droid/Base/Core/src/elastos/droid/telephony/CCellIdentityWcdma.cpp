
#include "elastos/droid/telephony/CCellIdentityWcdma.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String CCellIdentityWcdma::TAG("CCellIdentityWcdma");
const Boolean CCellIdentityWcdma::DBG;

CAR_INTERFACE_IMPL_2(CCellIdentityWcdma, Object, ICellIdentityWcdma, IParcelable)

CAR_OBJECT_IMPL(CCellIdentityWcdma)

CCellIdentityWcdma::CCellIdentityWcdma()
    : mMcc(Elastos::Core::Math::INT32_MAX_VALUE)
    , mMnc(Elastos::Core::Math::INT32_MAX_VALUE)
    , mLac(Elastos::Core::Math::INT32_MAX_VALUE)
    , mCid(Elastos::Core::Math::INT32_MAX_VALUE)
    , mPsc(Elastos::Core::Math::INT32_MAX_VALUE)
{
}

CCellIdentityWcdma::~CCellIdentityWcdma()
{
}

ECode CCellIdentityWcdma::constructor()
{
    return NOERROR;
}

ECode CCellIdentityWcdma::constructor(
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

ECode CCellIdentityWcdma::constructor(
    /* [in] */ ICellIdentityWcdma* cid)
{
    cid->GetMcc(&mMcc);
    cid->GetMnc(&mMnc);
    cid->GetLac(&mLac);
    cid->GetCid(&mCid);
    cid->GetPsc(&mCid);
    return NOERROR;
}

ECode CCellIdentityWcdma::Copy(
    /* [out] */ ICellIdentityWcdma** cid)
{
    VALIDATE_NOT_NULL(cid);
    CCellIdentityWcdma::New(this, cid);
    return NOERROR;
}

ECode CCellIdentityWcdma::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mMcc);
    source->ReadInt32(&mMnc);
    source->ReadInt32(&mLac);
    source->ReadInt32(&mCid);
    source->ReadInt32(&mPsc);
    return NOERROR;
}

ECode CCellIdentityWcdma::WriteToParcel(
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
    dest->WriteInt32(mPsc);
    return NOERROR;
}

ECode CCellIdentityWcdma::GetMcc(
    /* [out] */ Int32* mcc)
{
    VALIDATE_NOT_NULL(mcc);
    *mcc = mMcc;
    return NOERROR;
}

ECode CCellIdentityWcdma::GetMnc(
    /* [out] */ Int32* mnc)
{
    VALIDATE_NOT_NULL(mnc);
    *mnc = mMnc;
    return NOERROR;
}

ECode CCellIdentityWcdma::GetLac(
    /* [out] */ Int32* lac)
{
    VALIDATE_NOT_NULL(lac);
    *lac = mLac;
    return NOERROR;
}

ECode CCellIdentityWcdma::GetCid(
    /* [out] */ Int32* cid)
{
    VALIDATE_NOT_NULL(cid);
    *cid = mCid;
    return NOERROR;
}

ECode CCellIdentityWcdma::GetPsc(
    /* [out] */ Int32* psc)
{
    VALIDATE_NOT_NULL(psc);
    *psc = mPsc;
    return NOERROR;
}

ECode CCellIdentityWcdma::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    int primeNum = 31;
    *hashCode = ((mMcc * primeNum) + (mMnc * primeNum) +
            (mLac * primeNum) + (mCid * primeNum) +
            (mPsc * primeNum));
    return NOERROR;
}

ECode CCellIdentityWcdma::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    //if (super.equals(other)) {
    if(ICellIdentityWcdma::Probe(other) == NULL) {
        *res = FALSE;
        return NOERROR;
    }
    //try {
    Int32 mcc, mnc, lac, cid, psc;
    AutoPtr<ICellIdentityWcdma> o = ICellIdentityWcdma::Probe(other);
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

ECode CCellIdentityWcdma::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb("CellIdentityWcdma:{");
    sb.Append(" mMcc=");
    sb.Append(mMcc);
    sb.Append(" mMnc=");
    sb.Append(mMnc);
    sb.Append(" mLac=");
    sb.Append(mLac);
    sb.Append(" mCid=");
    sb.Append(mCid);
    sb.Append(" mPsc=");
    sb.Append(mPsc);
    sb.Append("}");

    *str = sb.ToString();
    return NOERROR;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
