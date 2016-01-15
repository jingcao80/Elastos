
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/widget/CExpandableListPosition.h"
#include "elastos/droid/widget/ExpandableListPosition.h"
#include "elastos/droid/widget/ExpandableListView.h"
#include "elastos/core/AutoLock.h"

using Elastos::Droid::Widget::CExpandableListPosition;
using Elastos::Droid::Widget::EIID_IExpandableListPosition;
using Elastos::Droid::Widget::ExpandableListView;
using Elastos::Core::AutoLock;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//                        ExpandableListPosition
//=====================================================================
const Int32 ExpandableListPosition::MAX_POOL_SIZE;
AutoPtr<IArrayList> ExpandableListPosition::sPool = ExpandableListPosition::InitSPool();
Object ExpandableListPosition::sLock;

CAR_INTERFACE_IMPL(ExpandableListPosition, Object, IExpandableListPosition)

ExpandableListPosition::ExpandableListPosition()
    : mGroupPos(0)
    , mChildPos(0)
    , mFlatListPos(0)
    , mType(0)
{
}

ECode ExpandableListPosition::constructor()
{
    return NOERROR;
}

ECode ExpandableListPosition::GetPackedPosition(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (type == CHILD) return ExpandableListView.getPackedPositionForChild(groupPos, childPos);
    // else return ExpandableListView.getPackedPositionForGroup(groupPos);

    if (mType == IExpandableListPosition::CHILD) {
        *result = ExpandableListView::GetPackedPositionForChild(mGroupPos, mChildPos);
    }
    else {
        *result = ExpandableListView::GetPackedPositionForGroup(mGroupPos);
    }
    return NOERROR;
}

AutoPtr<ExpandableListPosition> ExpandableListPosition::ObtainGroupPosition(
    /* [in] */ Int32 groupPosition)
{
    // ==================before translated======================
    // return obtain(GROUP, groupPosition, 0, 0);

    return Obtain(GROUP, groupPosition, 0, 0);
}

AutoPtr<ExpandableListPosition> ExpandableListPosition::ObtainChildPosition(
    /* [in] */ Int32 groupPosition,
    /* [in] */ Int32 childPosition)
{
    // ==================before translated======================
    // return obtain(CHILD, groupPosition, childPosition, 0);

    return Obtain(IExpandableListPosition::CHILD, groupPosition, childPosition, 0);
}

AutoPtr<ExpandableListPosition> ExpandableListPosition::ObtainPosition(
    /* [in] */ Int64 packedPosition)
{
    // ==================before translated======================
    // if (packedPosition == ExpandableListView.PACKED_POSITION_VALUE_NULL) {
    //     return null;
    // }
    //
    // ExpandableListPosition elp = getRecycledOrCreate();
    // elp.groupPos = ExpandableListView.getPackedPositionGroup(packedPosition);
    // if (ExpandableListView.getPackedPositionType(packedPosition) ==
    //         ExpandableListView.PACKED_POSITION_TYPE_CHILD) {
    //     elp.type = CHILD;
    //     elp.childPos = ExpandableListView.getPackedPositionChild(packedPosition);
    // } else {
    //     elp.type = GROUP;
    // }
    // return elp;

    if (packedPosition == -1/*ExpandableListView::PACKED_POSITION_VALUE_NULL*/) {
        return NULL;
    }

    AutoPtr<ExpandableListPosition> elp = GetRecycledOrCreate();
    elp->mGroupPos = ExpandableListView::GetPackedPositionGroup(packedPosition);
    if (ExpandableListView::GetPackedPositionType(packedPosition) == ExpandableListView::PACKED_POSITION_TYPE_CHILD) {
        elp->mType = IExpandableListPosition::CHILD;
        elp->mChildPos = ExpandableListView::GetPackedPositionChild(packedPosition);
    }
    else {
        elp->mType = IExpandableListPosition::GROUP;
    }
    return elp;
}

AutoPtr<ExpandableListPosition> ExpandableListPosition::Obtain(
    /* [in] */ Int32 type,
    /* [in] */ Int32 groupPos,
    /* [in] */ Int32 childPos,
    /* [in] */ Int32 flatListPos)
{
    // ==================before translated======================
    // ExpandableListPosition elp = getRecycledOrCreate();
    // elp.type = type;
    // elp.groupPos = groupPos;
    // elp.childPos = childPos;
    // elp.flatListPos = flatListPos;
    // return elp;

    AutoPtr<ExpandableListPosition> elp = GetRecycledOrCreate();
    elp->mType = type;
    elp->mGroupPos = groupPos;
    elp->mChildPos = childPos;
    elp->mFlatListPos = flatListPos;
    return elp;
}

ECode ExpandableListPosition::Recycle()
{
    // ==================before translated======================
    // synchronized (sPool) {
    //     if (sPool.size() < MAX_POOL_SIZE) {
    //         sPool.add(this);
    //     }
    // }

    AutoLock lock(sLock);
    Int32 size = 0;
    sPool->GetSize(&size);
    if (size < MAX_POOL_SIZE) {
        sPool->Add(TO_IINTERFACE(this));
    }
    return NOERROR;
}

ECode ExpandableListPosition::ResetState()
{
    // ==================before translated======================
    // groupPos = 0;
    // childPos = 0;
    // flatListPos = 0;
    // type = 0;

    mGroupPos = 0;
    mChildPos = 0;
    mFlatListPos = 0;
    mType = 0;
    return NOERROR;
}

AutoPtr<ExpandableListPosition> ExpandableListPosition::GetRecycledOrCreate()
{
    // ==================before translated======================
    // ExpandableListPosition elp;
    // synchronized (sPool) {
    //     if (sPool.size() > 0) {
    //         elp = sPool.remove(0);
    //     } else {
    //         return new ExpandableListPosition();
    //     }
    // }
    // elp.resetState();
    // return elp;

    AutoPtr<ExpandableListPosition> elp;
    // synchronized
    {
        AutoLock lock(sLock);
        Int32 size = 0;
        sPool->GetSize(&size);
        if (size > 0) {
            AutoPtr<IInterface> interfaceTmp;
            sPool->Get(0, (IInterface**)&interfaceTmp);
            sPool->Remove(interfaceTmp);

            IObject* objTmp = IObject::Probe(interfaceTmp);
            elp = (ExpandableListPosition*)objTmp;
        }
        else {
            CExpandableListPosition::New((IExpandableListPosition**)&elp);
        }
    }

    elp->ResetState();
    return elp;
}

AutoPtr<IArrayList> ExpandableListPosition::InitSPool()
{
    AutoPtr<IArrayList> result;
    CArrayList::New(MAX_POOL_SIZE, (IArrayList**)&result);
    return result;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos


