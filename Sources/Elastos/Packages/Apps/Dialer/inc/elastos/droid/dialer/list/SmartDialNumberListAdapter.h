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

#ifndef __ELASTOS_DROID_DIALER_LIST_SMARTDIALNUMBERLISTADAPTER_H__
#define __ELASTOS_DROID_DIALER_LIST_SMARTDIALNUMBERLISTADAPTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/dialer/list/DialerPhoneNumberListAdapter.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::Contacts::Common::List::IContactListItemView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Dialer::Dialpad::ISmartDialCursorLoader;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * List adapter to display the SmartDial search results.
 */
class SmartDialNumberListAdapter
    : public DialerPhoneNumberListAdapter
    , public ISmartDialNumberListAdapter
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context);

    using DialerPhoneNumberListAdapter::ConfigureLoader;

    /**
     * Sets query for the SmartDialCursorLoader.
     */
    CARAPI ConfigureLoader(
        /* [in] */ ISmartDialCursorLoader* loader);

    /**
     * Gets Uri for the list item at the given position.
     * @param position Location of the data of interest.
     * @return Data Uri of the entry.
     */
    CARAPI GetDataUri(
        /* [in] */ Int32 position,
        /* [out] */ IUri** uri);

    // @Override
    CARAPI SetQueryString(
        /* [in] */ const String& queryString);

protected:
    /**
     * Sets highlight options for a List item in the SmartDial search results.
     * @param view ContactListItemView where the result will be displayed.
     * @param cursor Object containing information of the associated List item.
     */
    // @Override
    CARAPI_(void) SetHighlight(
        /* [in] */ IContactListItemView* view,
        /* [in] */ ICursor* cursor);

private:
    static const String TAG; // = SmartDialNumberListAdapter.class.getSimpleName();
    static const Boolean DEBUG; // = false;

    // AutoPtr<SmartDialNameMatcher> mNameMatcher;
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_SMARTDIALNUMBERLISTADAPTER_H__
