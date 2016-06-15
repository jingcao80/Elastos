
#include "elastos/droid/telephony/SmsCbLocation.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

////=====================================================================
////                SmsCbLocation::InnerParcelableCreator
////=====================================================================
//SmsCbLocation::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ SmsCbLocation* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//AutoPtr<SmsCbLocation> SmsCbLocation::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in)
//{
//    // ==================before translated======================
//    // return new SmsCbLocation(in);
//    assert(0);
//    AutoPtr<SmsCbLocation> empty;
//    return empty;
//}
//
//AutoPtr< ArrayOf< AutoPtr<SmsCbLocation> > > SmsCbLocation::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size)
//{
//    // ==================before translated======================
//    // return new SmsCbLocation[size];
//    assert(0);
//    AutoPtr< ArrayOf< AutoPtr<SmsCbLocation> > > empty;
//    return empty;
//}

//=====================================================================
//                            SmsCbLocation
//=====================================================================
CAR_INTERFACE_IMPL_2(SmsCbLocation, Object, ISmsCbLocation, IParcelable);

SmsCbLocation::SmsCbLocation()
{
    // ==================before translated======================
    // mPlmn = "";
    // mLac = -1;
    // mCid = -1;
}

ECode SmsCbLocation::constructor(
    /* [in*/ const String& plmn)
{
    // ==================before translated======================
    // mPlmn = plmn;
    // mLac = -1;
    // mCid = -1;
    return NOERROR;
}

ECode SmsCbLocation::constructor(
    /* [in*/ const String& plmn,
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid)
{
    // ==================before translated======================
    // mPlmn = plmn;
    // mLac = lac;
    // mCid = cid;
    return NOERROR;
}

ECode SmsCbLocation::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // mPlmn = in.readString();
    // mLac = in.readInt();
    // mCid = in.readInt();
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
    // ==================before translated======================
    // return mPlmn;
    assert(0);
    return NOERROR;
}

ECode SmsCbLocation::GetLac(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mLac;
    assert(0);
    return NOERROR;
}

ECode SmsCbLocation::GetCid(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCid;
    assert(0);
    return NOERROR;
}

ECode SmsCbLocation::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    // ==================before translated======================
    // int hash = mPlmn.hashCode();
    // hash = hash * 31 + mLac;
    // hash = hash * 31 + mCid;
    // return hash;
    assert(0);
    return NOERROR;
}

ECode SmsCbLocation::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (o == this) {
    //     return true;
    // }
    // if (o == null || !(o instanceof SmsCbLocation)) {
    //     return false;
    // }
    // SmsCbLocation other = (SmsCbLocation) o;
    // return mPlmn.equals(other.mPlmn) && mLac == other.mLac && mCid == other.mCid;
    assert(0);
    return NOERROR;
}

ECode SmsCbLocation::ToString(
    /* [out] */ String* str)
{
    // ==================before translated======================
    // return '[' + mPlmn + ',' + mLac + ',' + mCid + ']';
    assert(0);
    return NOERROR;
}

ECode SmsCbLocation::IsInLocationArea(
    /* [in] */ ISmsCbLocation* area,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mCid != -1 && mCid != area.mCid) {
    //     return false;
    // }
    // if (mLac != -1 && mLac != area.mLac) {
    //     return false;
    // }
    // return mPlmn.equals(area.mPlmn);
    assert(0);
    return NOERROR;
}

ECode SmsCbLocation::IsInLocationArea(
    /* [in*/ const String& plmn,
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (!mPlmn.equals(plmn)) {
    //     return false;
    // }
    //
    // if (mLac != -1 && mLac != lac) {
    //     return false;
    // }
    //
    // if (mCid != -1 && mCid != cid) {
    //     return false;
    // }
    //
    // return true;
    assert(0);
    return NOERROR;
}

ECode SmsCbLocation::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    // ==================before translated======================
    // dest.writeString(mPlmn);
    // dest.writeInt(mLac);
    // dest.writeInt(mCid);
    assert(0);
    return NOERROR;
}

ECode SmsCbLocation::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    return NOERROR;
}

//Int32 SmsCbLocation::DescribeContents()
//{
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return 0;
//}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos


