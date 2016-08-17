
#ifndef __ELASTOS_DROID_COMMON_WIDGET_COMPOSITECURSORADAPTER_H__
#define __ELASTOS_DROID_COMMON_WIDGET_COMPOSITECURSORADAPTER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/widget/BaseAdapter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Common {
namespace Widget {

/**
 * A general purpose adapter that is composed of multiple cursors. It just
 * appends them in the order they are added.
 */
class CompositeCursorAdapter
    : public BaseAdapter
    , public ICompositeCursorAdapter
{
public:
    class Partition
        : public Object
        , public ICompositeCursorAdapterPartition
    {
    public:
        Partition(
            /* [in] */ Boolean showIfEmpty,
            /* [in] */ Boolean hasHeader)
            : mShowIfEmpty(showIfEmpty)
            , mHasHeader(hasHeader)
            , mIdColumnIndex(0)
            , mCount(0)
        {}

        virtual ~Partition() {}

        CAR_INTERFACE_DECL()

        /**
         * True if the directory should be shown even if no contacts are found.
         */
        CARAPI GetShowIfEmpty(
            /* [out] */ Boolean* result);

        CARAPI GetHasHeader(
            /* [out] */ Boolean* result);

        CARAPI IsEmpty(
            /* [out] */ Boolean* result);

    public:
        Boolean mShowIfEmpty;
        Boolean mHasHeader;

        AutoPtr<ICursor> mCursor;
        Int32 mIdColumnIndex;
        Int32 mCount;
    };

public:
    CompositeCursorAdapter();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Int32 initialCapacity);

    CARAPI GetContext(
        /* [out] */ IContext** context);

    /**
     * Registers a partition. The cursor for that partition can be set later.
     * Partitions should be added in the order they are supposed to appear in the
     * list.
     */
    CARAPI AddPartition(
        /* [in] */ Boolean showIfEmpty,
        /* [in] */ Boolean hasHeader);

    CARAPI AddPartition(
        /* [in] */ ICompositeCursorAdapterPartition* partition);

    CARAPI AddPartition(
        /* [in] */ Int32 location,
        /* [in] */ ICompositeCursorAdapterPartition* partition);

    CARAPI RemovePartition(
        /* [in] */ Int32 partitionIndex);

    /**
     * Removes cursors for all partitions.
     */
    // TODO: Is this really what this is supposed to do? Just remove the cursors? Not close them?
    // Not remove the partitions themselves? Isn't this leaking?
    CARAPI ClearPartitions();

    /**
     * Closes all cursors and removes all partitions.
     */
    CARAPI Close();

    CARAPI SetHasHeader(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ Boolean flag);

    CARAPI SetShowIfEmpty(
        /* [in] */ Int32 partitionIndex,
        /* [in] */ Boolean flag);

    CARAPI GetPartition(
        /* [in] */ Int32 partitionIndex,
        /* [out] */ ICompositeCursorAdapterPartition** partition);

    CARAPI GetPartitionCount(
        /* [out] */ Int32* count);

    /**
     * Returns true if the specified partition was configured to have a header.
     */
    CARAPI HasHeader(
        /* [in] */ Int32 partition,
        /* [out] */ Boolean* hasHeader);

    /**
     * Returns the total number of list items in all partitions.
     */
    CARAPI GetCount(
        /* [out] */ Int32* count);

    /**
     * Returns the cursor for the given partition
     */
    CARAPI GetCursor(
        /* [in] */ Int32 partition,
        /* [out] */ ICursor** cursor);

    /**
     * Changes the cursor for an individual partition.
     */
    CARAPI ChangeCursor(
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor);

    /**
     * Returns true if the specified partition has no cursor or an empty cursor.
     */
    CARAPI IsPartitionEmpty(
        /* [in] */ Int32 partition,
        /* [out] */ Boolean* isEmpty);

    /**
     * Given a list position, returns the index of the corresponding partition.
     */
    CARAPI GetPartitionForPosition(
        /* [in] */ Int32 position,
        /* [out] */ Int32* partition);

    /**
     * Given a list position, return the offset of the corresponding item in its
     * partition.  The header, if any, will have offset -1.
     */
    CARAPI GetOffsetInPartition(
        /* [in] */ Int32 position,
        /* [out] */ Int32* offset);

    /**
     * Returns the first list position for the specified partition.
     */
    CARAPI GetPositionForPartition(
        /* [in] */ Int32 partition,
        /* [out] */ Int32* position);

    // @Override
    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    /**
     * Returns the overall number of item view types across all partitions. An
     * implementation of this method needs to ensure that the returned count is
     * consistent with the values returned by {@link #getItemViewType(int,int)}.
     */
    CARAPI GetItemViewTypeCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    // @Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    /**
     * Returns a pre-positioned cursor for the specified list position.
     */
    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    /**
     * Returns the item ID for the specified list position.
     */
    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    // @Override
    CARAPI AreAllItemsEnabled(
        /* [out] */ Boolean* enabled);

    // @Override
    CARAPI IsEnabled(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* enabled);

    /**
     * Enable or disable data change notifications.  It may be a good idea to
     * disable notifications before making changes to several partitions at once.
     */
    CARAPI SetNotificationsEnabled(
        /* [in] */ Boolean flag);

    // @Override
    CARAPI NotifyDataSetChanged();

protected:
    CARAPI_(void) Invalidate();

    CARAPI_(void) EnsureCacheValid();

    /**
     * Returns the view type for the list item at the specified position in the
     * specified partition.
     */
    CARAPI_(Int32) GetItemViewType(
        /* [in] */ Int32 partition,
        /* [in] */ Int32 position);

    /**
     * Returns the header view for the specified partition, creating one if needed.
     */
    CARAPI_(AutoPtr<IView>) GetHeaderView(
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent);

    /**
     * Creates the header view for the specified partition.
     */
    CARAPI_(AutoPtr<IView>) NewHeaderView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ IViewGroup* parent);

    /**
     * Binds the header view for the specified partition.
     */
    CARAPI_(void) BindHeaderView(
        /* [in] */ IView* view,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor);

    /**
     * Returns an item view for the specified partition, creating one if needed.
     */
    CARAPI_(AutoPtr<IView>) GetView(
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent);

    /**
     * Creates an item view for the specified partition and position. Position
     * corresponds directly to the current cursor position.
     */
    virtual CARAPI_(AutoPtr<IView>) NewView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position,
        /* [in] */ IViewGroup* parent) = 0;

    /**
     * Binds an item view for the specified partition and position. Position
     * corresponds directly to the current cursor position.
     */
    virtual CARAPI_(void) BindView(
        /* [in] */ IView* v,
        /* [in] */ Int32 partition,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 position) = 0;

    /**
     * Returns true if the item at the specified offset of the specified
     * partition is selectable and clickable.
     */
    CARAPI_(Boolean) IsEnabled(
        /* [in] */ Int32 partition,
        /* [in] */ Int32 position);

private:
    static const Int32 INITIAL_CAPACITY = 2;

    AutoPtr<IContext> mContext;
    AutoPtr<IArrayList> mPartitions;
    Int32 mCount;
    Boolean mCacheValid;
    Boolean mNotificationsEnabled;
    Boolean mNotificationNeeded;
};

} // Widget
} // Common
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_COMMON_WIDGET_COMPOSITECURSORADAPTER_H__
