
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/cat/ToneSettings.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

////=====================================================================
////                 ToneSettings::InnerParcelableCreator
////=====================================================================
//ToneSettings::InnerParcelableCreator::InnerParcelableCreator(
//    /* [in] */ ToneSettings* owner)
//    : mOwner(owner)
//{
//    // ==================before translated======================
//    // mOwner = owner;
//}
//
//ECode ToneSettings::InnerParcelableCreator::CreateFromParcel(
//    /* [in] */ IParcel* in,
//    /* [out] */ ToneSettings** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new ToneSettings(in);
//    assert(0);
//    return NOERROR;
//}
//
//ECode ToneSettings::InnerParcelableCreator::NewArray(
//    /* [in] */ Int32 size,
//    /* [out] */ ToneSettings[]** result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return new ToneSettings[size];
//    assert(0);
//    return NOERROR;
//}
//
////=====================================================================
////                             ToneSettings
////=====================================================================
//const AutoPtr<IParcelable> AutoPtr< ::Creator<ToneSettings> > ToneSettings::CREATOR = new InnerParcelableCreator(this);
CAR_INTERFACE_IMPL_2(ToneSettings, Object, IToneSettings, IParcelable);

ToneSettings::ToneSettings()
{
}

ECode ToneSettings::constructor()
{
    return NOERROR;
}

ECode ToneSettings::constructor(
    /* [in] */ IDuration* duration,
    /* [in] */ Tone tone,
    /* [in] */ Boolean vibrate)
{
    mDuration = duration;
    mTone = tone;
    mVibrate = vibrate;
    return NOERROR;
}

//ECode ToneSettings::DescribeContents(
//    /* [out] */ Int32* result)
//{
//    VALIDATE_NOT_NULL(result);
//    // ==================before translated======================
//    // return 0;
//    assert(0);
//    return NOERROR;
//}

ECode ToneSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
    ///* [in] */ Int32 flags)
{
    IParcelable::Probe(mDuration)->WriteToParcel(dest);
    dest->WriteInt32(mTone);
    dest->WriteInt32(mVibrate ? 1 : 0);
    return NOERROR;
}

ECode ToneSettings::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    IParcelable::Probe(mDuration)->ReadFromParcel(in);
    in->ReadInt32(&mTone);
    Int32 vibrate = 0;
    in->ReadInt32(&vibrate);
    mVibrate = vibrate == 1;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
