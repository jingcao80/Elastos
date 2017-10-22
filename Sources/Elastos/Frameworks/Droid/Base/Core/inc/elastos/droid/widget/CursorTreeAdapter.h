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

#ifndef __ELASTOS_DROID_WIDGET_CURSORTREEADAPTER_H__
#define __ELASTOS_DROID_WIDGET_CURSORTREEADAPTER_H__

#include "elastos/droid/widget/BaseExpandableListAdapter.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/database/DataSetObserver.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::Database::IDataSetObserver;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * An adapter that exposes data from a series of {@link Cursor}s to an
 * {@link ExpandableListView} widget. The top-level {@link Cursor} (that is
 * given in the constructor) exposes the groups, while subsequent {@link Cursor}s
 * returned from {@link #getChildrenCursor(Cursor)} expose children within a
 * particular group. The Cursors must include a column named "_id" or this class
 * will not work.
 */
class CursorTreeAdapter
    : public BaseExpandableListAdapter
    , public ICursorTreeAdapter
    , public IFilterable
    , public ICursorFilterClient
{
public:
    CAR_INTERFACE_DECL()

    CursorTreeAdapter();

    /**
     * Constructor. The adapter will call {@link Cursor#requery()} on the cursor whenever
     * it changes so that the most recent data is always displayed.
     *
     * @param cursor The cursor from which to get the data for the groups.
     */
    CARAPI constructor(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContext* context);

    /**
     * Constructor.
     *
     * @param cursor The cursor from which to get the data for the groups.
     * @param context The context
     * @param autoRequery If TRUE the adapter will call {@link Cursor#requery()}
     *        on the cursor whenever it changes so the most recent data is
     *        always displayed.
     */
    CARAPI constructor(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContext* context,
        /* [in] */ Boolean autoRequery);

    /**
     * Sets the group Cursor.
     *
     * @param cursor The Cursor to set for the group. If there is an existing cursor
     * it will be closed.
     */
    virtual CARAPI SetGroupCursor(
        /* [in] */ ICursor* cursor);

    /**
     * Sets the children Cursor for a particular group. If there is an existing cursor
     * it will be closed.
     * <p>
     * This is useful when asynchronously querying to prevent blocking the UI.
     *
     * @param groupPosition The group whose children are being set via this Cursor.
     * @param childrenCursor The Cursor that contains the children of the group.
     */
    virtual CARAPI SetChildrenCursor(
        /* [in] */ Int32 groupPosition,
        /* [in] */ ICursor* childrenCursor);

    virtual CARAPI GetChild(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ IInterface** cursor);

    virtual CARAPI GetChildId(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ Int64* id);

    virtual CARAPI GetChildrenCount(
        /* [in] */ Int32 groupPosition,
        /* [out] */ Int32* count);

    virtual CARAPI GetGroup(
        /* [in] */ Int32 groupPosition,
        /* [out] */ IInterface** cursor);

    virtual CARAPI GetGroupCount(
        /* [out] */ Int32* count);

    virtual CARAPI GetGroupId(
        /* [in] */ Int32 groupPosition,
        /* [out] */ Int64* id);

    virtual CARAPI GetGroupView(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Boolean isExpanded,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI GetChildView(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [in] */ Boolean isLastChild,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI IsChildSelectable(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ Boolean* result);

    virtual CARAPI HasStableIds(
        /* [out] */ Boolean* result);

    virtual CARAPI NotifyDataSetChanged();

    /**
     * Notifies a data set change, but with the option of not releasing any
     * cached cursors.
     *
     * @param releaseCursors Whether to release and deactivate any cached
     *            cursors.
     */
    virtual CARAPI NotifyDataSetChanged(
        /* [in] */ Boolean releaseCursors);

    virtual CARAPI NotifyDataSetInvalidated();

    virtual CARAPI OnGroupCollapsed(
        /* [in] */ Int32 groupPosition) ;

    /**
     * Deactivates the Cursor and removes the helper from cache.
     *
     * @param groupPosition The group whose children Cursor and helper should be
     *            deactivated.
     */
    virtual CARAPI DeactivateChildrenCursorHelper(
        /* [in] */ Int32 groupPosition);

    /**
     * @see CursorAdapter#convertToString(Cursor)
     */
    virtual CARAPI ConvertToString(
        /* [in] */ ICursor* cursor,
        /* [out] */ ICharSequence** str);

    /**
     * @see CursorAdapter#runQueryOnBackgroundThread(CharSequence)
     */
    virtual CARAPI RunQueryOnBackgroundThread(
        /* [in] */ ICharSequence* constraint,
        /* [out] */ ICursor** cursor);

    virtual CARAPI GetFilter(
        /* [out] */ IFilter** filter);

    /**
     * @see CursorAdapter#getFilterQueryProvider()
     */
    virtual CARAPI GetFilterQueryProvider(
        /* [out] */ IFilterQueryProvider** provider);

    /**
     * @see CursorAdapter#setFilterQueryProvider(FilterQueryProvider)
     */
    virtual CARAPI SetFilterQueryProvider(
        /* [in] */ IFilterQueryProvider* filterQueryProvider);

    /**
     * @see CursorAdapter#changeCursor(Cursor)
     */
    virtual CARAPI ChangeCursor(
        /* [in] */ ICursor* cursor);

    /**
     * @see CursorAdapter#getCursor()
     */
    virtual CARAPI GetCursor(
        /* [out] */ ICursor** cursor);

protected:
    /**
     * Gets the Cursor for the children at the given group. Subclasses must
     * implement this method to return the children data for a particular group.
     * <p>
     * If you want to asynchronously query a provider to prevent blocking the
     * UI, it is possible to return NULL and at a later time call
     * {@link #setChildrenCursor(Int32, Cursor)}.
     * <p>
     * It is your responsibility to manage this Cursor through the Activity
     * lifecycle. It is a good idea to use {@link Activity#managedQuery} which
     * will handle this for you. In some situations, the adapter will deactivate
     * the Cursor on its own, but this will not always be the case, so please
     * ensure the Cursor is properly managed.
     *
     * @param groupCursor The cursor pointing to the group whose children cursor
     *            should be returned
     * @return The cursor for the children of a particular group, or NULL.
     */
    virtual CARAPI_(AutoPtr<ICursor>) GetChildrenCursor(
        /* [in] */ ICursor* groupCursor);

    /**
     * Makes a new group view to hold the group data pointed to by cursor.
     *
     * @param context Interface to application's global information
     * @param cursor The group cursor from which to get the data. The cursor is
     *            already moved to the correct position.
     * @param isExpanded Whether the group is expanded.
     * @param parent The parent to which the new view is attached to
     * @return The newly created view.
     */
    virtual CARAPI_(AutoPtr<IView>) NewGroupView(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean isExpanded,
        /* [in] */ IViewGroup* parent) = 0;

    /**
     * Bind an existing view to the group data pointed to by cursor.
     *
     * @param view Existing view, returned earlier by newGroupView.
     * @param context Interface to application's global information
     * @param cursor The cursor from which to get the data. The cursor is
     *            already moved to the correct position.
     * @param isExpanded Whether the group is expanded.
     */
    virtual CARAPI BindGroupView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean isExpanded) = 0;

    /**
     * Makes a new child view to hold the data pointed to by cursor.
     *
     * @param context Interface to application's global information
     * @param cursor The cursor from which to get the data. The cursor is
     *            already moved to the correct position.
     * @param isLastChild Whether the child is the last child within its group.
     * @param parent The parent to which the new view is attached to
     * @return the newly created view.
     */
    virtual CARAPI_(AutoPtr<IView>) NewChildView(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean isLastChild,
        /* [in] */ IViewGroup* parent);

    /**
     * Bind an existing view to the child data pointed to by cursor
     *
     * @param view Existing view, returned earlier by newChildView
     * @param context Interface to application's global information
     * @param cursor The cursor from which to get the data. The cursor is
     *            already moved to the correct position.
     * @param isLastChild Whether the child is the last child within its group.
     */
    virtual CARAPI BindChildView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean isLastChild) = 0;

    CARAPI Init(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContext* context,
        /* [in] */ Boolean autoRequery = TRUE);

private:
    CARAPI_(void) ReleaseCursorHelpers();

    /**
     * Helper class for Cursor management:
     * <li> Data validity
     * <li> Funneling the content and data set observers from a Cursor to a
     *      single data set observer for widgets
     * <li> ID from the Cursor for use in adapter IDs
     * <li> Swapping cursors but maintaining other metadata
     */

protected:
    class MyCursorHelper : public Object
    {
    public:
        MyCursorHelper(
            /* [in] */ ICursor* cursor,
            /* [in] */ CursorTreeAdapter* owner);

        CARAPI_(AutoPtr<ICursor>) GetCursor();

        CARAPI_(Int32) GetCount();

        CARAPI_(Int64) GetId(
            /* [in] */ Int32 position);

        CARAPI_(AutoPtr<ICursor>) MoveTo(
            /* [in] */ Int32 position);

        CARAPI_(void) ChangeCursor(
            /* [in] */ ICursor* cursor,
            /* [in] */ Boolean releaseCursors);

        CARAPI_(void) Deactivate();

        CARAPI_(Boolean) IsValid();

    private:
        class MyContentObserver : public ContentObserver
        {
        public:
            TO_STRING_IMPL("CursorTreeAdapter::MyCursorHelper::MyContentObserver")

            MyContentObserver(
                /* [in] */ MyCursorHelper* owner);

            CARAPI DeliverSelfNotifications(
                /* [out] */ Boolean* result);

            CARAPI OnChange(
                /* [in] */ Boolean selfChange);

        private:
            MyCursorHelper* mOwner;
        };

        class MyDataSetObserver
            : public DataSetObserver
        {
        public:
            TO_STRING_IMPL("CursorTreeAdapter::MyCursorHelper::MyDataSetObserver")

            MyDataSetObserver(
                /* [in] */ MyCursorHelper* owner);

            virtual CARAPI OnChanged();

            virtual CARAPI OnInvalidated();

        private:
            MyCursorHelper* mOwner;
        };

        AutoPtr<ICursor> mCursor;
        Boolean mDataValid;
        Int32 mRowIDColumn;
        AutoPtr<MyContentObserver> mContentObserver;
        AutoPtr<MyDataSetObserver> mDataSetObserver;

        CursorTreeAdapter* mOwner;
    };

    /**
     * Gets the cursor helper for the children in the given group.
     *
     * @param groupPosition The group whose children will be returned
     * @param requestCursor Whether to request a Cursor via
     *            {@link #getChildrenCursor(Cursor)} (TRUE), or to assume a call
     *            to {@link #setChildrenCursor(Int32, Cursor)} will happen shortly
     *            (FALSE).
     * @return The cursor helper for the children of the given group
     */
    CARAPI_(AutoPtr<MyCursorHelper>) GetChildrenCursorHelper(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Boolean requestCursor);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;
    Boolean mAutoRequery;

    /** The cursor helper that is used to get the groups */
    AutoPtr<MyCursorHelper> mGroupCursorHelper;

    /**
     * The map of a group position to the group's children cursor helper (the
     * cursor helper that is used to get the children for that group)
     */
    HashMap<Int32, AutoPtr<MyCursorHelper> > mChildrenCursorHelpers;

    // Filter related
    AutoPtr<ICursorFilter> mCursorFilter;
    AutoPtr<IFilterQueryProvider> mFilterQueryProvider;

    typedef HashMap<Int32, AutoPtr<MyCursorHelper> >::Iterator Iterator;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_CURSORTREEADAPTER_H__
