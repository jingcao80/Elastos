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

#ifndef __ELASTOS_DROID_DIALER_LIST_ALLCONTACTSFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_LIST_ALLCONTACTSFRAGMENT_H__

#include "elastos/droid/contacts/common/list/ContactEntryListFragment.h"
#include "elastos/droid/contacts/common/list/DefaultContactListAdapter.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::Contacts::Common::List::ContactEntryListFragment;
using Elastos::Droid::Contacts::Common::List::IContactEntryListAdapter;
using Elastos::Droid::Contacts::Common::List::DefaultContactListAdapter;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

/**
 * Fragments to show all contacts with phone numbers.
 */
class AllContactsFragment
    : public ContactEntryListFragment
    , public IAllContactsFragment
{
private:
    class MyDefaultContactListAdapter
        : public DefaultContactListAdapter
    {
    public:
        MyDefaultContactListAdapter(
            /* [in] */ AllContactsFragment* host)
            : mHost(host)
        {}

    protected:
        // @Override
        CARAPI_(void) BindView(
            /* [in] */ IView* itemView,
            /* [in] */ Int32 partition,
            /* [in] */ ICursor* cursor,
            /* [in] */ Int32 position);

    private:
        AllContactsFragment* mHost;
    };

public:
    AllContactsFragment();

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    // @Override
    CARAPI OnViewCreated(
        /* [in] */ IView* view,
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

protected:
    // @Override
    CARAPI_(AutoPtr<IContactEntryListAdapter>) CreateListAdapter();

    // @Override
    CARAPI_(AutoPtr<IView>) InflateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container);

    // @Override
    CARAPI_(void) OnItemClick(
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);
};

} // List
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_LIST_ALLCONTACTSFRAGMENT_H__
