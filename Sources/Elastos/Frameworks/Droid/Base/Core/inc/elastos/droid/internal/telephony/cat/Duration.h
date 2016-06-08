#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_DURATION_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_DURATION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Class for representing "Duration" object for CAT.
  *
  * {@hide}
  */
class Duration
    : public Object
    , public IDuration
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<Duration>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ Duration* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* in,
//            /* [out] */ Duration** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ Duration[]** result);
//
//    private:
//        Duration* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    Duration();

    CARAPI constructor();
    /**
      * @param timeInterval Between 1 and 255 inclusive.
      */
    CARAPI constructor(
        /* [in] */ Int32 timeInterval,
        /* [in] */ IDurationTimeUnit timeUnit);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
        ///* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // @Override
//    CARAPI DescribeContents(
//        /* [out] */ Int32* result);

private:
    CARAPI constructor(
        /* [in] */ IParcel* in);

public:
    Int32 timeInterval;
    IDurationTimeUnit timeUnit;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<Duration> > CREATOR;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_DURATION_H__

