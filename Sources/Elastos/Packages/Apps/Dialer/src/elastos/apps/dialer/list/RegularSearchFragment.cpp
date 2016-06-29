
#include "elastos/apps/dialer/list/RegularSearchFragment.h"
#include "elastos/apps/dialer/CObjectFactory.h"

using Elastos::Apps::Dialer::IObjectFactory;
using Elastos::Apps::Dialer::CObjectFactory;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

AutoPtr<ICachedNumberLookupService> CreateLooupService()
{
    AutoPtr<IObjectFactory> factory;
    CObjectFactory::AcquireSingleton((IObjectFactory**)&factory);
    AutoPtr<ICachedNumberLookupService> service;
    factory->NewCachedNumberLookupService((ICachedNumberLookupService**)&service);
    return service;
}

const Int32 RegularSearchFragment::SEARCH_DIRECTORY_RESULT_LIMIT = 5;

const AutoPtr<ICachedNumberLookupService> RegularSearchFragment::mCachedNumberLookupService = CreateLooupService();

CAR_INTERFACE_IMPL(RegularSearchFragment, SearchFragment, IRegularSearchFragment);

ECode RegularSearchFragment::constructor()
{
    return ConfigureDirectorySearch();
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
    assert(0 && "TODO");
    // IPinnedHeaderListView::Probe(listView)->SetScrollToSectionOnHeaderTouch(TRUE);
    return NOERROR;
}

// TODO:
// AutoPtr<IContactEntryListAdapter> RegularSearchFragment::CreateListAdapter()
// {
//     AutoPtr<IActivity> activity;
//     GetActivity(&activity);
//     AutoPtr<IRegularSearchListAdapter> adapter;
//     CRegularSearchListAdapter::New(activity, (IRegularSearchListAdapter**)&adapter);
//     adapter->SetDisplayPhotos(TRUE);
//     Boolean result;
//     UsesCallableUri(&result);
//     adapter->SetUseCallableUri(result);
//     return adapter;
// }

ECode RegularSearchFragment::CacheContactInfo(
    /* [in] */ Int32 position)
{
    if (mDraggedEntry != NULL) {
        UnstarAndUnpinContact(((CContactEntry*)mDraggedEntry)->mLookupUri);
        mAwaitingRemove = TRUE;
    }
    return NOERROR;
}

} // List
} // Dialer
} // Apps
} // Elastos
