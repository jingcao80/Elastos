
#include "list/SmartDialSearchFragment.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace List {

const String SmartDialSearchFragment::TAG("SmartDialSearchFragment");

CAR_INTERFACE_DECL(SmartDialSearchFragment, SearchFragment, ISmartDialSearchFragment);

ECode SmartDialSearchFragment::CreateListAdapter(
    /* [out] */ IContactEntryListAdapter** listAdapter)
{
    VALIDATE_NOT_NULL(listAdapter);
    AutoPtr<IActivity> activity;
    GetActivity(&activity);
    AutoPtr<ISmartDialNumberListAdapter> adapter;
    CSmartDialNumberListAdapter::New(
            IActivity::Probe(activity), (ISmartDialNumberListAdapter**)&adapter);
    Boolean result;
    SearchFragment::UsesCallableUri(&result);
    adapter->SetUseCallableUri(result);
    adapter->SetQuickContactEnabled(TRUE);
    // Disable the direct call shortcut for the smart dial fragment, since the call button
    // will already be showing anyway.
    adapter->SetShortcutEnabled(ISmartDialNumberListAdapter::SHORTCUT_DIRECT_CALL, FALSE);
    adapter->SetShortcutEnabled(ISmartDialNumberListAdapter::SHORTCUT_ADD_NUMBER_TO_CONTACTS,
            FALSE);
    *listAdapter = adapter;
    REFCOUNT_ADD(*listAdapter);
    return NOERROR;
}

ECode SmartDialSearchFragment::OnCreateLoader(
    /* [in] */ Int32 id,
    /* [in] */ IBundle* args,
    /* [out] */ ILoader** loader)
{
    VALIDATE_NOT_NULL(loader);

    // Smart dialing does not support Directory Load, falls back to normal search instead.
    Int32 loaderId;
    GetDirectoryLoaderId(&loaderId);
    if (id == loaderId) {
        return SearchFragment::OnCreateLoader(id, args, loader);
    }
    else {
        AutoPtr<IInterface> adpater;
        GetAdapter((IInterface**)&adpater);
        AutoPtr<IContext> context;
        SearchFragment::GetContext((IContext**)&context);
        AutoPtr<ISmartDialCursorLoader> smartLoader;
        CSmartDialCursorLoader::New(context, (ISmartDialCursorLoader**)&smartLoader);
        ISmartDialNumberListAdapter::Probe(adapter)->ConfigureLoader(smartLoader);
        *loader = smartLoader;
        return NOERROR;
    }
}

ECode SmartDialSearchFragment::GetPhoneUri(
    /* [in] */ Int32 position,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IInterface> adpater;
    GetAdapter((IInterface**)&adpater);
    return ISmartDialNumberListAdapter::Probe(adapter)->GetDataUri(position, uri);
}

} // List
} // Dialer
} // Apps
} // Elastos
