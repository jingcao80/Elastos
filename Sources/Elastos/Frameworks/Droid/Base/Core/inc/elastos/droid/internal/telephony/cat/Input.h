#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_INPUT_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_INPUT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

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
        /* [in] */ IParcel* source);

    virtual CARAPI SetIcon(
        /* [in] */ IBitmap* Icon,
        /* [out] */ Boolean* result);

private:
    CARAPI constructor(
        /* [in] */ IParcel* in);

public:
    String text;
    String defaultText;
    AutoPtr<IBitmap> icon;
    Int32 minLen;
    Int32 maxLen;
    Boolean ucs2;
    Boolean packed;
    Boolean digitOnly;
    Boolean echo;
    Boolean yesNo;
    Boolean helpAvailable;
    AutoPtr<IDuration> duration;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<Input> > CREATOR;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_INPUT_H__

