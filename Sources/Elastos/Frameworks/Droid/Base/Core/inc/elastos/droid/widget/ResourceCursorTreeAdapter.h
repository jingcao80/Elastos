
#ifndef __ELASTOS_DROID_WIDGET_RESOURCECURSORTREEADAPTER_H__
#define __ELASTOS_DROID_WIDGET_RESOURCECURSORTREEADAPTER_H__

#include "elastos/droid/widget/CursorTreeAdapter.h"

using Elastos::Droid::View::ILayoutInflater;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * A fairly simple ExpandableListAdapter that creates views defined in an XML
 * file. You can specify the XML file that defines the appearance of the views.
 */
class ResourceCursorTreeAdapter
    : public CursorTreeAdapter
    , public IResourceCursorTreeAdapter
{
public:
    CAR_INTERFACE_DECL()

    ResourceCursorTreeAdapter();

    /**
     * Constructor.
     *
     * @param context The context where the ListView associated with this
     *            SimpleListItemFactory is running
     * @param cursor The database cursor
     * @param collapsedGroupLayout resource identifier of a layout file that
     *            defines the views for collapsed groups.
     * @param expandedGroupLayout resource identifier of a layout file that
     *            defines the views for expanded groups.
     * @param childLayout resource identifier of a layout file that defines the
     *            views for all children but the last..
     * @param lastChildLayout resource identifier of a layout file that defines
     *            the views for the last child of a group.
     */
     CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 collapsedGroupLayout,
        /* [in] */ Int32 expandedGroupLayout,
        /* [in] */ Int32 childLayout,
        /* [in] */ Int32 lastChildLayout);

    /**
     * Constructor.
     *
     * @param context The context where the ListView associated with this
     *            SimpleListItemFactory is running
     * @param cursor The database cursor
     * @param collapsedGroupLayout resource identifier of a layout file that
     *            defines the views for collapsed groups.
     * @param expandedGroupLayout resource identifier of a layout file that
     *            defines the views for expanded groups.
     * @param childLayout resource identifier of a layout file that defines the
     *            views for all children.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 collapsedGroupLayout,
        /* [in] */ Int32 expandedGroupLayout,
        /* [in] */ Int32 childLayout);

    /**
     * Constructor.
     *
     * @param context The context where the ListView associated with this
     *            SimpleListItemFactory is running
     * @param cursor The database cursor
     * @param groupLayout resource identifier of a layout file that defines the
     *            views for all groups.
     * @param childLayout resource identifier of a layout file that defines the
     *            views for all children.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 groupLayout,
        /* [in] */ Int32 childLayout);

    virtual CARAPI NewChildView(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean isLastChild,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    virtual CARAPI NewGroupView(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Boolean isExpanded,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

private:
     Int32 mCollapsedGroupLayout;
     Int32 mExpandedGroupLayout;
     Int32 mChildLayout;
     Int32 mLastChildLayout;
     AutoPtr<ILayoutInflater> mInflater;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_RESOURCECURSORTREEADAPTER_H__
