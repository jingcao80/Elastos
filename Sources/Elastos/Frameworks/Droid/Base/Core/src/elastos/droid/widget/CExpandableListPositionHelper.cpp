
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
    AutoPtr<IExpandableListPosition> temp = ExpandableListPosition::ObtainGroupPosition(groupPosition);
    *position = temp;
    REFCOUNT_ADD(*position);
    return NOERROR;
}

ECode CExpandableListPositionHelper::ObtainChildPosition(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    AutoPtr<IExpandableListPosition> temp = ExpandableListPosition::ObtainChildPosition(groupPosition, childPosition);
    *position = temp;
    REFCOUNT_ADD(*position);
    return NOERROR;
}

ECode CExpandableListPositionHelper::ObtainPosition(
    /* [in] */ Int64 packedPosition,
    /* [out] */ IExpandableListPosition** position)
{
    VALIDATE_NOT_NULL(position);
    AutoPtr<IExpandableListPosition> temp = ExpandableListPosition::ObtainPosition(packedPosition);
    *position = temp;
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
    AutoPtr<IExpandableListPosition> temp = ExpandableListPosition::Obtain(type, groupPos, childPos, flatListPos);
    *position = temp;
    REFCOUNT_ADD(*position);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
