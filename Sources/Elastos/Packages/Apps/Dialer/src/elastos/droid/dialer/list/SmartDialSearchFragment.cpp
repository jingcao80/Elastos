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

ECode SmartDialSearchFragment::constructor()
{
    return SearchFragment::constructor();
}

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
