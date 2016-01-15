#ifndef __ELASTOS_DROID_WIDGET_EXPANDABLELISTPOSITION_H__
#define __ELASTOS_DROID_WIDGET_EXPANDABLELISTPOSITION_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Widget::IExpandableListPosition;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
  * ExpandableListPosition can refer to either a group's position or a child's
  * position. Referring to a child's position requires both a group position (the
  * group containing the child) and a child position (the child's position within
  * that group). To create objects, use {@link #obtainChildPosition(int, int)} or
  * {@link #obtainGroupPosition(int)}.
  */
class ExpandableListPosition
    : public Object
    , public IExpandableListPosition
{
public:
    CAR_INTERFACE_DECL()

    ExpandableListPosition();

    CARAPI constructor();

    virtual CARAPI GetPackedPosition(
        /* [out] */ Int64* result);

    static CARAPI_(AutoPtr<ExpandableListPosition>) ObtainGroupPosition(
        /* [in] */ Int32 groupPosition);

    static CARAPI_(AutoPtr<ExpandableListPosition>) ObtainChildPosition(
        /* [in] */ Int32 groupPosition,
        /* [in] */ Int32 childPosition);

    static CARAPI_(AutoPtr<ExpandableListPosition>) ObtainPosition(
        /* [in] */ Int64 packedPosition);

    static CARAPI_(AutoPtr<ExpandableListPosition>) Obtain(
        /* [in] */ Int32 type,
        /* [in] */ Int32 groupPos,
        /* [in] */ Int32 childPos,
        /* [in] */ Int32 flatListPos);

    /**
      * Do not call this unless you obtained this via ExpandableListPosition.obtain().
      * PositionMetadata will handle recycling its own children.
      */
    virtual CARAPI Recycle();

private:
    CARAPI ResetState();

    static CARAPI_(AutoPtr<ExpandableListPosition>) GetRecycledOrCreate();

    static CARAPI_(AutoPtr<IArrayList>) InitSPool();

public:
    Int32 mGroupPos;
    Int32 mChildPos;
    Int32 mFlatListPos;
    Int32 mType;

private:
    static const Int32 MAX_POOL_SIZE = 5;
    static AutoPtr<IArrayList> sPool;
    static Object sLock;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_EXPANDABLELISTPOSITION_H__

