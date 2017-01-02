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

#ifndef __ELASTOS_DROID_WIDGET_SIMPLEEXPANDABLELISTADAPTER_H__
#define __ELASTOS_DROID_WIDGET_SIMPLEEXPANDABLELISTADAPTER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.View.h"
#include "elastos/droid/widget/BaseExpandableListAdapter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * An easy adapter to map static data to group and child views defined in an XML
 * file. You can separately specify the data backing the group as a List of
 * Maps. Each entry in the ArrayList corresponds to one group in the expandable
 * list. The Maps contain the data for each row. You also specify an XML file
 * that defines the views used to display a group, and a mapping from keys in
 * the Map to specific views. This process is similar for a child, except it is
 * one-level deeper so the data backing is specified as a List<List<Map>>,
 * where the first List corresponds to the group of the child, the second List
 * corresponds to the position of the child within the group, and finally the
 * Map holds the data for that particular child.
 */
class SimpleExpandableListAdapter
    : public BaseExpandableListAdapter
    , public ISimpleExpandableListAdapter
{
public:
    CAR_INTERFACE_DECL()

    SimpleExpandableListAdapter();

    /**
     * Constructor
     *
     * @param context The context where the {@link ExpandableListView}
     *            associated with this {@link SimpleExpandableListAdapter} is
     *            running
     * @param groupData A List of Maps. Each entry in the List corresponds to
     *            one group in the list. The Maps contain the data for each
     *            group, and should include all the entries specified in
     *            "groupFrom"
     * @param groupFrom A list of keys that will be fetched from the Map
     *            associated with each group.
     * @param groupTo The group views that should display column in the
     *            "groupFrom" parameter. These should all be TextViews. The
     *            first N views in this list are given the values of the first N
     *            columns in the groupFrom parameter.
     * @param groupLayout resource identifier of a view layout that defines the
     *            views for a group. The layout file should include at least
     *            those named views defined in "groupTo"
     * @param childData A List of List of Maps. Each entry in the outer List
     *            corresponds to a group (index by group position), each entry
     *            in the inner List corresponds to a child within the group
     *            (index by child position), and the Map corresponds to the data
     *            for a child (index by values in the childFrom array). The Map
     *            contains the data for each child, and should include all the
     *            entries specified in "childFrom"
     * @param childFrom A list of keys that will be fetched from the Map
     *            associated with each child.
     * @param childTo The child views that should display column in the
     *            "childFrom" parameter. These should all be TextViews. The
     *            first N views in this list are given the values of the first N
     *            columns in the childFrom parameter.
     * @param childLayout resource identifier of a view layout that defines the
     *            views for a child. The layout file should include at least
     *            those named views defined in "childTo"
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IList* groupData,
        /* [in] */ Int32 groupLayout,
        /* [in] */ ArrayOf<String>* groupFrom,
        /* [in] */ ArrayOf<Int32>* groupTo,
        /* [in] */ IList* childData,
        /* [in] */ Int32 childLayout,
        /* [in] */ ArrayOf<String>* childFrom,
        /* [in] */ ArrayOf<Int32>* childTo);

    /**
     * Constructor
     *
     * @param context The context where the {@link ExpandableListView}
     *            associated with this {@link SimpleExpandableListAdapter} is
     *            running
     * @param groupData A List of Maps. Each entry in the List corresponds to
     *            one group in the list. The Maps contain the data for each
     *            group, and should include all the entries specified in
     *            "groupFrom"
     * @param groupFrom A list of keys that will be fetched from the Map
     *            associated with each group.
     * @param groupTo The group views that should display column in the
     *            "groupFrom" parameter. These should all be TextViews. The
     *            first N views in this list are given the values of the first N
     *            columns in the groupFrom parameter.
     * @param expandedGroupLayout resource identifier of a view layout that
     *            defines the views for an expanded group. The layout file
     *            should include at least those named views defined in "groupTo"
     * @param collapsedGroupLayout resource identifier of a view layout that
     *            defines the views for a collapsed group. The layout file
     *            should include at least those named views defined in "groupTo"
     * @param childData A List of List of Maps. Each entry in the outer List
     *            corresponds to a group (index by group position), each entry
     *            in the inner List corresponds to a child within the group
     *            (index by child position), and the Map corresponds to the data
     *            for a child (index by values in the childFrom array). The Map
     *            contains the data for each child, and should include all the
     *            entries specified in "childFrom"
     * @param childFrom A list of keys that will be fetched from the Map
     *            associated with each child.
     * @param childTo The child views that should display column in the
     *            "childFrom" parameter. These should all be TextViews. The
     *            first N views in this list are given the values of the first N
     *            columns in the childFrom parameter.
     * @param childLayout resource identifier of a view layout that defines the
     *            views for a child. The layout file should include at least
     *            those named views defined in "childTo"
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IList* groupData,
        /* [in] */ Int32 expandedGroupLayout,
        /* [in] */ Int32 collapsedGroupLayout,
        /* [in] */ ArrayOf<String>* groupFrom,
        /* [in] */ ArrayOf<Int32>* groupTo,
        /* [in] */ IList* childData,
        /* [in] */ Int32 childLayout,
        /* [in] */ ArrayOf<String>* childFrom,
        /* [in] */ ArrayOf<Int32>* childTo);

    /**
     * Constructor
     *
     * @param context The context where the {@link ExpandableListView}
     *            associated with this {@link SimpleExpandableListAdapter} is
     *            running
     * @param groupData A List of Maps. Each entry in the List corresponds to
     *            one group in the list. The Maps contain the data for each
     *            group, and should include all the entries specified in
     *            "groupFrom"
     * @param groupFrom A list of keys that will be fetched from the Map
     *            associated with each group.
     * @param groupTo The group views that should display column in the
     *            "groupFrom" parameter. These should all be TextViews. The
     *            first N views in this list are given the values of the first N
     *            columns in the groupFrom parameter.
     * @param expandedGroupLayout resource identifier of a view layout that
     *            defines the views for an expanded group. The layout file
     *            should include at least those named views defined in "groupTo"
     * @param collapsedGroupLayout resource identifier of a view layout that
     *            defines the views for a collapsed group. The layout file
     *            should include at least those named views defined in "groupTo"
     * @param childData A List of List of Maps. Each entry in the outer List
     *            corresponds to a group (index by group position), each entry
     *            in the inner List corresponds to a child within the group
     *            (index by child position), and the Map corresponds to the data
     *            for a child (index by values in the childFrom array). The Map
     *            contains the data for each child, and should include all the
     *            entries specified in "childFrom"
     * @param childFrom A list of keys that will be fetched from the Map
     *            associated with each child.
     * @param childTo The child views that should display column in the
     *            "childFrom" parameter. These should all be TextViews. The
     *            first N views in this list are given the values of the first N
     *            columns in the childFrom parameter.
     * @param childLayout resource identifier of a view layout that defines the
     *            views for a child (unless it is the last child within a group,
     *            in which case the lastChildLayout is used). The layout file
     *            should include at least those named views defined in "childTo"
     * @param lastChildLayout resource identifier of a view layout that defines
     *            the views for the last child within each group. The layout
     *            file should include at least those named views defined in
     *            "childTo"
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IList* groupData,
        /* [in] */ Int32 expandedGroupLayout,
        /* [in] */ Int32 collapsedGroupLayout,
        /* [in] */ ArrayOf<String>* groupFrom,
        /* [in] */ ArrayOf<Int32>* groupTo,
        /* [in] */ IList* childData,
        /* [in] */ Int32 childLayout,
        /* [in] */ Int32 lastChildLayout,
        /* [in] */ ArrayOf<String>* childFrom,
        /* [in] */ ArrayOf<Int32>* childTo);

    CARAPI GetChild(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ IInterface** child);

    CARAPI GetChildId(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ Int64* id);

    CARAPI GetChildView(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [in] */ Boolean isLastChild,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    /**
     * Instantiates a new View for a child.
     * @param isLastChild Whether the child is the last child within its group.
     * @param parent The eventual parent of this new View.
     * @return A new child View
     */
    CARAPI NewChildView(
        /* [in] */ Boolean isLastChild,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    CARAPI GetChildrenCount(
        /* [in] */ Int32 groupPosition,
        /* [out] */ Int32* count);

    CARAPI GetGroup(
        /* [in] */ Int32 groupPosition,
        /* [out] */ IInterface** child);

    CARAPI GetGroupCount(
        /* [out] */ Int32* count);

    CARAPI GetGroupId(
        /* [in] */ Int32 groupPosition,
        /* [out] */ Int64* id);

    CARAPI GetGroupView(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Boolean isExpanded,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    /**
     * Instantiates a new View for a group.
     * @param isExpanded Whether the group is currently expanded.
     * @param parent The eventual parent of this new View.
     * @return A new group View
     */
    CARAPI NewGroupView(
        /* [in] */ Boolean isExpanded,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    CARAPI IsChildSelectable(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition,
        /* [out] */ Boolean* result);

    CARAPI HasStableIds(
        /* [out] */ Boolean* has);

private:
    CARAPI BindView(
        /* [in] */ IView* view,
        /* [in] */ IMap* data,
        /* [in] */ ArrayOf<String>* from,
        /* [in] */ ArrayOf<Int32>* to);

private:
    AutoPtr<IList> mGroupData; // List<? extends Map<String, ?>>
    Int32 mExpandedGroupLayout;
    Int32 mCollapsedGroupLayout;
    AutoPtr<ArrayOf<String> > mGroupFrom;
    AutoPtr<ArrayOf<Int32> > mGroupTo;

    AutoPtr<IList> mChildData; // List<? extends List<? extends Map<String, ?>>>
    Int32 mChildLayout;
    Int32 mLastChildLayout;
    AutoPtr<ArrayOf<String> > mChildFrom;
    AutoPtr<ArrayOf<Int32> > mChildTo;

    AutoPtr<ILayoutInflater> mInflater;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_SIMPLEEXPANDABLELISTADAPTER_H__
