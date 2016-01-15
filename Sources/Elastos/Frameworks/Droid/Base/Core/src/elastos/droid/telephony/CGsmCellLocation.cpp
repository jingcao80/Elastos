
#include "CGsmCellLocation.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Telephony {

ECode CGsmCellLocation::constructor()
{
    mLac = -1;
    mCid = -1;
    mPsc = -1;
    return NOERROR;
}

ECode CGsmCellLocation::constructor(
    /* [in] */ IBundle* bundle)
{
    mLac = bundle->GetInt32(String("lac"), &mLac);
    mCid = bundle->GetInt32(String("cid"), &mCid);
    mPsc = bundle->GetInt32(String("psc"), &mPsc);
    return NOERROR;
}

PInterface CGsmCellLocation::Probe(
    /* [in]  */ REIID riid)
{
    return _CGsmCellLocation::Probe(riid);
}

ECode CGsmCellLocation::FillInNotifierBundle(
    /* [in] */ IBundle* bundle)
{
    bundle->PutInt32(String("lac"), mLac);
    bundle->PutInt32(String("cid"), mCid);
    bundle->PutInt32(String("psc"), mPsc);
    return NOERROR;
}

ECode CGsmCellLocation::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mLac == -1 && mCid == -1 && mPsc == -1);
    return NOERROR;
}

ECode CGsmCellLocation::GetLac(
    /* [out] */ Int32* lac)
{
    VALIDATE_NOT_NULL(lac);
    *lac = mLac;
    return NOERROR;
}

ECode CGsmCellLocation::GetCid(
    /* [out] */ Int32* cid)
{
    VALIDATE_NOT_NULL(cid);
    *cid = mCid;
    return NOERROR;
}

ECode CGsmCellLocation::GetPsc(
    /* [out] */ Int32* psc)
{
    VALIDATE_NOT_NULL(psc);
    *psc = mPsc;
    return NOERROR;
}

ECode CGsmCellLocation::SetLac(
    /* [in] */ Int32 lac)
{
    mLac = lac;
    return NOERROR;
}

ECode CGsmCellLocation::SetCid(
    /* [in] */ Int32 cid)
{
    mCid = cid;
    return NOERROR;
}

ECode CGsmCellLocation::SetPsc(
    /* [in] */ Int32 psc)
{
    mPsc = psc;
    return NOERROR;
}

ECode CGsmCellLocation::SetStateInvalid()
{
    mLac = -1;
    mCid = -1;
    mPsc = -1;
    return NOERROR;
}

ECode CGsmCellLocation::SetLacAndCid(
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid)
{
    mLac = lac;
    mCid = cid;
    return NOERROR;
}

ECode CGsmCellLocation::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mLac ^ mCid;
    return NOERROR;
}

ECode CGsmCellLocation::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IGsmCellLocation> s;

    //try {
    s = IGsmCellLocation::Probe(o);
    // } catch (ClassCastException ex) {
    //     return false;
    // }

    if (o == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 lac; s->GetLac(&lac);
    Int32 cid; s->GetCid(&cid);
    Int32 psc; s->GetPsc(&psc);
    *result = (EqualsHandlesNulls(mLac, lac) && EqualsHandlesNulls(mCid, cid)
        && EqualsHandlesNulls(mPsc, psc));

    return NOERROR;
}

ECode CGsmCellLocation::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    StringBuilder sb;
    sb.Append("["); sb.Append(mLac);
    sb.Append(","); sb.Append(mCid);
    sb.Append(","); sb.Append(mPsc);
    sb.Append("]");
    *str = sb.ToString();
    return NOERROR;
}

Boolean CGsmCellLocation::EqualsHandlesNulls(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b)
{
    return a == b;
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

