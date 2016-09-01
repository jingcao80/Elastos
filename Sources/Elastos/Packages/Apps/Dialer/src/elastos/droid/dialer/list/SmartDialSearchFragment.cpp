
#include "elastos/droid/dialer/list/SmartDialSearchFragment.h"
#include "elastos/droid/dialer/list/SmartDialNumberListAdapter.h"
#include "elastos/droid/dialer/dialpad/SmartDialCursorLoader.h"

using Elastos::Droid::Dialer::Dialpad::SmartDialCursorLoader;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

const String SmartDialSearchFragment::TAG("SmartDialSearchFragment");

CAR_INTERFACE_IMPL(SmartDialSearchFragment, SearchFragment, ISmartDialSearchFragment);

AutoPtr<IContactEntryListAdapter> SmartDialSearchFragment::CreateListAdapter()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<SmartDialNumberListAdapter> adapter = new SmartDialNumberListAdapter();
    adapter->constructor(IContext::Probe(activity));
    adapter->SetUseCallableUri(UsesCallableUri());
    adapter->SetQuickContactEnabled(TRUE);
    // Disable the direct call shortcut for the smart dial fragment, since the call button
    // will already be showing anyway.
    Boolean result;
    adapter->SetShortcutEnabled(SmartDialNumberListAdapter::SHORTCUT_DIRECT_CALL, FALSE, &result);
    adapter->SetShortcutEnabled(SmartDialNumberListAdapter::SHORTCUT_ADD_NUMBER_TO_CONTACTS,
            FALSE, &result);
    return IContactEntryListAdapter::Probe(adapter);
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
        AutoPtr<IInterface> adapter;
        GetAdapter((IInterface**)&adapter);
        AutoPtr<IContext> context;
        SearchFragment::GetContext((IContext**)&context);
        AutoPtr<SmartDialCursorLoader> smartLoader = new SmartDialCursorLoader(context);
        ISmartDialNumberListAdapter::Probe(adapter)->ConfigureLoader(ISmartDialCursorLoader::Probe(smartLoader));
        *loader = ILoader::Probe(smartLoader);
        REFCOUNT_ADD(*loader)
    }
    return NOERROR;
}

ECode SmartDialSearchFragment::GetPhoneUri(
    /* [in] */ Int32 position,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);

    AutoPtr<IInterface> adapter;
    GetAdapter((IInterface**)&adapter);
    return ISmartDialNumberListAdapter::Probe(adapter)->GetDataUri(position, uri);
}

} // List
} // Dialer
} // Droid
} // Elastos
