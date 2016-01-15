
#include "elastos/droid/widget/CExpandableListPositionHelper.h"
#include "elastos/droid/widget/ExpandableListPosition.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_SINGLETON_IMPL(CExpandableListPositionHelper)

CAR_INTERFACE_IMPL(CExpandableListPositionHelper, Singleton, IExpandableListPositionHelper)

ECode CExpandableListPositionHelper::ObtainGroupPosition(
    /* [in] */ Int32 groupPosition,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListPosition::ObtainGroupPosition(groupPosition);
    REFCOUNT_ADD(*position);
    return NOERROR;
}

ECode CExpandableListPositionHelper::ObtainChildPosition(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListPosition::ObtainChildPosition(groupPosition, childPosition);
    REFCOUNT_ADD(*position);
    return NOERROR;
}

ECode CExpandableListPositionHelper::ObtainPosition(
    /* [in] */ Int64 packedPosition,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListPosition::ObtainPosition(packedPosition);
    REFCOUNT_ADD(*position);
    return NOERROR;
}

ECode CExpandableListPositionHelper::Obtain(
    /* [in] */ Int32 type,
    /* [in] */ Int32 groupPos,
    /* [in] */ Int32 childPos,
    /* [in] */ Int32 flatListPos,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    *position = ExpandableListPosition::Obtain(type, groupPos, childPos, flatListPos);
    REFCOUNT_ADD(*position);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
