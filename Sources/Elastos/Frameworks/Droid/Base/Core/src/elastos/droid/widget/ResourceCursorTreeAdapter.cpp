#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/widget/ResourceCursorTreeAdapter.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(ResourceCursorTreeAdapter, CursorTreeAdapter, IResourceCursorTreeAdapter);
ResourceCursorTreeAdapter::ResourceCursorTreeAdapter()
    : mCollapsedGroupLayout(0)
    , mExpandedGroupLayout(0)
    , mChildLayout(0)
    , mLastChildLayout(0)
{}

ECode ResourceCursorTreeAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 collapsedGroupLayout,
    /* [in] */ Int32 expandedGroupLayout,
    /* [in] */ Int32 childLayout,
    /* [in] */ Int32 lastChildLayout)
{
    CursorTreeAdapter::constructor(cursor, context);
    mCollapsedGroupLayout = collapsedGroupLayout;
    mExpandedGroupLayout = expandedGroupLayout;
    mChildLayout = childLayout;
    mLastChildLayout = lastChildLayout;

    AutoPtr<IInterface> sTemp;
    context->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&sTemp);
    mInflater = ILayoutInflater::Probe(sTemp);
    return NOERROR;
}

ECode ResourceCursorTreeAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 collapsedGroupLayout,
    /* [in] */ Int32 expandedGroupLayout,
    /* [in] */ Int32 childLayout)
{
    return constructor(context, cursor, collapsedGroupLayout, expandedGroupLayout, childLayout, childLayout);
}

ECode ResourceCursorTreeAdapter::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 groupLayout,
    /* [in] */ Int32 childLayout)
{
    return constructor(context, cursor, groupLayout, groupLayout, childLayout, childLayout);
}

ECode ResourceCursorTreeAdapter::NewChildView(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean isLastChild,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return mInflater->Inflate((isLastChild) ? mLastChildLayout : mChildLayout, parent, FALSE, view);
}

ECode ResourceCursorTreeAdapter::NewGroupView(
    /* [in] */ IContext* context,
    /* [in] */ ICursor* cursor,
    /* [in] */ Boolean isExpanded,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    return mInflater->Inflate((isExpanded) ? mExpandedGroupLayout : mCollapsedGroupLayout, parent, FALSE, view);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
