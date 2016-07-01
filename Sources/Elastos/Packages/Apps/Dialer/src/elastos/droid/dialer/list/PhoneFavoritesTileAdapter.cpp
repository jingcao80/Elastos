
#include "elastos/droid/dialer/list/PhoneFavoritesTileAdapter.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/View.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.Provider.h"
#include "R.h"

using Elastos::Droid::Content::IContentUris;
using Elastos::Droid::Content::CContentUris;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::IContentProviderOperation;
using Elastos::Droid::Content::IContentProviderOperationHelper;
using Elastos::Droid::Content::CContentProviderOperationHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::IContacts;
using Elastos::Droid::Provider::IContactsContractCommonDataKindsPhone;
using Elastos::Droid::Provider::CContactsContractCommonDataKindsPhone;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::View;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IList;
using Elastos::Utility::CLinkedList;
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

PhoneFavoritesTileAdapter::ContactEntryComparator::ContactEntryComparator(
    /* [in] */ PhoneFavoritesTileAdapter* host)
    : mHost(host)
{}

// TODO:
// ECode PhoneFavoritesTileAdapter::ContactEntryComparator::Compare(
//     /* [in] */ IContactEntry* lhs,
//     /* [in] */ IContactEntry* rhs,
//     /* [out] */ Int32* result)
// {
//     VALIDATE_NOT_NULL(result);

//     assert(0 && "TODO");
//     // ComparisonChain.start()
//     //                 .compare(lhs.pinned, rhs.pinned)
//     //                 .compare(lhs.name, rhs.name)
//     //                 .result();
//     return NOERROR;
// }

//=================================================================
// PhoneFavoritesTileAdapter
//=================================================================
CAR_INTERFACE_IMPL_2(PhoneFavoritesTileAdapter, BaseAdapter, IPhoneFavoritesTileAdapter, IOnDragDropListener);

const String PhoneFavoritesTileAdapter::TAG("PhoneFavoritesTileAdapter");
const Boolean PhoneFavoritesTileAdapter::DEBUG = FALSE;
const Int32 PhoneFavoritesTileAdapter::TILES_SOFT_LIMIT = 20;

PhoneFavoritesTileAdapter::PhoneFavoritesTileAdapter()
    : mDraggedEntryIndex(-1)
    , mDropEntryIndex(-1)
    , mDragEnteredEntryIndex(-1)
    , mAwaitingRemove(FALSE)
    , mDelayCursorUpdates(FALSE)
    , mInDragging(FALSE)
{
    mContactEntryComparator = new ContactEntryComparator(this);
}

// TODO:
// ECode PhoneFavoritesTileAdapter::constructor(
//     /* [in] */ IContext* context,
//     /* [in] */ IContactTileViewListener* listener,
//     /* [in] */ IOnDataSetChangedForAnimationListener* dataSetChangedListener)
// {
//     mDataSetChangedListener = dataSetChangedListener;
//     mListener = listener;
//     mContext = context;
//     context->GetResources((IResources**)&mResources);
//     mNumFrequents = 0;
//     CArrayList::New((IArrayList**)&mContactEntries);

//     BindColumnIndices();

//     return NOERROR;
// }

// TODO:
// ECode PhoneFavoritesTileAdapter::SetPhotoLoader(
//     /* [in] */ IContactPhotoManager* photoLoader)
// {
//     mPhotoManager = photoLoader;
//     return NOERROR;
// }

ECode PhoneFavoritesTileAdapter::SetInDragging(
    /* [in] */ Boolean inDragging)
{
    mDelayCursorUpdates = inDragging;
    mInDragging = inDragging;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::GetInDragging(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mInDragging;
    return NOERROR;
}

void PhoneFavoritesTileAdapter::BindColumnIndices()
{
    assert(0 && "TODO");
    // mIdIndex = IContactTileLoaderFactory::CONTACT_ID;
    // mLookupIndex = IContactTileLoaderFactory::LOOKUP_KEY;
    // mPhotoUriIndex = IContactTileLoaderFactory::PHOTO_URI;
    // mNameIndex = IContactTileLoaderFactory::DISPLAY_NAME;
    // mStarredIndex = IContactTileLoaderFactory::STARRED;
    // mPresenceIndex = IContactTileLoaderFactory::CONTACT_PRESENCE;
    // mStatusIndex = IContactTileLoaderFactory::CONTACT_STATUS;

    // mPhoneNumberIndex = IContactTileLoaderFactory::PHONE_NUMBER;
    // mPhoneNumberTypeIndex = IContactTileLoaderFactory::PHONE_NUMBER_TYPE;
    // mPhoneNumberLabelIndex = IContactTileLoaderFactory::PHONE_NUMBER_LABEL;
    // mIsDefaultNumberIndex = IContactTileLoaderFactory::IS_DEFAULT_NUMBER;
    // mPinnedIndex = IContactTileLoaderFactory::PINNED;
    // mContactIdIndex = IContactTileLoaderFactory::CONTACT_ID_FOR_DATA;
}

void PhoneFavoritesTileAdapter::SaveNumFrequentsFromCursor(
    /* [in] */ ICursor* cursor)
{
    Int32 count;
    cursor->GetCount(&count);
    mNumFrequents = count - mNumStarred;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::SetContactCursor(
    /* [in] */ ICursor* cursor)
{
    Boolean closed;
    if (!mDelayCursorUpdates && cursor != NULL && cursor->IsClosed(&closed), !closed) {
        mNumStarred = GetNumStarredContacts(cursor);
        if (mAwaitingRemove) {
            mDataSetChangedListener->cacheOffsetsForDatasetChange();
        }

        SaveNumFrequentsFromCursor(cursor);
        SaveCursorToCache(cursor);
        // cause a refresh of any views that rely on this data
        NotifyDataSetChanged();
        // about to start redraw
        mDataSetChangedListener->OnDataSetChangedForAnimation();
    }
    return NOERROR;
}

void PhoneFavoritesTileAdapter::SaveCursorToCache(
    /* [in] */ ICursor* cursor)
{
    mContactEntries->Clear();

    cursor->MoveToPosition(-1);

    Int32 count;
    cursor->GetCount(&count);
    assert(0 && "TODO");
    // AutoPtr<ILongSparseArray> duplicates;
    // CLongSparseArray::New(count, (ILongSparseArray**)&duplicates);

    // Track the length of {@link #mContactEntries} and compare to {@link #TILES_SOFT_LIMIT}.
    Int32 counter = 0;

    Boolean succeeded;
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
        assert(0 && "TODO");
        // duplicates->Get(id, (IInterface**)&item);
        // CContactEntry* existing = (ContactEntry*)item;
        if (existing != NULL) {
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
        Int32 starred, index;
        cursor->GetInt32(mStarredIndex, &starred);
        cursor->GetInt32(mIsDefaultNumberIndex, &index);
        Boolean isStarred = starred > 0;
        Boolean isDefaultNumber = index > 0;

        AutoPtr<IContactEntry> obj;
        CContactEntry::New((IContactEntry**)&obj);
        CContactEntry* contact = (CContactEntry*)obj;

        contact->mId = id;
        if (!TextUtils::IsEmpty(name)) {
            contact->mName = name;
        }
        else {
            mResources->GetString(R::string::missing_name, &contact->mName);
        }

        contact->mPhotoUri = (photoUri != NULL ? Uri::Parse(photoUri) : NULL);
        contact->mLookupKey = lookupKey;
        AutoPtr<IContentUris> contentUris;
        CContentUris::AcquireSingleton((IContentUris**)&contentUris);
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        AutoPtr<IUri> uri;
        helper->WithAppendedPath(IContacts::CONTENT_LOOKUP_URI, lookupKey, (IUri**)&uri);
        contentUris->WithAppendedId(
                uri, id, &contact->mLookupUri);
        contact->mIsFavorite = isStarred;
        contact->mIsDefaultNumber = isDefaultNumber;

        // Set phone number and label
        Int32 phoneNumberType;
        cursor->GetInt32(mPhoneNumberTypeIndex, &phoneNumberType);
        String phoneNumberCustomLabel;
        cursor->GetString(mPhoneNumberLabelIndex, &phoneNumberCustomLabel);

        AutoPtr<IContactsContractCommonDataKindsPhone> phone;
        CContactsContractCommonDataKindsPhone::AcquireSingleton(
                (IContactsContractCommonDataKindsPhone**)&phone);
        AutoPtr<ICharSequence> cs;
        phone->GetTypeLabel(mResources, phoneNumberType,
                CoreUtils::Convert(phoneNumberCustomLabel), (ICharSequence**)&cs);
        cs->ToString(&contact->mPhoneLabel);
        cursor->GetInt32(mPhoneNumberIndex, &contact->mPhoneNumber);

        contact->mPinned = pinned;
        mContactEntries->Add(contact);

        assert(0 && "TODO");
        // duplicates->Put(id, contact);

        counter++;
    }

    mAwaitingRemove = FALSE;

    ArrangeContactsByPinnedPosition(mContactEntries);

    NotifyDataSetChanged();
}

Int32 PhoneFavoritesTileAdapter::GetNumStarredContacts(
    /* [in] */ ICursor* cursor)
{
    cursor->MoveToPosition(-1);
    Boolean succeeded;
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

ECode PhoneFavoritesTileAdapter::GetNumFrequents(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumFrequents;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(result);
    if (mContactEntries == NULL) {
        *count = 0;
        return NOERROR;
    }

    mContactEntries->GetSize(count);
    return NOERROR;
}

// TODO:
// ECode PhoneFavoritesTileAdapter::GetItem(
//     /* [in] */ Int32 position,
//     /* [out] */ IContactEntry** item)
// {
//     VALIDATE_NOT_NULL(item);
//     AutoPtr<IInterface> obj;
//     mContactEntries->Get(position, &obj);
//     *item = IContactEntry::Probe(obj);
//     REFCOUNT_ADD(*item);
//     return NOERROR;
// }

ECode PhoneFavoritesTileAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    assert(0 && "TODO");
    // AutoPtr<IContactEntry> item;
    // GetItem(position, (IContactEntry**)&item);
    // *id = ((CContactEntry*)item)->mId;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::HasStableIds(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
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
    VALIDATE_NOT_NULL(view);
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
        View::Inflate(mContext,
                R::layout::phone_favorite_tile_view, NULL, (IView**)&temp);
        tileView = IPhoneFavoriteTileView::Probe(temp);
    }
    assert(0 && "TODO");
    // tileView->SetPhotoManager(mPhotoManager);
    // tileView->SetListener(mListener);

    // AutoPtr<IContactEntry> item;
    // GetItem(position, (IContactEntry**)&item);
    // tileView->LoadFromContact(item);
    *view = tileView;
    REFCOUNT_ADD(*view);

    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = ViewTypes::COUNT;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = ViewTypes::TILE;
    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::PopContactEntry(
    /* [in] */ Int32 index)
{
    if (IsIndexInBound(index)) {
        AutoPtr<IInterface> item;
        mContactEntries->Get(index, (IInterface**)&item);
        assert(0 && "TODO");
        // mDraggedEntry = IContactEntry::Probe(item);
        mDraggedEntryIndex = index;
        mDragEnteredEntryIndex = index;
        MarkDropArea(mDragEnteredEntryIndex);
    }
    return NOERROR;
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
        Int32 oldIndex = mDragEnteredEntryIndex;
        mContactEntries->Remove(mDragEnteredEntryIndex);
        mDragEnteredEntryIndex = itemIndex;
        assert(0 && "TODO");
        // mContactEntries->Add(mDragEnteredEntryIndex, ContactEntry::BLANK_ENTRY);
        // ((ContactEntry*)ContactEntry::BLANK_ENTRY)->mId = ((ContactEntry*)mDraggedEntry)->mId;
        mDataSetChangedListener->OnDataSetChangedForAnimation();
        NotifyDataSetChanged();
    }
}

ECode PhoneFavoritesTileAdapter::HandleDrop()
{
    Boolean changed = FALSE;
    assert(0 && "TODO");
    // if (mDraggedEntry != NULL) {
    //     if (IsIndexInBound(mDragEnteredEntryIndex) &&
    //             mDragEnteredEntryIndex != mDraggedEntryIndex) {
    //         // Don't add the ContactEntry here (to prevent a double animation from occuring).
    //         // When we receive a new cursor the list of contact entries will automatically be
    //         // populated with the dragged ContactEntry at the correct spot.
    //         mDropEntryIndex = mDragEnteredEntryIndex;
    //         mContactEntries->Set(mDropEntryIndex, mDraggedEntry);
    //         mDataSetChangedListener->CacheOffsetsForDatasetChange();
    //         changed = true;
    //     }
    //     else if (IsIndexInBound(mDraggedEntryIndex)) {
    //         // If {@link #mDragEnteredEntryIndex} is invalid,
    //         // falls back to the original position of the contact.
    //         mContactEntries->Remove(mDragEnteredEntryIndex);
    //         mContactEntries->Add(mDraggedEntryIndex, mDraggedEntry);
    //         mDropEntryIndex = mDraggedEntryIndex;
    //         NotifyDataSetChanged();
    //     }

    //     if (changed && mDropEntryIndex < PIN_LIMIT) {
    //         AutoPtr<IArrayList> operations = GetReflowedPinningOperations(
    //                  mContactEntries, mDraggedEntryIndex, mDropEntryIndex);
    //         Boolean isEmpty;
    //         if (operations->IsEmpty(&isEmpty), !isEmpty) {
    //             // update the database here with the new pinned positions
    //             // try {
    //             AutoPtr<IContentResolver> resolver;
    //             mContext->GetContentResolver((IContentResolver**)&resolver);
    //             resolver->ApplyBatch(IContactsContract::AUTHORITY, operations);
    //             // } catch (RemoteException | OperationApplicationException e) {
    //             //     Log.e(TAG, "Exception thrown when pinning contacts", e);
    //             // }
    //         }
    //     }
    //     mDraggedEntry = NULL;
    // }

    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::DropToUnsupportedView()
{
    if (IsIndexInBound(mDragEnteredEntryIndex)) {
        mContactEntries->Remove(mDragEnteredEntryIndex);
        assert(0 && "TODO");
        // mContactEntries->Add(mDraggedEntryIndex, mDraggedEntry);
        NotifyDataSetChanged();
    }

    return NOERROR;
}

ECode PhoneFavoritesTileAdapter::CleanTempVariables()
{
    mDraggedEntryIndex = -1;
    mDropEntryIndex = -1;
    mDragEnteredEntryIndex = -1;
    assert(0 && "TODO");
    // mDraggedEntry = NULL;
    return NOERROR;
}

void PhoneFavoritesTileAdapter::UnstarAndUnpinContact(
    /* [in] */ IUri* contactUri)
{
    AutoPtr<IContentValues> values;
    CContentValues::New(2, (IContentValues**)&values);
    values->Put(IContacts::STARRED, FALSE);
    values->Put(IContacts::PINNED, IPinnedPositions::DEMOTED);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    resolver->Update(contactUri, values, NULL, NULL);
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
        assert(0 && "TODO");
        // CContactEntry* contact = (CContactEntry*)item;
        // Decide whether the contact is hidden(demoted), pinned, or unpinned
        // if (contact->mPinned > PIN_LIMIT || contact->mPinned == IPinnedPositions::UNPINNED) {
        //     unpinnedContacts->Add(contact);
        // }
        // else if (contact->mPinned > IPinnedPositions::DEMOTED) {
        //     // Demoted or contacts with negative pinned positions are ignored.
        //     // Pinned contacts go into a priority queue where they are ranked by pinned
        //     // position. This is required because the contacts provider does not return
        //     // contacts ordered by pinned position.
        //     pinnedQueue->Add(contact);
        // }
    }

    Int32 pinnedSize, unpinnedSize;
    pinnedQueue->GetSize(&pinnedSize);
    unpinnedContacts->GetSize(&unpinnedSize);
    Int32 maxToPin = Elastos::Core::Math::Min(PIN_LIMIT, pinnedSize + unpinnedSize);

    toArrange->Clear();
    Boolean isEmpty;
    for (Int32 i = 1; i < maxToPin + 1; i++) {
        assert(0 && "TODO");
        // AutoPtr<IInterface> e;
        // if (pinnedQueue->IsEmpty(&isEmpty), !isEmpty &&
        //         pinnedQueue->Peek((IInterface**)&e), ((CContactEntry*)e)->mPinned <= i) {
        //     AutoPtr<IInterface> toPin;
        //     pinnedQueue->Poll((IInterface**)&toPin);
        //     ((CContactEntry*)toPin)->mPinned = i;
        //     toArrange->Add(toPin);
        // }
        // else if (unpinnedContacts->IsEmpty(&isEmpty), !isEmpty) {
        //     AutoPtr<IInterface> obj;
        //     unpinnedContacts->Remove(0, (IInterface**)&obj);
        //     toArrange->Add(obj);
        // }
    }

    // If there are still contacts in pinnedContacts at this point, it means that the pinned
    // positions of these pinned contacts exceed the actual number of contacts in the list.
    // For example, the user had 10 frequents, starred and pinned one of them at the last spot,
    // and then cleared frequents. Contacts in this situation should become unpinned.
    while (pinnedQueue->IsEmpty(&isEmpty), !isEmpty) {
        AutoPtr<IInterface> entry;
        pinnedQueue->Poll((IInterface**)&entry);
        assert(0 && "TODO");
        // ((CContactEntry*)entry)->mPinned  = IPinnedPositions::UNPINNED;
        toArrange->Add(entry);
    }

    // Any remaining unpinned contacts that weren't in the gaps between the pinned contacts
    // now just get appended to the end of the list.
    toArrange->AddAll(unpinnedContacts);
}

AutoPtr<IArrayList> PhoneFavoritesTileAdapter::GetReflowedPinningOperations(
    /* [in] */ IArrayList* list,
    /* [in] */ Int32 oldPos,
    /* [in] */ Int32 newPinPos)
{
    AutoPtr<IArrayList> positions;
    CArrayList::New((IArrayList**)&positions);
    Int32 lowerBound = Math::Min(oldPos, newPinPos);
    Int32 upperBound = Math::Max(oldPos, newPinPos);
    for (Int32 i = lowerBound; i <= upperBound; i++) {
        AutoPtr<IInterface> entry;
        list->Get(i, (IInterface**)&entry);

        // Pinned positions in the database start from 1 instead of being zero-indexed like
        // arrays, so offset by 1.
        Int32 databasePinnedPosition = i + 1;
        assert(0 && "TODO");
        // if (((CContactEntry*)entry)->mPinned == databasePinnedPosition) continue;

        AutoPtr<IUri> uri = Uri::WithAppendedPath(IContacts::CONTENT_URI,
                StringUtils::ToString(((CContactEntry*)entry)->mId));
        AutoPtr<IContentValues> values;
        CContentValues::New((IContentValues**)&values);
        values->Put(IContacts::PINNED, databasePinnedPosition);

        AutoPtr<IContentProviderOperationHelper> helper;
        CContentProviderOperationHelper::AcquireSingleton(
                (IContentProviderOperationHelper**)&helper);
        AutoPtr<IContentProviderOperationBuilder> builder;
        helper->NewUpdate(uri, (IContentProviderOperationBuilder**)&builder);
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
    assert(0 && "TODO");
    // AutoPtr<IContactEntry> entry;
    // view->GetContactEntry((IContactEntry**)&entry);
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
    assert(0 && "TODO");
    // AutoPtr<IContactEntry> entry;
    // view->GetContactEntry((IContactEntry**)&entry);
    // Int32 itemIndex;
    // mContactEntries->IndexOf(entry, &itemIndex);
    // if (mInDragging &&
    //         mDragEnteredEntryIndex != itemIndex &&
    //         IsIndexInBound(itemIndex) &&
    //         itemIndex < PIN_LIMIT &&
    //         itemIndex >= 0) {
    //     MarkDropArea(itemIndex);
    // }

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
    assert(0 && "TODO");
    // if (mDraggedEntry != NULL) {
    //     UnstarAndUnpinContact(((CContactEntry*)mDraggedEntry)->mLookupUri);
    //     mAwaitingRemove = TRUE;
    // }
    return NOERROR;
}

} // List
} // Dialer
} // Droid
} // Elastos
