
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/TextMessage.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                 TextMessage::InnerParcelableCreator
////=====================================================================
//TextMessage::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ TextMessage* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode TextMessage::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ TextMessage** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new TextMessage(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode TextMessage::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ TextMessage[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new TextMessage[size];
//    assert(0);
//    return NOERROR;
//}
//
//=====================================================================
//                             TextMessage
//=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<TextMessage> > TextMessage::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL_2(TextMessage, Object, ITextMessage, IParcelable);

TextMessage::TextMessage()
{
}

ECode TextMessage::constructor()
{
    return NOERROR;
}

//ECode TextMessage::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode TextMessage::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    dest->WriteString(mTitle);
    dest->WriteString(mText);
    IParcelable::Probe(mIcon)->WriteToParcel(dest);
    dest->WriteInt32(mIconSelfExplanatory ? 1 : 0);
    dest->WriteInt32(mIsHighPriority ? 1 : 0);
    dest->WriteInt32(mResponseNeeded ? 1 : 0);
    dest->WriteInt32(mUserClear ? 1 : 0);
    IParcelable::Probe(mDuration)->WriteToParcel(dest);
    return NOERROR;
}

ECode TextMessage::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mTitle);
    in->ReadString(&mText);
    IParcelable::Probe(mIcon)->ReadFromParcel(in);
    Int32 iconSelfExplanatory = 0;
    in->ReadInt32(&iconSelfExplanatory);
    mIconSelfExplanatory = iconSelfExplanatory == 1 ? TRUE : FALSE;
    Int32 isHighPriority = 0;
    in->ReadInt32(&isHighPriority);
    mIsHighPriority = isHighPriority == 1 ? TRUE : FALSE;
    Int32 responseNeeded = 0;
    in->ReadInt32(&responseNeeded);
    mResponseNeeded = responseNeeded == 1 ? TRUE : FALSE;
    Int32 userClear = 0;
    in->ReadInt32(&userClear);
    mUserClear = userClear == 1 ? TRUE : FALSE;
    IParcelable::Probe(mDuration)->ReadFromParcel(in);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
