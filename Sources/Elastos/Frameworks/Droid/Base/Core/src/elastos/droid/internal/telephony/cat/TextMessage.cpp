
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
////=====================================================================
////                             TextMessage
////=====================================================================
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
    // ==================before translated======================
    // dest.writeString(title);
    // dest.writeString(text);
    // dest.writeParcelable(icon, 0);
    // dest.writeInt(iconSelfExplanatory ? 1 : 0);
    // dest.writeInt(isHighPriority ? 1 : 0);
    // dest.writeInt(responseNeeded ? 1 : 0);
    // dest.writeInt(userClear ? 1 : 0);
    // dest.writeParcelable(duration, 0);
    assert(0);
    return NOERROR;
}

ECode TextMessage::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    constructor(source);
    return NOERROR;
}

ECode TextMessage::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // title = in.readString();
    // text = in.readString();
    // icon = in.readParcelable(null);
    // iconSelfExplanatory = in.readInt() == 1 ? true : false;
    // isHighPriority = in.readInt() == 1 ? true : false;
    // responseNeeded = in.readInt() == 1 ? true : false;
    // userClear = in.readInt() == 1 ? true : false;
    // duration = in.readParcelable(null);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
