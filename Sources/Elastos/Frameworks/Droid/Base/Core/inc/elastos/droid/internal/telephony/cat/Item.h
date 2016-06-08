#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_ITEM_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_ITEM_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Graphics::IBitmap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Represents an Item COMPREHENSION-TLV object.
  *
  * {@hide}
  */
class Item
    : public Object
    , public IItem
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<Item>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ Item* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* in,
//            /* [out] */ Item** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ Item[]** result);
//
//    private:
//        Item* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    Item();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 id,
        /* [in] */ const String& text);

    CARAPI constructor(
        /* [in] */ IParcel* in);

    // @Override
    //CARAPI DescribeContents(
    //    /* [out] */ Int32* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
        ///* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

public:
    /** Identifier of the item. */
    Int32 id;
    /** Text string of the item. */
    String text;
    /** Icon of the item */
    AutoPtr<IBitmap> icon;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<Item> > CREATOR;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_ITEM_H__
