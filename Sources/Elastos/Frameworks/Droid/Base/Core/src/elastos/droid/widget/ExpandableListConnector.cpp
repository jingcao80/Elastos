
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/widget/AdapterView.h"
#include "elastos/droid/widget/ExpandableListConnector.h"
#include "elastos/core/AutoLock.h"
#include "elastos/core/Math.h"

using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Widget::AdapterView;
using Elastos::Droid::Widget::IExpandableListPosition;
using Elastos::Droid::Widget::IHeterogeneousExpandableList;
using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IComparable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//                ExpandableListConnector::GroupMetadata
//=====================================================================
const Int32 ExpandableListConnector::GroupMetadata::REFRESH;
AutoPtr<IParcelable> ExpandableListConnector::GroupMetadata::CREATOR = ExpandableListConnector::GroupMetadata::InitCreator();

CAR_INTERFACE_IMPL_2(ExpandableListConnector::GroupMetadata, Object, IParcelable, IComparable)

AutoPtr<ExpandableListConnector::GroupMetadata> ExpandableListConnector::GroupMetadata::Obtain(
    /* [in] */ Int32 flPos,
    /* [in] */ Int32 lastChildFlPos,
    /* [in] */ Int32 gPos,
    /* [in] */ Int64 gId)
{
    // ==================before translated======================
    // GroupMetadata gm = new GroupMetadata();
    // gm.flPos = flPos;
    // gm.lastChildFlPos = lastChildFlPos;
    // gm.gPos = gPos;
    // gm.gId = gId;
    // return gm;

    AutoPtr<ExpandableListConnector::GroupMetadata> gm = new GroupMetadata();
    gm->mFlPos = flPos;
    gm->mLastChildFlPos = lastChildFlPos;
    gm->mGPos = gPos;
    gm->mGId = gId;
    return gm;
}

ECode ExpandableListConnector::GroupMetadata::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (another == null) {
    //     throw new IllegalArgumentException();
    // }
    //
    // return gPos - another.gPos;

    IObject* objTmp = IObject::Probe(another);
    GroupMetadata* anotherTmp = (GroupMetadata*)objTmp;
    if(anotherTmp == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = this->mGPos - anotherTmp->mGPos;
    return NOERROR;
}

ECode ExpandableListConnector::GroupMetadata::DescribeContents(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return 0;

    *result = 0;
    return NOERROR;
}

ECode ExpandableListConnector::GroupMetadata::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    VALIDATE_NOT_NULL(dest);
    // ==================before translated======================
    // dest.writeInt(flPos);
    // dest.writeInt(lastChildFlPos);
    // dest.writeInt(gPos);
    // dest.writeLong(gId);

    dest->WriteInt32(mFlPos);
    dest->WriteInt32(mLastChildFlPos);
    dest->WriteInt32(this->mGPos);
    dest->WriteInt64(mGId);
    return NOERROR;
}

// overwrite CAR func for compile
ECode ExpandableListConnector::GroupMetadata::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    assert(0);
    return NOERROR;
}

// overwrite CAR func for compile
ECode ExpandableListConnector::GroupMetadata::WriteToParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    assert(0);
    return NOERROR;
}

ExpandableListConnector::GroupMetadata::GroupMetadata()
    : mFlPos(0)
    , mLastChildFlPos(0)
    , mGPos(0)
    , mGId(0)
{
}

AutoPtr<IParcelable> ExpandableListConnector::GroupMetadata::InitCreator()
{
    AutoPtr<IParcelable> result = new ExpandableListConnector::InnerParcelableCreator();
    return result;
}

//=====================================================================
//              ExpandableListConnector::PositionMetadata
//=====================================================================
const Int32 ExpandableListConnector::PositionMetadata::MAX_POOL_SIZE;
AutoPtr<IArrayList> ExpandableListConnector::PositionMetadata::sPool = ExpandableListConnector::PositionMetadata::InitSPool();
Object ExpandableListConnector::PositionMetadata::sPoolLock;

CAR_INTERFACE_IMPL(ExpandableListConnector::PositionMetadata, Object, IPositionMetadata)

AutoPtr<ExpandableListConnector::PositionMetadata> ExpandableListConnector::PositionMetadata::Obtain(
    /* [in] */ Int32 flatListPos,
    /* [in] */ Int32 type,
    /* [in] */ Int32 groupPos,
    /* [in] */ Int32 childPos,
    /* [in] */ GroupMetadata* groupMetadata,
    /* [in] */ Int32 groupInsertIndex)
{
    // ==================before translated======================
    // PositionMetadata pm = getRecycledOrCreate();
    // pm.position = ExpandableListPosition.obtain(type, groupPos, childPos, flatListPos);
    // pm.groupMetadata = groupMetadata;
    // pm.groupInsertIndex = groupInsertIndex;
    // return pm;

    AutoPtr<PositionMetadata> pm = GetRecycledOrCreate();
    pm->mPosition = ExpandableListPosition::Obtain(type, groupPos, childPos,flatListPos);
    pm->mGroupMetadata = groupMetadata;
    pm->mGroupInsertIndex = groupInsertIndex;
    return pm;
}

ECode ExpandableListConnector::PositionMetadata::Recycle()
{
    // ==================before translated======================
    // resetState();
    // synchronized (sPool) {
    //     if (sPool.size() < MAX_POOL_SIZE) {
    //         sPool.add(this);
    //     }
    // }

    ResetState();
    AutoLock lock(sPoolLock);
    Int32 size = 0;
    sPool->GetSize(&size);
    if(size < MAX_POOL_SIZE) {
        sPool->Add(TO_IINTERFACE(this));
    }
    return NOERROR;
}

ECode ExpandableListConnector::PositionMetadata::IsExpanded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return groupMetadata != null;

    *result = mGroupMetadata != NULL;
    return NOERROR;
}

ExpandableListConnector::PositionMetadata::PositionMetadata()
    : mGroupInsertIndex(0)
{
}

ECode ExpandableListConnector::PositionMetadata::ResetState()
{
    // ==================before translated======================
    // if (position != null) {
    //     position.recycle();
    //     position = null;
    // }
    // groupMetadata = null;
    // groupInsertIndex = 0;

    if(mPosition != NULL) {
        mPosition->Recycle();
        mPosition = NULL;
    }

    mGroupMetadata = NULL;
    mGroupInsertIndex = 0;
    return NOERROR;
}

AutoPtr<ExpandableListConnector::PositionMetadata> ExpandableListConnector::PositionMetadata::GetRecycledOrCreate()
{
    // ==================before translated======================
    // PositionMetadata pm;
    // synchronized (sPool) {
    //     if (sPool.size() > 0) {
    //         pm = sPool.remove(0);
    //     } else {
    //         return new PositionMetadata();
    //     }
    // }
    // pm.resetState();
    // return pm;

    AutoPtr<PositionMetadata> pm = NULL;
    // synchronized
    {
        AutoLock lock(sPoolLock);
        Int32 size = 0;
        sPool->GetSize(&size);

        if(size > 0) {
            AutoPtr<IInterface> interfaceTmp;
            sPool->Get(0, (IInterface**)&interfaceTmp);
            sPool->Remove(interfaceTmp);

            AutoPtr<IPositionMetadata> tmp = IPositionMetadata::Probe(interfaceTmp);
            pm = (PositionMetadata*)tmp.Get();
        }
        else {
            pm = new PositionMetadata();
            return pm;
        }
    }

    pm->ResetState();
    return pm;
}

AutoPtr<IArrayList> ExpandableListConnector::PositionMetadata::InitSPool()
{
    AutoPtr<IArrayList> result;
    CArrayList::New(MAX_POOL_SIZE, (IArrayList**)&result);
    return result;
}

//=====================================================================
//              ExpandableListConnector::MyDataSetObserver
//=====================================================================
CAR_INTERFACE_IMPL(ExpandableListConnector::MyDataSetObserver, Object, IDataSetObserver)

ExpandableListConnector::MyDataSetObserver::MyDataSetObserver(
    /* [in] */ ExpandableListConnector* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ExpandableListConnector::MyDataSetObserver::OnChanged()
{
    // ==================before translated======================
    // refreshExpGroupMetadataList(true, true);
    //
    // notifyDataSetChanged();

    mOwner->RefreshExpGroupMetadataList(TRUE, TRUE);
    mOwner->NotifyDataSetChanged();
    return NOERROR;
}

ECode ExpandableListConnector::MyDataSetObserver::OnInvalidated()
{
    // ==================before translated======================
    // refreshExpGroupMetadataList(true, true);
    //
    // notifyDataSetInvalidated();

    mOwner->RefreshExpGroupMetadataList(TRUE, TRUE);
    mOwner->NotifyDataSetInvalidated();
    return NOERROR;
}

//=====================================================================
//           ExpandableListConnector::InnerParcelableCreator
//=====================================================================
CAR_INTERFACE_IMPL(ExpandableListConnector::InnerParcelableCreator, Object, IParcelable)

ExpandableListConnector::InnerParcelableCreator::InnerParcelableCreator()
{
}

ECode ExpandableListConnector::InnerParcelableCreator::CreateFromParcel(
    /* [in] */ IParcel* in,
    /* [out] */ GroupMetadata** result)
{
    VALIDATE_NOT_NULL(in);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // GroupMetadata gm = GroupMetadata.obtain(
    //         in.readInt(),
    //         in.readInt(),
    //         in.readInt(),
    //         in.readLong());
    // return gm;

    Int32 flPos, lastChildFlPos, pos;
    Int64 id;
    in->ReadInt32(&flPos);
    in->ReadInt32(&lastChildFlPos);
    in->ReadInt32(&pos);
    in->ReadInt64(&id);
    *result = GroupMetadata::Obtain(flPos, lastChildFlPos, pos, id);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ExpandableListConnector::InnerParcelableCreator::NewArray(
    /* [in] */ Int32 size,
    /* [out] */ ArrayOf<GroupMetadata*>** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return new GroupMetadata[size];

    *result = ArrayOf<GroupMetadata*>::Alloc(size);
    for (Int32 idx=0; idx<size; ++idx) {
        (**result)[idx] = GroupMetadata::Obtain(0, 0, 0, 0);
    }
    REFCOUNT_ADD(*result);
    return NOERROR;
}

// overwrite CAR func for compile
ECode ExpandableListConnector::InnerParcelableCreator::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    return NOERROR;
}

// overwrite CAR func for compile
ECode ExpandableListConnector::InnerParcelableCreator::WriteToParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);
    return NOERROR;
}

//=====================================================================
//                       ExpandableListConnector
//=====================================================================
CAR_INTERFACE_IMPL_2(ExpandableListConnector, BaseAdapter, IExpandableListConnector, IFilterable)

ExpandableListConnector::ExpandableListConnector()
    : mExpandableListAdapter(NULL)
    , mTotalExpChildrenCount(0)
    , mMaxExpGroupCount(Elastos::Core::Math::INT32_MAX_VALUE)
{
}

ECode ExpandableListConnector::constructor(
    /* [in] */ IExpandableListAdapter* expandableListAdapter)
{
    // ==================before translated======================
    // mExpGroupMetadataList = new ArrayList<GroupMetadata>();
    //
    // setExpandableListAdapter(expandableListAdapter);

    CArrayList::New((IList**)&mExpGroupMetadataList);
    SetExpandableListAdapter(expandableListAdapter);
    return NOERROR;
}

ECode ExpandableListConnector::SetExpandableListAdapter(
    /* [in] */ IExpandableListAdapter* expandableListAdapter)
{
    VALIDATE_NOT_NULL(expandableListAdapter);
    // ==================before translated======================
    // if (mExpandableListAdapter != null) {
    //     mExpandableListAdapter.unregisterDataSetObserver(mDataSetObserver);
    // }
    //
    // mExpandableListAdapter = expandableListAdapter;
    // expandableListAdapter.registerDataSetObserver(mDataSetObserver);

    if (mExpandableListAdapter != NULL) {
        FAIL_RETURN(mExpandableListAdapter->UnregisterDataSetObserver(mDataSetObserver));
    }

    mExpandableListAdapter = expandableListAdapter;
    return expandableListAdapter->RegisterDataSetObserver(mDataSetObserver);
}

ECode ExpandableListConnector::GetUnflattenedPos(
    /* [in] */ Int32 flPos,
    /* [out] */ IPositionMetadata** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // /* Keep locally since frequent use */
    // final ArrayList<GroupMetadata> egml = mExpGroupMetadataList;
    // final int numExpGroups = egml.size();
    //
    // /* Binary search variables */
    // int leftExpGroupIndex = 0;
    // int rightExpGroupIndex = numExpGroups - 1;
    // int midExpGroupIndex = 0;
    // GroupMetadata midExpGm;
    //
    // if (numExpGroups == 0) {
    //     /*
    //      * There aren't any expanded groups (hence no visible children
    //      * either), so flPos must be a group and its group pos will be the
    //      * same as its flPos
    //      */
    //     return PositionMetadata.obtain(flPos, ExpandableListPosition.GROUP, flPos,
    //             -1, null, 0);
    // }
    //
    // /*
    //  * Binary search over the expanded groups to find either the exact
    //  * expanded group (if we're looking for a group) or the group that
    //  * contains the child we're looking for. If we are looking for a
    //  * collapsed group, we will not have a direct match here, but we will
    //  * find the expanded group just before the group we're searching for (so
    //  * then we can calculate the group position of the group we're searching
    //  * for). If there isn't an expanded group prior to the group being
    //  * searched for, then the group being searched for's group position is
    //  * the same as the flat list position (since there are no children before
    //  * it, and all groups before it are collapsed).
    //  */
    // while (leftExpGroupIndex <= rightExpGroupIndex) {
    //     midExpGroupIndex =
    //             (rightExpGroupIndex - leftExpGroupIndex) / 2
    //                     + leftExpGroupIndex;
    //     midExpGm = egml.get(midExpGroupIndex);
    //
    //     if (flPos > midExpGm.lastChildFlPos) {
    //         /*
    //          * The flat list position is after the current middle group's
    //          * last child's flat list position, so search right
    //          */
    //         leftExpGroupIndex = midExpGroupIndex + 1;
    //     } else if (flPos < midExpGm.flPos) {
    //         /*
    //          * The flat list position is before the current middle group's
    //          * flat list position, so search left
    //          */
    //         rightExpGroupIndex = midExpGroupIndex - 1;
    //     } else if (flPos == midExpGm.flPos) {
    //         /*
    //          * The flat list position is this middle group's flat list
    //          * position, so we've found an exact hit
    //          */
    //         return PositionMetadata.obtain(flPos, ExpandableListPosition.GROUP,
    //                 midExpGm.gPos, -1, midExpGm, midExpGroupIndex);
    //     } else if (flPos <= midExpGm.lastChildFlPos
    //             /* && flPos > midGm.flPos as deduced from previous
    //              * conditions */) {
    //         /* The flat list position is a child of the middle group */
    //
    //         /*
    //          * Subtract the first child's flat list position from the
    //          * specified flat list pos to get the child's position within
    //          * the group
    //          */
    //         final int childPos = flPos - (midExpGm.flPos + 1);
    //         return PositionMetadata.obtain(flPos, ExpandableListPosition.CHILD,
    //                 midExpGm.gPos, childPos, midExpGm, midExpGroupIndex);
    //     }
    // }
    //
    // /*
    //  * If we've reached here, it means the flat list position must be a
    //  * group that is not expanded, since otherwise we would have hit it
    //  * in the above search.
    //  */
    //
    //
    // /**
    //  * If we are to expand this group later, where would it go in the
    //  * mExpGroupMetadataList ?
    //  */
    // int insertPosition = 0;
    //
    // /** What is its group position in the list of all groups? */
    // int groupPos = 0;
    //
    // /*
    //  * To figure out exact insertion and prior group positions, we need to
    //  * determine how we broke out of the binary search.  We backtrack
    //  * to see this.
    //  */
    // if (leftExpGroupIndex > midExpGroupIndex) {
    //
    //     /*
    //      * This would occur in the first conditional, so the flat list
    //      * insertion position is after the left group. Also, the
    //      * leftGroupPos is one more than it should be (since that broke out
    //      * of our binary search), so we decrement it.
    //      */
    //     final GroupMetadata leftExpGm = egml.get(leftExpGroupIndex-1);
    //
    //     insertPosition = leftExpGroupIndex;
    //
    //     /*
    //      * Sums the number of groups between the prior exp group and this
    //      * one, and then adds it to the prior group's group pos
    //      */
    //     groupPos =
    //         (flPos - leftExpGm.lastChildFlPos) + leftExpGm.gPos;
    // } else if (rightExpGroupIndex < midExpGroupIndex) {
    //
    //     /*
    //      * This would occur in the second conditional, so the flat list
    //      * insertion position is before the right group. Also, the
    //      * rightGroupPos is one less than it should be, so increment it.
    //      */
    //     final GroupMetadata rightExpGm = egml.get(++rightExpGroupIndex);
    //
    //     insertPosition = rightExpGroupIndex;
    //
    //     /*
    //      * Subtracts this group's flat list pos from the group after's flat
    //      * list position to find out how many groups are in between the two
    //      * groups. Then, subtracts that number from the group after's group
    //      * pos to get this group's pos.
    //      */
    //     groupPos = rightExpGm.gPos - (rightExpGm.flPos - flPos);
    // } else {
    //     // TODO: clean exit
    //     throw new RuntimeException("Unknown state");
    // }
    //
    // return PositionMetadata.obtain(flPos, ExpandableListPosition.GROUP, groupPos, -1,
    //         null, insertPosition);

    assert(0);
    AutoPtr<IList> egml = mExpGroupMetadataList;
    Int32 numExpGroups = 0;
    egml->GetSize(&numExpGroups);
    Int32 leftExpGroupIndex = 0;
    Int32 rightExpGroupIndex = numExpGroups - 1;
    Int32 midExpGroupIndex = 0;
    AutoPtr<GroupMetadata> midExpGm;

    if (numExpGroups == 0) {
        *result = PositionMetadata::Obtain(flPos, IExpandableListPosition::GROUP, flPos, -1, NULL, 0);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    while (leftExpGroupIndex <= rightExpGroupIndex) {
        midExpGroupIndex = (rightExpGroupIndex - leftExpGroupIndex) / 2 + leftExpGroupIndex;

        AutoPtr<IInterface> interfaceTmp;
        egml->Get(midExpGroupIndex, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        midExpGm = (GroupMetadata*)objTmp;

        if (flPos > midExpGm->mLastChildFlPos) {
            leftExpGroupIndex = midExpGroupIndex + 1;
        }
        else if (flPos < midExpGm->mFlPos) {
            rightExpGroupIndex = midExpGroupIndex - 1;
        }
        else if (flPos == midExpGm->mFlPos) {
            *result = PositionMetadata::Obtain(flPos, IExpandableListPosition::GROUP,
                midExpGm->mGPos, -1, midExpGm, midExpGroupIndex);
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        else if (flPos <= midExpGm->mLastChildFlPos) {
            Int32 childPos = flPos - (midExpGm.Get()->mFlPos + 1);
            *result = PositionMetadata::Obtain(flPos, ExpandableListPosition::CHILD,
                midExpGm->mGPos, childPos, midExpGm, midExpGroupIndex);
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    Int32 insertPosition = 0;
    Int32 groupPos = 0;
    if (leftExpGroupIndex > midExpGroupIndex) {
        AutoPtr<IInterface> interfaceTmp;
        egml->Get(leftExpGroupIndex-1, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* leftExpGm = (GroupMetadata*)objTmp;

        insertPosition = leftExpGroupIndex;
        groupPos = (flPos - leftExpGm->mLastChildFlPos) + leftExpGm->mGPos;
    }
    else if (rightExpGroupIndex < midExpGroupIndex) {
        AutoPtr<IInterface> interfaceTmp;
        egml->Get(++rightExpGroupIndex, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* rightExpGm = (GroupMetadata*)objTmp;

        insertPosition = rightExpGroupIndex;
        groupPos = rightExpGm->mGPos - (rightExpGm->mFlPos - flPos);
    }
    else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = PositionMetadata::Obtain(flPos, ExpandableListPosition::GROUP, groupPos, -1, NULL, insertPosition);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ExpandableListConnector::GetFlattenedPos(
    /* [in] */ IExpandableListPosition* pos,
    /* [out] */ IPositionMetadata** result)
{
    VALIDATE_NOT_NULL(pos);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final ArrayList<GroupMetadata> egml = mExpGroupMetadataList;
    // final int numExpGroups = egml.size();
    //
    // /* Binary search variables */
    // int leftExpGroupIndex = 0;
    // int rightExpGroupIndex = numExpGroups - 1;
    // int midExpGroupIndex = 0;
    // GroupMetadata midExpGm;
    //
    // if (numExpGroups == 0) {
    //     /*
    //      * There aren't any expanded groups, so flPos must be a group and
    //      * its flPos will be the same as its group pos.  The
    //      * insert position is 0 (since the list is empty).
    //      */
    //     return PositionMetadata.obtain(pos.groupPos, pos.type,
    //             pos.groupPos, pos.childPos, null, 0);
    // }
    //
    // /*
    //  * Binary search over the expanded groups to find either the exact
    //  * expanded group (if we're looking for a group) or the group that
    //  * contains the child we're looking for.
    //  */
    // while (leftExpGroupIndex <= rightExpGroupIndex) {
    //     midExpGroupIndex = (rightExpGroupIndex - leftExpGroupIndex)/2 + leftExpGroupIndex;
    //     midExpGm = egml.get(midExpGroupIndex);
    //
    //     if (pos.groupPos > midExpGm.gPos) {
    //         /*
    //          * It's after the current middle group, so search right
    //          */
    //         leftExpGroupIndex = midExpGroupIndex + 1;
    //     } else if (pos.groupPos < midExpGm.gPos) {
    //         /*
    //          * It's before the current middle group, so search left
    //          */
    //         rightExpGroupIndex = midExpGroupIndex - 1;
    //     } else if (pos.groupPos == midExpGm.gPos) {
    //         /*
    //          * It's this middle group, exact hit
    //          */
    //
    //         if (pos.type == ExpandableListPosition.GROUP) {
    //             /* If it's a group, give them this matched group's flPos */
    //             return PositionMetadata.obtain(midExpGm.flPos, pos.type,
    //                     pos.groupPos, pos.childPos, midExpGm, midExpGroupIndex);
    //         } else if (pos.type == ExpandableListPosition.CHILD) {
    //             /* If it's a child, calculate the flat list pos */
    //             return PositionMetadata.obtain(midExpGm.flPos + pos.childPos
    //                     + 1, pos.type, pos.groupPos, pos.childPos,
    //                     midExpGm, midExpGroupIndex);
    //         } else {
    //             return null;
    //         }
    //     }
    // }
    //
    // /*
    //  * If we've reached here, it means there was no match in the expanded
    //  * groups, so it must be a collapsed group that they're search for
    //  */
    // if (pos.type != ExpandableListPosition.GROUP) {
    //     /* If it isn't a group, return null */
    //     return null;
    // }
    //
    // /*
    //  * To figure out exact insertion and prior group positions, we need to
    //  * determine how we broke out of the binary search. We backtrack to see
    //  * this.
    //  */
    // if (leftExpGroupIndex > midExpGroupIndex) {
    //
    //     /*
    //      * This would occur in the first conditional, so the flat list
    //      * insertion position is after the left group.
    //      *
    //      * The leftGroupPos is one more than it should be (from the binary
    //      * search loop) so we subtract 1 to get the actual left group.  Since
    //      * the insertion point is AFTER the left group, we keep this +1
    //      * value as the insertion point
    //      */
    //     final GroupMetadata leftExpGm = egml.get(leftExpGroupIndex-1);
    //     final int flPos =
    //             leftExpGm.lastChildFlPos
    //                     + (pos.groupPos - leftExpGm.gPos);
    //
    //     return PositionMetadata.obtain(flPos, pos.type, pos.groupPos,
    //             pos.childPos, null, leftExpGroupIndex);
    // } else if (rightExpGroupIndex < midExpGroupIndex) {
    //
    //     /*
    //      * This would occur in the second conditional, so the flat list
    //      * insertion position is before the right group. Also, the
    //      * rightGroupPos is one less than it should be (from binary search
    //      * loop), so we increment to it.
    //      */
    //     final GroupMetadata rightExpGm = egml.get(++rightExpGroupIndex);
    //     final int flPos =
    //             rightExpGm.flPos
    //                     - (rightExpGm.gPos - pos.groupPos);
    //     return PositionMetadata.obtain(flPos, pos.type, pos.groupPos,
    //             pos.childPos, null, rightExpGroupIndex);
    // } else {
    //     return null;
    // }

    assert(0);
    AutoPtr<IList> egml = mExpGroupMetadataList;
    Int32 numExpGroups = 0;
    egml->GetSize(&numExpGroups);
    Int32 leftExpGroupIndex = 0;
    Int32 rightExpGroupIndex = numExpGroups - 1;
    Int32 midExpGroupIndex = 0;
    AutoPtr<GroupMetadata> midExpGm;

    if (numExpGroups == 0) {
        *result = PositionMetadata::Obtain(((ExpandableListPosition*)pos)->mGroupPos,
            ((ExpandableListPosition*)pos)->mType, ((ExpandableListPosition*)pos)->mGroupPos,
            ((ExpandableListPosition*)pos)->mChildPos, NULL, 0);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    while (leftExpGroupIndex <= rightExpGroupIndex) {
        midExpGroupIndex = (rightExpGroupIndex - leftExpGroupIndex)/2 + leftExpGroupIndex;

        AutoPtr<IInterface> interfaceTmp;
        egml->Get(midExpGroupIndex, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        midExpGm = (GroupMetadata*)objTmp;

        if (((ExpandableListPosition*)pos)->mGroupPos > midExpGm->mGPos) {
            leftExpGroupIndex = midExpGroupIndex + 1;
        }
        else if (((ExpandableListPosition*)pos)->mGroupPos < midExpGm->mGPos) {
            rightExpGroupIndex = midExpGroupIndex - 1;
        }
        else if (((ExpandableListPosition*)pos)->mGroupPos == midExpGm->mGPos) {
            if (((ExpandableListPosition*)pos)->mType == IExpandableListPosition::GROUP) {
                *result = PositionMetadata::Obtain(midExpGm->mFlPos, ((ExpandableListPosition*)pos)->mType,
                    ((ExpandableListPosition*)pos)->mGroupPos,
                    ((ExpandableListPosition*)pos)->mChildPos, midExpGm, midExpGroupIndex);
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
            else if (((ExpandableListPosition*)pos)->mType == IExpandableListPosition::CHILD) {
                *result = PositionMetadata::Obtain(midExpGm->mFlPos + ((ExpandableListPosition*)pos)->mChildPos + 1,
                    ((ExpandableListPosition*)pos)->mType, ((ExpandableListPosition*)pos)->mGroupPos,
                    ((ExpandableListPosition*)pos)->mChildPos, midExpGm, midExpGroupIndex);
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
            else {
                *result = NULL;
                return NOERROR;
            }
        }
    }

    if (((ExpandableListPosition*)pos)->mType != IExpandableListPosition::GROUP) {
        *result = NULL;
        return NOERROR;
    }

    if (leftExpGroupIndex > midExpGroupIndex) {
        AutoPtr<IInterface> interfaceTmp;
        egml->Get(leftExpGroupIndex-1, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* leftExpGm = (GroupMetadata*)objTmp;

        Int32 flPos = leftExpGm->mLastChildFlPos + (((ExpandableListPosition*)pos)->mGroupPos - leftExpGm->mGPos);
        *result = PositionMetadata::Obtain(flPos, ((ExpandableListPosition*)pos)->mType, ((ExpandableListPosition*)pos)->mGroupPos,
            ((ExpandableListPosition*)pos)->mChildPos, NULL, leftExpGroupIndex);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (rightExpGroupIndex < midExpGroupIndex) {
        AutoPtr<IInterface> interfaceTmp;
        egml->Get(++rightExpGroupIndex, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* rightExpGm = (GroupMetadata*)objTmp;

        Int32 flPos = rightExpGm->mFlPos - (rightExpGm->mGPos - ((ExpandableListPosition*)pos)->mGroupPos);
        *result = PositionMetadata::Obtain(flPos, ((ExpandableListPosition*)pos)->mType, ((ExpandableListPosition*)pos)->mGroupPos,
            ((ExpandableListPosition*)pos)->mChildPos, NULL, rightExpGroupIndex);
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else {
        *result = NULL;
        return NOERROR;
    }
}

Boolean ExpandableListConnector::AreAllItemsEnabled()
{
    // ==================before translated======================
    // return mExpandableListAdapter.areAllItemsEnabled();

    Boolean enabled;
    mExpandableListAdapter->AreAllItemsEnabled(&enabled);
    return enabled;
}

Boolean ExpandableListConnector::IsEnabled(
    /* [in] */ Int32 flatListPos)
{
    // ==================before translated======================
    // final PositionMetadata metadata = getUnflattenedPos(flatListPos);
    // final ExpandableListPosition pos = metadata.position;
    //
    // boolean retValue;
    // if (pos.type == ExpandableListPosition.CHILD) {
    //     retValue = mExpandableListAdapter.isChildSelectable(pos.groupPos, pos.childPos);
    // } else {
    //     // Groups are always selectable
    //     retValue = true;
    // }
    //
    // metadata.recycle();
    //
    // return retValue;

    AutoPtr<IPositionMetadata> posMetadata;
    GetUnflattenedPos(flatListPos, (IPositionMetadata**)&posMetadata);
    PositionMetadata* metadata = (PositionMetadata*)posMetadata.Get();
    AutoPtr<IExpandableListPosition> pos = metadata->mPosition;
    Boolean retValue = FALSE;
    if (((ExpandableListPosition*)pos.Get())->mType == IExpandableListPosition::CHILD) {
        mExpandableListAdapter->IsChildSelectable(((ExpandableListPosition*)pos.Get())->mGroupPos,
            ((ExpandableListPosition*)pos.Get())->mChildPos, &retValue);
    }
    else {
        retValue = TRUE;
    }

    metadata->Recycle();
    return retValue;
}

ECode ExpandableListConnector::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // /*
    //  * Total count for the list view is the number groups plus the
    //  * number of children from currently expanded groups (a value we keep
    //  * cached in this class)
    //  */
    // return mExpandableListAdapter.getGroupCount() + mTotalExpChildrenCount;

    Int32 count = 0;
    mExpandableListAdapter->GetGroupCount(&count);
    *result = count + mTotalExpChildrenCount;
    return NOERROR;
}

ECode ExpandableListConnector::GetItem(
    /* [in] */ Int32 flatListPos,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final PositionMetadata posMetadata = getUnflattenedPos(flatListPos);
    //
    // Object retValue;
    // if (posMetadata.position.type == ExpandableListPosition.GROUP) {
    //     retValue = mExpandableListAdapter
    //             .getGroup(posMetadata.position.groupPos);
    // } else if (posMetadata.position.type == ExpandableListPosition.CHILD) {
    //     retValue = mExpandableListAdapter.getChild(posMetadata.position.groupPos,
    //             posMetadata.position.childPos);
    // } else {
    //     // TODO: clean exit
    //     throw new RuntimeException("Flat list position is of unknown type");
    // }
    //
    // posMetadata.recycle();
    //
    // return retValue;

    AutoPtr<IPositionMetadata> metadata;
    GetUnflattenedPos(flatListPos, (IPositionMetadata**)&metadata);
    PositionMetadata* posMetadata = (PositionMetadata*)metadata.Get();

    if (posMetadata->mPosition->mType == IExpandableListPosition::GROUP) {
        mExpandableListAdapter->GetGroup(posMetadata->mPosition->mGroupPos, (IInterface**)result);
    }
    else if (posMetadata->mPosition->mType == IExpandableListPosition::CHILD) {
        mExpandableListAdapter->GetChild(posMetadata->mPosition->mGroupPos,
            posMetadata->mPosition->mChildPos, (IInterface**)result);
    }
    else {
        // TODO: clean exit
        //throw new RuntimeException("Flat list position is of unknown type");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    posMetadata->Recycle();
    return NOERROR;
}

ECode ExpandableListConnector::GetItemId(
    /* [in] */ Int32 flatListPos,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final PositionMetadata posMetadata = getUnflattenedPos(flatListPos);
    // final long groupId = mExpandableListAdapter.getGroupId(posMetadata.position.groupPos);
    //
    // long retValue;
    // if (posMetadata.position.type == ExpandableListPosition.GROUP) {
    //     retValue = mExpandableListAdapter.getCombinedGroupId(groupId);
    // } else if (posMetadata.position.type == ExpandableListPosition.CHILD) {
    //     final long childId = mExpandableListAdapter.getChildId(posMetadata.position.groupPos,
    //             posMetadata.position.childPos);
    //     retValue = mExpandableListAdapter.getCombinedChildId(groupId, childId);
    // } else {
    //     // TODO: clean exit
    //     throw new RuntimeException("Flat list position is of unknown type");
    // }
    //
    // posMetadata.recycle();
    //
    // return retValue;

    AutoPtr<IPositionMetadata> metadata;
    GetUnflattenedPos(flatListPos, (IPositionMetadata**)&metadata);
    PositionMetadata* posMetadata = (PositionMetadata*)metadata.Get();
    Int64 groupId = 0;
    mExpandableListAdapter->GetGroupId(posMetadata->mPosition->mGroupPos,
        &groupId);

    if (posMetadata->mPosition->mType == IExpandableListPosition::GROUP) {
        mExpandableListAdapter->GetCombinedGroupId(groupId, result);
    }
    else if (posMetadata->mPosition->mType == IExpandableListPosition::CHILD) {
        Int64 childId;
        mExpandableListAdapter->GetChildId(posMetadata->mPosition->mGroupPos,
            posMetadata->mPosition->mChildPos, &childId);
        mExpandableListAdapter->GetCombinedChildId(groupId, childId, result);
    }
    else {
        // TODO: clean exit
        //throw new RuntimeException("Flat list position is of unknown type");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    posMetadata->Recycle();
    return NOERROR;
}

ECode ExpandableListConnector::GetView(
    /* [in] */ Int32 flatListPos,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(convertView);
    VALIDATE_NOT_NULL(parent);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final PositionMetadata posMetadata = getUnflattenedPos(flatListPos);
    //
    // View retValue;
    // if (posMetadata.position.type == ExpandableListPosition.GROUP) {
    //     retValue = mExpandableListAdapter.getGroupView(posMetadata.position.groupPos,
    //             posMetadata.isExpanded(), convertView, parent);
    // } else if (posMetadata.position.type == ExpandableListPosition.CHILD) {
    //     final boolean isLastChild = posMetadata.groupMetadata.lastChildFlPos == flatListPos;
    //
    //     retValue = mExpandableListAdapter.getChildView(posMetadata.position.groupPos,
    //             posMetadata.position.childPos, isLastChild, convertView, parent);
    // } else {
    //     // TODO: clean exit
    //     throw new RuntimeException("Flat list position is of unknown type");
    // }
    //
    // posMetadata.recycle();
    //
    // return retValue;

    AutoPtr<IPositionMetadata> metadata;
    GetUnflattenedPos(flatListPos, (IPositionMetadata**)&metadata);
    PositionMetadata* posMetadata = (PositionMetadata*)metadata.Get();

    Boolean expanded = FALSE;
    if (posMetadata->mPosition->mType == IExpandableListPosition::GROUP) {
        mExpandableListAdapter->GetGroupView(posMetadata->mPosition->mGroupPos,
            (posMetadata->IsExpanded(&expanded), expanded), convertView, parent, (IView**)result);
    }
    else if (posMetadata->mPosition->mType == IExpandableListPosition::CHILD) {
        Boolean isLastChild = posMetadata->mGroupMetadata->mLastChildFlPos == flatListPos;

        mExpandableListAdapter->GetChildView(posMetadata->mPosition->mGroupPos,
            posMetadata->mPosition->mChildPos, isLastChild, convertView, parent, (IView**)result);
    }
    else {
        // TODO: clean exit
        //throw new RuntimeException("Flat list position is of unknown type");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    posMetadata->Recycle();
    return NOERROR;
}

Int32 ExpandableListConnector::GetItemViewType(
    /* [in] */ Int32 flatListPos)
{
    // ==================before translated======================
    // final PositionMetadata metadata = getUnflattenedPos(flatListPos);
    // final ExpandableListPosition pos = metadata.position;
    //
    // int retValue;
    // if (mExpandableListAdapter instanceof HeterogeneousExpandableList) {
    //     HeterogeneousExpandableList adapter =
    //             (HeterogeneousExpandableList) mExpandableListAdapter;
    //     if (pos.type == ExpandableListPosition.GROUP) {
    //         retValue = adapter.getGroupType(pos.groupPos);
    //     } else {
    //         final int childType = adapter.getChildType(pos.groupPos, pos.childPos);
    //         retValue = adapter.getGroupTypeCount() + childType;
    //     }
    // } else {
    //     if (pos.type == ExpandableListPosition.GROUP) {
    //         retValue = 0;
    //     } else {
    //         retValue = 1;
    //     }
    // }
    //
    // metadata.recycle();
    //
    // return retValue;

    AutoPtr<IPositionMetadata> metadata;
    GetUnflattenedPos(flatListPos, (IPositionMetadata**)&metadata);
    PositionMetadata* posMetadata = (PositionMetadata*)metadata.Get();
    AutoPtr<IExpandableListPosition> pos = posMetadata->mPosition;

    Int32 result = 0;
    IHeterogeneousExpandableList* adapter = IHeterogeneousExpandableList::Probe(mExpandableListAdapter);
    if (adapter != NULL) {
        if (((ExpandableListPosition*)pos.Get())->mType == IExpandableListPosition::GROUP) {
            adapter->GetGroupType(((ExpandableListPosition*)pos.Get())->mGroupPos, &result);
        }
        else {
            Int32 childType;
            adapter->GetChildType(((ExpandableListPosition*)pos.Get())->mGroupPos,
                ((ExpandableListPosition*)pos.Get())->mChildPos, &childType);
            adapter->GetGroupTypeCount(&result);
            result += childType;
        }
    }
    else {
        if (((ExpandableListPosition*)pos.Get())->mType == IExpandableListPosition::GROUP) {
            result = 0;
        }
        else {
            result = 1;
        }
    }

    metadata->Recycle();
    return result;
}

Int32 ExpandableListConnector::GetViewTypeCount()
{
    // ==================before translated======================
    // if (mExpandableListAdapter instanceof HeterogeneousExpandableList) {
    //     HeterogeneousExpandableList adapter =
    //             (HeterogeneousExpandableList) mExpandableListAdapter;
    //     return adapter.getGroupTypeCount() + adapter.getChildTypeCount();
    // } else {
    //     return 2;
    // }

    IHeterogeneousExpandableList* adapter = IHeterogeneousExpandableList::Probe(mExpandableListAdapter);
    if (adapter != NULL) {
        Int32 groupCount = 0;
        Int32 childCount = 0;
        adapter->GetGroupTypeCount(&groupCount);
        adapter->GetChildTypeCount(&childCount);
        return groupCount + childCount;
    }
    else {
        return 2;
    }
}

Boolean ExpandableListConnector::HasStableIds()
{
    // ==================before translated======================
    // return mExpandableListAdapter.hasStableIds();

    Boolean ret = FALSE;
    mExpandableListAdapter->HasStableIds(&ret);
    return ret;
}

ECode ExpandableListConnector::CollapseGroup(
    /* [in] */ Int32 groupPos,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ExpandableListPosition elGroupPos = ExpandableListPosition.obtain(
    //         ExpandableListPosition.GROUP, groupPos, -1, -1);
    // PositionMetadata pm = getFlattenedPos(elGroupPos);
    // elGroupPos.recycle();
    // if (pm == null) return false;
    //
    // boolean retValue = collapseGroup(pm);
    // pm.recycle();
    // return retValue;

    AutoPtr<IExpandableListPosition> elGroupPos = ExpandableListPosition::Obtain(IExpandableListPosition::GROUP, groupPos, -1, -1);
    AutoPtr<IPositionMetadata> pm;
    GetFlattenedPos(elGroupPos, (IPositionMetadata**)&pm);
    elGroupPos->Recycle();

    if (pm == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    CollapseGroup(pm, result);
    pm->Recycle();
    return NOERROR;
}

ECode ExpandableListConnector::CollapseGroup(
    /* [in] */ IPositionMetadata* metadata,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(metadata);
    VALIDATE_NOT_NULL(result);

    PositionMetadata* posMetadata = (PositionMetadata*)metadata;
    // ==================before translated======================
    // /*
    //  * Collapsing requires removal from mExpGroupMetadataList
    //  */
    //
    // /*
    //  * If it is null, it must be already collapsed. This group metadata
    //  * object should have been set from the search that returned the
    //  * position metadata object.
    //  */
    // if (posMetadata.groupMetadata == null) return false;
    //
    // // Remove the group from the list of expanded groups
    // mExpGroupMetadataList.remove(posMetadata.groupMetadata);
    //
    // // Refresh the metadata
    // refreshExpGroupMetadataList(false, false);
    //
    // // Notify of change
    // notifyDataSetChanged();
    //
    // // Give the callback
    // mExpandableListAdapter.onGroupCollapsed(posMetadata.groupMetadata.gPos);
    //
    // return true;

    if (posMetadata->mGroupMetadata == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    mExpGroupMetadataList->Remove(TO_IINTERFACE(posMetadata->mGroupMetadata));
    RefreshExpGroupMetadataList(FALSE, FALSE);
    NotifyDataSetChanged();
    mExpandableListAdapter->OnGroupCollapsed(posMetadata->mGroupMetadata->mGPos);
    *result = TRUE;
    return NOERROR;
}

ECode ExpandableListConnector::ExpandGroup(
    /* [in] */ Int32 groupPos,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ExpandableListPosition elGroupPos = ExpandableListPosition.obtain(
    //         ExpandableListPosition.GROUP, groupPos, -1, -1);
    // PositionMetadata pm = getFlattenedPos(elGroupPos);
    // elGroupPos.recycle();
    // boolean retValue = expandGroup(pm);
    // pm.recycle();
    // return retValue;

    AutoPtr<ExpandableListPosition> elGroupPos = ExpandableListPosition::Obtain(IExpandableListPosition::GROUP, groupPos, -1, -1);
    AutoPtr<IPositionMetadata> pm;
    GetFlattenedPos(elGroupPos, (IPositionMetadata**)&pm);
    elGroupPos->Recycle();

    ExpandGroup(pm, result);
    pm->Recycle();
    return NOERROR;
}

ECode ExpandableListConnector::ExpandGroup(
    /* [in] */ IPositionMetadata* metadata,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(metadata);
    VALIDATE_NOT_NULL(result);

    PositionMetadata* posMetadata = (PositionMetadata*)metadata;
    // ==================before translated======================
    // /*
    //  * Expanding requires insertion into the mExpGroupMetadataList
    //  */
    //
    // if (posMetadata.position.groupPos < 0) {
    //     // TODO clean exit
    //     throw new RuntimeException("Need group");
    // }
    //
    // if (mMaxExpGroupCount == 0) return false;
    //
    // // Check to see if it's already expanded
    // if (posMetadata.groupMetadata != null) return false;
    //
    // /* Restrict number of expanded groups to mMaxExpGroupCount */
    // if (mExpGroupMetadataList.size() >= mMaxExpGroupCount) {
    //     /* Collapse a group */
    //     // TODO: Collapse something not on the screen instead of the first one?
    //     // TODO: Could write overloaded function to take GroupMetadata to collapse
    //     GroupMetadata collapsedGm = mExpGroupMetadataList.get(0);
    //
    //     int collapsedIndex = mExpGroupMetadataList.indexOf(collapsedGm);
    //
    //     collapseGroup(collapsedGm.gPos);
    //
    //     /* Decrement index if it is after the group we removed */
    //     if (posMetadata.groupInsertIndex > collapsedIndex) {
    //         posMetadata.groupInsertIndex--;
    //     }
    // }
    //
    // GroupMetadata expandedGm = GroupMetadata.obtain(
    //         GroupMetadata.REFRESH,
    //         GroupMetadata.REFRESH,
    //         posMetadata.position.groupPos,
    //         mExpandableListAdapter.getGroupId(posMetadata.position.groupPos));
    //
    // mExpGroupMetadataList.add(posMetadata.groupInsertIndex, expandedGm);
    //
    // // Refresh the metadata
    // refreshExpGroupMetadataList(false, false);
    //
    // // Notify of change
    // notifyDataSetChanged();
    //
    // // Give the callback
    // mExpandableListAdapter.onGroupExpanded(expandedGm.gPos);
    //
    // return true;

    if (posMetadata->mPosition->mGroupPos < 0) {
        // TODO clean exit
        //throw new RuntimeException("Need group");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mMaxExpGroupCount == 0) {
        *result = FALSE;
        return NOERROR;
    }

    if (posMetadata->mGroupMetadata != NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Int32 size = 0;
    mExpGroupMetadataList->GetSize(&size);
    if (size >= mMaxExpGroupCount) {
        AutoPtr<IInterface> interfaceTmp;
        mExpGroupMetadataList->Get(0, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* collapsedGm = (GroupMetadata*)objTmp;

        Int32 collapsedIndex = 0;
        mExpGroupMetadataList->IndexOf(interfaceTmp, &collapsedIndex);

        Boolean resTmp = FALSE;
        CollapseGroup(collapsedGm->mGPos, &resTmp);

        if (posMetadata->mGroupInsertIndex > collapsedIndex) {
            posMetadata->mGroupInsertIndex--;
        }
    }

    Int64 id = 0;
    mExpandableListAdapter->GetGroupId(posMetadata->mPosition->mGroupPos, &id);
    AutoPtr<GroupMetadata> expandedGm = GroupMetadata::Obtain(GroupMetadata::REFRESH, GroupMetadata::REFRESH,
        posMetadata->mPosition->mGroupPos, id);

    mExpGroupMetadataList->Add(posMetadata->mGroupInsertIndex, TO_IINTERFACE(expandedGm));
    RefreshExpGroupMetadataList(FALSE, FALSE);
    NotifyDataSetChanged();
    mExpandableListAdapter->OnGroupExpanded(expandedGm->mGPos);
    *result = TRUE;
    return NOERROR;
}

ECode ExpandableListConnector::IsGroupExpanded(
    /* [in] */ Int32 groupPosition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // GroupMetadata groupMetadata;
    // for (int i = mExpGroupMetadataList.size() - 1; i >= 0; i--) {
    //     groupMetadata = mExpGroupMetadataList.get(i);
    //
    //     if (groupMetadata.gPos == groupPosition) {
    //         return true;
    //     }
    // }
    //
    // return false;

    Int32 size = 0;
    mExpGroupMetadataList->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        mExpGroupMetadataList->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* groupMetadata = (GroupMetadata*)objTmp;

        if (groupMetadata->mGPos == groupPosition) {
            *result = TRUE;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode ExpandableListConnector::SetMaxExpGroupCount(
    /* [in] */ Int32 maxExpGroupCount)
{
    // ==================before translated======================
    // mMaxExpGroupCount = maxExpGroupCount;

    mMaxExpGroupCount = maxExpGroupCount;
    return NOERROR;
}

ECode ExpandableListConnector::GetAdapter(
    /* [out] */ IExpandableListAdapter** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mExpandableListAdapter;

    *result = mExpandableListAdapter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ExpandableListConnector::GetFilter(
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // ExpandableListAdapter adapter = getAdapter();
    // if (adapter instanceof Filterable) {
    //     return ((Filterable) adapter).getFilter();
    // } else {
    //     return null;
    // }

    AutoPtr<IExpandableListAdapter> adapter;
    GetAdapter((IExpandableListAdapter**)&adapter);
    IFilterable* filter = IFilterable::Probe(adapter);
    if (filter != NULL) {
        filter->GetFilter((IFilter**)&result);
    }
    else {
        *result = NULL;
    }
    return NOERROR;
}

ECode ExpandableListConnector::GetExpandedGroupMetadataList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mExpGroupMetadataList;

    *result = mExpGroupMetadataList;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ExpandableListConnector::SetExpandedGroupMetadataList(
    /* [in] */ IList* expandedGroupMetadataList)
{
    VALIDATE_NOT_NULL(expandedGroupMetadataList);
    // ==================before translated======================
    //
    // if ((expandedGroupMetadataList == null) || (mExpandableListAdapter == null)) {
    //     return;
    // }
    //
    // // Make sure our current data set is big enough for the previously
    // // expanded groups, if not, ignore this request
    // int numGroups = mExpandableListAdapter.getGroupCount();
    // for (int i = expandedGroupMetadataList.size() - 1; i >= 0; i--) {
    //     if (expandedGroupMetadataList.get(i).gPos >= numGroups) {
    //         // Doh, for some reason the client doesn't have some of the groups
    //         return;
    //     }
    // }
    //
    // mExpGroupMetadataList = expandedGroupMetadataList;
    // refreshExpGroupMetadataList(true, false);

    Int32 size = 0;
    expandedGroupMetadataList->GetSize(&size);
    if (size > 0 || mExpandableListAdapter == NULL) {
        return NOERROR;
    }

    Int32 numGroups = 0;
    mExpandableListAdapter->GetGroupCount(&numGroups);
    for (Int32 i = size - 1; i >= 0; --i) {
        AutoPtr<IInterface> interfaceTmp;
        expandedGroupMetadataList->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* groupMetadata = (GroupMetadata*)objTmp;
        if (groupMetadata->mGPos >= numGroups) {
            return NOERROR;
        }
    }

    mExpGroupMetadataList = expandedGroupMetadataList;
    RefreshExpGroupMetadataList(TRUE, FALSE);
    return NOERROR;
}

Boolean ExpandableListConnector::IsEmpty()
{
    // ==================before translated======================
    // ExpandableListAdapter adapter = getAdapter();
    // return adapter != null ? adapter.isEmpty() : true;

    Boolean empty = FALSE;
    AutoPtr<IExpandableListAdapter> adapter;
    GetAdapter((IExpandableListAdapter**)&adapter);
    if (adapter) {
        adapter->IsEmpty(&empty);
    }
    return empty;
}

ECode ExpandableListConnector::FindGroupPosition(
    /* [in] */ Int64 groupIdToMatch,
    /* [in] */ Int32 seedGroupPosition,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // int count = mExpandableListAdapter.getGroupCount();
    //
    // if (count == 0) {
    //     return AdapterView.INVALID_POSITION;
    // }
    //
    // // If there isn't a selection don't hunt for it
    // if (groupIdToMatch == AdapterView.INVALID_ROW_ID) {
    //     return AdapterView.INVALID_POSITION;
    // }
    //
    // // Pin seed to reasonable values
    // seedGroupPosition = Math.max(0, seedGroupPosition);
    // seedGroupPosition = Math.min(count - 1, seedGroupPosition);
    //
    // long endTime = SystemClock.uptimeMillis() + AdapterView.SYNC_MAX_DURATION_MILLIS;
    //
    // long rowId;
    //
    // // first position scanned so far
    // int first = seedGroupPosition;
    //
    // // last position scanned so far
    // int last = seedGroupPosition;
    //
    // // True if we should move down on the next iteration
    // boolean next = false;
    //
    // // True when we have looked at the first item in the data
    // boolean hitFirst;
    //
    // // True when we have looked at the last item in the data
    // boolean hitLast;
    //
    // // Get the item ID locally (instead of getItemIdAtPosition), so
    // // we need the adapter
    // ExpandableListAdapter adapter = getAdapter();
    // if (adapter == null) {
    //     return AdapterView.INVALID_POSITION;
    // }
    //
    // while (SystemClock.uptimeMillis() <= endTime) {
    //     rowId = adapter.getGroupId(seedGroupPosition);
    //     if (rowId == groupIdToMatch) {
    //         // Found it!
    //         return seedGroupPosition;
    //     }
    //
    //     hitLast = last == count - 1;
    //     hitFirst = first == 0;
    //
    //     if (hitLast && hitFirst) {
    //         // Looked at everything
    //         break;
    //     }
    //
    //     if (hitFirst || (next && !hitLast)) {
    //         // Either we hit the top, or we are trying to move down
    //         last++;
    //         seedGroupPosition = last;
    //         // Try going up next time
    //         next = false;
    //     } else if (hitLast || (!next && !hitFirst)) {
    //         // Either we hit the bottom, or we are trying to move up
    //         first--;
    //         seedGroupPosition = first;
    //         // Try going down next time
    //         next = true;
    //     }
    //
    // }
    //
    // return AdapterView.INVALID_POSITION;

    Int32 count = 0;
    mExpandableListAdapter->GetGroupCount(&count);
    if (count == 0) {
        *result = IAdapterView::INVALID_POSITION;
        return NOERROR;
    }

    // If there isn't a selection don't hunt for it
    if (groupIdToMatch == IAdapterView::INVALID_ROW_ID) {
        *result = IAdapterView::INVALID_POSITION;
        return NOERROR;
    }

    // Pin seed to reasonable values
    seedGroupPosition = Elastos::Core::Math::Max(0, seedGroupPosition);
    seedGroupPosition = Elastos::Core::Math::Min(count - 1, seedGroupPosition);

    Int64 endTime = SystemClock::GetUptimeMillis() + AdapterView::SYNC_MAX_DURATION_MILLIS;
    Int64 rowId;
    Int32 first = seedGroupPosition;
    Int32 last = seedGroupPosition;
    Boolean next = FALSE;
    Boolean hitFirst;
    Boolean hitLast;
    AutoPtr<IExpandableListAdapter> adapter;
    GetAdapter((IExpandableListAdapter**)&adapter);
    if (adapter == NULL) {
        *result = IAdapterView::INVALID_POSITION;
        return NOERROR;
    }

    while (SystemClock::GetUptimeMillis() <= endTime) {
        adapter->GetGroupId(seedGroupPosition, &rowId);
        if (rowId == groupIdToMatch) {
            *result = seedGroupPosition;
            return NOERROR;
        }

        hitLast = (last == count - 1);
        hitFirst = (first == 0);

        if (hitLast && hitFirst) {
            // Looked at everything
            break;
        }

        if (hitFirst || (next && !hitLast)) {
            last++;
            seedGroupPosition = last;
            next = FALSE;
        }
        else if (hitLast || (!next && !hitFirst)) {
            --first;
            seedGroupPosition = first;
            next = TRUE;
        }
    }

    *result = IAdapterView::INVALID_POSITION;
    return NOERROR;
}

ECode ExpandableListConnector::RefreshExpGroupMetadataList(
    /* [in] */ Boolean forceChildrenCountRefresh,
    /* [in] */ Boolean syncGroupPositions)
{
    // ==================before translated======================
    // final ArrayList<GroupMetadata> egml = mExpGroupMetadataList;
    // int egmlSize = egml.size();
    // int curFlPos = 0;
    //
    // /* Update child count as we go through */
    // mTotalExpChildrenCount = 0;
    //
    // if (syncGroupPositions) {
    //     // We need to check whether any groups have moved positions
    //     boolean positionsChanged = false;
    //
    //     for (int i = egmlSize - 1; i >= 0; i--) {
    //         GroupMetadata curGm = egml.get(i);
    //         int newGPos = findGroupPosition(curGm.gId, curGm.gPos);
    //         if (newGPos != curGm.gPos) {
    //             if (newGPos == AdapterView.INVALID_POSITION) {
    //                 // Doh, just remove it from the list of expanded groups
    //                 egml.remove(i);
    //                 egmlSize--;
    //             }
    //
    //             curGm.gPos = newGPos;
    //             if (!positionsChanged) positionsChanged = true;
    //         }
    //     }
    //
    //     if (positionsChanged) {
    //         // At least one group changed positions, so re-sort
    //         Collections.sort(egml);
    //     }
    // }
    //
    // int gChildrenCount;
    // int lastGPos = 0;
    // for (int i = 0; i < egmlSize; i++) {
    //     /* Store in local variable since we'll access freq */
    //     GroupMetadata curGm = egml.get(i);
    //
    //     /*
    //      * Get the number of children, try to refrain from calling
    //      * another class's method unless we have to (so do a subtraction)
    //      */
    //     if ((curGm.lastChildFlPos == GroupMetadata.REFRESH) || forceChildrenCountRefresh) {
    //         gChildrenCount = mExpandableListAdapter.getChildrenCount(curGm.gPos);
    //     } else {
    //         /* Num children for this group is its last child's fl pos minus
    //          * the group's fl pos
    //          */
    //         gChildrenCount = curGm.lastChildFlPos - curGm.flPos;
    //     }
    //
    //     /* Update */
    //     mTotalExpChildrenCount += gChildrenCount;
    //
    //     /*
    //      * This skips the collapsed groups and increments the flat list
    //      * position (for subsequent exp groups) by accounting for the collapsed
    //      * groups
    //      */
    //     curFlPos += (curGm.gPos - lastGPos);
    //     lastGPos = curGm.gPos;
    //
    //     /* Update the flat list positions, and the current flat list pos */
    //     curGm.flPos = curFlPos;
    //     curFlPos += gChildrenCount;
    //     curGm.lastChildFlPos = curFlPos;
    // }

    AutoPtr<IList> egml = mExpGroupMetadataList;
    Int32 egmlSize = 0;
    egml->GetSize(&egmlSize);
    Int32 curFlPos = 0;
    mTotalExpChildrenCount = 0;

    if (syncGroupPositions) {
        // We need to check whether any groups have moved positions
        Boolean positionsChanged = FALSE;
        Int32 newGPos = 0;
        for (Int32 i = egmlSize - 1; i >= 0; --i) {
            AutoPtr<IInterface> interfaceTmp;
            egml->Get(i, (IInterface**)&interfaceTmp);
            IObject* objTmp = IObject::Probe(interfaceTmp);
            GroupMetadata* curGm = (GroupMetadata*)objTmp;
            FindGroupPosition(curGm->mGId, curGm->mGPos, &newGPos);
            if (newGPos != curGm->mGPos) {
                if (newGPos == IAdapterView::INVALID_POSITION) {
                    // Doh, just remove it from the list of expanded groups
                    egml->Remove(i);
                    --egmlSize;
                }

                curGm->mGPos = newGPos;
                if (!positionsChanged)
                    positionsChanged = TRUE;
            }
        }

        if (positionsChanged) {
            AutoPtr<ICollections> connection;
            CCollections::AcquireSingleton((ICollections**)&connection);
            connection->Sort(egml);
        }
    }

    Int32 gChildrenCount = 0;
    Int32 lastGPos = 0;
    for (Int32 i = 0; i < egmlSize; ++i) {
        AutoPtr<IInterface> interfaceTmp;
        egml->Get(i, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* curGm = (GroupMetadata*)objTmp;

        if (curGm->mLastChildFlPos == GroupMetadata::REFRESH || forceChildrenCountRefresh) {
            mExpandableListAdapter->GetChildrenCount(curGm->mGPos, &gChildrenCount);
        }
        else {
            gChildrenCount = curGm->mLastChildFlPos - curGm->mFlPos;
        }

        mTotalExpChildrenCount += gChildrenCount;
        curFlPos += (curGm->mGPos - lastGPos);
        lastGPos = curGm->mGPos;

        curGm->mFlPos = curFlPos;
        curFlPos += gChildrenCount;
        curGm->mLastChildFlPos = curFlPos;
    }
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos


