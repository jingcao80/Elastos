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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CCONTACTLISTFILTERVIEW_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CCONTACTLISTFILTERVIEW_H__

#include "_Elastos_Droid_Contacts_Common_List_CContactListFilterView.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Contacts::Common::Model::IAccountTypeManager;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IRadioButton;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapterView;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

/**
 * Contact list filter parameters.
 */
CarClass(CContactListFilterView)
    , public LinearLayout
    , public IContactListFilterView
{
public:
    CContactListFilterView()
        : mSingleAccount(FALSE)
    {}

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetContactListFilter(
        /* [in] */ IContactListFilter* filter);

    CARAPI GetContactListFilter(
        /* [out] */ IContactListFilter** filter);

    CARAPI SetSingleAccount(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI SetActivated(
        /* [in] */ Boolean activated);

    CARAPI BindView(
        /* [in] */ IAccountTypeManager* accountTypes);

private:
    CARAPI_(void) BindView(
        /* [in] */ Int32 iconResource,
        /* [in] */ Int32 textResource);

private:
    static const String TAG;

    AutoPtr<IImageView> mIcon;
    AutoPtr<ITextView> mAccountType;
    AutoPtr<ITextView> mAccountUserName;
    AutoPtr<IRadioButton> mRadioButton;
    AutoPtr<IContactListFilter> mFilter;
    Boolean mSingleAccount;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CCONTACTLISTFILTERVIEW_H__
