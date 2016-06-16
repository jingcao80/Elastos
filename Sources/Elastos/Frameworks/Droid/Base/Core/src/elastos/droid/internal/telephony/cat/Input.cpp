
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/Input.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                    Input::InnerParcelableCreator
////=====================================================================
//Input::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ Input* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode Input::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ Input** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Input(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode Input::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ Input[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new Input[size];
//    assert(0);
//    return NOERROR;
//}
//
////=====================================================================
////                                Input
////=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<Input> > Input::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL_2(Input, Object, IInput, IParcelable);

Input::Input()
{
    // ==================before translated======================
    // text = "";
    // defaultText = null;
    // icon = null;
    // minLen = 0;
    // maxLen = 1;
    // ucs2 = false;
    // packed = false;
    // digitOnly = false;
    // echo = false;
    // yesNo = false;
    // helpAvailable = false;
    // duration = null;
}

ECode Input::constructor()
{
    return NOERROR;
}

//ECode Input::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode Input::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    // ==================before translated======================
    // dest.writeString(text);
    // dest.writeString(defaultText);
    // dest.writeParcelable(icon, 0);
    // dest.writeInt(minLen);
    // dest.writeInt(maxLen);
    // dest.writeInt(ucs2 ? 1 : 0);
    // dest.writeInt(packed ? 1 : 0);
    // dest.writeInt(digitOnly ? 1 : 0);
    // dest.writeInt(echo ? 1 : 0);
    // dest.writeInt(yesNo ? 1 : 0);
    // dest.writeInt(helpAvailable ? 1 : 0);
    // dest.writeParcelable(duration, 0);
    assert(0);
    return NOERROR;
}

ECode Input::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    constructor(source);
    return NOERROR;
}

ECode Input::SetIcon(
    /* [in] */ IBitmap* Icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return true;
    assert(0);
    return NOERROR;
}

ECode Input::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // text = in.readString();
    // defaultText = in.readString();
    // icon = in.readParcelable(null);
    // minLen = in.readInt();
    // maxLen = in.readInt();
    // ucs2 = in.readInt() == 1 ? true : false;
    // packed = in.readInt() == 1 ? true : false;
    // digitOnly = in.readInt() == 1 ? true : false;
    // echo = in.readInt() == 1 ? true : false;
    // yesNo = in.readInt() == 1 ? true : false;
    // helpAvailable = in.readInt() == 1 ? true : false;
    // duration = in.readParcelable(null);
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
