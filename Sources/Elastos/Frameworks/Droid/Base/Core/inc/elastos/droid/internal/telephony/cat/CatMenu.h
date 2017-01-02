//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
