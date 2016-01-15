#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CExpandableListViewHelper.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(CExpandableListViewHelper, Singleton, IExpandableListViewHelper);

CAR_SINGLETON_IMPL(CExpandableListViewHelper);

ECode CExpandableListViewHelper::GetPackedPositionType(
    /* [in] */ Int64 packedPosition,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = ExpandableListView::GetPackedPositionType(packedPosition);
    return NOERROR;
}

ECode CExpandableListViewHelper::GetPackedPositionGroup(
    /* [in] */ Int64 packedPosition,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListView::GetPackedPositionGroup(packedPosition);
    return NOERROR;
}

ECode CExpandableListViewHelper::GetPackedPositionChild(
    /* [in] */ Int64 packedPosition,
    /* [out] */ Int32* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListView::GetPackedPositionChild(packedPosition);
    return NOERROR;
}

ECode CExpandableListViewHelper::GetPackedPositionForChild(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ Int64* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListView::GetPackedPositionForChild(groupPosition, childPosition);
    return NOERROR;
}

ECode CExpandableListViewHelper::GetPackedPositionForGroup(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Int64* position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListView::GetPackedPositionForGroup(groupPosition);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
