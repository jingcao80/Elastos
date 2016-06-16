
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
    // ==================before translated======================
    // this.duration = duration;
    // this.tone = tone;
    // this.vibrate = vibrate;
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
    // ==================before translated======================
    // dest.writeParcelable(duration, 0);
    // dest.writeParcelable(tone, 0);
    // dest.writeInt(vibrate ? 1 : 0);
    assert(0);
    return NOERROR;
}

ECode ToneSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    constructor(source);
    return NOERROR;
}

ECode ToneSettings::constructor(
    /* [in] */ IParcel* in)
{
    // ==================before translated======================
    // duration = in.readParcelable(null);
    // tone = in.readParcelable(null);
    // vibrate = in.readInt() == 1;
    return NOERROR;
}

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
