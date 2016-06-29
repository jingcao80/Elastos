
#include "elastos/apps/dialer/list/AllContactsFragment.h"
#include "elastos/apps/dialer/util/DialerUtils.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Net.h"
#include "R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IContactsContractQuickContact;
using Elastos::Droid::Provider::CContactsContractQuickContact;
using Elastos::Droid::Widget::IListView;
using Elastos::Apps::Dialer::Util::DialerUtils;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

// TODO:
// AllContactsFragment::MyDefaultContactListAdapter::MyDefaultContactListAdapter(
//     /* [in] */ AllContactsFragment* host)
//     : mHost(host)
// {}

// ECode AllContactsFragment::MyDefaultContactListAdapter::BindView(
//     /* [in] */ IView* itemView,
//     /* [in] */ Int32 partition,
//     /* [in] */ ICursor* cursor,
//     /* [in] */ Int32 position)
// {
//     DefaultContactListAdapter::BindView(itemView, partition, cursor, position);

//     AutoPtr<IUri> uri;
//     GetContactUri(partition, cursor, (IUri**)&uri);
//     itemView->SetTag(uri);

//     return NOERROR;
// }

CAR_INTERFACE_IMPL(AllContactsFragment, /*ContactEntryListFragment*/Fragment, IAllContactsFragment);

AllContactsFragment::AllContactsFragment()
{
    assert(0 && "TODO");
    // SetQuickContactEnabled(FALSE);
    // SetAdjustSelectionBoundsEnabled(TRUE);
    // SetPhotoLoaderEnabled(TRUE);
    // SetSectionHeaderDisplayEnabled(TRUE);
    // SetDarkTheme(FALSE);
    // SetVisibleScrollbarEnabled(TRUE);
}

ECode AllContactsFragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    assert(0 && "TODO");
    // ContactEntryListFragment::OnViewCreated(view, savedInstanceState);

    AutoPtr<IView> emptyListView;
    view->FindViewById(R::id::empty_list_view, (IView**)&emptyListView);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    DialerUtils::ConfigureEmptyListView(emptyListView, R::drawable::empty_contacts,
            R::string::all_contacts_empty, res);
    assert(0 && "TODO");
    // AutoPtr<IListView> listView;
    // GetListView((IListView**)&listView);
    // listView->SetEmptyView(emptyListView);

    // ViewUtil::AddBottomPaddingToListViewForFab(listView, res);
    return NOERROR;
}

// TODO:
// ECode AllContactsFragment::CreateListAdapter(
//     /* [out] */ IContactEntryListAdapter** result)
// {
//     VALIDATE_NOT_NULL(result);

//     AutoPtr<MyDefaultContactListAdapter> adapter = new MyDefaultContactListAdapter(this);
//     AutoPtr<IActivity> activity;
//     GetActivity((IActivity**)&activity);
//     adapter->constructor(IContext::Probe(activity));
//     adapter->SetDisplayPhotos(TRUE);
//     adapter->SetFilter(ContactListFilter::CreateFilterWithType(
//             IContactListFilter::FILTER_TYPE_DEFAULT));
//     adapter->SetSectionHeaderDisplayEnabled(IsSectionHeaderDisplayEnabled());
//     *result = adapter;
//     REFCOUNT_ADD(*result);

//     return NOERROR;
// }

ECode AllContactsFragment::InflateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
     return inflater->Inflate(R::layout::show_all_contacts_fragment, NULL, view);
}

ECode AllContactsFragment::OnItemClick(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    AutoPtr<IInterface> tag;
    view->GetTag((IInterface**)&tag);
    IUri* uri = IUri::Probe(tag);
    if (uri != NULL) {
        AutoPtr<IContactsContractQuickContact> qc;
        CContactsContractQuickContact::AcquireSingleton((IContactsContractQuickContact**)&qc);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        qc->ShowQuickContact(IContext::Probe(activity), view,
                uri, IContactsContractQuickContact::MODE_LARGE, NULL);
    }

    return NOERROR;
}

ECode AllContactsFragment::OnItemClick(
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    // Do nothing. Implemented to satisfy ContactEntryListFragment.
    return NOERROR;
}

} // List
} // Dialer
} // Apps
} // Elastos