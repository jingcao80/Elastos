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

#include "elastos/droid/dialer/list/AllContactsFragment.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
#include "elastos/droid/contacts/common/list/CContactListFilter.h"
#include "elastos/droid/contacts/common/util/ViewUtil.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Net.h"
#include "R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Contacts::Common::List::CContactListFilter;
using Elastos::Droid::Contacts::Common::List::IContactListFilter;
using Elastos::Droid::Contacts::Common::Util::ViewUtil;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Dialer::List::EIID_IAllContactsFragment;
using Elastos::Droid::Dialer::Util::DialerUtils;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IContactsContractQuickContact;
using Elastos::Droid::Provider::CContactsContractQuickContact;
using Elastos::Droid::Widget::IListView;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
//AllContactsFragment::MyDefaultContactListAdapter
//=================================================================
void AllContactsFragment::MyDefaultContactListAdapter::BindView(
    /* [in] */ IView* itemView,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position)
{
    DefaultContactListAdapter::BindView(itemView, partition, cursor, position);

    AutoPtr<IUri> uri = GetContactUri(partition, cursor);
    itemView->SetTag(uri);
}


//=================================================================
// AllContactsFragment
//=================================================================
CAR_INTERFACE_IMPL(AllContactsFragment, ContactEntryListFragment, IAllContactsFragment)

AllContactsFragment::AllContactsFragment()
{
    SetQuickContactEnabled(FALSE);
    SetAdjustSelectionBoundsEnabled(TRUE);
    SetPhotoLoaderEnabled(TRUE);
    SetSectionHeaderDisplayEnabled(TRUE);
    SetDarkTheme(FALSE);
    SetVisibleScrollbarEnabled(TRUE);
}

ECode AllContactsFragment::constructor()
{
    return ContactEntryListFragment::constructor();
}

ECode AllContactsFragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    ContactEntryListFragment::OnViewCreated(view, savedInstanceState);

    AutoPtr<IView> emptyListView;
    view->FindViewById(Elastos::Droid::Dialer::R::id::empty_list_view, (IView**)&emptyListView);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    DialerUtils::ConfigureEmptyListView(emptyListView, Elastos::Droid::Dialer::R::drawable::empty_contacts,
            R::string::all_contacts_empty, res);
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    IAdapterView::Probe(listView)->SetEmptyView(emptyListView);

    ViewUtil::AddBottomPaddingToListViewForFab(listView, res);
    return NOERROR;
}

AutoPtr<IContactEntryListAdapter> AllContactsFragment::CreateListAdapter()
{
    AutoPtr<MyDefaultContactListAdapter> adapter = new MyDefaultContactListAdapter(this);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    adapter->constructor(IContext::Probe(activity));
    adapter->SetDisplayPhotos(TRUE);
    adapter->SetFilter(CContactListFilter::CreateFilterWithType(
            IContactListFilter::FILTER_TYPE_DEFAULT));
    Boolean isEnabled;
    IsSectionHeaderDisplayEnabled(&isEnabled);
    adapter->SetSectionHeaderDisplayEnabled(isEnabled);
    return adapter;
}

AutoPtr<IView> AllContactsFragment::InflateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container)
{
    AutoPtr<IView> view;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::show_all_contacts_fragment,
            NULL, (IView**)&view);
    return view;
}

ECode AllContactsFragment::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> tag;
    view->GetTag((IInterface**)&tag);
    if (tag != NULL) {
        AutoPtr<IUri> uri = IUri::Probe(tag);
        AutoPtr<IContactsContractQuickContact> qc;
        CContactsContractQuickContact::AcquireSingleton((IContactsContractQuickContact**)&qc);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        qc->ShowQuickContact(IContext::Probe(activity), view,
                uri, IContactsContractQuickContact::MODE_LARGE, NULL);
    }

    return NOERROR;
}

void AllContactsFragment::OnItemClick(
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    // Do nothing. Implemented to satisfy ContactEntryListFragment.
}

} // List
} // Dialer
} // Droid
} // Elastos