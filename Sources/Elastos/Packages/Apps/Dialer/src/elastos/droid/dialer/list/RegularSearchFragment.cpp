
#include "elastos/droid/dialer/list/RegularSearchFragment.h"
#include "elastos/droid/dialer/list/RegularSearchListAdapter.h"
#include "elastos/droid/dialerbind/ObjectFactory.h"

using Elastos::Droid::Contacts::Common::List::IPinnedHeaderListView;
using Elastos::Droid::DialerBind::ObjectFactory;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

static AutoPtr<ICachedNumberLookupService> CreateLooupService()
{
    AutoPtr<ICachedNumberLookupService> service = ObjectFactory::NewCachedNumberLookupService();
    return service;
}

const Int32 RegularSearchFragment::SEARCH_DIRECTORY_RESULT_LIMIT = 5;

const AutoPtr<ICachedNumberLookupService> RegularSearchFragment::mCachedNumberLookupService = CreateLooupService();

CAR_INTERFACE_IMPL(RegularSearchFragment, SearchFragment, IRegularSearchFragment);

RegularSearchFragment::RegularSearchFragment()
{
    ConfigureDirectorySearch();
}

ECode RegularSearchFragment::constructor()
{
    return SearchFragment::constructor();
}

ECode RegularSearchFragment::ConfigureDirectorySearch()
{
    SetDirectorySearchEnabled(TRUE);
    SetDirectoryResultLimit(SEARCH_DIRECTORY_RESULT_LIMIT);
    return NOERROR;
}

ECode RegularSearchFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container)
{
    SearchFragment::OnCreateView(inflater, container);
    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    IPinnedHeaderListView::Probe(listView)->SetScrollToSectionOnHeaderTouch(TRUE);
    return NOERROR;
}

AutoPtr<IContactEntryListAdapter> RegularSearchFragment::CreateListAdapter()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<RegularSearchListAdapter> adapter = new RegularSearchListAdapter();
    adapter->constructor(IContext::Probe(activity));
    adapter->SetDisplayPhotos(TRUE);
    adapter->SetUseCallableUri(UsesCallableUri());
    return adapter;
}

ECode RegularSearchFragment::CacheContactInfo(
    /* [in] */ Int32 position)
{
    if (mCachedNumberLookupService != NULL) {
        AutoPtr<IInterface> temp;
        GetAdapter((IInterface**)&temp);
        AutoPtr<IRegularSearchListAdapter> adapter = IRegularSearchListAdapter::Probe(temp);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<ICachedContactInfo> info;
        adapter->GetContactInfo(mCachedNumberLookupService, position, (ICachedContactInfo**)&info);
        mCachedNumberLookupService->AddContact(context, info);
    }
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
