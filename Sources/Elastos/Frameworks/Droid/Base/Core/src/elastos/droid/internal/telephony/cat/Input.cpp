
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
    mText = "";
    mDefaultText = NULL;
    mIcon = NULL;
    mMinLen = 0;
    mMaxLen = 1;
    mUcs2 = FALSE;
    mPacked = FALSE;
    mDigitOnly = FALSE;
    mEcho = FALSE;
    mYesNo = FALSE;
    mHelpAvailable = FALSE;
    mDuration = NULL;
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
    dest->WriteString(mText);
    dest->WriteString(mDefaultText);
    IParcelable::Probe(mIcon)->WriteToParcel(dest);
    dest->WriteInt32(mMinLen);
    dest->WriteInt32(mMaxLen);
    dest->WriteInt32(mUcs2 ? 1 : 0);
    dest->WriteInt32(mPacked ? 1 : 0);
    dest->WriteInt32(mDigitOnly ? 1 : 0);
    dest->WriteInt32(mEcho ? 1 : 0);
    dest->WriteInt32(mYesNo ? 1 : 0);
    dest->WriteInt32(mHelpAvailable ? 1 : 0);
    IParcelable::Probe(mDuration)->WriteToParcel(dest);
    return NOERROR;
}

ECode Input::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadString(&mText);
    in->ReadString(&mDefaultText);
    IParcelable::Probe(mIcon)->ReadFromParcel(in);
    in->ReadInt32(&mMinLen);
    in->ReadInt32(&mMaxLen);
    Int32 ucs2 = 0;
    in->ReadInt32(&ucs2);
    mUcs2 = ucs2 == 1 ? TRUE : FALSE;
    Int32 packed = 0;
    in->ReadInt32(&packed);
    mPacked = packed == 1 ? TRUE : FALSE;
    Int32 digitOnly = 0;
    in->ReadInt32(&digitOnly);
    mDigitOnly = digitOnly == 1 ? TRUE : FALSE;
    Int32 echo = 0;
    in->ReadInt32(&echo);
    mEcho = echo == 1 ? TRUE : FALSE;
    Int32 yesNo = 0;
    in->ReadInt32(&yesNo);
    mYesNo = yesNo == 1 ? TRUE : FALSE;
    Int32 helpAvailable = 0;
    in->ReadInt32(&helpAvailable);
    mHelpAvailable = helpAvailable == 1 ? TRUE : FALSE;
    IParcelable::Probe(mDuration)->ReadFromParcel(in);
    return NOERROR;
}

ECode Input::SetIcon(
    /* [in] */ IBitmap* Icon,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode Input::GetIcon(
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIcon;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Input::GetText(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mText;
    return NOERROR;
}

ECode Input::SetText(
    /* [in] */ const String& text)
{
    mText = text;
    return NOERROR;
}

ECode Input::GetDefaultText(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDefaultText;
    return NOERROR;
}

ECode Input::SetDefaultText(
    /* [in] */ const String& defaltText)
{
    mDefaultText = defaltText;
    return NOERROR;
}

ECode Input::GetMinLen(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMinLen;
    return NOERROR;
}

ECode Input::SetMinLen(
    /* [in] */ Int32 len)
{
    mMinLen = len;
    return NOERROR;
}

ECode Input::GetMaxLen(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMaxLen;
    return NOERROR;
}

ECode Input::SetMaxLen(
    /* [in] */ Int32 len)
{
    mMaxLen = len;
    return NOERROR;
}

ECode Input::GetUcs2(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUcs2;
    return NOERROR;
}

ECode Input::SetUcs2(
    /* [in] */ Boolean ucs2)
{
    mUcs2 = ucs2;
    return NOERROR;
}

ECode Input::GetPacked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPacked;
    return NOERROR;
}

ECode Input::SetPacked(
    /* [in] */ Boolean packed)
{
    mPacked = packed;
    return NOERROR;
}

ECode Input::GetDigitOnly(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDigitOnly;
    return NOERROR;
}

ECode Input::SetDigitOnly(
    /* [in] */ Boolean digitOnly)
{
    mDigitOnly = digitOnly;
    return NOERROR;
}

ECode Input::GetEcho(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEcho;
    return NOERROR;
}

ECode Input::SetEcho(
    /* [in] */ Boolean echo)
{
    mEcho = echo;
    return NOERROR;
}

ECode Input::GetYesNo(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mYesNo;
    return NOERROR;
}

ECode Input::SetYesNo(
    /* [in] */ Boolean yesNo)
{
    mYesNo = yesNo;
    return NOERROR;
}

ECode Input::GetHelpAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHelpAvailable;
    return NOERROR;
}

ECode Input::SetHelpAvailable(
    /* [in] */ Boolean helpAvailable)
{
    mHelpAvailable = helpAvailable;
    return NOERROR;
}

ECode Input::GetDuration(
    /* [out] */ IDuration** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDuration;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode Input::SetDuration(
    /* [in] */ IDuration* duration)
{
    mDuration = duration;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
