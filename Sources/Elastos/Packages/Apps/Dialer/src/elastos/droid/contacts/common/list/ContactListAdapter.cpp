
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/contacts/common/list/ContactListAdapter.h"
#include "elastos/droid/contacts/common/list/ContactListItemView.h"
#include "elastos/droid/contacts/common/list/DirectoryPartition.h"
#include "elastos/droid/contacts/common/preference/ContactsPreferences.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringUtils.h>
#include "R.h"

using Elastos::Droid::Contacts::Common::List::EIID_IContactListAdapter;
using Elastos::Droid::Contacts::Common::Preference::ContactsPreferences;
using Elastos::Droid::Common::Widget::ICompositeCursorAdapterPartition;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::IContactsContractContactStatusColumns;
using Elastos::Droid::Provider::IContactsContractSearchSnippetColumns;
using Elastos::Droid::Provider::IContactsContractContactsColumns;
using Elastos::Droid::Provider::IContactsContractContactNameColumns;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::IContactsContractDirectory;
using Elastos::Droid::Provider::IContactsContractContactCounts;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringUtils;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

//=================================================================
// ContactListAdapter::ContactQuery
//=================================================================
static AutoPtr<ArrayOf<String> > InitProjectionPrimary()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(8);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = IContactsContractContactNameColumns::DISPLAY_NAME_PRIMARY;
    (*projection)[2] = IContactsContractContactStatusColumns::CONTACT_PRESENCE;
    (*projection)[3] = IContactsContractContactStatusColumns::CONTACT_STATUS;
    (*projection)[4] = IContactsContractContactsColumns::PHOTO_ID;
    (*projection)[5] = IContactsContractContactsColumns::PHOTO_THUMBNAIL_URI;
    (*projection)[6] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*projection)[7] = IContactsContractContactsColumns::IS_USER_PROFILE;
    return projection;
}
const AutoPtr<ArrayOf<String> > ContactListAdapter::ContactQuery::CONTACT_PROJECTION_PRIMARY = InitProjectionPrimary();

static AutoPtr<ArrayOf<String> > InitProjectionAlternative()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(8);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = IContactsContractContactNameColumns::DISPLAY_NAME_ALTERNATIVE;
    (*projection)[2] = IContactsContractContactStatusColumns::CONTACT_PRESENCE;
    (*projection)[3] = IContactsContractContactStatusColumns::CONTACT_STATUS;
    (*projection)[4] = IContactsContractContactsColumns::PHOTO_ID;
    (*projection)[5] = IContactsContractContactsColumns::PHOTO_THUMBNAIL_URI;
    (*projection)[6] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*projection)[7] = IContactsContractContactsColumns::IS_USER_PROFILE;
    return projection;
}
const AutoPtr<ArrayOf<String> > ContactListAdapter::ContactQuery::CONTACT_PROJECTION_ALTERNATIVE = InitProjectionAlternative();

static AutoPtr<ArrayOf<String> > InitFilterProjectionPrimary()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(9);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = IContactsContractContactNameColumns::DISPLAY_NAME_PRIMARY;
    (*projection)[2] = IContactsContractContactStatusColumns::CONTACT_PRESENCE;
    (*projection)[3] = IContactsContractContactStatusColumns::CONTACT_STATUS;
    (*projection)[4] = IContactsContractContactsColumns::PHOTO_ID;
    (*projection)[5] = IContactsContractContactsColumns::PHOTO_THUMBNAIL_URI;
    (*projection)[6] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*projection)[7] = IContactsContractContactsColumns::IS_USER_PROFILE;
    (*projection)[8] = IContactsContractSearchSnippetColumns::SNIPPET;
    return projection;
}
const AutoPtr<ArrayOf<String> > ContactListAdapter::ContactQuery::FILTER_PROJECTION_PRIMARY = InitFilterProjectionPrimary();

static AutoPtr<ArrayOf<String> > InitFilterProjectionAlternative()
{
    AutoPtr<ArrayOf<String> > projection = ArrayOf<String>::Alloc(9);
    (*projection)[0] = IBaseColumns::ID;
    (*projection)[1] = IContactsContractContactNameColumns::DISPLAY_NAME_ALTERNATIVE;
    (*projection)[2] = IContactsContractContactStatusColumns::CONTACT_PRESENCE;
    (*projection)[3] = IContactsContractContactStatusColumns::CONTACT_STATUS;
    (*projection)[4] = IContactsContractContactsColumns::PHOTO_ID;
    (*projection)[5] = IContactsContractContactsColumns::PHOTO_THUMBNAIL_URI;
    (*projection)[6] = IContactsContractContactsColumns::LOOKUP_KEY;
    (*projection)[7] = IContactsContractContactsColumns::IS_USER_PROFILE;
    (*projection)[8] = IContactsContractSearchSnippetColumns::SNIPPET;
    return projection;
}
const AutoPtr<ArrayOf<String> > ContactListAdapter::ContactQuery::FILTER_PROJECTION_ALTERNATIVE = InitFilterProjectionAlternative();
const Int32 ContactListAdapter::ContactQuery::CONTACT_ID;
const Int32 ContactListAdapter::ContactQuery::CONTACT_DISPLAY_NAME;
const Int32 ContactListAdapter::ContactQuery::CONTACT_PRESENCE_STATUS;
const Int32 ContactListAdapter::ContactQuery::CONTACT_CONTACT_STATUS;
const Int32 ContactListAdapter::ContactQuery::CONTACT_PHOTO_ID;
const Int32 ContactListAdapter::ContactQuery::CONTACT_PHOTO_URI;
const Int32 ContactListAdapter::ContactQuery::CONTACT_LOOKUP_KEY;
const Int32 ContactListAdapter::ContactQuery::CONTACT_IS_USER_PROFILE;
const Int32 ContactListAdapter::ContactQuery::CONTACT_SNIPPET;


//=================================================================
// ContactListAdapter
//=================================================================
CAR_INTERFACE_IMPL(ContactListAdapter, ContactEntryListAdapter, IContactListAdapter)

ContactListAdapter::ContactListAdapter()
    : mSelectedContactDirectoryId(0)
    , mSelectedContactId(0)
{}

ECode ContactListAdapter::constructor(
    /* [in] */ IContext* context)
{
    ContactEntryListAdapter::constructor(context);
    context->GetText(Elastos::Droid::Dialer::R::string::missing_name, (ICharSequence**)&mUnknownNameText);
    return NOERROR;
}

void ContactListAdapter::SetPhotoPosition(
    /* [in] */ PhotoPosition photoPosition)
{
    mPhotoPosition = photoPosition;
}

PhotoPosition ContactListAdapter::GetPhotoPosition()
{
    return mPhotoPosition;
}

AutoPtr<ICharSequence> ContactListAdapter::GetUnknownNameText()
{
    return mUnknownNameText;
}

Int64 ContactListAdapter::GetSelectedContactDirectoryId()
{
    return mSelectedContactDirectoryId;
}

String ContactListAdapter::GetSelectedContactLookupKey()
{
    return mSelectedContactLookupKey;
}

Int64 ContactListAdapter::GetSelectedContactId()
{
    return mSelectedContactId;
}

void ContactListAdapter::SetSelectedContact(
    /* [in] */ Int64 selectedDirectoryId,
    /* [in] */ const String& lookupKey,
    /* [in] */ Int64 contactId)
{
    mSelectedContactDirectoryId = selectedDirectoryId;
    mSelectedContactLookupKey = lookupKey;
    mSelectedContactId = contactId;
}

AutoPtr<IUri> ContactListAdapter::BuildSectionIndexerUri(
    /* [in] */ IUri* uri)
{
    AutoPtr<IUriBuilder> builder;
    uri->BuildUpon((IUriBuilder**)&builder);
    builder->AppendQueryParameter(IContactsContractContactCounts::EXTRA_ADDRESS_BOOK_INDEX,
            String("true"));
    AutoPtr<IUri> u;
    builder->Build((IUri**)&u);
    return u;
}

ECode ContactListAdapter::GetContactDisplayName(
    /* [in] */ Int32 position,
    /* [out] */ String* displayName)
{
    VALIDATE_NOT_NULL(displayName)
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    AutoPtr<ICursor> cursor = ICursor::Probe(item);
    return cursor->GetString(ContactQuery::CONTACT_DISPLAY_NAME, displayName);
}

AutoPtr<IUri> ContactListAdapter::GetContactUri(
    /* [in] */ Int32 position)
{
    Int32 partitionIndex;
    GetPartitionForPosition(position, &partitionIndex);
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    AutoPtr<ICursor> item = ICursor::Probe(item);
    if (item != NULL) {
        return GetContactUri(partitionIndex, item)
    }
    return NULL;
}

AutoPtr<IUri> ContactListAdapter::GetContactUri(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor)
{
    Int64 contactId;
    cursor->GetInt64(ContactQuery::CONTACT_ID, &contactId);
    String lookupKey;
    cursor->GetString(ContactQuery::CONTACT_LOOKUP_KEY, &lookupKey);
    AutoPtr<IUri> uri;
    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    contacts->GetLookupUri(contactId, lookupKey, (IUri**)&uri);
    AutoPtr<ICompositeCursorAdapterPartition> temp;
    GetPartition(partitionIndex, (ICompositeCursorAdapterPartition**)&temp);
    Int64 directoryId = ((DirectoryPartition*)partition.Get())->GetDirectoryId();
    if (directoryId != IContactsContractDirectory::DEFAULT) {
        AutoPtr<IUriBuilder> builder;
        uri->BuildUpon((IUriBuilder**)&builder);
        builder->AppendQueryParameter(IContactsContract::DIRECTORY_PARAM_KEY,
                StringUtils::ToString(directoryId));
        AutoPtr<IUri> u;
        builder->Build((IUri**)&u);
        uri = u;
    }
    return uri;
}

Boolean ContactListAdapter::IsSelectedContact(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<ICompositeCursorAdapterPartition> temp;
    GetPartition(partitionIndex, (ICompositeCursorAdapterPartition**)&temp);
    Int64 directoryId = ((DirectoryPartition*)temp.Get())->GetDirectoryId();
    if (GetSelectedContactDirectoryId() != directoryId) {
        return FALSE;
    }
    String lookupKey = GetSelectedContactLookupKey();
    String str;
    cursor->GetString(ContactQuery::CONTACT_LOOKUP_KEY, &str);
    if (!lookupKey.IsNull() && TextUtils::Equals(lookupKey, str)) {
        return TRUE;
    }

    Int64 id;
    return directoryId != IContactsContractDirectory::DEFAULT &&
            directoryId != IContactsContractDirectory::LOCAL_INVISIBLE
                    && (cursor->GetInt64(ContactQuery::CONTACT_ID, &id), GetSelectedContactId() == id);
}

AutoPtr<IView> ContactListAdapter::NewView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> temp = ContactEntryListAdapter::NewView(context, partition, cursor, position, parent);
    AutoPtr<ContactListItemView> view = (ContactListItemView*)IContactListItemView::Probe(temp);
    view->SetUnknownNameText(mUnknownNameText);
    view->SetQuickContactEnabled(IsQuickContactEnabled());
    view->SetAdjustSelectionBoundsEnabled(IsAdjustSelectionBoundsEnabled());
    view->SetActivatedStateSupported(IsSelectionVisible());
    view->SetPhotoPosition(mPhotoPosition);
    return temp;
}

void ContactListAdapter::BindSectionHeaderAndDivider(
    /* [in] */ IContactListItemView* view,
    /* [in] */ Int32 position,
    /* [in] */ ICursor* cursor)
{
    view->SetIsSectionHeaderEnabled(IsSectionHeaderDisplayEnabled());
    if (IsSectionHeaderDisplayEnabled()) {
        AutoPtr<IIndexerListAdapterPlacement> temp;
        GetItemPlacementInSection(position, (IIndexerListAdapterPlacement**)&temp);
        AutoPtr<Placement> placement = (Placement*)temp.Get();
        view->SetSectionHeader(placement->mSectionHeader);
    }
    else {
        view->SetSectionHeader(NULL);
    }
}

void ContactListAdapter::BindPhoto(
    /* [in] */ IContactListItemView* view,
    /* [in] */ Int32 partitionIndex,
    /* [in] */ ICursor* cursor)
{
    if (!IsPhotoSupported(partitionIndex)) {
        view->RemovePhotoView();
        return;
    }

    // Set the photo, if available
    Int64 photoId = 0;
    Boolean isNull;
    if (cursor->IsNull(ContactQuery::CONTACT_PHOTO_ID, &isNull), !isNull) {
        cursor->GetInt64(ContactQuery::CONTACT_PHOTO_ID, &photoId);
    }

    if (photoId != 0) {
        AutoPtr<IImageView> photoView = view->GetPhotoView();
        Boolean result;
        GetCircularPhotos(&result);
        GetPhotoLoader()->LoadThumbnail(photoView, photoId, FALSE, result, NULL);
    }
    else {
        String photoUriString;
        cursor->GetString(ContactQuery::CONTACT_PHOTO_URI, &photoUriString);
        AutoPtr<IUri> photoUri;
        if (!photoUriString.IsNull()) {
            AutoPtr<IUriHelper> helper;
            CUriHelper::AcquireSingleton((IUriHelper**)&helper);
            AutoPtr<IUri> uri;
            helper->Parse(photoUriString, (IUri**)&photoUri);
        }
        AutoPtr<IContactPhotoManagerDefaultImageRequest> request;
        if (photoUri == NULL) {
            GetDefaultImageRequestFromCursor(cursor,
                    ContactQuery::CONTACT_DISPLAY_NAME,
                    ContactQuery::CONTACT_LOOKUP_KEY, (IContactPhotoManagerDefaultImageRequest**)&request);
        }
        AutoPtr<IImageView> photoView = view->GetPhotoView();
        Boolean result;
        GetCircularPhotos(&result);
        GetPhotoLoader()->LoadDirectoryPhoto(photoView, photoUri, FALSE, result, request);
    }
}

void ContactListAdapter::BindNameAndViewId(
    /* [in] */ IContactListItemView* view,
    /* [in] */ ICursor* cursor)
{
    Int32 order;
    GetContactNameDisplayOrder(&order);
    view->ShowDisplayName(
            cursor, ContactQuery::CONTACT_DISPLAY_NAME, order);
    // Note: we don't show phonetic any more (See issue 5265330)

    BindViewId(view, cursor, ContactQuery::CONTACT_ID);
}

void ContactListAdapter::BindPresenceAndStatusMessage(
    /* [in] */ IContactListItemView* view,
    /* [in] */ ICursor* cursor)
{
    view->ShowPresenceAndStatusMessage(cursor, ContactQuery::CONTACT_PRESENCE_STATUS,
            ContactQuery::CONTACT_CONTACT_STATUS);
}

void ContactListAdapter::BindSearchSnippet(
    /* [in] */ IContactListItemView* view,
    /* [in] */ ICursor* cursor)
{
    view->ShowSnippet(cursor, ContactQuery::CONTACT_SNIPPET);
}

Int32 ContactListAdapter::GetSelectedContactPosition()
{
    if (mSelectedContactLookupKey.IsNull() && mSelectedContactId == 0) {
        return -1;
    }

    AutoPtr<ICursor> cursor;
    Int32 partitionIndex = -1;
    Int32 partitionCount;
    GetPartitionCount(&partitionCount);
    for (Int32 i = 0; i < partitionCount; i++) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<DirectoryPartition> partition = (DirectoryPartition*)IDirectoryPartition::Probe(temp);
        if (partition->GetDirectoryId() == mSelectedContactDirectoryId) {
            partitionIndex = i;
            break;
        }
    }
    if (partitionIndex == -1) {
        return -1;
    }

    GetCursor(partitionIndex, (ICursor**)&cursor);
    if (cursor == NULL) {
        return -1;
    }

    Boolean result;
    cursor->MoveToPosition(-1, &result);      // Reset cursor
    Int32 offset = -1;
    while (cursor->MoveToNext(&result), result) {
        if (!mSelectedContactLookupKey.IsNull()) {
            String lookupKey;
            cursor->GetString(ContactQuery::CONTACT_LOOKUP_KEY, &lookupKey);
            if (mSelectedContactLookupKey.Equals(lookupKey)) {
                cursor->GetPosition(&offset);
                break;
            }
        }
        if (mSelectedContactId != 0 && (mSelectedContactDirectoryId == IContactsContractDirectory::DEFAULT
                || mSelectedContactDirectoryId == IContactsContractDirectory::LOCAL_INVISIBLE)) {
            Int64 contactId;
            cursor->GetInt64(ContactQuery::CONTACT_ID, &contactId);
            if (contactId == mSelectedContactId) {
                cursor->GetPosition(&offset);
                break;
            }
        }
    }
    if (offset == -1) {
        return -1;
    }

    Int32 pos;
    GetPositionForPartition(partitionIndex, &pos);
    Int32 position = pos + offset;
    Boolean hasHeader;
    if (HasHeader(partitionIndex, &hasHeader), hasHeader) {
        position++;
    }
    return position;
}

Boolean ContactListAdapter::HasValidSelection()
{
    return GetSelectedContactPosition() != -1;
}

AutoPtr<IUri> ContactListAdapter::GetFirstContactUri()
{
    Int32 partitionCount;
    GetPartitionCount(&partitionCount);
    for (Int32 i = 0; i < partitionCount; i++) {
        AutoPtr<ICompositeCursorAdapterPartition> temp;
        GetPartition(i, (ICompositeCursorAdapterPartition**)&temp);
        AutoPtr<DirectoryPartition> partition = (DirectoryPartition*)IDirectoryPartition::Probe(temp);
        if (partition->IsLoading()) {
            continue;
        }

        AutoPtr<ICursor> cursor;
        GetCursor(i, (ICursor**)&cursor);
        if (cursor == NULL) {
            continue;
        }

        Boolean result;
        if (cursor->MoveToFirst(&result), !result) {
            continue;
        }

        return GetContactUri(i, cursor);
    }

    return NULL;
}

ECode ContactListAdapter::ChangeCursor(
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor)
{
    ContactEntryListAdapter::ChangeCursor(partitionIndex, cursor);

    // Check if a profile exists
    Int32 count;
    if (cursor != NULL && (cursor->GetCount(&count), count > 0)) {
        Boolean result;
        cursor->MoveToFirst(&result);
        Int32 value;
        cursor->GetInt32(ContactQuery::CONTACT_IS_USER_PROFILE, &value);
        SetProfileExists(value == 1);
    }
}

AutoPtr<ArrayOf<String> > ContactListAdapter::GetProjection(
    /* [in] */ Boolean forSearch)
{
    Int32 sortOrder;
    GetContactNameDisplayOrder(&sortOrder);
    if (forSearch) {
        if (sortOrder == ContactsPreferences::DISPLAY_ORDER_PRIMARY) {
            return ContactQuery::FILTER_PROJECTION_PRIMARY;
        }
        else {
            return ContactQuery::FILTER_PROJECTION_ALTERNATIVE;
        }
    }
    else {
        if (sortOrder == ContactsPreferences::DISPLAY_ORDER_PRIMARY) {
            return ContactQuery::CONTACT_PROJECTION_PRIMARY;
        }
        else {
            return ContactQuery::CONTACT_PROJECTION_ALTERNATIVE;
        }
    }
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos