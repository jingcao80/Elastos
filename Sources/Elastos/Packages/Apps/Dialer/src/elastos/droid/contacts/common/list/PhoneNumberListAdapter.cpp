
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/contacts/common/list/PhoneNumberListAdapter.h"
#include "elastos/droid/contacts/common/list/ContactListItemView.h"
#include "elastos/droid/contacts/common/extensions/ExtensionsFactory.h"
#include "elastos/droid/contacts/common/preference/ContactsPreferences.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Contacts::IContentUris;
using Elastos::Droid::Contacts::CContentUris;
using Elastos::Droid::Contacts::Common::Extensions::ExtensionsFactory;
using Elastos::Droid::Contacts::Common::List::EIID_IPhoneNumberListAdapter;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractPhoneLookupColumns;
using Elastos::Droid::Provider::IContactsContractStreamItemsColumns;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContactNameColumns;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsCallable;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsCallable;
using Elastos::Droid::Provider::IContactsContractDirectory;
using Elastos::Droid::Provider::IContactsContractContactCounts;
using Elastos::Droid::Provider::IContactsContractData;
using Elastos::Droid::Provider::CContactsContractData;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// PhoneNumberListAdapter::PhoneQuery
//=================================================================
static AutoPtr<ArrayOf<String> > InitProjectionPrimary()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(9);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = IContactsContractPhoneLookupColumns::TYPE;
    (*projection)[2] = IContactsContractPhoneLookupColumns::LABEL;
    (*projection)[3] = IContactsContractPhoneLookupColumns::NUMBER;
    (*projection)[4] = IContactsContractStreamItemsColumns::CONTACT_ID;
    (*projection)[5] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*projection)[6] = IContactsContractContactsColumns::PHOTO_ID;
    (*projection)[7] = IContactsContractContactNameColumns::DISPLAY_NAME_PRIMARY;
    (*projection)[8] = IContactsContractContactsColumns::PHOTO_THUMBNAIL_URI;
    return projection;
}
const AutoPtr<ArrayOf<String> > PhoneNumberListAdapter::PhoneQuery::PROJECTION_PRIMARY = InitProjectionPrimary();

static AutoPtr<ArrayOf<String> > InitProjectionAlternative()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(9);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = IContactsContractPhoneLookupColumns::TYPE;
    (*projection)[2] = IContactsContractPhoneLookupColumns::LABEL;
    (*projection)[3] = IContactsContractPhoneLookupColumns::NUMBER;
    (*projection)[4] = IContactsContractStreamItemsColumns::CONTACT_ID;
    (*projection)[5] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*projection)[6] = IContactsContractContactsColumns::PHOTO_ID;
    (*projection)[7] = IContactsContractContactNameColumns::DISPLAY_NAME_ALTERNATIVE;
    (*projection)[8] = IContactsContractContactsColumns::PHOTO_THUMBNAIL_URI;
    return projection;
}
const AutoPtr<ArrayOf<String> > PhoneNumberListAdapter::PhoneQuery::PROJECTION_ALTERNATIVE = InitProjectionAlternative()();
const Int32 PhoneNumberListAdapter::PhoneQuery::PHONE_ID;
const Int32 PhoneNumberListAdapter::PhoneQuery::PHONE_TYPE;
const Int32 PhoneNumberListAdapter::PhoneQuery::PHONE_LABEL;
const Int32 PhoneNumberListAdapter::PhoneQuery::PHONE_NUMBER;
const Int32 PhoneNumberListAdapter::PhoneQuery::CONTACT_ID;
const Int32 PhoneNumberListAdapter::PhoneQuery::LOOKUP_KEY;
const Int32 PhoneNumberListAdapter::PhoneQuery::PHOTO_ID;
const Int32 PhoneNumberListAdapter::PhoneQuery::DISPLAY_NAME;
const Int32 PhoneNumberListAdapter::PhoneQuery::PHOTO_URI;


//=================================================================
// PhoneNumberListAdapter
//=================================================================
const String PhoneNumberListAdapter::TAG("PhoneNumberListAdapter");
const String PhoneNumberListAdapter::IGNORE_NUMBER_TOO_LONG_CLAUSE("length(" + Phone.NUMBER + ") < 1000");

CAR_INTERFACE_IMPL(PhoneNumberListAdapter, ContactEntryListAdapter, IPhoneNumberListAdapter)

PhoneNumberListAdapter::PhoneNumberListAdapter()
    : mFirstExtendedDirectoryId(Elastos::Core::Math::INT64_MAX_VALUE)
    , mUseCallableUri(FALSE)
{}

ECode PhoneNumberListAdapter::constructor(
    /* [in] */ IContext* context)
{
    ContactEntryListAdapter::constructor(context);
    SetDefaultFilterHeaderText(Elastos::Droid::Dialer::R::string::list_filter_phones);
    context->GetText(Elastos::Droid::R::string::unknownName, (ICharSequence**)&mUnknownNameText);
    mCountryIso = GeoUtil::GetCurrentCountryIso(context);

    AutoPtr<IExtendedPhoneDirectoriesManager> manager
            = ExtensionsFactory::GetExtendedPhoneDirectoriesManager();
    if (manager != NULL) {
        manager->GetExtendedDirectories(mContext, (IList**)&mExtendedDirectories);
    }
    else {
        // Empty list to avoid sticky NPE's
        CArrayList::New((IList**)&mExtendedDirectories);
    }
    return NOERROR;
}

AutoPtr<ICharSequence> PhoneNumberListAdapter::GetUnknownNameText()
{
    return mUnknownNameText;
}

ECode PhoneNumberListAdapter::ConfigureLoader(
    /* [in] */ ICursorLoader* loader,
    /* [in] */ Int64 directoryId)
{
    String query;
    GetQueryString(&query);
    if (query.IsNull()) {
        query = String("");
    }
    if (IsExtendedDirectory(directoryId)) {
        AutoPtr<DirectoryPartition> directory = GetExtendedDirectoryFromId(directoryId);
        String contentUri = directory->GetContentUri();
        if (contentUri.IsNull()) {
            Logger::E(TAG, "Extended directory must have a content URL: %s", directory.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->Parse(contentUri, (IUri**)&uri);
        AutoPtr<IUriBuilder> builder;
        uri->BuildUpon((IUriBuilder**)&builder);
        builder->AppendPath(query);
        Int32 result;
        GetDirectoryResultLimit(&result);
        builder->AppendQueryParameter(IContactsContract::LIMIT_PARAM_KEY,
                StringUtils::ToString(result));
        AutoPtr<IUri> u;
        builder->Build((IUri**)&u);
        loader->SetUri(u);
        loader->SetProjection(PROJECTION_PRIMARY);
    }
    else {
        Boolean isRemoteDirectoryQuery = IsRemoteDirectory(directoryId);
        AutoPtr<IUriBuilder> builder;
        Boolean isSearchMode;
        if (IsSearchMode(&isSearchMode), isSearchMode) {
            AutoPtr<IUri> baseUri;
            if (isRemoteDirectoryQuery) {
                AutoPtr<IContactsContractCommonDataKindsPhone> phone;
                CContactsContractCommonDataKindsPhone::AcquireSingleton(
                        (IContactsContractCommonDataKindsPhone**)&phone);
                phone->GetCONTENT_FILTER_URI((IUri**)&baseUri);
            }
            else if (mUseCallableUri) {
                AutoPtr<IContactsContractCommonDataKindsCallable> callable;
                CContactsContractCommonDataKindsCallable::AcquireSingleton(
                        (IContactsContractCommonDataKindsCallable**)&callable);
                callable->GetCONTENT_FILTER_URI((IUri**)&baseUri);
            }
            else {
                AutoPtr<IContactsContractCommonDataKindsPhone> phone;
                CContactsContractCommonDataKindsPhone::AcquireSingleton(
                        (IContactsContractCommonDataKindsPhone**)&phone);
                phone->GetCONTENT_FILTER_URI((IUri**)&baseUri);
            }
            baseUri->BuildUpon((IUriBuilder**)&builder);
            builder->AppendPath(query);      // Builder will encode the query
            builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
                    StringUtils::ToString(directoryId));
            if (isRemoteDirectoryQuery) {
                AutoPtr<IDirectoryPartition> dp = GetDirectoryById(directoryId);
                Int32 result;
                GetDirectoryResultLimit(dp, &result);
                builder->AppendQueryParameter(IContactsContract::LIMIT_PARAM_KEY,
                        StringUtils::ToString(result));
            }
        }
        else {
            AutoPtr<IUri> baseUri;
            if (mUseCallableUri) {
                AutoPtr<IContactsContractCommonDataKindsCallable> callable;
                CContactsContractCommonDataKindsCallable::AcquireSingleton(
                        (IContactsContractCommonDataKindsCallable**)&callable);
                callable->GetCONTENT_FILTER_URI((IUri**)&baseUri);
            }
            else {
                AutoPtr<IContactsContractCommonDataKindsPhone> phone;
                CContactsContractCommonDataKindsPhone::AcquireSingleton(
                        (IContactsContractCommonDataKindsPhone**)&phone);
                phone->GetCONTENT_FILTER_URI((IUri**)&baseUri);
            }
            baseUri->BuildUpon((IUriBuilder**)&builder);
            builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
                    StringUtils::ToString(IContactsContractDirectory::DEFAULT));
            Boolean enabled;
            if (IsSectionHeaderDisplayEnabled(&enabled), enabled) {
                builder->AppendQueryParameter(IContactsContractContactCounts::EXTRA_ADDRESS_BOOK_INDEX,
                        String("true"));
            }
            AutoPtr<IContactListFilter> filter;
            GetFilter((IContactListFilter**)&filter);
            ApplyFilter(loader, builder, directoryId, filter);
        }

        // Ignore invalid phone numbers that are too long. These can potentially cause freezes
        // in the UI and there is no reason to display them.
        String prevSelection;
        loader->GetSelection(&prevSelection);
        String newSelection;
        if (!TextUtils::IsEmpty(prevSelection)) {
            newSelection = prevSelection + " AND " + IGNORE_NUMBER_TOO_LONG_CLAUSE;
        }
        else {
            newSelection = IGNORE_NUMBER_TOO_LONG_CLAUSE;
        }
        loader->SetSelection(newSelection);

        // Remove duplicates when it is possible.
        builder->AppendQueryParameter(IContactsContract::REMOVE_DUPLICATE_ENTRIES, String("true"));
        AutoPtr<IUri> u;
        builder->Build((IUri**)&u);
        loader->SetUri(u);

        // TODO a projection that includes the search snippet
        Int32 order;
        if (GetContactNameDisplayOrder(&order), order == ContactsPreferences::DISPLAY_ORDER_PRIMARY) {
            loader->SetProjection(PROJECTION_PRIMARY);
        }
        else {
            loader->SetProjection(PROJECTION_ALTERNATIVE);
        }

        Int32 order;
        if (GetSortOrder(&order), order == ContactsPreferences::SORT_ORDER_PRIMARY) {
            loader->SetSortOrder(IContactsContractContactNameColumns::SORT_KEY_PRIMARY);
        }
        else {
            loader->SetSortOrder(IContactsContractContactNameColumns::SORT_KEY_ALTERNATIVE);
        }
    }
    return NOERROR;
}

Boolean PhoneNumberListAdapter::IsExtendedDirectory(
    /* [in] */ Int64 directoryId)
{
    return directoryId >= mFirstExtendedDirectoryId;
}

AutoPtr<IDirectoryPartition> PhoneNumberListAdapter::GetExtendedDirectoryFromId(
    /* [in] */ Int64 directoryId)
{
    Int32 directoryIndex = (Int32)(directoryId - mFirstExtendedDirectoryId);
    AutoPtr<IInterface> value;
    mExtendedDirectories->Get(directoryIndex, (IInterface**)&value);
    return IDirectoryPartition::Probe(value);
}

void PhoneNumberListAdapter::ApplyFilter(
    /* [in] */ ICursorLoader* loader,
    /* [in] */ IUriBuilder* uriBuilder,
    /* [in] */ Int64 directoryId,
    /* [in] */ IContactListFilter* filter)
{
    if (filter == NULL || directoryId != IContactsContractDirectory::DEFAULT) {
        return;
    }

    StringBuilder selection;

    Int32 filterType;
    filter->GetFilterType(&filterType);
    switch (filterType) {
        case IContactListFilter::FILTER_TYPE_CUSTOM: {
            selection.Append(IContactsContractContactsColumns::IN_VISIBLE_GROUP + "=1");
            selection.Append(String(" AND ") + Contacts.HAS_PHONE_NUMBER + "=1");
            break;
        }
        case IContactListFilter::FILTER_TYPE_ACCOUNT: {
            AutoPtr<IUriBuilder> result;
            filter->AddAccountQueryParameterToUrl(uriBuilder, (IUriBuilder**)&result);
            break;
        }
        case IContactListFilter::FILTER_TYPE_ALL_ACCOUNTS:
        case IContactListFilter::FILTER_TYPE_DEFAULT:
            break; // No selection needed.
        case IContactListFilter::FILTER_TYPE_WITH_PHONE_NUMBERS_ONLY:
            break; // This adapter is always "phone only", so no selection needed either.
        default:
            Logger::W(TAG, "Unsupported filter type came (type: %d, toString: %s) showing all contacts.",
                    filterType, TO_CSTR(filter));
            // No selection.
            break;
    }
    loader->SetSelection(selection.ToString());
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(0);
    loader->SetSelectionArgs(args);
}

ECode PhoneNumberListAdapter::GetContactDisplayName(
    /* [in] */ Int32 position,
    /* [out] */ String* displayName)
{
    valiVALIDATE_NOT_NULL(displayName)
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    AutoPtr<ICursor> cursor = ICursor::Probe(item);
    return cursor->GetString(DISPLAY_NAME, displayName);
}

ECode PhoneNumberListAdapter::GetPhoneNumber(
    /* [in] */ Int32 position,
    /* [out] */ String* displayNumber)
{
    valiVALIDATE_NOT_NULL(displayNumber)
    *displayNumber = String(NULL);
    AutoPtr<IInterface> temp;
    GetItem(position, (IInterface**)&temp);
    AutoPtr<ICursor> item = ICursor::Probe(item);
    if (item != NULL) {
        return item->GetString(PHONE_NUMBER, displayNumber);
    }
    return NOERROR;
}

ECode PhoneNumberListAdapter::GetDataUri(
    /* [in] */ Int32 position,
    /* [out] */ IUri** uri)
{
    valiVALIDATE_NOT_NULL(uri)
    *uri = NULL;
    Int32 partitionIndex;
    GetPartitionForPosition(position, &partitionIndex);
    AutoPtr<IInterface> temp;
    GetItem(position, (IInterface**)&temp);
    AutoPtr<ICursor> item = ICursor::Probe(item);
    if (item != NULL) {
        return GetDataUri(partitionIndex, item, uri);
    }
    return NOERROR;
}

ECode PhoneNumberListAdapter::GetDataUri(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor,
    /* [out] */ IUri** uri)
{
    valiVALIDATE_NOT_NULL(uri)
    *uri = NULL;

    AutoPtr<ICompositeCursorAdapterPartition> temp;
    GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
    AutoPtr<IDirectoryPartition> partition = IDirectoryPartition::Probe(temp);
    Int64 directoryId = ((DirectoryPartition*)partition.Get())->GetDirectoryId();
    if (!IsRemoteDirectory(directoryId)) {
        Int64 phoneId;
        cursor->GetInt64(PHONE_ID, &phoneId);
        AutoPtr<IContentUris> uris;
        CContentUris::AcquireSingleton((IContentUris**)&uris);
        AutoPtr<IContactsContractData> data;
        CContactsContractData::AcquireSingleton((IContactsContractData**)&data);
        AutoPtr<IUri> u;
        data->GetCONTENT_URI((IUri**)&U);
        return uris->WithAppendedId(U, phoneId, uri);
    }
    return NOERROR;
}

AutoPtr<IView> PhoneNumberListAdapter::NewView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> temp = ContactEntryListAdapter::NewView(context, partition, cursor, position, parent);
    AutoPtr<ContactListItemView> view = (ContactListItemView*)IContactListItemView::Probe(temp);
    view->SetUnknownNameText(mUnknownNameText);
    Boolean enabled;
    IsQuickContactEnabled(&enabled);
    view->SetQuickContactEnabled(enabled);
    view->SetPhotoPosition(mPhotoPosition);
    return temp;
}

void PhoneNumberListAdapter::SetHighlight(
    /* [in] */ IContactListItemView* view,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<ContactListItemView> view = (ContactListItemView*)IContactListItemView::Probe(temp);
    Boolean isSearchMode;
    IsSearchMode(&isSearchMode);
    String str(NULL);
    if (isSearchMode)
        GetUpperCaseQueryString(&str);
    view->SetHighlightedPrefix(str);
}

Int32 PhoneNumberListAdapter::GetResultCount(
    /* [in] */ ICursor* cursor)
{
    if (cursor == NULL) {
        return 0;
    }
    Boolean result;
    cursor->MoveToPosition(-1, &result);
    Int64 curContactId = -1;
    Int32 numContacts = 0;
    Boolean next;
    while(cursor->MoveToNext(&next), next) {
        Int64 contactId;
        cursor->GetInt64(CONTACT_ID, &contactId);
        if (contactId != curContactId) {
            curContactId = contactId;
            ++numContacts;
        }
    }
    return numContacts;
}

void PhoneNumberListAdapter::BindView(
    /* [in] */ IView* itemView,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position)
{
    ContactEntryListAdapter::BindView(itemView, partition, cursor, position);
    AutoPtr<ContactListItemView> view = (ContactListItemView*)IContactListItemView::Probe(itemView);

    SetHighlight(view, cursor);

    // Look at elements before and after this position, checking if contact IDs are same.
    // If they have one same contact ID, it means they can be grouped.
    //
    // In one group, only the first entry will show its photo and its name, and the other
    // entries in the group show just their data (e.g. phone number, email address).
    Boolean result;
    cursor->MoveToPosition(position, &result);
    Boolean isFirstEntry = TRUE;
    Boolean showBottomDivider = TRUE;
    Int64 currentContactId;
    cursor->GetInt64(CONTACT_ID, &currentContactId);
    Boolean isBeforeFirst;
    if ((cursor->MoveToPrevious(&result), result)
            && (cursor->IsBeforeFirst(&isBeforeFirst), !isBeforeFirst)) {
        Int64 previousContactId;
        cursor->GetInt64(CONTACT_ID, &previousContactId);
        if (currentContactId == previousContactId) {
            isFirstEntry = FALSE;
        }
    }
    cursor->MoveToPosition(position, &result);
    Boolean isAfterLast;
    if ((cursor->MoveToNext(&result), result) && (cursor->IsAfterLast(&isAfterLast), !isAfterLast)) {
        Int64 nextContactId;
        cursor->GetInt64(CONTACT_ID, &nextContactId);
        if (currentContactId == nextContactId) {
            // The following entry should be in the same group, which means we don't want a
            // divider between them.
            // TODO: we want a different divider than the divider between groups. Just hiding
            // this divider won't be enough.
            showBottomDivider = FALSE;
        }
    }
    cursor->MoveToPosition(position, &result);

    BindViewId((IContactListItemView*)view, cursor, PHONE_ID);

    BindSectionHeaderAndDivider((IContactListItemView*)view, position);
    if (isFirstEntry) {
        BindName((IContactListItemView*)view, cursor);
        Boolean enabled;
        if (IsQuickContactEnabled(&enabled), enabled) {
            BindQuickContact((IContactListItemView*)view, partition, cursor, PHOTO_ID,
                    PHOTO_URI, CONTACT_ID, LOOKUP_KEY, DISPLAY_NAME);
        }
        else {
            Boolean displayPhotos;
            if (GetDisplayPhotos(&displayPhotos), displayPhotos) {
                BindPhoto((IContactListItemView*)view, partition, cursor);
            }
        }
    }
    else {
        UnbindName((IContactListItemView*)view);

        view->RemovePhotoView(TRUE, FALSE);
    }

    AutoPtr<ICompositeCursorAdapterPartition> temp;
    GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
    AutoPtr<DirectoryPartition> directory = (DirectoryPartition*)IDirectoryPartition::Probe(temp);
    BindPhoneNumber((IContactListItemView*)view, cursor, directory->IsDisplayNumber());
}

void PhoneNumberListAdapter::BindPhoneNumber(
    /* [in] */ IContactListItemView* view,
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean displayNumber);

void PhoneNumberListAdapter::BindSectionHeaderAndDivider(
    /* [in] */ IContactListItemView* view,
    /* [in] */ Int32 position);

void PhoneNumberListAdapter::BindName(
    /* [in] */ IContactListItemView* view,
    /* [in] */ ICursor* cursor);

void PhoneNumberListAdapter::UnbindName(
    /* [in] */ IContactListItemView* view);

void PhoneNumberListAdapter::BindPhoto(
    /* [in] */ IContactListItemView* view,
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor);

ECode PhoneNumberListAdapter::SetPhotoPosition(
    /* [in] */ PhotoPosition photoPosition);

ECode PhoneNumberListAdapter::GetPhotoPosition(
    /* [out] */ PhotoPosition* photoPosition);

ECode PhoneNumberListAdapter::SetUseCallableUri(
    /* [in] */ Boolean useCallableUri);

ECode PhoneNumberListAdapter::UsesCallableUri(
    /* [out] */ Boolean* useCallableUri);

ECode PhoneNumberListAdapter::ChangeDirectories(
    /* [in] */ ICursor* cursor);

AutoPtr<IUri> PhoneNumberListAdapter::GetContactUri(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 contactIdColumn,
    /* [in] */ Int32 lookUpKeyColumn);

} // List
} // Common
} // Contacts
} // Droid
} // Elastos