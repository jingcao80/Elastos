
#include "list/AllContactsFragment.h"

using Elastos::Droid::Provider::IContactsContractQuickContact;
using Elastos::Droid::Provider::CContactsContractQuickContact;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

AllContactsFragment::MyDefaultContactListAdapter::MyDefaultContactListAdapter(
    /* [in] */ AllContactsFragment* host)
    : mHost(host)
{}

ECode AllContactsFragment::MyDefaultContactListAdapter::BindView(
    /* [in] */ IView* itemView,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position)
{
    DefaultContactListAdapter::BindView(itemView, partition, cursor, position);

    AutoPtr<IUri> uri;
    GetContactUri(partition, cursor, (IUri**)&uri);
    itemView->SetTag(uri);

    return NOERROR;
}

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

ECode AllContactsFragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    ContactEntryListFragment::OnViewCreated(view, savedInstanceState);

    View emptyListView = view.findViewById(R.id.empty_list_view);
    DialerUtils.configureEmptyListView(emptyListView, R.drawable.empty_contacts,
            R.string.all_contacts_empty, getResources());
    getListView().setEmptyView(emptyListView);

    ViewUtil.addBottomPaddingToListViewForFab(getListView(), getResources());
}

ECode AllContactsFragment::CreateListAdapter(
    /* [out] */ IContactEntryListAdapter** result)
{
    VALUE_NOT_NULL(result);

    AutoPtr<MyDefaultContactListAdapter> adapter = new MyDefaultContactListAdapter(this);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    adapter->constructor(IContext::Probe(activity));
    adapter->SetDisplayPhotos(TRUE);
    adapter->SetFilter(ContactListFilter::CreateFilterWithType(
            IContactListFilter::FILTER_TYPE_DEFAULT));
    adapter->SetSectionHeaderDisplayEnabled(IsSectionHeaderDisplayEnabled());
    *result = adapter;
    REFCOUNT_ADD(*result);

    return NOERROR;
}

ECode AllContactsFragment::InflateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [out] */ IView** view)
{
    VALUE_NOT_NULL(view);
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