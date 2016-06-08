#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TONESETTINGS_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TONESETTINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Container class for PlayTone commands parameters.
  *
  */
class ToneSettings
    : public Object
    , public IToneSettings
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<ToneSettings>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ ToneSettings* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* in,
//            /* [out] */ ToneSettings** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ ToneSettings[]** result);
//
//    private:
//        ToneSettings* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    ToneSettings();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDuration* duration,
        /* [in] */ Tone tone,
        /* [in] */ Boolean vibrate);

    // @Override
    //CARAPI DescribeContents(
    //    /* [out] */ Int32* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
        ///* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

private:
    CARAPI constructor(
        /* [in] */ IParcel* in);

public:
    AutoPtr<IDuration> duration;
    Tone tone;
    Boolean vibrate;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<ToneSettings> > CREATOR;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TONESETTINGS_H__
