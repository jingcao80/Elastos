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

#include "elastos/droid/dialer/list/PhoneFavoritesTileAdapter.h"
#include "elastos/droid/contacts/common/ContactTileLoaderFactory.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/View.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Contacts::Common::ContactTileLoaderFactory;
using Elastos::Droid::Contacts::Common::List::IContactTileView;
using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Content::IContentProviderOperationHelper;
using Elastos::Droid::Content::CContentProviderOperationHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentProviderOperationBuilder;
using Elastos::Droid::Content::IContentProviderResult;
using Elastos::Droid::Dialer::List::EIID_IOnDragDropListener;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IContactsContractContactOptionsColumns;
using Elastos::Droid::Provider::IContactsContractPinnedPositions;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::View;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::IList;
using Elastos::Droid::Utility::CInt64SparseArray;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IPriorityQueue;
using Elastos::Utility::CPriorityQueue;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace List {

//=================================================================
// PhoneFavoritesTileAdapter::ContactEntryComparator
//=================================================================
CAR_INTERFACE_IMPL(PhoneFavoritesTileAdapter::ContactEntryComparator, Object, IComparator);

ECode PhoneFavoritesTileAdapter::ContactEntryComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ContactEntry> lhsEntry = (ContactEntry*)(IObject*)lhs;
    AutoPtr<ContactEntry> rhsEntry = (ContactEntry*)(IObject*)rhs;
    *result = (lhsEntry->mPinned == rhsEntry->mPinned) && (lhsEntry->mName.Equals(rhsEntry->mName));
    // ComparisonChain.start()
    //                 .compare(lhs.pinned, rhs.pinned)
    //                 .compare(lhs.name, rhs.name)
    //                 .result();
    return NOERROR;
}


//=================================================================
// PhoneFavoritesTileAdapter
//=================================================================
CAR_INTERFACE_IMPL(PhoneFavoritesTileAdapter, BaseAdapter, IOnDragDropListener);

const Int32 PhoneFavoritesTileAdapter::PIN_LIMIT;
const String PhoneFavoritesTileAdapter::TAG("PhoneFavoritesTileAdapter");
const Boolean PhoneFavoritesTileAdapter::DEBUG;
const Int32 PhoneFavoritesTileAdapter::TILES_SOFT_LIMIT;

PhoneFavoritesTileAdapter::PhoneFavoritesTileAdapter(
    /* [in] */ IContext* context,
    /* [in] */ IContactTileViewListener* listener,
    /* [in] */ IOnDataSetChangedForAnimationListener* dataSetChangedListener)
    : mNumFrequents(0)
    , mNumStarred(0)
    , mIdIndex(0)
    , mLookupIndex(0)
    , mPhotoUriIndex(0)
    , mNameIndex(0)
    , mPresenceIndex(0)
    , mStatusIndex(0)
    , mDraggedEntryIndex(-1)
    , mDropEntryIndex(-1)
    , mDragEnteredEntryIndex(-1)
    , mAwaitingRemove(FALSE)
    , mDelayCursorUpdates(FALSE)
    , mPhoneNumberIndex(0)
    , mPhoneNumberTypeIndex(0)
    , mPhoneNumberLabelIndex(0)
    , mIsDefaultNumberIndex(0)
    , mStarredIndex(0)
    , mPinnedIndex(0)
    , mContactIdIndex(0)
    , mInDragging(FALSE)
{
    mContactEntryComparator = new ContactEntryComparator(this);

    mDataSetChangedListener = dataSetChangedListener;
    mListener = listener;
    mContext = context;
    context->GetResources((IResources**)&mResources);
    CArrayList::New((IArrayList**)&mContactEntries);

    BindColumnIndices();
}

void PhoneFavoritesTileAdapter::SetPhotoLoader(
    /* [in] */ IContactPhotoManager* photoLoader)
{
    mPhotoManager = photoLoader;
}

void PhoneFavoritesTileAdapter::SetInDragging(
    /* [in] */ Boolean inDragging)
{
    mDelayCursorUpdates = inDragging;
    mInDragging = inDragging;
}

Boolean PhoneFavoritesTileAdapter::GetInDragging()
{
    return mInDragging;
}

void PhoneFavoritesTileAdapter::BindColumnIndices()
{
    mIdIndex = ContactTileLoaderFactory::CONTACT_ID;
    mLookupIndex = ContactTileLoaderFactory::LOOKUP_KEY;
    mPhotoUriIndex = ContactTileLoaderFactory::PHOTO_URI;
    mNameIndex = ContactTileLoaderFactory::DISPLAY_NAME;
    mStarredIndex = ContactTileLoaderFactory::STARRED;
    mPresenceIndex = ContactTileLoaderFactory::CONTACT_PRESENCE;
    mStatusIndex = ContactTileLoaderFactory::CONTACT_STATUS;

    mPhoneNumberIndex = ContactTileLoaderFactory::PHONE_NUMBER;
    mPhoneNumberTypeIndex = ContactTileLoaderFactory::PHONE_NUMBER_TYPE;
    mPhoneNumberLabelIndex = ContactTileLoaderFactory::PHONE_NUMBER_LABEL;
    mIsDefaultNumberIndex = ContactTileLoaderFactory::IS_DEFAULT_NUMBER;
    mPinnedIndex = ContactTileLoaderFactory::PINNED;
    mContactIdIndex = ContactTileLoaderFactory::CONTACT_ID_FOR_DATA;
}

void PhoneFavoritesTileAdapter::SaveNumFrequentsFromCursor(
    /* [in] */ ICursor* cursor)
{
    Int32 count;
    cursor->GetCount(&count);
    mNumFrequents = count - mNumStarred;
}

void PhoneFavoritesTileAdapter::SetContactCursor(
    /* [in] */ ICursor* cursor)
{
    Boolean closed;
    if (!mDelayCursorUpdates && cursor != NULL && (cursor->IsClosed(&closed), !closed)) {
        mNumStarred = GetNumStarredContacts(cursor);
        if (mAwaitingRemove) {
            mDataSetChangedListener->CacheOffsetsForDatasetChange();
        }

        SaveNumFrequentsFromCursor(cursor);
        SaveCursorToCache(cursor);
        // cause a refresh of any views that rely on this data
        NotifyDataSetChanged();
        // about to start redraw
        mDataSetChangedListener->OnDataSetChangedForAnimation(NULL);
    }
}

void PhoneFavoritesTileAdapter::SaveCursorToCache(
    /* [in] */ ICursor* cursor)
{
    mContactEntries->Clear();

    Boolean succeeded;
    cursor->MoveToPosition(-1, &succeeded);

    Int32 count;
    cursor->GetCount(&count);
    AutoPtr<IInt64SparseArray> duplicates;
    CInt64SparseArray::New(count, (IInt64SparseArray**)&duplicates);

    // Track the length of {@link #mContactEntries} and compare to {@link #TILES_SOFT_LIMIT}.
    Int32 counter = 0;

    while (cursor->MoveToNext(&succeeded), succeeded) {
        Int32 starred;
        cursor->GetInt32(mStarredIndex, &starred);
        Int64 id;

        // We display a maximum of TILES_SOFT_LIMIT contacts, or the total number of starred
        // whichever is greater.
        if (starred < 1 && counter >= TILES_SOFT_LIMIT) {
            break;
        }
        else {
            cursor->GetInt64(mContactIdIndex, &id);
        }

        AutoPtr<IInterface> item;
        duplicates->Get(id, (IInterface**)&item);
        AutoPtr<ContactEntry> existing;
        if (item != NULL) {
            existing = (ContactEntry*)(IObject*)item.Get();
            // Check if the existing number is a default number. If not, clear the phone number
            // and label fields so that the disambiguation dialog will show up.
            if (!existing->mIsDefaultNumber) {
                existing->mPhoneLabel = NULL;
                existing->mPhoneNumber = NULL;
            }
            continue;
        }

        String photoUri;
        cursor->GetString(mPhotoUriIndex, &photoUri);
        String lookupKey;
        cursor->GetString(mLookupIndex, &lookupKey);
        Int32 pinned;
        cursor->GetInt32(mPinnedIndex, &pinned);
        String name;
        cursor->GetString(mNameIndex, &name);
        Int32 index;
        cursor->GetInt32(mStarredIndex, &starred);
        cursor->GetInt32(mIsDefaultNumberIndex, &index);
        Boolean isStarred = starred > 0;
        Boolean isDefaultNumber = index > 0;

        AutoPtr<ContactEntry> contact = new ContactEntry();

        contact->mId = id;
        if (!TextUtils::IsEmpty(name)) {
            contact->mName = name;
        }
        else {
            mResources->GetString(Elastos::Droid::Dialer::R::string::missing_name, &contact->mName);
        }

        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        contact->mPhotoUri = NULL;
        if (!photoUri.IsNull()) {
            helper->Parse(photoUri, (IUri**)&contact->mPhotoUri);
        }
        contact->mLookupKey = lookupKey;

        AutoPtr<IContactsContractContacts> contacts;
        CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
        AutoPtr<IUri> contentLookupUri;
        contacts->GetCONTENT_LOOKUP_URI((IUri**)&contentLookupUri);
        AutoPtr<IUri> uri;
        helper->WithAppendedPath(contentLookupUri, lookupKey, (IUri**)&uri);
        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        contact->mLookupUri = NULL;
        contentUris->WithAppendedId(uri, id, (IUri**)&contact->mLookupUri);
        contact->mIsFavorite = isStarred;
        contact->mIsDefaultNumber = isDefaultNumber;

        // Set phone number and label
        Int32 phoneNumberType;
        cursor->GetInt32(mPhoneNumberTypeIndex, &phoneNumberType);
        String phoneNumberCustomLabel;
        cursor->GetString(mPhoneNumberLabelIndex, &phoneNumberCustomLabel);

        AutoPtr<IContactsContractCommonDataKindsPhone> phone;
        CContactsContractCommonDataKindsPhone::AcquireSingleton((IContactsContractCommonDataKindsPhone**)&phone);
        AutoPtr<ICharSequence> cs;
        phone->GetTypeLabel(mResources, phoneNumberType,
                CoreUtils::Convert(phoneNumberCustomLabel), (ICharSequence**)&cs);
        cs->ToString(&contact->mPhoneLabel);
        cursor->GetString(mPhoneNumberIndex, &contact->mPhoneNumber);

        contact->mPinned = pinned;
        mContactEntries->Add((IObject*)contact);

        duplicates->Put(id, (IObject*)contact);

        counter++;
    }

    mAwaitingRemove = FALSE;

    ArrangeContactsByPinnedPosition(mContactEntries);

    NotifyDataSetChanged();
}

Int32 PhoneFavoritesTileAdapter::GetNumStarredContacts(
    /* [in] */ ICursor* cursor)
{
    Boolean succeeded;
    cursor->MoveToPosition(-1, &succeeded);
    while (cursor->MoveToNext(&succeeded), succeeded) {
        Int32 index;
        cursor->GetInt32(mStarredIndex, &index);
        if (index == 0) {
            Int32 position;
            cursor->GetPosition(&position);
            return position;
        }
    }

    // There are not NON Starred contacts in cursor
    // Set divider positon to end
    Int32 count;
    cursor->GetCount(&count);
    return count;
}

Int32 PhoneFavoritesTileAdapter::GetNumFrequents()
{
    return mNumFrequents;
}

ECode PhoneFavoritesTileAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    if (mContactEntries == NULL) {
        *count = 0;
        return NOERROR;
    }

    return mContactEntries->GetSize(count);
}

ECode PhoneFavoritesTileAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    AutoPtr<IInterface> obj;
    return mContactEntries->Get(position, item);
}

ECode PhoneFavoritesTileAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id)
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    *id = ((ContactEntry*)(IObject*)item.Get())->mId;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::HasStableIds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 count;
    GetCount(&count);
    *result = count > 0;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::NotifyDataSetChanged()
{
    if (DEBUG) {
        Logger::V(TAG, "notifyDataSetChanged");
    }
    return BaseAdapter::NotifyDataSetChanged();
}

ECode PhoneFavoritesTileAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    if (DEBUG) {
        Logger::V(TAG, "get view for %d", position);
    }

    Int32 itemViewType;
    GetItemViewType(position, &itemViewType);

    AutoPtr<IPhoneFavoriteTileView> tileView;

    if (IPhoneFavoriteTileView::Probe(convertView)) {
        tileView  = IPhoneFavoriteTileView::Probe(convertView);
    }

    if (tileView == NULL) {
        AutoPtr<IView> temp;
        Elastos::Droid::View::View::Inflate(mContext,
                Elastos::Droid::Dialer::R::layout::phone_favorite_tile_view, NULL, (IView**)&temp);
        tileView = IPhoneFavoriteTileView::Probe(temp);
    }
    AutoPtr<IContactTileView> tv = IContactTileView::Probe(tileView);
    tv->SetPhotoManager(mPhotoManager);
    tv->SetListener(mListener);

    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    tv->LoadFromContact(item);
    *view = IView::Probe(tileView);
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = ViewTypes::COUNT;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = ViewTypes::TILE;
    return NOERROR;
}

void PhoneFavoritesTileAdapter::PopContactEntry(
    /* [in] */ Int32 index)
{
    if (IsIndexInBound(index)) {
        AutoPtr<IInterface> item;
        mContactEntries->Get(index, (IInterface**)&item);
        mDraggedEntry = (ContactEntry*)(IObject*)item.Get();
        mDraggedEntryIndex = index;
        mDragEnteredEntryIndex = index;
        MarkDropArea(mDragEnteredEntryIndex);
    }
}

Boolean PhoneFavoritesTileAdapter::IsIndexInBound(
    /* [in] */ Int32 itemIndex)
{
    Int32 size;
    mContactEntries->GetSize(&size);
    return itemIndex >= 0 && itemIndex < size;
}

void PhoneFavoritesTileAdapter::MarkDropArea(
    /* [in] */ Int32 itemIndex)
{
    if (IsIndexInBound(mDragEnteredEntryIndex) && IsIndexInBound(itemIndex)) {
        mDataSetChangedListener->CacheOffsetsForDatasetChange();
        // Remove the old placeholder item and place the new placeholder item.
        // Int32 oldIndex = mDragEnteredEntryIndex;
        mContactEntries->Remove(mDragEnteredEntryIndex);
        mDragEnteredEntryIndex = itemIndex;
        mContactEntries->Add(mDragEnteredEntryIndex, (IObject*)ContactEntry::BLANK_ENTRY);
        ContactEntry::BLANK_ENTRY->mId = mDraggedEntry->mId;
        mDataSetChangedListener->OnDataSetChangedForAnimation(NULL);
        NotifyDataSetChanged();
    }
}

void PhoneFavoritesTileAdapter::HandleDrop()
{
    Boolean changed = FALSE;
    if (mDraggedEntry != NULL) {
        if (IsIndexInBound(mDragEnteredEntryIndex) &&
                mDragEnteredEntryIndex != mDraggedEntryIndex) {
            // Don't add the ContactEntry here (to prevent a double animation from occuring).
            // When we receive a new cursor the list of contact entries will automatically be
            // populated with the dragged ContactEntry at the correct spot.
            mDropEntryIndex = mDragEnteredEntryIndex;
            mContactEntries->Set(mDropEntryIndex, (IObject*)mDraggedEntry);
            mDataSetChangedListener->CacheOffsetsForDatasetChange();
            changed = TRUE;
        }
        else if (IsIndexInBound(mDraggedEntryIndex)) {
            // If {@link #mDragEnteredEntryIndex} is invalid,
            // falls back to the original position of the contact.
            mContactEntries->Remove(mDragEnteredEntryIndex);
            mContactEntries->Add(mDraggedEntryIndex, (IObject*)mDraggedEntry);
            mDropEntryIndex = mDraggedEntryIndex;
            NotifyDataSetChanged();
        }

        if (changed && mDropEntryIndex < PIN_LIMIT) {
            AutoPtr<IArrayList> operations = GetReflowedPinningOperations(
                     mContactEntries, mDraggedEntryIndex, mDropEntryIndex);
            Boolean isEmpty;
            if (operations->IsEmpty(&isEmpty), !isEmpty) {
                // update the database here with the new pinned positions
                // try {
                AutoPtr<IContentResolver> resolver;
                mContext->GetContentResolver((IContentResolver**)&resolver);
                AutoPtr<ArrayOf<IContentProviderResult*> > providerResults;
                resolver->ApplyBatch(IContactsContract::AUTHORITY, operations,
                        (ArrayOf<IContentProviderResult*>**)&providerResults);
                // } catch (RemoteException | OperationApplicationException e) {
                //     Log.e(TAG, "Exception thrown when pinning contacts", e);
                // }
            }
        }
        mDraggedEntry = NULL;
    }
}

void PhoneFavoritesTileAdapter::DropToUnsupportedView()
{
    if (IsIndexInBound(mDragEnteredEntryIndex)) {
        mContactEntries->Remove(mDragEnteredEntryIndex);
        mContactEntries->Add(mDraggedEntryIndex, (IObject*)mDraggedEntry);
        NotifyDataSetChanged();
    }
}

void PhoneFavoritesTileAdapter::CleanTempVariables()
{
    mDraggedEntryIndex = -1;
    mDropEntryIndex = -1;
    mDragEnteredEntryIndex = -1;
    mDraggedEntry = NULL;
}

void PhoneFavoritesTileAdapter::UnstarAndUnpinContact(
    /* [in] */ IUri* contactUri)
{
    AutoPtr<IContentValues> values;
    CContentValues::New(2, (IContentValues**)&values);
    values->Put(IContactsContractContactOptionsColumns::STARRED, FALSE);
    values->Put(IContactsContractContactOptionsColumns::PINNED, IContactsContractPinnedPositions::DEMOTED);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 result;
    resolver->Update(contactUri, values, String(NULL), NULL, &result);
}

void PhoneFavoritesTileAdapter::ArrangeContactsByPinnedPosition(
    /* [in] */ IArrayList* toArrange)
{
    AutoPtr<IPriorityQueue> pinnedQueue;
    CPriorityQueue::New(PIN_LIMIT,
            mContactEntryComparator, (IPriorityQueue**)&pinnedQueue);

    AutoPtr<IList> unpinnedContacts;
    CLinkedList::New((IList**)&unpinnedContacts);

    Int32 length;
    toArrange->GetSize(&length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> item;
        toArrange->Get(i, (IInterface**)&item);
        AutoPtr<ContactEntry> contact = (ContactEntry*)(IObject*)item.Get();
        // Decide whether the contact is hidden(demoted), pinned, or unpinned
        if (contact->mPinned > PIN_LIMIT || contact->mPinned == IContactsContractPinnedPositions::UNPINNED) {
            unpinnedContacts->Add(item);
        }
        else if (contact->mPinned > IContactsContractPinnedPositions::DEMOTED) {
            // Demoted or contacts with negative pinned positions are ignored.
            // Pinned contacts go into a priority queue where they are ranked by pinned
            // position. This is required because the contacts provider does not return
            // contacts ordered by pinned position.
            pinnedQueue->Add(item);
        }
    }

    Int32 pinnedSize, unpinnedSize;
    pinnedQueue->GetSize(&pinnedSize);
    unpinnedContacts->GetSize(&unpinnedSize);
    Int32 maxToPin = Elastos::Core::Math::Min(PIN_LIMIT, pinnedSize + unpinnedSize);

    toArrange->Clear();
    for (Int32 i = 1; i < maxToPin + 1; i++) {
        AutoPtr<IInterface> e;
        Boolean isEmpty;
        if ((pinnedQueue->IsEmpty(&isEmpty), !isEmpty) &&
                (pinnedQueue->Peek((IInterface**)&e), ((ContactEntry*)(IObject*)e.Get())->mPinned <= i)) {
            AutoPtr<IInterface> toPin;
            pinnedQueue->Poll((IInterface**)&toPin);
            ((ContactEntry*)(IObject*)toPin.Get())->mPinned = i;
            toArrange->Add(toPin);
        }
        else if (unpinnedContacts->IsEmpty(&isEmpty), !isEmpty) {
            AutoPtr<IInterface> obj;
            unpinnedContacts->Remove(0, (IInterface**)&obj);
            toArrange->Add(obj);
        }
    }

    // If there are still contacts in pinnedContacts at this point, it means that the pinned
    // positions of these pinned contacts exceed the actual number of contacts in the list.
    // For example, the user had 10 frequents, starred and pinned one of them at the last spot,
    // and then cleared frequents. Contacts in this situation should become unpinned.
    Boolean isEmpty;
    while (pinnedQueue->IsEmpty(&isEmpty), !isEmpty) {
        AutoPtr<IInterface> entry;
        pinnedQueue->Poll((IInterface**)&entry);
        ((ContactEntry*)(IObject*)entry.Get())->mPinned  = IContactsContractPinnedPositions::UNPINNED;
        toArrange->Add(entry);
    }

    // Any remaining unpinned contacts that weren't in the gaps between the pinned contacts
    // now just get appended to the end of the list.
    toArrange->AddAll(ICollection::Probe(unpinnedContacts));
}

AutoPtr<IArrayList> PhoneFavoritesTileAdapter::GetReflowedPinningOperations(
    /* [in] */ IArrayList* list,
    /* [in] */ Int32 oldPos,
    /* [in] */ Int32 newPinPos)
{
    AutoPtr<IArrayList> positions;
    CArrayList::New((IArrayList**)&positions);
    Int32 lowerBound = Elastos::Core::Math::Min(oldPos, newPinPos);
    Int32 upperBound = Elastos::Core::Math::Max(oldPos, newPinPos);
    for (Int32 i = lowerBound; i <= upperBound; i++) {
        AutoPtr<IInterface> entry;
        list->Get(i, (IInterface**)&entry);

        // Pinned positions in the database start from 1 instead of being zero-indexed like
        // arrays, so offset by 1.
        Int32 databasePinnedPosition = i + 1;
        AutoPtr<ContactEntry> entryObj = (ContactEntry*)(IObject*)entry.Get();
        if (entryObj->mPinned == databasePinnedPosition) continue;

        AutoPtr<IContactsContractContacts> contacts;
        CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
        AutoPtr<IUri> contentUri;
        contacts->GetCONTENT_URI((IUri**)&contentUri);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->WithAppendedPath(contentUri, StringUtils::ToString(entryObj->mId), (IUri**)&uri);
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        values->Put(IContactsContractContactOptionsColumns::PINNED, databasePinnedPosition);

        AutoPtr<IContentProviderOperationHelper> providerHelper;
        CContentProviderOperationHelper::AcquireSingleton((IContentProviderOperationHelper**)&providerHelper);
        AutoPtr<IContentProviderOperationBuilder> builder;
        providerHelper->NewUpdate(uri, (IContentProviderOperationBuilder**)&builder);
        builder->WithValues(values);
        AutoPtr<IContentProviderOperation> operation;
        builder->Build((IContentProviderOperation**)&operation);
        positions->Add(operation);
    }
    return positions;
}

ECode PhoneFavoritesTileAdapter::OnDragStarted(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPhoneFavoriteSquareTileView* view)
{
    SetInDragging(TRUE);
    AutoPtr<IInterface> entry;
    view->GetContactEntry((IInterface**)&entry);
    Int32 itemIndex;
    mContactEntries->IndexOf(entry, &itemIndex);
    PopContactEntry(itemIndex);

    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::OnDragHovered(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPhoneFavoriteSquareTileView* view)
{
    if (view == NULL) {
        // The user is hovering over a view that is not a contact tile, no need to do
        // anything here.
        return NOERROR;
    }
    AutoPtr<IInterface> entry;
    view->GetContactEntry((IInterface**)&entry);
    Int32 itemIndex;
    mContactEntries->IndexOf(entry, &itemIndex);
    if (mInDragging &&
            mDragEnteredEntryIndex != itemIndex &&
            IsIndexInBound(itemIndex) &&
            itemIndex < PIN_LIMIT &&
            itemIndex >= 0) {
        MarkDropArea(itemIndex);
    }

    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::OnDragFinished(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    SetInDragging(FALSE);
    // A contact has been dragged to the RemoveView in order to be unstarred,  so simply wait
    // for the new contact cursor which will cause the UI to be refreshed without the unstarred
    // contact.
    if (!mAwaitingRemove) {
        HandleDrop();
    }
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::OnDroppedOnRemove()
{
    if (mDraggedEntry != NULL) {
        UnstarAndUnpinContact(mDraggedEntry->mLookupUri);
        mAwaitingRemove = TRUE;
    }
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
