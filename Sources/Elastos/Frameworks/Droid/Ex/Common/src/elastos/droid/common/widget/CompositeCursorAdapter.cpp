
#include "elastos/droid/common/widget/CompositeCursorAdapter.h"
#include <elastos/utility/logging/Logger.h>
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.IO.h"

using Elastos::Droid::Common::Widget::EIID_ICompositeCursorAdapterPartition;
using Elastos::Droid::Common::Widget::EIID_ICompositeCursorAdapter;
using Elastos::IO::ICloseable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Common {
namespace Widget {

//=================================================================
// CompositeCursorAdapter::Partition
//=================================================================
CAR_INTERFACE_IMPL(CompositeCursorAdapter::Partition, Object, ICompositeCursorAdapterPartition)

ECode CompositeCursorAdapter::Partition::GetShowIfEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mShowIfEmpty;
    return NOERROR;
}

ECode CompositeCursorAdapter::Partition::GetHasHeader(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHasHeader;
    return NOERROR;
}

ECode CompositeCursorAdapter::Partition::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mCount == 0;
    return NOERROR;
}


//=================================================================
// CompositeCursorAdapter
//=================================================================
const Int32 CompositeCursorAdapter::INITIAL_CAPACITY;

CompositeCursorAdapter::CompositeCursorAdapter()
    : mCount(0)
    , mCacheValid(TRUE)
    , mNotificationsEnabled(TRUE)
    , mNotificationNeeded(FALSE)
{}

CAR_INTERFACE_IMPL(CompositeCursorAdapter, BaseAdapter, ICompositeCursorAdapter)

ECode CompositeCursorAdapter::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, INITIAL_CAPACITY);
}

ECode CompositeCursorAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 initialCapacity)
{
    mContext = context;
    CArrayList::New((IArrayList**)&mPartitions);
    return NOERROR;
}

ECode CompositeCursorAdapter::GetContext(
    /* [out] */ IContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = mContext;
    REFCOUNT_ADD(*context)
    return NOERROR;
}

ECode CompositeCursorAdapter::AddPartition(
    /* [in] */ Boolean showIfEmpty,
    /* [in] */ Boolean hasHeader)
{
    AutoPtr<ICompositeCursorAdapterPartition> partition
            = (ICompositeCursorAdapterPartition*)new Partition(showIfEmpty, hasHeader);
    return AddPartition(partition);
}

ECode CompositeCursorAdapter::AddPartition(
    /* [in] */ ICompositeCursorAdapterPartition* partition)
{
    mPartitions->Add(partition);
    Invalidate();
    NotifyDataSetChanged();
    return NOERROR;
}

ECode CompositeCursorAdapter::AddPartition(
    /* [in] */ Int32 location,
    /* [in] */ ICompositeCursorAdapterPartition* partition)
{
    mPartitions->Add(location, partition);
    Invalidate();
    NotifyDataSetChanged();
    return NOERROR;
}

ECode CompositeCursorAdapter::RemovePartition(
    /* [in] */ Int32 partitionIndex)
{
    AutoPtr<IInterface> temp;
    mPartitions->Get(partitionIndex, (IInterface**)&temp);
    AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
    AutoPtr<ICursor> cursor = partition->mCursor;
    Boolean isClosed;
    if (cursor != NULL && (cursor->IsClosed(&isClosed), !isClosed)) {
        ICloseable::Probe(cursor)->Close();
    }
    mPartitions->Remove(partitionIndex);
    Invalidate();
    NotifyDataSetChanged();
    return NOERROR;
}

ECode CompositeCursorAdapter::ClearPartitions()
{
    AutoPtr<IIterator> it;
    mPartitions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(value);
        partition->mCursor = NULL;
    }
    Invalidate();
    NotifyDataSetChanged();
    return NOERROR;
}

ECode CompositeCursorAdapter::Close()
{
    AutoPtr<IIterator> it;
    mPartitions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(value);
        AutoPtr<ICursor> cursor = partition->mCursor;
        Boolean isClosed;
        if (cursor != NULL && (cursor->IsClosed(&isClosed), !isClosed)) {
            ICloseable::Probe(cursor)->Close();
        }
    }
    mPartitions->Clear();
    Invalidate();
    NotifyDataSetChanged();
    return NOERROR;
}

ECode CompositeCursorAdapter::SetHasHeader(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ Boolean flag)
{
    AutoPtr<IInterface> temp;
    mPartitions->Get(partitionIndex, (IInterface**)&temp);
    AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
    partition->mHasHeader = flag;
    Invalidate();
    return NOERROR;
}

ECode CompositeCursorAdapter::SetShowIfEmpty(
    /* [in] */ Int32 partitionIndex,
    /* [in] */ Boolean flag)
{
    AutoPtr<IInterface> temp;
    mPartitions->Get(partitionIndex, (IInterface**)&temp);
    AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
    partition->mShowIfEmpty = flag;
    Invalidate();
    return NOERROR;
}

ECode CompositeCursorAdapter::GetPartition(
    /* [in] */ Int32 partitionIndex,
    /* [out] */ ICompositeCursorAdapterPartition** partition)
{
    VALIDATE_NOT_NULL(partition)
    AutoPtr<IInterface> temp;
    mPartitions->Get(partitionIndex, (IInterface**)&temp);
    *partition = ICompositeCursorAdapterPartition::Probe(temp);
    REFCOUNT_ADD(*partition)
    return NOERROR;
}

void CompositeCursorAdapter::Invalidate()
{
    mCacheValid = FALSE;
}

ECode CompositeCursorAdapter::GetPartitionCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return mPartitions->GetSize(count);
}

void CompositeCursorAdapter::EnsureCacheValid()
{
    if (mCacheValid) {
        return;
    }

    mCount = 0;
    AutoPtr<IIterator> it;
    mPartitions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(value);
        AutoPtr<ICursor> cursor = partition->mCursor;
        Int32 count = 0;
        if (cursor != NULL) {
            cursor->GetCount(&count);
        }
        if (partition->mHasHeader) {
            if (count != 0 || partition->mShowIfEmpty) {
                count++;
            }
        }
        partition->mCount = count;
        mCount += count;
    }

    mCacheValid = TRUE;
}

ECode CompositeCursorAdapter::HasHeader(
    /* [in] */ Int32 partition,
    /* [out] */ Boolean* hasHeader)
{
    VALIDATE_NOT_NULL(hasHeader)
    AutoPtr<IInterface> temp;
    mPartitions->Get(partition, (IInterface**)&temp);
    AutoPtr<Partition> p = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
    *hasHeader = p->mHasHeader;
    return NOERROR;
}

ECode CompositeCursorAdapter::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    EnsureCacheValid();
    *count = mCount;
    return NOERROR;
}

ECode CompositeCursorAdapter::GetCursor(
    /* [in] */ Int32 partition,
    /* [out] */ ICursor** cursor)
{
    VALIDATE_NOT_NULL(cursor)
    AutoPtr<IInterface> temp;
    mPartitions->Get(partition, (IInterface**)&temp);
    AutoPtr<Partition> p = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
    *cursor = p->mCursor;
    REFCOUNT_ADD(*cursor)
    return NOERROR;
}

ECode CompositeCursorAdapter::ChangeCursor(
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor)
{
    AutoPtr<IInterface> temp;
    mPartitions->Get(partition, (IInterface**)&temp);
    AutoPtr<Partition> p = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
    AutoPtr<ICursor> prevCursor = p->mCursor;
    if (prevCursor.Get() != cursor) {
        Boolean isClosed;
        if (prevCursor != NULL && (prevCursor->IsClosed(&isClosed), !isClosed)) {
            ICloseable::Probe(prevCursor)->Close();
        }
        p->mCursor = cursor;
        if (cursor != NULL) {
            cursor->GetColumnIndex(String("_id"), &p->mIdColumnIndex);
        }
        Invalidate();
        NotifyDataSetChanged();
    }
    return NOERROR;
}

ECode CompositeCursorAdapter::IsPartitionEmpty(
    /* [in] */ Int32 partition,
    /* [out] */ Boolean* isEmpty)
{
    VALIDATE_NOT_NULL(isEmpty)
    AutoPtr<IInterface> temp;
    mPartitions->Get(partition, (IInterface**)&temp);
    AutoPtr<Partition> p = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
    AutoPtr<ICursor> cursor = p->mCursor;
    Int32 count;
    *isEmpty = cursor == NULL || (cursor->GetCount(&count), count == 0);
    return NOERROR;
}

ECode CompositeCursorAdapter::GetPartitionForPosition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* partition)
{
    VALIDATE_NOT_NULL(partition)
    EnsureCacheValid();
    Int32 start = 0;
    Int32 size;
    mPartitions->GetSize(&size);
    for (Int32 i = 0, n = size; i < n; i++) {
        AutoPtr<IInterface> temp;
        mPartitions->Get(i, (IInterface**)&temp);
        AutoPtr<Partition> p = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
        Int32 end = start + p->mCount;
        if (position >= start && position < end) {
            *partition = i;
            return NOERROR;
        }
        start = end;
    }
    *partition = -1;
    return NOERROR;
}

ECode CompositeCursorAdapter::GetOffsetInPartition(
    /* [in] */ Int32 position,
    /* [out] */ Int32* _offset)
{
    VALIDATE_NOT_NULL(_offset)
    EnsureCacheValid();
    Int32 start = 0;
    AutoPtr<IIterator> it;
    mPartitions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(value);
        Int32 end = start + partition->mCount;
        if (position >= start && position < end) {
            Int32 offset = position - start;
            if (partition->mHasHeader) {
                offset--;
            }
            *_offset = offset;
            return NOERROR;
        }
        start = end;
    }
    *_offset = -1;
    return NOERROR;
}

ECode CompositeCursorAdapter::GetPositionForPartition(
    /* [in] */ Int32 partition,
    /* [out] */ Int32* _position)
{
    VALIDATE_NOT_NULL(_position)
    EnsureCacheValid();
    Int32 position = 0;
    for (Int32 i = 0; i < partition; i++) {
        AutoPtr<IInterface> temp;
        mPartitions->Get(i, (IInterface**)&temp);
        AutoPtr<Partition> p = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
        position += p->mCount;
    }
    *_position = position;
    return NOERROR;
}

ECode CompositeCursorAdapter::GetViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 c;
    GetItemViewTypeCount(&c);
    *count = c + 1;
    return NOERROR;
}

ECode CompositeCursorAdapter::GetItemViewTypeCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 1;
    return NOERROR;
}

Int32 CompositeCursorAdapter::GetItemViewType(
    /* [in] */ Int32 partition,
    /* [in] */ Int32 position)
{
    return 1;
}

ECode CompositeCursorAdapter::GetItemViewType(
    /* [in] */ Int32 position,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)
    *type = 0;

    EnsureCacheValid();
    Int32 start = 0;
    Int32 size;
    mPartitions->GetSize(&size);
    for (Int32 i = 0, n = size; i < n; i++) {
        AutoPtr<IInterface> temp;
        mPartitions->Get(i, (IInterface**)&temp);
        AutoPtr<Partition> p = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
        Int32 end = start  + p->mCount;
        if (position >= start && position < end) {
            Int32 offset = position - start;
            if (p->mHasHeader) {
                offset--;
            }
            if (offset == -1) {
                *type = IAdapter::IGNORE_ITEM_VIEW_TYPE;
                return NOERROR;
            }
            else {
                *type = GetItemViewType(i, offset);
                return NOERROR;
            }
        }
        start = end;
    }

    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode CompositeCursorAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** _view)
{
    VALIDATE_NOT_NULL(_view)
    *_view = NULL;

    EnsureCacheValid();
    Int32 start = 0;
    Int32 size;
    mPartitions->GetSize(&size);
    for (Int32 i = 0, n = size; i < n; i++) {
        AutoPtr<IInterface> temp;
        mPartitions->Get(i, (IInterface**)&temp);
        AutoPtr<Partition> p = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
        Int32 end = start + p->mCount;
        if (position >= start && position < end) {
            Int32 offset = position - start;
            if (p->mHasHeader) {
                offset--;
            }
            AutoPtr<IView> view;
            if (offset == -1) {
                view = GetHeaderView(i, p->mCursor, convertView, parent);
            }
            else {
                Boolean result;
                if (p->mCursor->MoveToPosition(offset, &result), !result) {
                    Logger::E("CompositeCursorAdapter", "Couldn't move cursor to position %d", offset);
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
                view = GetView(i, p->mCursor, offset, convertView, parent);
            }
            if (view == NULL) {
                Logger::E("CompositeCursorAdapter", "View should not be null, partition: %d position: %d", i, offset);
                return E_NULL_POINTER_EXCEPTION;
            }
            *_view = view;
            REFCOUNT_ADD(*_view)
            return NOERROR;
        }
        start = end;
    }

    return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

AutoPtr<IView> CompositeCursorAdapter::GetHeaderView(
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> view;
    if (convertView != NULL) {
        view = convertView;
    }
    else {
        view = NewHeaderView(mContext, partition, cursor, parent);
    }
    BindHeaderView(view, partition, cursor);
    return view;
}

AutoPtr<IView> CompositeCursorAdapter::NewHeaderView(
    /* [in] */ IContext* context,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ IViewGroup* parent)
{
    return NULL;
}

void CompositeCursorAdapter::BindHeaderView(
    /* [in] */ IView* view,
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor)
{}

AutoPtr<IView> CompositeCursorAdapter::GetView(
    /* [in] */ Int32 partition,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent)
{
    AutoPtr<IView> view;
    if (convertView != NULL) {
        view = convertView;
    }
    else {
        view = NewView(mContext, partition, cursor, position, parent);
    }
    BindView(view, partition, cursor, position);
    return view;
}

ECode CompositeCursorAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** item)
{
    VALIDATE_NOT_NULL(item)
    EnsureCacheValid();
    Int32 start = 0;
    AutoPtr<IIterator> it;
    mPartitions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(value);
        Int32 end = start + partition->mCount;
        if (position >= start && position < end) {
            Int32 offset = position - start;
            if (partition->mHasHeader) {
                offset--;
            }
            if (offset == -1) {
                *item = NULL;
                return NOERROR;
            }
            AutoPtr<ICursor> cursor = partition->mCursor;
            Boolean result;
            cursor->MoveToPosition(offset, &result);
            *item = cursor;
            REFCOUNT_ADD(*item)
            return NOERROR;
        }
        start = end;
    }

    *item = NULL;
    return NOERROR;
}

ECode CompositeCursorAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* id)
{
    EnsureCacheValid();
    Int32 start = 0;
    AutoPtr<IIterator> it;
    mPartitions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(value);
        Int32 end = start + partition->mCount;
        if (position >= start && position < end) {
            Int32 offset = position - start;
            if (partition->mHasHeader) {
                offset--;
            }
            if (offset == -1) {
                *id = 0;
                return NOERROR;
            }
            if (partition->mIdColumnIndex == -1) {
                *id = 0;
                return NOERROR;
            }

            AutoPtr<ICursor> cursor = partition->mCursor;
            Boolean isClosed, result;
            if (cursor == NULL || (cursor->IsClosed(&isClosed), isClosed)
                    || (cursor->MoveToPosition(offset, &result), !result)) {
                *id = 0;
                return NOERROR;
            }
            return cursor->GetInt64(partition->mIdColumnIndex, id);
        }
        start = end;
    }

    *id = 0;
    return NOERROR;
}

ECode CompositeCursorAdapter::AreAllItemsEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    AutoPtr<IIterator> it;
    mPartitions->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> value;
        it->GetNext((IInterface**)&value);
        AutoPtr<Partition> partition = (Partition*)ICompositeCursorAdapterPartition::Probe(value);
        if (partition->mHasHeader) {
            *enabled = FALSE;
            return NOERROR;
        }
    }
    *enabled = TRUE;
    return NOERROR;
}

ECode CompositeCursorAdapter::IsEnabled(
    /* [in] */ Int32 position,
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    EnsureCacheValid();
    Int32 start = 0;
    Int32 size;
    mPartitions->GetSize(&size);
    for (Int32 i = 0, n = size; i < n; i++) {
        AutoPtr<IInterface> temp;
        mPartitions->Get(i, (IInterface**)&temp);
        AutoPtr<Partition> p = (Partition*)ICompositeCursorAdapterPartition::Probe(temp);
        Int32 end = start + p->mCount;
        if (position >= start && position < end) {
            Int32 offset = position - start;
            if (p->mHasHeader && offset == 0) {
                *enabled = FALSE;
                return NOERROR;
            }
            else {
                *enabled = IsEnabled(i, offset);
                return NOERROR;
            }
        }
        start = end;
    }

    *enabled = FALSE;
    return NOERROR;
}

Boolean CompositeCursorAdapter::IsEnabled(
    /* [in] */ Int32 partition,
    /* [in] */ Int32 position)
{
    return TRUE;
}

ECode CompositeCursorAdapter::SetNotificationsEnabled(
    /* [in] */ Boolean flag)
{
    mNotificationsEnabled = flag;
    if (flag && mNotificationNeeded) {
        NotifyDataSetChanged();
    }
    return NOERROR;
}

ECode CompositeCursorAdapter::NotifyDataSetChanged()
{
    if (mNotificationsEnabled) {
        mNotificationNeeded = FALSE;
        return BaseAdapter::NotifyDataSetChanged();
    }
    else {
        mNotificationNeeded = TRUE;
    }
    return NOERROR;
}

} // Widget
} // Common
} // Droid
} // Elastos