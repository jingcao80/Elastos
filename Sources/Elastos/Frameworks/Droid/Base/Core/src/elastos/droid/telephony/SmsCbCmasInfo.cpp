
#include "elastos/droid/telephony/SmsCbCmasInfo.h"

namespace Elastos {
namespace Droid {
namespace Telephony {

////=====================================================================
////                SmsCbCmasInfo::InnerParcelableCreator
////=====================================================================
//SmsCbCmasInfo::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ SmsCbCmasInfo* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//AutoPtr<SmsCbCmasInfo> SmsCbCmasInfo::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in)
//{
//    // ==================before translated======================
//    // return new SmsCbCmasInfo(in);
//    assert(0);
//    AutoPtr<SmsCbCmasInfo> empty;
//    return empty;
//}
//
//AutoPtr< ArrayOf< AutoPtr<SmsCbCmasInfo> > > SmsCbCmasInfo::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size)
//{
//    // ==================before translated======================
//    // return new SmsCbCmasInfo[size];
//    assert(0);
//    AutoPtr< ArrayOf< AutoPtr<SmsCbCmasInfo> > > empty;
//    return empty;
//}

//=====================================================================
//                            SmsCbCmasInfo
//=====================================================================
CAR_INTERFACE_IMPL_2(SmsCbCmasInfo, Object, ISmsCbCmasInfo, IParcelable);

SmsCbCmasInfo::SmsCbCmasInfo()
{
}

ECode SmsCbCmasInfo::constructor(
    /* [in] */ Int32 messageClass,
    /* [in] */ Int32 category,
    /* [in] */ Int32 responseType,
    /* [in] */ Int32 severity,
    /* [in] */ Int32 urgency,
    /* [in] */ Int32 certainty)
{
    // ==================before translated======================
    // mMessageClass = messageClass;
    // mCategory = category;
    // mResponseType = responseType;
    // mSeverity = severity;
    // mUrgency = urgency;
    // mCertainty = certainty;
    return NOERROR;
}

ECode SmsCbCmasInfo::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // mMessageClass = in.readInt();
    // mCategory = in.readInt();
    // mResponseType = in.readInt();
    // mSeverity = in.readInt();
    // mUrgency = in.readInt();
    // mCertainty = in.readInt();
    return NOERROR;
}

ECode SmsCbCmasInfo::constructor()
{
    return NOERROR;
}

ECode SmsCbCmasInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    // ==================before translated======================
    // dest.writeInt(mMessageClass);
    // dest.writeInt(mCategory);
    // dest.writeInt(mResponseType);
    // dest.writeInt(mSeverity);
    // dest.writeInt(mUrgency);
    // dest.writeInt(mCertainty);
    assert(0);
    return NOERROR;
}

ECode SmsCbCmasInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    return NOERROR;
}


ECode SmsCbCmasInfo::GetMessageClass(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mMessageClass;
    assert(0);
    return NOERROR;
}

ECode SmsCbCmasInfo::GetCategory(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCategory;
    assert(0);
    return NOERROR;
}

ECode SmsCbCmasInfo::GetResponseType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mResponseType;
    assert(0);
    return NOERROR;
}

ECode SmsCbCmasInfo::GetSeverity(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mSeverity;
    assert(0);
    return NOERROR;
}

ECode SmsCbCmasInfo::GetUrgency(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUrgency;
    assert(0);
    return NOERROR;
}

ECode SmsCbCmasInfo::GetCertainty(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCertainty;
    assert(0);
    return NOERROR;
}

ECode SmsCbCmasInfo::ToString(
    /* [out] */ String* str)
{
    // ==================before translated======================
    // return "SmsCbCmasInfo{messageClass=" + mMessageClass + ", category=" + mCategory
    //         + ", responseType=" + mResponseType + ", severity=" + mSeverity
    //         + ", urgency=" + mUrgency + ", certainty=" + mCertainty + '}';
    assert(0);
    return NOERROR;
}

//Int32 SmsCbCmasInfo::DescribeContents()
//{
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return 0;
//}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos
