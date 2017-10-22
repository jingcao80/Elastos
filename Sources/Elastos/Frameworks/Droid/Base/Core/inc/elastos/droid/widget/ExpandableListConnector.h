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

#ifndef __ELASTOS_DROID_WIDGET_EXPANDABLELISTCONNECTOR_H__
#define __ELASTOS_DROID_WIDGET_EXPANDABLELISTCONNECTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/widget/ExpandableListPosition.h"
#include "elastos/droid/database/DataSetObserver.h"

using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::ExpandableListPosition;
using Elastos::Droid::Widget::IExpandableListAdapter;
using Elastos::Droid::Widget::IExpandableListPosition;
using Elastos::Droid::Widget::IFilter;
using Elastos::Droid::Widget::IFilterable;
using Elastos::Core::IComparable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Widget {

class ExpandableListConnector
    : public BaseAdapter
    , public IExpandableListConnector
    , public IFilterable
{
public:
    /**
      * Metadata about an expanded group to help convert from a flat list
      * position to either a) group position for groups, or b) child position for
      * children
      */
    class GroupMetadata
        : public Object
        , public IGroupMetadata
        , public IParcelable
        , public IComparable
    {
    public:
        CAR_INTERFACE_DECL()

        static CARAPI_(AutoPtr<GroupMetadata>) Obtain(
            /* [in] */ Int32 flPos,
            /* [in] */ Int32 lastChildFlPos,
            /* [in] */ Int32 gPos,
            /* [in] */ Int64 gId);

        virtual CARAPI CompareTo(
            /* [in] */ IInterface* another,
            /* [out] */ Int32* result);

        virtual CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        // overwrite CAR func for compile
        virtual CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

    private:
        GroupMetadata();

    public:
        static const Int32 REFRESH = -1;
        Int32 mFlPos;
        Int32 mLastChildFlPos;
        Int32 mGPos;
        Int64 mGId;
    };

    /**
      * Data type that contains an expandable list position (can refer to either a group
      * or child) and some extra information regarding referred item (such as
      * where to insert into the flat list, etc.)
      */
    class PositionMetadata
        : public Object
        , public IPositionMetadata
    {
    public:
        CAR_INTERFACE_DECL()

        static CARAPI_(AutoPtr<PositionMetadata>) Obtain(
            /* [in] */ Int32 flatListPos,
            /* [in] */ Int32 type,
            /* [in] */ Int32 groupPos,
            /* [in] */ Int32 childPos,
            /* [in] */ GroupMetadata* groupMetadata,
            /* [in] */ Int32 groupInsertIndex);

        virtual CARAPI Recycle();

        /**
          * Checks whether the group referred to in this object is expanded,
          * or not (at the time this object was created)
          *
          * @return whether the group at groupPos is expanded or not
          */
        virtual CARAPI IsExpanded(
            /* [out] */ Boolean* result);

    private:
        /**
          * Use {@link #obtain(int, int, int, int, GroupMetadata, int)}
          */
        PositionMetadata();

        CARAPI ResetState();

        static CARAPI_(AutoPtr<PositionMetadata>) GetRecycledOrCreate();

        static CARAPI_(AutoPtr<IArrayList>) InitSPool();

    public:
        AutoPtr<ExpandableListPosition> mPosition;
        AutoPtr<GroupMetadata> mGroupMetadata;
        Int32 mGroupInsertIndex;

    private:
        static const Int32 MAX_POOL_SIZE = 5;
        static AutoPtr<IArrayList> sPool;
        static Object sPoolLock;
    };

protected:
    class MyDataSetObserver
        : public DataSetObserver
    {
    public:
        MyDataSetObserver(
            /* [in] */ ExpandableListConnector* owner);

        // @Override
        CARAPI OnChanged();

        // @Override
        CARAPI OnInvalidated();

        TO_STRING_IMPL("ExpandableListConnector::MyDataSetObserver")

    private:
        ExpandableListConnector* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    ExpandableListConnector();

    /**
      * Constructs the connector
      */
    CARAPI constructor(
        /* [in] */ IExpandableListAdapter* expandableListAdapter);

    /**
      * Point to the {@link ExpandableListAdapter} that will give us data/Views
      *
      * @param expandableListAdapter the adapter that supplies us with data/Views
      */
    virtual CARAPI SetExpandableListAdapter(
        /* [in] */ IExpandableListAdapter* expandableListAdapter);

    /**
      * Translates a flat list position to either a) group pos if the specified
      * flat list position corresponds to a group, or b) child pos if it
      * corresponds to a child.  Performs a binary search on the expanded
      * groups list to find the flat list pos if it is an exp group, otherwise
      * finds where the flat list pos fits in between the exp groups.
      *
      * @param flPos the flat list position to be translated
      * @return the group position or child position of the specified flat list
      *         position encompassed in a {@link PositionMetadata} object
      *         that contains additional useful info for insertion, etc.
      */
    virtual CARAPI GetUnflattenedPos(
        /* [in] */ Int32 flPos,
        /* [out] */ IPositionMetadata** result);

    /**
      * Translates either a group pos or a child pos (+ group it belongs to) to a
      * flat list position.  If searching for a child and its group is not expanded, this will
      * return null since the child isn't being shown in the ListView, and hence it has no
      * position.
      *
      * @param pos a {@link ExpandableListPosition} representing either a group position
      *        or child position
      * @return the flat list position encompassed in a {@link PositionMetadata}
      *         object that contains additional useful info for insertion, etc., or null.
      */
    virtual CARAPI GetFlattenedPos(
        /* [in] */ IExpandableListPosition* pos,
        /* [out] */ IPositionMetadata** result);

    // @Override
    CARAPI AreAllItemsEnabled(
        /* [out] */ Boolean* enabled);

    // @Override
    CARAPI IsEnabled(
        /* [in] */ Int32 flatListPos,
        /* [out] */ Boolean* enabled);

    virtual CARAPI GetCount(
        /* [out] */ Int32* result);

    virtual CARAPI GetItem(
        /* [in] */ Int32 flatListPos,
        /* [out] */ IInterface** result);

    virtual CARAPI GetItemId(
        /* [in] */ Int32 flatListPos,
        /* [out] */ Int64* result);

    virtual CARAPI GetView(
        /* [in] */ Int32 flatListPos,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** result);

    // @Override
    CARAPI GetItemViewType(
        /* [in] */ Int32 flatListPos,
        /* [out] */ Int32* type);

    // @Override
    CARAPI GetViewTypeCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI HasStableIds(
        /* [out] */ Boolean* has);

    /**
      * Collapse a group in the grouped list view
      *
      * @param groupPos position of the group to collapse
      */
    virtual CARAPI CollapseGroup(
        /* [in] */ Int32 groupPos,
        /* [out] */ Boolean* result);

    virtual CARAPI CollapseGroup(
        /* [in] */ IPositionMetadata* posMetadata,
        /* [out] */ Boolean* result);

    /**
      * Expand a group in the grouped list view
      * @param groupPos the group to be expanded
      */
    virtual CARAPI ExpandGroup(
        /* [in] */ Int32 groupPos,
        /* [out] */ Boolean* result);

    virtual CARAPI ExpandGroup(
        /* [in] */ IPositionMetadata* posMetadata,
        /* [out] */ Boolean* result);

    /**
      * Whether the given group is currently expanded.
      * @param groupPosition The group to check.
      * @return Whether the group is currently expanded.
      */
    virtual CARAPI IsGroupExpanded(
        /* [in] */ Int32 groupPosition,
        /* [out] */ Boolean* result);

    /**
      * Set the maximum number of groups that can be expanded at any given time
      */
    virtual CARAPI SetMaxExpGroupCount(
        /* [in] */ Int32 maxExpGroupCount);

    virtual CARAPI GetAdapter(
        /* [out] */ IExpandableListAdapter** result);

    virtual CARAPI GetFilter(
        /* [out] */ IFilter** result);

    virtual CARAPI GetExpandedGroupMetadataList(
        /* [out] */ IList** result);

    virtual CARAPI SetExpandedGroupMetadataList(
        /* [in] */ IList* expandedGroupMetadataList);

    // @Override
    CARAPI IsEmpty(
        /* [out] */ Boolean* empty);

    /**
      * Searches the expandable list adapter for a group position matching the
      * given group ID. The search starts at the given seed position and then
      * alternates between moving up and moving down until 1) we find the right
      * position, or 2) we run out of time, or 3) we have looked at every
      * position
      *
      * @return Position of the row that matches the given row ID, or
      *         {@link AdapterView#INVALID_POSITION} if it can't be found
      * @see AdapterView#findSyncPosition()
      */
    virtual CARAPI FindGroupPosition(
        /* [in] */ Int64 groupIdToMatch,
        /* [in] */ Int32 seedGroupPosition,
        /* [out] */ Int32* result);

private:
    /**
      * Traverses the expanded group metadata list and fills in the flat list
      * positions.
      *
      * @param forceChildrenCountRefresh Forces refreshing of the children count
      *        for all expanded groups.
      * @param syncGroupPositions Whether to search for the group positions
      *         based on the group IDs. This should only be needed when calling
      *         this from an onChanged callback.
      */
    // @SuppressWarnings("unchecked")
    CARAPI RefreshExpGroupMetadataList(
        /* [in] */ Boolean forceChildrenCountRefresh,
        /* [in] */ Boolean syncGroupPositions);

private:
    AutoPtr<IExpandableListAdapter> mExpandableListAdapter;
    AutoPtr<IList> mExpGroupMetadataList;
    Int32 mTotalExpChildrenCount;
    Int32 mMaxExpGroupCount;
    /*const*/ AutoPtr<IDataSetObserver> mDataSetObserver;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::Widget::ExpandableListConnector::GroupMetadata*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif // __ELASTOS_DROID_WIDGET_EXPANDABLELISTCONNECTOR_H__

