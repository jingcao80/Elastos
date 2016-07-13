#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TEXTMESSAGE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TEXTMESSAGE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

class TextMessage
    : public Object
    , public ITextMessage
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<TextMessage>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ TextMessage* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* in,
//            /* [out] */ TextMessage** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ TextMessage[]** result);
//
//    private:
//        TextMessage* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    TextMessage();

    CARAPI constructor();

    // @Override
    //CARAPI DescribeContents(
    //    /* [out] */ Int32* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
        ///* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
            /* [in] */ IParcel* in);

public:
    String mTitle;
    String mText;
    AutoPtr<IBitmap> mIcon;
    Boolean mIconSelfExplanatory;
    Boolean mIsHighPriority;
    Boolean mResponseNeeded;
    Boolean mUserClear;
    AutoPtr<IDuration> mDuration;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<TextMessage> > CREATOR;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_TEXTMESSAGE_H__

