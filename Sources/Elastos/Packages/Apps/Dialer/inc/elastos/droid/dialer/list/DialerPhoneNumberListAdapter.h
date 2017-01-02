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

#ifndef __ELASTOS_DROID_DIALER_LIST_DIALERPHONENUMBERLISTADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_DIALERPHONENUMBERLISTADAPTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/contacts/common/list/PhoneNumberListAdapter.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Contacts::Common::List::PhoneNumberListAdapter;
using Elastos::Droid::Contacts::Common::List::IContactListItemView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;

namespace Elastos{
namespace Droid{
namespace Dialer {
namespace List {

/**
 * {@link PhoneNumberListAdapter} with the following added shortcuts, that are displayed as list
 * items:
 * 1) Directly calling the phone number query
 * 2) Adding the phone number query to a contact
 *
 * These shortcuts can be enabled or disabled to toggle whether or not they show up in the
 * list.
 */
class DialerPhoneNumberListAdapter
    : public PhoneNumberListAdapter
    , public IDialerPhoneNumberListAdapter
{
public:
    CAR_INTERFACE_DECL()

    DialerPhoneNumberListAdapter();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    /**
     * @return The number of enabled shortcuts. Ranges from 0 to a maximum of SHORTCUT_COUNT
     */
    CARAPI GetShortcutCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    // @Override
    CARAPI GetViewTypeCount(
        /* [in] */ Int32* count);

    // @Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    /**
     * @param position The position of the item
     * @return The enabled shortcut type matching the given position if the item is a
     * shortcut, -1 otherwise
     */
    CARAPI GetShortcutTypeFromPosition(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    // @Override
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    // @Override
    CARAPI IsEnabled(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* result);

    /**
     * @return True if the shortcut state (disabled vs enabled) was changed by this operation
     */
    CARAPI SetShortcutEnabled(
        /* [in] */ Int32 shortcutType,
        /* [in] */ Boolean visible,
        /* [out] */ Boolean* enabled);

    CARAPI GetFormattedQueryString(
        /* [out] */ String* str);

    // @Override
    CARAPI SetQueryString(
        /* [in] */ const String& queryString);

private:
    CARAPI AssignShortcutToView(
        /* [in] */ IContactListItemView* v,
        /* [in] */ Int32 shortcutType);

private:
    String mFormattedQueryString;
    String mCountryIso;

    AutoPtr<ArrayOf<Boolean> > mShortcutEnabled;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_DIALERPHONENUMBERLISTADAPTER_H__
