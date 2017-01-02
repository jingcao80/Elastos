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

#ifndef __ELASTOS_DROID_WIDGET_SIMPLECURSORTREEADAPTER_H__
#define __ELASTOS_DROID_WIDGET_SIMPLECURSORTREEADAPTER_H__

#include "elastos/droid/widget/ResourceCursorTreeAdapter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * An easy adapter to map columns from a cursor to TextViews or ImageViews
 * defined in an XML file. You can specify which columns you want, which views
 * you want to display the columns, and the XML file that defines the appearance
 * of these views. Separate XML files for child and groups are possible.
 *
 * Binding occurs in two phases. First, if a
 * {@link android.widget.SimpleCursorTreeAdapter.ViewBinder} is available,
 * {@link ViewBinder#setViewValue(android.view.View, android.database.Cursor, int)}
 * is invoked. If the returned value is true, binding has occurred. If the
 * returned value is false and the view to bind is a TextView,
 * {@link #setViewText(TextView, String)} is invoked. If the returned value
 * is false and the view to bind is an ImageView,
 * {@link #setViewImage(ImageView, String)} is invoked. If no appropriate
 * binding can be found, an {@link IllegalStateException} is thrown.
 */
class SimpleCursorTreeAdapter
    : public ResourceCursorTreeAdapter
    , public ISimpleCursorTreeAdapter
{
public:
    CAR_INTERFACE_DECL()

    SimpleCursorTreeAdapter();

    /**
     * Constructor.
     *
     * @param context The context where the {@link ExpandableListView}
     *            associated with this {@link SimpleCursorTreeAdapter} is
     *            running
     * @param cursor The database cursor
     * @param collapsedGroupLayout The resource identifier of a layout file that
     *            defines the views for a collapsed group. The layout file
     *            should include at least those named views defined in groupTo.
     * @param expandedGroupLayout The resource identifier of a layout file that
     *            defines the views for an expanded group. The layout file
     *            should include at least those named views defined in groupTo.
     * @param groupFrom A list of column names that will be used to display the
     *            data for a group.
     * @param groupTo The group views (from the group layouts) that should
     *            display column in the "from" parameter. These should all be
     *            TextViews or ImageViews. The first N views in this list are
     *            given the values of the first N columns in the from parameter.
     * @param childLayout The resource identifier of a layout file that defines
     *            the views for a child (except the last). The layout file
     *            should include at least those named views defined in childTo.
     * @param lastChildLayout The resource identifier of a layout file that
     *            defines the views for the last child within a group. The
     *            layout file should include at least those named views defined
     *            in childTo.
     * @param childFrom A list of column names that will be used to display the
     *            data for a child.
     * @param childTo The child views (from the child layouts) that should
     *            display column in the "from" parameter. These should all be
     *            TextViews or ImageViews. The first N views in this list are
     *            given the values of the first N columns in the from parameter.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 collapsedGroupLayout,
        /* [in] */ Int32 expandedGroupLayout,
        /* [in] */ ArrayOf<String>* groupFrom,
        /* [in] */ ArrayOf<Int32>* groupTo,
        /* [in] */ Int32 childLayout,
        /* [in] */ Int32 lastChildLayout,
        /* [in] */ ArrayOf<String>* childFrom,
        /* [in] */ ArrayOf<Int32>* childTo);

    /**
     * Constructor.
     *
     * @param context The context where the {@link ExpandableListView}
     *            associated with this {@link SimpleCursorTreeAdapter} is
     *            running
     * @param cursor The database cursor
     * @param collapsedGroupLayout The resource identifier of a layout file that
     *            defines the views for a collapsed group. The layout file
     *            should include at least those named views defined in groupTo.
     * @param expandedGroupLayout The resource identifier of a layout file that
     *            defines the views for an expanded group. The layout file
     *            should include at least those named views defined in groupTo.
     * @param groupFrom A list of column names that will be used to display the
     *            data for a group.
     * @param groupTo The group views (from the group layouts) that should
     *            display column in the "from" parameter. These should all be
     *            TextViews or ImageViews. The first N views in this list are
     *            given the values of the first N columns in the from parameter.
     * @param childLayout The resource identifier of a layout file that defines
     *            the views for a child. The layout file
     *            should include at least those named views defined in childTo.
     * @param childFrom A list of column names that will be used to display the
     *            data for a child.
     * @param childTo The child views (from the child layouts) that should
     *            display column in the "from" parameter. These should all be
     *            TextViews or ImageViews. The first N views in this list are
     *            given the values of the first N columns in the from parameter.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 collapsedGroupLayout,
        /* [in] */ Int32 expandedGroupLayout,
        /* [in] */ ArrayOf<String>* groupFrom,
        /* [in] */ ArrayOf<Int32>* groupTo,
        /* [in] */ Int32 childLayout,
        /* [in] */ ArrayOf<String>* childFrom,
        /* [in] */ ArrayOf<Int32>* childTo);

    /**
     * Constructor.
     *
     * @param context The context where the {@link ExpandableListView}
     *            associated with this {@link SimpleCursorTreeAdapter} is
     *            running
     * @param cursor The database cursor
     * @param groupLayout The resource identifier of a layout file that defines
     *            the views for a group. The layout file should include at least
     *            those named views defined in groupTo.
     * @param groupFrom A list of column names that will be used to display the
     *            data for a group.
     * @param groupTo The group views (from the group layouts) that should
     *            display column in the "from" parameter. These should all be
     *            TextViews or ImageViews. The first N views in this list are
     *            given the values of the first N columns in the from parameter.
     * @param childLayout The resource identifier of a layout file that defines
     *            the views for a child. The layout file should include at least
     *            those named views defined in childTo.
     * @param childFrom A list of column names that will be used to display the
     *            data for a child.
     * @param childTo The child views (from the child layouts) that should
     *            display column in the "from" parameter. These should all be
     *            TextViews or ImageViews. The first N views in this list are
     *            given the values of the first N columns in the from parameter.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 groupLayout,
        /* [in] */ ArrayOf<String>* groupFrom,
        /* [in] */ ArrayOf<Int32>* groupTo,
        /* [in] */ Int32 childLayout,
        /* [in] */ ArrayOf<String>* childFrom,
        /* [in] */ ArrayOf<Int32>* childTo);

    virtual CARAPI GetViewBinder(
        /* [out] */ ISimpleCursorTreeAdapterViewBinder** viewBinder);

    /**
     * Sets the binder used to bind data to views.
     *
     * @param viewBinder the binder used to bind data to views, can be null to
     *        remove the existing binder
     *
     * @see #getViewBinder()
     */
    virtual CARAPI SetViewBinder(
        /* [in] */ ISimpleCursorTreeAdapterViewBinder* viewBinder);
    /**
     * Called by bindView() to set the text for a TextView but only if
     * there is no existing ViewBinder or if the existing ViewBinder cannot
     * handle binding to a TextView.
     *
     * Intended to be overridden by Adapters that need to filter strings
     * retrieved from the database.
     *
     * @param v TextView to receive text
     * @param text the text to be set for the TextView
     */
    virtual CARAPI SetViewText(
        /* [in] */ ITextView* v,
        /* [in] */ const String& text);

protected:
    virtual CARAPI BindChildView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean isLastChild);

    virtual CARAPI BindGroupView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean isExpanded);

    /**
     * Called by bindView() to set the image for an ImageView. By default, the
     * value will be treated as a Uri. Intended to be overridden by Adapters
     * that need to filter strings retrieved from the database.
     *
     * @param v ImageView to receive an image
     * @param value the value retrieved from the cursor
     */
    virtual CARAPI SetViewImage(
        /* [in] */ IImageView* v,
        /* [in] */ const String& value);

private:
    CARAPI Init(
        /* [in] */ ArrayOf<String>* groupFromNames,
        /* [in] */ ArrayOf<Int32>* groupTo,
        /* [in] */ ArrayOf<String>* childFromNames,
        /* [in] */ ArrayOf<Int32>* childTo);

    CARAPI BindView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ ArrayOf<Int32>* from,
        /* [in] */ ArrayOf<Int32>* to);

    CARAPI InitFromColumns(
        /* [in] */ ICursor* cursor,
        /* [in] */ ArrayOf<String>* fromColumnNames,
        /* [in] */ ArrayOf<Int32>* fromColumns);

private:
    /** The name of the columns that contain the data to display for a group. */
     AutoPtr<ArrayOf<String> > mGroupFromNames;

    /** The indices of columns that contain data to display for a group. */
     AutoPtr<ArrayOf<Int32> > mGroupFrom;
    /**
     * The View IDs that will display a group's data fetched from the
     * corresponding column.
     */
     AutoPtr<ArrayOf<Int32> > mGroupTo;

    /** The name of the columns that contain the data to display for a child. */
     AutoPtr<ArrayOf<String> > mChildFromNames;

    /** The indices of columns that contain data to display for a child. */
     AutoPtr<ArrayOf<Int32> > mChildFrom;
    /**
     * The View IDs that will display a child's data fetched from the
     * corresponding column.
     */
     AutoPtr<ArrayOf<Int32> > mChildTo;

    /**
     * View binder, if supplied
     */
     AutoPtr<ISimpleCursorTreeAdapterViewBinder> mViewBinder;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_SIMPLECURSORTREEADAPTER_H__
