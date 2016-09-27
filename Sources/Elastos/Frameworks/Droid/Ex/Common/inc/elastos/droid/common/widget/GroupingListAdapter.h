
#ifndef __ELASTOS_DROID_COMMON_WIDGET_GROUPINGLISTADAPTER_H__
#define __ELASTOS_DROID_COMMON_WIDGET_GROUPINGLISTADAPTER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/database/DataSetObserver.h"
#include "elastos/droid/widget/BaseAdapter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::BaseAdapter;

namespace Elastos {
namespace Droid {
namespace Common {
namespace Widget {

/**
 * A general purpose adapter that is composed of multiple cursors. It just
 * appends them in the order they are added.
 */
class GroupingListAdapter
    : public BaseAdapter
    , public IGroupingListAdapter
{
protected:
    /**
     * Information about a specific list item: is it a group, if so is it expanded.
     * Otherwise, is it a stand-alone item or a group member.
     */
    class PositionMetadata : public Object
    {
    public:
        PositionMetadata()
            : mItemType(0)
            , mIsExpanded(FALSE)
            , mCursorPosition(0)
            , mChildCount(0)
            , mGroupPosition(0)
            , mListPosition(-1)
        {}

    protected:
        Int32 mItemType;
        Boolean mIsExpanded;
        Int32 mCursorPosition;
        Int32 mChildCount;

    private:
        Int32 mGroupPosition;
        Int32 mListPosition;
        friend class GroupingListAdapter;
    };

private:
    class ChangeObserver : public ContentObserver
    {
    public:
        TO_STRING_IMPL("GroupingListAdapter::ChangeObserver")

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ GroupingListAdapter* host);

        // @Override
        CARAPI DeliverSelfNotifications(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        GroupingListAdapter* mHost;
    };

    class MyDataSetObserver : public DataSetObserver
    {
    public:
        CARAPI constructor(
            /* [in] */ GroupingListAdapter* host);

        // @Override
        CARAPI OnChanged();

        CARAPI OnInvalidated();

        TO_STRING_IMPL("GroupingListAdapter::MyDataSetObserver")

    private:
        GroupingListAdapter* mHost;
    };

public:
    GroupingListAdapter();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI ChangeCursor(
        /* [in] */ ICursor* cursor);

    CARAPI GetCursor(
        /* [out] */ ICursor** cursor);

    // @Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    /**
     * Figures out whether the item at the specified position represents a
     * stand-alone element, a group or a group child. Also computes the
     * corresponding cursor position.
     */
    CARAPI_(void) ObtainPositionMetadata(
        /* [in] */ PositionMetadata* metadata,
        /* [in] */ Int32 position);

    /**
     * Returns true if the specified position in the list corresponds to a
     * group header.
     */
    CARAPI IsGroupHeader(
        /* [in] */ Int32 position,
        /* [out] */ Boolean* isHeader);

    /**
     * Given a position of a groups header in the list, returns the size of
     * the corresponding group.
     */
    CARAPI GetGroupSize(
        /* [in] */ Int32 position,
        /* [out] */ Int32* size);

    /**
     * Mark group as expanded if it is collapsed and vice versa.
     */
    CARAPI ToggleGroup(
        /* [in] */ Int32 position);

    // @Override
    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetItemViewType(
        /* [in] */ Int32 position,
        /* [out] */ Int32* type);

    // @Override
    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    // @Override
    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* itemId);

    // @Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

protected:
    /**
     * Finds all groups of adjacent items in the cursor and calls {@link #addGroup} for
     * each of them.
     */
    virtual CARAPI_(void) AddGroups(
        /* [in] */ ICursor* cursor) = 0;

    virtual CARAPI_(AutoPtr<IView>) NewStandAloneView(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* parent) = 0;

    virtual CARAPI_(void) BindStandAloneView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor) = 0;

    virtual CARAPI_(AutoPtr<IView>) NewGroupView(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* parent) = 0;

    virtual CARAPI_(void) BindGroupView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 groupSize,
        /* [in] */ Boolean expanded) = 0;

    virtual CARAPI_(AutoPtr<IView>) NewChildView(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* parent) = 0;

    virtual CARAPI_(void) BindChildView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor) = 0;

    CARAPI_(void) OnContentChanged();

    /**
     * Records information about grouping in the list.  Should be called by the overridden
     * {@link #addGroups} method.
     */
    CARAPI_(void) AddGroup(
        /* [in] */ Int32 cursorPosition,
        /* [in] */ Int32 size,
        /* [in] */ Boolean expanded);

private:
    /**
     * Cache should be reset whenever the cursor changes or groups are expanded or collapsed.
     */
    CARAPI_(void) ResetCache();

    /**
     * Scans over the entire cursor looking for duplicate phone numbers that need
     * to be collapsed.
     */
    CARAPI_(void) FindGroups();

    // Copy/paste from ArrayUtils
    CARAPI_(Int32) IdealInt64ArraySize(
        /* [in] */ Int32 need);

    // Copy/paste from ArrayUtils
    CARAPI_(Int32) IdealByteArraySize(
        /* [in] */ Int32 need);

protected:
    AutoPtr<IContentObserver> mChangeObserver;

    AutoPtr<IDataSetObserver> mDataSetObserver;

private:
    static const Int32 GROUP_METADATA_ARRAY_INITIAL_SIZE = 16;
    static const Int32 GROUP_METADATA_ARRAY_INCREMENT = 128;
    static const Int64 GROUP_OFFSET_MASK    = 0x00000000FFFFFFFFL;
    static const Int64 GROUP_SIZE_MASK     = 0x7FFFFFFF00000000L;
    static const Int64 EXPANDED_GROUP_MASK = 0x8000000000000000L;

    AutoPtr<IContext> mContext;
    AutoPtr<ICursor> mCursor;

    /**
     * Count of list items.
     */
    Int32 mCount;

    Int32 mRowIdColumnIndex;

    /**
     * Count of groups in the list.
     */
    Int32 mGroupCount;

    /**
     * Information about where these groups are located in the list, how large they are
     * and whether they are expanded.
     */
    AutoPtr<ArrayOf<Int64> > mGroupMetadata;

    AutoPtr<ISparseInt32Array> mPositionCache;
    Int32 mLastCachedListPosition;
    Int32 mLastCachedCursorPosition;
    Int32 mLastCachedGroup;

    /**
     * A reusable temporary instance of PositionMetadata
     */
    AutoPtr<PositionMetadata> mPositionMetadata;
};

} // Widget
} // Common
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_COMMON_WIDGET_GROUPINGLISTADAPTER_H__
