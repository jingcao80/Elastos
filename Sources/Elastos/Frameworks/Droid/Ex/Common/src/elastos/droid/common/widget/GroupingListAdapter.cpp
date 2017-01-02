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

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/common/widget/GroupingListAdapter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Common::Widget::EIID_IGroupingListAdapter;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Common {
namespace Widget {

//=================================================================
// GroupingListAdapter::ChangeObserver
//=================================================================
ECode GroupingListAdapter::ChangeObserver::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ GroupingListAdapter* host)
{
    mHost = host;
    ContentObserver::constructor(handler);
    return NOERROR;
}

ECode GroupingListAdapter::ChangeObserver::DeliverSelfNotifications(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode GroupingListAdapter::ChangeObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->OnContentChanged();
    return NOERROR;
}


//=================================================================
// GroupingListAdapter::MyDataSetObserver
//=================================================================
ECode GroupingListAdapter::MyDataSetObserver::constructor(
    /* [in] */ GroupingListAdapter* host)
{
    mHost = host;
    DataSetObserver::constructor();
    return NOERROR;
}

ECode GroupingListAdapter::MyDataSetObserver::OnChanged()
{
    mHost->NotifyDataSetChanged();
    return NOERROR;
}

ECode GroupingListAdapter::MyDataSetObserver::OnInvalidated()
{
    mHost->NotifyDataSetInvalidated();
    return NOERROR;
}


//=================================================================
// GroupingListAdapter
//=================================================================
const Int32 GroupingListAdapter::GROUP_METADATA_ARRAY_INITIAL_SIZE;
const Int32 GroupingListAdapter::GROUP_METADATA_ARRAY_INCREMENT;
const Int64 GroupingListAdapter::GROUP_OFFSET_MASK;
const Int64 GroupingListAdapter::GROUP_SIZE_MASK;
const Int64 GroupingListAdapter::EXPANDED_GROUP_MASK;

GroupingListAdapter::GroupingListAdapter()
    : mCount(0)
    , mRowIdColumnIndex(0)
    , mGroupCount(0)
    , mLastCachedListPosition(0)
    , mLastCachedCursorPosition(0)
    , mLastCachedGroup(0)
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    AutoPtr<ChangeObserver> changeObserver = new ChangeObserver();
    changeObserver->constructor(handler, this);
    mChangeObserver = (IContentObserver*)changeObserver;
    AutoPtr<MyDataSetObserver> dataSetObserver = new MyDataSetObserver();
    dataSetObserver->constructor(this);
    mDataSetObserver = (IDataSetObserver*)dataSetObserver;
    CSparseInt32Array::New((ISparseInt32Array**)&mPositionCache);
    mPositionMetadata = new PositionMetadata();
}

CAR_INTERFACE_IMPL(GroupingListAdapter, BaseAdapter, IGroupingListAdapter)

ECode GroupingListAdapter::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    ResetCache();
    return NOERROR;
}

void GroupingListAdapter::ResetCache()
{
    mCount = -1;
    mLastCachedListPosition = -1;
    mLastCachedCursorPosition = -1;
    mLastCachedGroup = -1;
    mPositionMetadata->mListPosition = -1;
    mPositionCache->Clear();
}

void GroupingListAdapter::OnContentChanged()
{}

ECode GroupingListAdapter::ChangeCursor(
    /* [in] */ ICursor* cursor)
{
    if (cursor == mCursor.Get()) {
        return NOERROR;
    }

    if (mCursor != NULL) {
        mCursor->UnregisterContentObserver(mChangeObserver);
        mCursor->UnregisterDataSetObserver(mDataSetObserver);
        ICloseable::Probe(mCursor)->Close();
    }
    mCursor = cursor;
    ResetCache();
    FindGroups();

    if (cursor != NULL) {
        cursor->RegisterContentObserver(mChangeObserver);
        cursor->RegisterDataSetObserver(mDataSetObserver);
        cursor->GetColumnIndexOrThrow(String("_id"), &mRowIdColumnIndex);
        NotifyDataSetChanged();
    }
    else {
        // notify the observers about the lack of a data set
        NotifyDataSetInvalidated();
    }
    return NOERROR;
}

ECode GroupingListAdapter::GetCursor(
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    *cursor = mCursor;
    REFCOUNT_ADD(*cursor)
    return NOERROR;
}

void GroupingListAdapter::FindGroups()
{
    mGroupCount = 0;
    mGroupMetadata = ArrayOf<Int64>::Alloc(GROUP_METADATA_ARRAY_INITIAL_SIZE);

    if (mCursor == NULL) {
        return;
    }

    AddGroups(mCursor);
}

void GroupingListAdapter::AddGroup(
    /* [in] */ Int32 cursorPosition,
    /* [in] */ Int32 size,
    /* [in] */ Boolean expanded)
{
    if (mGroupCount >= mGroupMetadata->GetLength()) {
        Int32 newSize = IdealInt64ArraySize(
                mGroupMetadata->GetLength() + GROUP_METADATA_ARRAY_INCREMENT);
        AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(newSize);
        array->Copy(0, mGroupMetadata, 0, mGroupCount);
        mGroupMetadata = array;
    }

    Int64 metadata = ((Int64)size << 32) | cursorPosition;
    if (expanded) {
        metadata |= EXPANDED_GROUP_MASK;
    }
    (*mGroupMetadata)[mGroupCount++] = metadata;
}

Int32 GroupingListAdapter::IdealInt64ArraySize(
    /* [in] */ Int32 need)
{
    return IdealByteArraySize(need * 8) / 8;
}

Int32 GroupingListAdapter::IdealByteArraySize(
    /* [in] */ Int32 need)
{
    for (Int32 i = 4; i < 32; i++) {
        if (need <= (1 << i) - 12)
            return (1 << i) - 12;
    }

    return need;
}

ECode GroupingListAdapter::GetCount(
    /* [out] */ Int32* _count)
{
    VALIDATE_NOT_NULL(_count)
    *_count = 0;
    if (mCursor == NULL) {
        return NOERROR;
    }

    if (mCount != -1) {
        *_count = mCount;
        return NOERROR;
    }

    Int32 cursorPosition = 0;
    Int32 count = 0;
    for (Int32 i = 0; i < mGroupCount; i++) {
        Int64 metadata = (*mGroupMetadata)[i];
        Int32 offset = (Int32)(metadata & GROUP_OFFSET_MASK);
        Boolean expanded = (metadata & EXPANDED_GROUP_MASK) != 0;
        Int32 size = (Int32)((metadata & GROUP_SIZE_MASK) >> 32);

        count += (offset - cursorPosition);

        if (expanded) {
            count += size + 1;
        }
        else {
            count++;
        }

        cursorPosition = offset + size;
    }

    Int32 c;
    mCursor->GetCount(&c);
    mCount = count + c - cursorPosition;
    *_count = mCount;
    return NOERROR;
}

void GroupingListAdapter::ObtainPositionMetadata(
    /* [in] */ PositionMetadata* metadata,
    /* [in] */ Int32 position)
{
    // If the description object already contains requested information, just return
    if (metadata->mListPosition == position) {
        return;
    }

    Int32 listPosition = 0;
    Int32 cursorPosition = 0;
    Int32 firstGroupToCheck = 0;

    // Check cache for the supplied position.  What we are looking for is
    // the group descriptor immediately preceding the supplied position.
    // Once we have that, we will be able to tell whether the position
    // is the header of the group, a member of the group or a standalone item.
    if (mLastCachedListPosition != -1) {
        if (position <= mLastCachedListPosition) {

            // Have SparceIntArray do a binary search for us.
            Int32 index;
            mPositionCache->IndexOfKey(position, &index);

            // If we get back a positive number, the position corresponds to
            // a group header.
            if (index < 0) {

                // We had a cache miss, but we did obtain valuable information anyway.
                // The negative number will allow us to compute the location of
                // the group header immediately preceding the supplied position.
                index = ~index - 1;

                Int32 size;
                if (mPositionCache->GetSize(&size), index >= size) {
                    index--;
                }
            }

            // A non-negative index gives us the position of the group header
            // corresponding or preceding the position, so we can
            // search for the group information at the supplied position
            // starting with the cached group we just found
            if (index >= 0) {
                mPositionCache->KeyAt(index, &listPosition);
                mPositionCache->ValueAt(index, &firstGroupToCheck);
                Int64 descriptor = (*mGroupMetadata)[firstGroupToCheck];
                cursorPosition = (Int32)(descriptor & GROUP_OFFSET_MASK);
            }
        }
        else {

            // If we haven't examined groups beyond the supplied position,
            // we will start where we left off previously
            firstGroupToCheck = mLastCachedGroup;
            listPosition = mLastCachedListPosition;
            cursorPosition = mLastCachedCursorPosition;
        }
    }

    for (Int32 i = firstGroupToCheck; i < mGroupCount; i++) {
        Int64 group = (*mGroupMetadata)[i];
        Int32 offset = (Int32)(group & GROUP_OFFSET_MASK);

        // Move pointers to the beginning of the group
        listPosition += (offset - cursorPosition);
        cursorPosition = offset;

        if (i > mLastCachedGroup) {
            mPositionCache->Append(listPosition, i);
            mLastCachedListPosition = listPosition;
            mLastCachedCursorPosition = cursorPosition;
            mLastCachedGroup = i;
        }

        // Now we have several possibilities:
        // A) The requested position precedes the group
        if (position < listPosition) {
            metadata->mItemType = ITEM_TYPE_STANDALONE;
            metadata->mCursorPosition = cursorPosition - (listPosition - position);
            return;
        }

        Boolean expanded = (group & EXPANDED_GROUP_MASK) != 0;
        Int32 size = (Int32)((group & GROUP_SIZE_MASK) >> 32);

        // B) The requested position is a group header
        if (position == listPosition) {
            metadata->mItemType = ITEM_TYPE_GROUP_HEADER;
            metadata->mGroupPosition = i;
            metadata->mIsExpanded = expanded;
            metadata->mChildCount = size;
            metadata->mCursorPosition = offset;
            return;
        }

        if (expanded) {
            // C) The requested position is an element in the expanded group
            if (position < listPosition + size + 1) {
                metadata->mItemType = ITEM_TYPE_IN_GROUP;
                metadata->mCursorPosition = cursorPosition + (position - listPosition) - 1;
                return;
            }

            // D) The element is past the expanded group
            listPosition += size + 1;
        }
        else {

            // E) The element is past the collapsed group
            listPosition++;
        }

        // Move cursor past the group
        cursorPosition += size;
    }

    // The required item is past the last group
    metadata->mItemType = ITEM_TYPE_STANDALONE;
    metadata->mCursorPosition = cursorPosition + (position - listPosition);
}

ECode GroupingListAdapter::IsGroupHeader(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* isHeader)
{
    VALIDATE_NOT_NULL(isHeader)
    ObtainPositionMetadata(mPositionMetadata, position);
    *isHeader = mPositionMetadata->mItemType == ITEM_TYPE_GROUP_HEADER;
    return NOERROR;
}

ECode GroupingListAdapter::GetGroupSize(
    /* [in] */ Int32 position,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    ObtainPositionMetadata(mPositionMetadata, position);
    *size = mPositionMetadata->mChildCount;
    return NOERROR;
}

ECode GroupingListAdapter::ToggleGroup(
    /* [in] */ Int32 position)
{
    ObtainPositionMetadata(mPositionMetadata, position);
    if (mPositionMetadata->mItemType != ITEM_TYPE_GROUP_HEADER) {
        Logger::E("GroupingListAdapter", "Not a group at position %d", position);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }


    if (mPositionMetadata->mIsExpanded) {
        (*mGroupMetadata)[mPositionMetadata->mGroupPosition] &= ~EXPANDED_GROUP_MASK;
    }
    else {
        (*mGroupMetadata)[mPositionMetadata->mGroupPosition] |= EXPANDED_GROUP_MASK;
    }
    ResetCache();
    NotifyDataSetChanged();
    return NOERROR;
}

ECode GroupingListAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 3;
    return NOERROR;
}

ECode GroupingListAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    ObtainPositionMetadata(mPositionMetadata, position);
    *type = mPositionMetadata->mItemType;
    return NOERROR;
}

ECode GroupingListAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    *item = NULL;
    if (mCursor == NULL) {
        return NOERROR;
    }

    ObtainPositionMetadata(mPositionMetadata, position);
    Boolean result;
    if (mCursor->MoveToPosition(mPositionMetadata->mCursorPosition, &result), result) {
        *item = IInterface::Probe(mCursor);
        REFCOUNT_ADD(*item)
    }
    else {
        *item = NULL;
    }
    return NOERROR;
}

ECode GroupingListAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* itemId)
{
    VALIDATE_NOT_NULL(itemId)
    AutoPtr<IInterface> item;
    GetItem(position, (IInterface**)&item);
    if (item != NULL) {
        return mCursor->GetInt64(mRowIdColumnIndex, itemId);
    }
    else {
        *itemId = -1;
    }
    return NOERROR;
}

ECode GroupingListAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** _view)
{
    VALIDATE_NOT_NULL(_view)
    *_view = NULL;
    ObtainPositionMetadata(mPositionMetadata, position);
    AutoPtr<IView> view = convertView;
    if (view == NULL) {
        switch (mPositionMetadata->mItemType) {
            case ITEM_TYPE_STANDALONE:
                view = NewStandAloneView(mContext, parent);
                break;
            case ITEM_TYPE_GROUP_HEADER:
                view = NewGroupView(mContext, parent);
                break;
            case ITEM_TYPE_IN_GROUP:
                view = NewChildView(mContext, parent);
                break;
        }
    }

    Boolean result;
    mCursor->MoveToPosition(mPositionMetadata->mCursorPosition, &result);
    switch (mPositionMetadata->mItemType) {
        case ITEM_TYPE_STANDALONE:
            BindStandAloneView(view, mContext, mCursor);
            break;
        case ITEM_TYPE_GROUP_HEADER:
            BindGroupView(view, mContext, mCursor, mPositionMetadata->mChildCount,
                    mPositionMetadata->mIsExpanded);
            break;
        case ITEM_TYPE_IN_GROUP:
            BindChildView(view, mContext, mCursor);
            break;

    }
    *_view = view;
    REFCOUNT_ADD(*_view)
    return NOERROR;
}

} // Widget
} // Common
} // Droid
} // Elastos