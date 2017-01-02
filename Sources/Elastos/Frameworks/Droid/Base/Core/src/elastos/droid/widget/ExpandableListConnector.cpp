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

CAR_INTERFACE_IMPL_3(ExpandableListConnector::GroupMetadata, Object,
        IGroupMetadata, IParcelable, IComparable)

ExpandableListConnector::GroupMetadata::GroupMetadata()
    : mFlPos(0)
    , mLastChildFlPos(0)
    , mGPos(0)
    , mGId(0)
{}

AutoPtr<ExpandableListConnector::GroupMetadata> ExpandableListConnector::GroupMetadata::Obtain(
    /* [in] */ Int32 flPos,
    /* [in] */ Int32 lastChildFlPos,
    /* [in] */ Int32 gPos,
    /* [in] */ Int64 gId)
{
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
    VALIDATE_NOT_NULL(result)

    IGroupMetadata* objTmp = IGroupMetadata::Probe(another);
    GroupMetadata* anotherTmp = (GroupMetadata*)objTmp;
    if(anotherTmp == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = this->mGPos - anotherTmp->mGPos;
    return NOERROR;
}

ECode ExpandableListConnector::GroupMetadata::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest)

    dest->WriteInt32(mFlPos);
    dest->WriteInt32(mLastChildFlPos);
    dest->WriteInt32(mGPos);
    dest->WriteInt64(mGId);
    return NOERROR;
}

ECode ExpandableListConnector::GroupMetadata::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source)

    source->ReadInt32(&mFlPos);
    source->ReadInt32(&mLastChildFlPos);
    source->ReadInt32(&mGPos);
    source->ReadInt64(&mGId);
    return NOERROR;
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
    AutoPtr<PositionMetadata> pm = GetRecycledOrCreate();
    pm->mPosition = ExpandableListPosition::Obtain(type, groupPos, childPos,flatListPos);
    pm->mGroupMetadata = groupMetadata;
    pm->mGroupInsertIndex = groupInsertIndex;
    return pm;
}

ECode ExpandableListConnector::PositionMetadata::Recycle()
{
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
    VALIDATE_NOT_NULL(result)

    *result = mGroupMetadata != NULL;
    return NOERROR;
}

ExpandableListConnector::PositionMetadata::PositionMetadata()
    : mGroupInsertIndex(0)
{
}

ECode ExpandableListConnector::PositionMetadata::ResetState()
{
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
ExpandableListConnector::MyDataSetObserver::MyDataSetObserver(
    /* [in] */ ExpandableListConnector* owner)
    : mOwner(owner)
{
    assert(mOwner);
}

ECode ExpandableListConnector::MyDataSetObserver::OnChanged()
{
    mOwner->RefreshExpGroupMetadataList(TRUE, TRUE);
    mOwner->NotifyDataSetChanged();
    return NOERROR;
}

ECode ExpandableListConnector::MyDataSetObserver::OnInvalidated()
{
    mOwner->RefreshExpGroupMetadataList(TRUE, TRUE);
    mOwner->NotifyDataSetInvalidated();
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
    CArrayList::New((IList**)&mExpGroupMetadataList);
    SetExpandableListAdapter(expandableListAdapter);
    return NOERROR;
}

ECode ExpandableListConnector::SetExpandableListAdapter(
    /* [in] */ IExpandableListAdapter* expandableListAdapter)
{
    VALIDATE_NOT_NULL(expandableListAdapter)

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
    VALIDATE_NOT_NULL(result)

    AutoPtr<IList> egml = mExpGroupMetadataList;
    Int32 numExpGroups = 0;
    egml->GetSize(&numExpGroups);
    Int32 leftExpGroupIndex = 0;
    Int32 rightExpGroupIndex = numExpGroups - 1;
    Int32 midExpGroupIndex = 0;
    AutoPtr<GroupMetadata> midExpGm;

    if (numExpGroups == 0) {
        AutoPtr<IPositionMetadata> temp = PositionMetadata::Obtain(flPos, IExpandableListPosition::GROUP, flPos, -1, NULL, 0);
        *result = temp;
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
            AutoPtr<IPositionMetadata> temp = PositionMetadata::Obtain(flPos, IExpandableListPosition::GROUP,
                midExpGm->mGPos, -1, midExpGm, midExpGroupIndex);
            *result = temp;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        else if (flPos <= midExpGm->mLastChildFlPos) {
            Int32 childPos = flPos - (midExpGm.Get()->mFlPos + 1);
            AutoPtr<IPositionMetadata> temp = PositionMetadata::Obtain(flPos, ExpandableListPosition::CHILD,
                midExpGm->mGPos, childPos, midExpGm, midExpGroupIndex);
            *result = temp;
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

    AutoPtr<IPositionMetadata> temp = PositionMetadata::Obtain(flPos, ExpandableListPosition::GROUP, groupPos, -1, NULL, insertPosition);
    *result = temp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ExpandableListConnector::GetFlattenedPos(
    /* [in] */ IExpandableListPosition* pos,
    /* [out] */ IPositionMetadata** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    VALIDATE_NOT_NULL(pos)

    AutoPtr<IList> egml = mExpGroupMetadataList;
    Int32 numExpGroups = 0;
    egml->GetSize(&numExpGroups);
    Int32 leftExpGroupIndex = 0;
    Int32 rightExpGroupIndex = numExpGroups - 1;
    Int32 midExpGroupIndex = 0;
    AutoPtr<GroupMetadata> midExpGm;

    ExpandableListPosition* elp = (ExpandableListPosition*)pos;
    if (numExpGroups == 0) {
        AutoPtr<IPositionMetadata> temp = PositionMetadata::Obtain(elp->mGroupPos,
            elp->mType, elp->mGroupPos, elp->mChildPos, NULL, 0);
        *result = temp;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    while (leftExpGroupIndex <= rightExpGroupIndex) {
        midExpGroupIndex = (rightExpGroupIndex - leftExpGroupIndex)/2 + leftExpGroupIndex;

        AutoPtr<IInterface> interfaceTmp;
        egml->Get(midExpGroupIndex, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        midExpGm = (GroupMetadata*)objTmp;

        if (elp->mGroupPos > midExpGm->mGPos) {
            leftExpGroupIndex = midExpGroupIndex + 1;
        }
        else if (elp->mGroupPos < midExpGm->mGPos) {
            rightExpGroupIndex = midExpGroupIndex - 1;
        }
        else if (elp->mGroupPos == midExpGm->mGPos) {
            if (elp->mType == IExpandableListPosition::GROUP) {
                AutoPtr<IPositionMetadata> temp = PositionMetadata::Obtain(midExpGm->mFlPos, elp->mType,
                    elp->mGroupPos, elp->mChildPos, midExpGm, midExpGroupIndex);
                *result = temp;
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
            else if (elp->mType == IExpandableListPosition::CHILD) {
                AutoPtr<IPositionMetadata> temp = PositionMetadata::Obtain(midExpGm->mFlPos + elp->mChildPos + 1,
                    elp->mType, elp->mGroupPos, elp->mChildPos, midExpGm, midExpGroupIndex);
                *result = temp;
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
            return NOERROR;
        }
    }

    if (elp->mType != IExpandableListPosition::GROUP) {
        return NOERROR;
    }

    if (leftExpGroupIndex > midExpGroupIndex) {
        AutoPtr<IInterface> interfaceTmp;
        egml->Get(leftExpGroupIndex-1, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* leftExpGm = (GroupMetadata*)objTmp;

        Int32 flPos = leftExpGm->mLastChildFlPos + (elp->mGroupPos - leftExpGm->mGPos);
        AutoPtr<IPositionMetadata> temp = PositionMetadata::Obtain(flPos, elp->mType, elp->mGroupPos,
            elp->mChildPos, NULL, leftExpGroupIndex);
        *result = temp;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (rightExpGroupIndex < midExpGroupIndex) {
        AutoPtr<IInterface> interfaceTmp;
        egml->Get(++rightExpGroupIndex, (IInterface**)&interfaceTmp);
        IObject* objTmp = IObject::Probe(interfaceTmp);
        GroupMetadata* rightExpGm = (GroupMetadata*)objTmp;

        Int32 flPos = rightExpGm->mFlPos - (rightExpGm->mGPos - elp->mGroupPos);
        AutoPtr<IPositionMetadata> temp = PositionMetadata::Obtain(flPos, elp->mType, elp->mGroupPos,
            elp->mChildPos, NULL, rightExpGroupIndex);
        *result = temp;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }

    return NOERROR;
}

Boolean ExpandableListConnector::AreAllItemsEnabled()
{
    Boolean enabled;
    mExpandableListAdapter->AreAllItemsEnabled(&enabled);
    return enabled;
}

Boolean ExpandableListConnector::IsEnabled(
    /* [in] */ Int32 flatListPos)
{
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
    VALIDATE_NOT_NULL(result)

    Int32 count = 0;
    mExpandableListAdapter->GetGroupCount(&count);
    *result = count + mTotalExpChildrenCount;
    return NOERROR;
}

ECode ExpandableListConnector::GetItem(
    /* [in] */ Int32 flatListPos,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IPositionMetadata> metadata;
    GetUnflattenedPos(flatListPos, (IPositionMetadata**)&metadata);
    PositionMetadata* posMetadata = (PositionMetadata*)metadata.Get();

    if (posMetadata->mPosition->mType == IExpandableListPosition::GROUP) {
        mExpandableListAdapter->GetGroup(posMetadata->mPosition->mGroupPos, result);
    }
    else if (posMetadata->mPosition->mType == IExpandableListPosition::CHILD) {
        mExpandableListAdapter->GetChild(posMetadata->mPosition->mGroupPos,
            posMetadata->mPosition->mChildPos, result);
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
    VALIDATE_NOT_NULL(result)

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

    AutoPtr<IPositionMetadata> metadata;
    GetUnflattenedPos(flatListPos, (IPositionMetadata**)&metadata);
    PositionMetadata* posMetadata = (PositionMetadata*)metadata.Get();

    Boolean expanded = FALSE;
    if (posMetadata->mPosition->mType == IExpandableListPosition::GROUP) {
        mExpandableListAdapter->GetGroupView(posMetadata->mPosition->mGroupPos,
            (posMetadata->IsExpanded(&expanded), expanded), convertView, parent, result);
    }
    else if (posMetadata->mPosition->mType == IExpandableListPosition::CHILD) {
        Boolean isLastChild = posMetadata->mGroupMetadata->mLastChildFlPos == flatListPos;

        mExpandableListAdapter->GetChildView(posMetadata->mPosition->mGroupPos,
            posMetadata->mPosition->mChildPos, isLastChild, convertView, parent, result);
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
    VALIDATE_NOT_NULL(result)

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
    VALIDATE_NOT_NULL(result)

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
    mMaxExpGroupCount = maxExpGroupCount;
    return NOERROR;
}

ECode ExpandableListConnector::GetAdapter(
    /* [out] */ IExpandableListAdapter** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mExpandableListAdapter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ExpandableListConnector::GetFilter(
    /* [out] */ IFilter** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IExpandableListAdapter> adapter;
    GetAdapter((IExpandableListAdapter**)&adapter);
    IFilterable* filter = IFilterable::Probe(adapter);
    if (filter != NULL) {
        filter->GetFilter(result);
    }
    else {
        *result = NULL;
    }
    return NOERROR;
}

ECode ExpandableListConnector::GetExpandedGroupMetadataList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mExpGroupMetadataList;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode ExpandableListConnector::SetExpandedGroupMetadataList(
    /* [in] */ IList* expandedGroupMetadataList)
{
    VALIDATE_NOT_NULL(expandedGroupMetadataList)

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
    VALIDATE_NOT_NULL(result)

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


