#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_INPUT_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_INPUT_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Container class for CAT GET INPUT, GET IN KEY commands parameters.
  *
  */
class Input
    : public Object
    , public IInput
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<Input>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ Input* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* in,
//            /* [out] */ Input** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ Input[]** result);
//
//    private:
//        Input* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    Input();

    CARAPI constructor();

    // @Override
    //CARAPI DescribeContents(
    //    /* [out] */ Int32* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
    //    /* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI SetIcon(
        /* [in] */ IBitmap* Icon,
        /* [out] */ Boolean* result);

    CARAPI GetIcon(
        /* [out] */ IBitmap** result);

    CARAPI GetText(
        /* [out] */ String* result);

    CARAPI SetText(
        /* [in] */ const String& text);

    CARAPI GetDefaultText(
        /* [out] */ String* result);

    CARAPI SetDefaultText(
        /* [in] */ const String& defaltText);

    CARAPI GetMinLen(
        /* [out] */ Int32* result);

    CARAPI SetMinLen(
        /* [in] */ Int32 len);

    CARAPI GetMaxLen(
        /* [out] */ Int32* result);

    CARAPI SetMaxLen(
        /* [in] */ Int32 len);

    CARAPI GetUcs2(
        /* [out] */ Boolean* result);

    CARAPI SetUcs2(
        /* [in] */ Boolean text);

    CARAPI GetPacked(
        /* [out] */ Boolean* result);

    CARAPI SetPacked(
        /* [in] */ Boolean text);

    CARAPI GetDigitOnly(
        /* [out] */ Boolean* result);

    CARAPI SetDigitOnly(
        /* [in] */ Boolean text);

    CARAPI GetEcho(
        /* [out] */ Boolean* result);

    CARAPI SetEcho(
        /* [in] */ Boolean text);

    CARAPI GetYesNo(
        /* [out] */ Boolean* result);

    CARAPI SetYesNo(
        /* [in] */ Boolean text);

    CARAPI GetHelpAvailable(
        /* [out] */ Boolean* result);

    CARAPI SetHelpAvailable(
        /* [in] */ Boolean text);

    CARAPI GetDuration(
        /* [out] */ IDuration** result);

    CARAPI SetDuration(
        /* [in] */ IDuration* duration);

public:
    String mText;
    String mDefaultText;
    AutoPtr<IBitmap> mIcon;
    Int32 mMinLen;
    Int32 mMaxLen;
    Boolean mUcs2;
    Boolean mPacked;
    Boolean mDigitOnly;
    Boolean mEcho;
    Boolean mYesNo;
    Boolean mHelpAvailable;
    AutoPtr<IDuration> mDuration;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<Input> > CREATOR;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_INPUT_H__

