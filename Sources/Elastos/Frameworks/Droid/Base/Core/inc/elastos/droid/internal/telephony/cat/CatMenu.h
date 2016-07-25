#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_MENU_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_MENU_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Graphics::IBitmap;
// import java.util.ArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cat {

/**
  * Container class for CAT menu (SET UP MENU, SELECT ITEM) parameters.
  *
  */
class CatMenu
    : public Object
    , public IMenu
    , public IParcelable
{
//private:
//    class InnerParcelableCreator
//        : public Object
//        , public Parcelable::Creator<Menu>
//    {
//    public:
//        InnerParcelableCreator(
//            /* [in] */ Menu* owner);
//
//        // @Override
//        CARAPI CreateFromParcel(
//            /* [in] */ IParcel* in,
//            /* [out] */ Menu** result);
//
//        // @Override
//        CARAPI NewArray(
//            /* [in] */ Int32 size,
//            /* [out] */ Menu[]** result);
//
//    private:
//        Menu* mOwner;
//    };

public:
    CAR_INTERFACE_DECL();

    CatMenu();

    CARAPI constructor();

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

public:
    AutoPtr<IList> mItems;  /*<Item>*/
    AutoPtr<IList> mTitleAttrs; /*<TextAttribute>*/
    PresentationType mPresentationType;
    String mTitle;
    AutoPtr<IBitmap> mTitleIcon;
    Int32 mDefaultItem;
    Boolean mSoftKeyPreferred;
    Boolean mHelpAvailable;
    Boolean mTitleIconSelfExplanatory;
    Boolean mItemsIconSelfExplanatory;
    //static const AutoPtr<IParcelable> AutoPtr< ::Creator<Menu> > CREATOR;
};

} // namespace Cat
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CAT_MENU_H__
