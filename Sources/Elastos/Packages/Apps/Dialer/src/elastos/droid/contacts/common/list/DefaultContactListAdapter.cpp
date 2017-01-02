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

#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/contacts/common/list/DefaultContactListAdapter.h"
#include "elastos/droid/contacts/common/list/ProfileAndContactsLoader.h"
#include "elastos/droid/contacts/common/list/ContactListItemView.h"
#include "elastos/droid/contacts/common/preference/ContactsPreferences.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::Common::Widget::ICompositeCursorAdapterPartition;
using Elastos::Droid::Contacts::Common::Preference::ContactsPreferences;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Preference::IPreferenceManagerHelper;
using Elastos::Droid::Preference::CPreferenceManagerHelper;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContactOptionsColumns;
using Elastos::Droid::Provider::IContactsContractSearchSnippetColumns;
using Elastos::Droid::Provider::IContactsContractContactNameColumns;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::IContactsContractDirectory;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

ECode DefaultContactListAdapter::constructor(
    /* [in] */ IContext* context)
{
    return ContactListAdapter::constructor(context);
}

ECode DefaultContactListAdapter::ConfigureLoader(
    /* [in] */ ICursorLoader* loader,
    /* [in] */ Int64 directoryId)
{
    AutoPtr<IProfileAndContactsLoader> l = IProfileAndContactsLoader::Probe(loader);
    if (l != NULL) {
        Boolean  result;
        ShouldIncludeProfile(&result);
        ((ProfileAndContactsLoader*)l.Get())->SetLoadProfile(result);
    }

    AutoPtr<IContactListFilter> filter;
    GetFilter((IContactListFilter**)&filter);
    Boolean isSearchMode;
    if (IsSearchMode(&isSearchMode), isSearchMode) {
        String query;
        GetQueryString(&query);
        if (query.IsNull()) {
            query = String("");
        }
        query = query.Trim();
        if (TextUtils::IsEmpty(query)) {
            // Regardless of the directory, we don't want anything returned,
            // so let's just send a "nothing" query to the local directory.
            AutoPtr<IContactsContractContacts> contacts;
            CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
            AutoPtr<IUri> uri;
            contacts->GetCONTENT_URI((IUri**)&uri);
            loader->SetUri(uri);
            loader->SetProjection(GetProjection(FALSE));
            loader->SetSelection(String("0"));
        }
        else {
            AutoPtr<IContactsContractContacts> contacts;
            CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
            AutoPtr<IUri> contentUri;
            contacts->GetCONTENT_FILTER_URI((IUri**)&contentUri);
            AutoPtr<IUriBuilder> builder;
            contentUri->BuildUpon((IUriBuilder**)&builder);
            builder->AppendPath(query);      // Builder will encode the query
            builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
                    StringUtils::ToString(directoryId));
            if (directoryId != IContactsContractDirectory::DEFAULT
                    && directoryId != IContactsContractDirectory::LOCAL_INVISIBLE) {
                Int32 dir;
                GetDirectoryResultLimit(GetDirectoryById(directoryId), &dir);
                builder->AppendQueryParameter(IContactsContract::LIMIT_PARAM_KEY,
                        StringUtils::ToString(dir));
            }
            builder->AppendQueryParameter(IContactsContractSearchSnippetColumns::DEFERRED_SNIPPETING_KEY, String("1"));
            AutoPtr<IUri> uri;
            builder->Build((IUri**)&uri);
            loader->SetUri(uri);
            loader->SetProjection(GetProjection(TRUE));
        }
    }
    else {
        ConfigureUri(loader, directoryId, filter);
        loader->SetProjection(GetProjection(FALSE));
        ConfigureSelection(loader, directoryId, filter);
    }

    String sortOrder;
    Int32 order;
    if (GetSortOrder(&order), order == ContactsPreferences::SORT_ORDER_PRIMARY) {
        sortOrder = IContactsContractContactNameColumns::SORT_KEY_PRIMARY;
    }
    else {
        sortOrder = IContactsContractContactNameColumns::SORT_KEY_ALTERNATIVE;
    }

    loader->SetSortOrder(sortOrder);
    return NOERROR;
}

void DefaultContactListAdapter::ConfigureUri(
    /* [in] */ ICursorLoader* loader,
    /* [in] */ Int64 directoryId,
    /* [in] */ IContactListFilter* filter)
{
    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> uri;
    contacts->GetCONTENT_URI((IUri**)&uri);
    Int32 filterType;
    if (filter != NULL
            && (filter->GetFilterType(&filterType), filterType == IContactListFilter::FILTER_TYPE_SINGLE_CONTACT)) {
        String lookupKey = GetSelectedContactLookupKey();
        if (!lookupKey.IsNull()) {
            AutoPtr<IContactsContractContacts> contacts;
            CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
            AutoPtr<IUri> lookupUri;
            contacts->GetCONTENT_LOOKUP_URI((IUri**)&lookupUri);
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            helper->WithAppendedPath(lookupUri, lookupKey, (IUri**)&uri);
        }
        else {
            AutoPtr<IContactsContractContacts> contacts;
            CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
            AutoPtr<IUri> contentUri;
            contacts->GetCONTENT_URI((IUri**)&contentUri);
            AutoPtr<IContentUris> uris;
            CContentUris::AcquireSingleton((IContentUris**)&uris);
            uris->WithAppendedId(contentUri, GetSelectedContactId(), (IUri**)&uri);
        }
    }

    Boolean isEnabled;
    if (directoryId == IContactsContractDirectory::DEFAULT && (IsSectionHeaderDisplayEnabled(&isEnabled), isEnabled)) {
        uri = ContactListAdapter::BuildSectionIndexerUri(uri);
    }

    // The "All accounts" filter is the same as the entire contents of Directory.DEFAULT
    if (filter != NULL
            && (filter->GetFilterType(&filterType), filterType != IContactListFilter::FILTER_TYPE_CUSTOM)
            && (filter->GetFilterType(&filterType), filterType != IContactListFilter::FILTER_TYPE_SINGLE_CONTACT)) {
        AutoPtr<IUriBuilder> builder;
        uri->BuildUpon((IUriBuilder**)&builder);
        builder->AppendQueryParameter(
                IContactsContract::DIRECTORY_PARAM_KEY, StringUtils::ToString(IContactsContractDirectory::DEFAULT));
        if (filter->GetFilterType(&filterType), filterType == IContactListFilter::FILTER_TYPE_ACCOUNT) {
            AutoPtr<IUriBuilder> out;
            filter->AddAccountQueryParameterToUrl(builder, (IUriBuilder**)&out);
        }
        uri = NULL;
        builder->Build((IUri**)&uri);
    }

    loader->SetUri(uri);
}

void DefaultContactListAdapter::ConfigureSelection(
    /* [in] */ ICursorLoader* loader,
    /* [in] */ Int64 directoryId,
    /* [in] */ IContactListFilter* filter)
{
    if (filter == NULL) {
        return;
    }

    if (directoryId != IContactsContractDirectory::DEFAULT) {
        return;
    }

    StringBuilder selection;
    AutoPtr<ArrayOf<String> > selectionArgs = ArrayOf<String>::Alloc(0);

    Int32 filterType;
    filter->GetFilterType(&filterType);
    switch (filterType) {
        case IContactListFilter::FILTER_TYPE_ALL_ACCOUNTS: {
            // We have already added directory=0 to the URI, which takes care of this
            // filter
            break;
        }
        case IContactListFilter::FILTER_TYPE_SINGLE_CONTACT: {
            // We have already added the lookup key to the URI, which takes care of this
            // filter
            break;
        }
        case IContactListFilter::FILTER_TYPE_STARRED: {
            selection.Append(IContactsContractContactOptionsColumns::STARRED + "!=0");
            break;
        }
        case IContactListFilter::FILTER_TYPE_WITH_PHONE_NUMBERS_ONLY: {
            selection.Append(IContactsContractContactsColumns::HAS_PHONE_NUMBER + "=1");
            break;
        }
        case IContactListFilter::FILTER_TYPE_CUSTOM: {
            selection.Append(IContactsContractContactsColumns::IN_VISIBLE_GROUP + "=1");
            if (IsCustomFilterForPhoneNumbersOnly()) {
                selection.Append(" AND ");
                selection.Append(IContactsContractContactsColumns::HAS_PHONE_NUMBER + "=1");
            }
            break;
        }
        case IContactListFilter::FILTER_TYPE_ACCOUNT: {
            // We use query parameters for account filter, so no selection to add here.
            break;
        }
    }
    loader->SetSelection(selection.ToString());
    loader->SetSelectionArgs(selectionArgs);
}

void DefaultContactListAdapter::BindView(
    /* [in] */ IView* itemView,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position)
{
    ContactListAdapter::BindView(itemView, partition, cursor, position);
    AutoPtr<ContactListItemView> view = (ContactListItemView*)IContactListItemView::Probe(itemView);

    Boolean isSearchMode;
    String str(NULL);
    if (IsSearchMode(&isSearchMode), isSearchMode) {
        GetUpperCaseQueryString(&str);
    }
    view->SetHighlightedPrefix(str);

    Boolean isVisible;
    if (IsSelectionVisible(&isVisible), isVisible) {
        view->SetActivated(IsSelectedContact(partition, cursor));
    }

    BindSectionHeaderAndDivider(view, position, cursor);

    Boolean isEnabled;
    if (IsQuickContactEnabled(&isEnabled), isEnabled) {
        BindQuickContact(view, partition, cursor, ContactQuery::CONTACT_PHOTO_ID,
                ContactQuery::CONTACT_PHOTO_URI, ContactQuery::CONTACT_ID,
                ContactQuery::CONTACT_LOOKUP_KEY, ContactQuery::CONTACT_DISPLAY_NAME);
    }
    else {
        Boolean result;
        if (GetDisplayPhotos(&result), result) {
            BindPhoto(view, partition, cursor);
        }
    }

    BindNameAndViewId(view, cursor);
    BindPresenceAndStatusMessage(view, cursor);

    if (IsSearchMode(&isSearchMode), isSearchMode) {
        BindSearchSnippet(view, cursor);
    }
    else {
        view->SetSnippet(String(NULL));
    }
}

Boolean DefaultContactListAdapter::IsCustomFilterForPhoneNumbersOnly()
{
    // TODO: this flag should not be stored in shared prefs.  It needs to be in the db.
    AutoPtr<ISharedPreferences> prefs;
    AutoPtr<IPreferenceManagerHelper> helper;
    CPreferenceManagerHelper::AcquireSingleton((IPreferenceManagerHelper**)&helper);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    helper->GetDefaultSharedPreferences(ctx, (ISharedPreferences**)&prefs);
    Boolean value;
    prefs->GetBoolean(ContactsPreferences::PREF_DISPLAY_ONLY_PHONES,
            ContactsPreferences::PREF_DISPLAY_ONLY_PHONES_DEFAULT, &value);
    return value;
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos