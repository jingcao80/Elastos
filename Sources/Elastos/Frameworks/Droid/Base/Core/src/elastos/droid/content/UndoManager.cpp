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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/content/UndoManager.h"

namespace Elastos {
namespace Droid {
namespace Content {

UndoManager::UndoState::UndoState(
    /* [in] */ UndoManager* manager,
    /* [in] */ Int32 commitId)
    : mManager(manager)
    , mCommitId(commitId)
{
    assert(0 && "TODO");
}

UndoManager::UndoState::UndoState(
    /* [in] */ UndoManager* manager,
    /* [in] */ IParcel* p,
    /* [in] */ IClassLoader* loader)
    : mManager(manager)
{
    assert(0 && "TODO");

    // p->ReadInt32(&mCommitId);
    // Int32 ival;
    // p->ReadInt32(&ival);
    // mCanMerge = ival != 0;
    // p->ReadInt32(&ival);
    // mExecuted = ival != 0;
    // assert(0 && "TODO");
    // // mLabel = TextUtils::CHAR_SEQUENCE_CREATOR.CreateFromParcel(p);
    // Int32 N;
    // p->ReadInt32(&N);
    // for (Int32 i = 0; i < N; i++) {
    //     AutoPtr<IUndoOwner> owner;
    //     mManager->RestoreOwner(p, (IUndoOwner**)&owner);
    //     AutoPtr<UndoOperation> op = new UndoOperation();
    //     IParcelable::Probe(op)->ReadFromParcel(p);
    //     op->mOwner = owner;
    //     mOperations.add(op);
    // }
}

ECode UndoManager::UndoState::WriteToParcel(
    /* [in] */ IParcel* p)
{
    // if (mRecent != null) {
    //     throw new IllegalStateException("Can't save state before committing");
    // }
    // p.writeInt(mCommitId);
    // p.writeInt(mCanMerge ? 1 : 0);
    // p.writeInt(mExecuted ? 1 : 0);
    // TextUtils.writeToParcel(mLabel, p, 0);
    // final int N = mOperations.size();
    // p.writeInt(N);
    // for (int i=0; i<N; i++) {
    //     UndoOperation op = mOperations.get(i);
    //     mManager.saveOwner(op.mOwner, p);
    //     p.writeParcelable(op, 0);
    // }
    return NOERROR;
}

Int32 UndoManager::UndoState::GetCommitId()
{
    return mCommitId;
}

ECode UndoManager::UndoState::SetLabel(
    /* [in] */ ICharSequence* label)
{
    mLabel = label;
    return NOERROR;
}

ECode UndoManager::UndoState::UpdateLabel(
    /* [in] */ ICharSequence* label)
{
    if (mLabel != NULL) {
        mLabel = label;
    }
    return NOERROR;
}

AutoPtr<ICharSequence> UndoManager::UndoState::GetLabel()
{
    return mLabel;
}

Boolean UndoManager::UndoState::SetCanMerge(
    /* [in] */ Boolean state)
{
    // Don't allow re-enabling of merging if state has been executed.
    if (state && mExecuted) {
        return FALSE;
    }
    mCanMerge = state;
    return TRUE;
}

ECode UndoManager::UndoState::MakeExecuted()
{
    mExecuted = TRUE;
    return NOERROR;
}

Boolean UndoManager::UndoState::CanMerge()
{
    return mCanMerge && !mExecuted;
}

Int32 UndoManager::UndoState::CountOperations()
{
    //return mOperations.size();
    return 0;
}

Boolean UndoManager::UndoState::HasOperation(
    /* [in] */ IUndoOwner* owner)
{
    // final int N = mOperations.size();
    // if (owner == null) {
    //     return N != 0;
    // }
    // for (int i=0; i<N; i++) {
    //     if (mOperations.get(i).getOwner() == owner) {
    //         return true;
    //     }
    // }
    // return false;
    return TRUE;
}

Boolean UndoManager::UndoState::HasMultipleOwners()
{
    // final int N = mOperations.size();
    // if (N <= 1) {
    //     return false;
    // }
    // UndoOwner owner = mOperations.get(0).getOwner();
    // for (int i=1; i<N; i++) {
    //     if (mOperations.get(i).getOwner() != owner) {
    //         return true;
    //     }
    // }
    // return false;
    return TRUE;
}

ECode UndoManager::UndoState::AddOperation(
    /* [in] */ IUndoOperation* op)
{
    // if (mOperations.contains(op)) {
    //     throw new IllegalStateException("Already holds " + op);
    // }
    // mOperations.add(op);
    // if (mRecent == null) {
    //     mRecent = new ArrayList<UndoOperation<?>>();
    //     mRecent.add(op);
    // }
    // op.mOwner.mOpCount++;
    return NOERROR;
}

ECode UndoManager::UndoState::GetLastOperation(
    /* [in] */ ClassID clazz,
    /* [inj] */ IUndoOwner* owner)
{
    // final int N = mOperations.size();
    // if (clazz == null && owner == null) {
    //     return N > 0 ? (T)mOperations.get(N-1) : null;
    // }
    // // First look for the top-most operation with the same owner.
    // for (int i=N-1; i>=0; i--) {
    //     UndoOperation<?> op = mOperations.get(i);
    //     if (owner != null && op.getOwner() != owner) {
    //         continue;
    //     }
    //     // Return this operation if it has the same class that the caller wants.
    //     // Note that we don't search deeper for the class, because we don't want
    //     // to end up with a different order of operations for the same owner.
    //     if (clazz != null && op.getClass() != clazz) {
    //         return null;
    //     }
    //     return (T)op;
    // }

    // return null;
    return NOERROR;
}

Boolean UndoManager::UndoState::MatchOwner(
    /* [in] */ IUndoOwner* owner)
{
    // for (int i=mOperations.size()-1; i>=0; i--) {
    //     if (mOperations.get(i).matchOwner(owner)) {
    //         return true;
    //     }
    // }
    // return false;
    return TRUE;
}

Boolean UndoManager::UndoState::HasData()
{
    // for (int i=mOperations.size()-1; i>=0; i--) {
    //     if (mOperations.get(i).hasData()) {
    //         return true;
    //     }
    // }
    // return false;
    return TRUE;
}

ECode UndoManager::UndoState::Commit()
{
    // final int N = mRecent != null ? mRecent.size() : 0;
    // for (int i=0; i<N; i++) {
    //     mRecent.get(i).commit();
    // }
    // mRecent = null;
    return NOERROR;
}

ECode UndoManager::UndoState::Undo()
{
    // for (int i=mOperations.size()-1; i>=0; i--) {
    //     mOperations.get(i).undo();
    // }
    return NOERROR;
}

ECode UndoManager::UndoState::Redo()
{
    // final int N = mOperations.size();
    // for (int i=0; i<N; i++) {
    //     mOperations.get(i).redo();
    // }
    return NOERROR;
}

ECode UndoManager::UndoState::Destroy()
{
    // for (int i=mOperations.size()-1; i>=0; i--) {
    //     UndoOwner owner = mOperations.get(i).mOwner;
    //     owner.mOpCount--;
    //     if (owner.mOpCount <= 0) {
    //         if (owner.mOpCount < 0) {
    //             throw new IllegalStateException("Underflow of op count on owner " + owner
    //                     + " in op " + mOperations.get(i));
    //         }
    //         mManager.removeOwner(owner);
    //     }
    // }
    return NOERROR;
}


CAR_INTERFACE_IMPL(UndoManager, Object, IUndoManager)

UndoManager::UndoManager()
    : mUpdateCount(0)
    , mHistorySize(20)
    , mCommitId(-1)
    , mInUndo(FALSE)
    , mMerged(FALSE)
    , mStateSeq(0)
    , mNextSavedIdx(0)
{}

UndoManager::~UndoManager()
{}

ECode UndoManager::GetOwner(
    /* [in] */ const String& tag,
    /* [in] */ IInterface* data,
    /* [out] */ IUndoOwner** owner)
{
    // if (tag == null) {
    //     throw new NullPointerException("tag can't be null");
    // }
    // if (data == null) {
    //     throw new NullPointerException("data can't be null");
    // }
    // UndoOwner owner = mOwners.get(tag);
    // if (owner != null) {
    //     if (owner.mData != data) {
    //         if (owner.mData != null) {
    //             throw new IllegalStateException("Owner " + owner + " already exists with data "
    //                     + owner.mData + " but giving different data " + data);
    //         }
    //         owner.mData = data;
    //     }
    //     return owner;
    // }

    // owner = new UndoOwner(tag);
    // owner.mManager = this;
    // owner.mData = data;
    // mOwners.put(tag, owner);
    // return owner;
    return NOERROR;
}

ECode UndoManager::RemoveOwner(
    /* [in] */ IUndoOwner* owner)
{
    // // XXX need to figure out how to prune.
    // if (false) {
    //     mOwners.remove(owner.mTag);
    //     owner.mManager = null;
    // }
    return NOERROR;
}

ECode UndoManager::SaveInstanceState(
    /* [out] */ IParcelable** p)
{
    // if (mUpdateCount > 0) {
    //     throw new IllegalStateException("Can't save state while updating");
    // }
    // ParcelableParcel pp = new ParcelableParcel(getClass().getClassLoader());
    // Parcel p = pp.getParcel();
    // mStateSeq++;
    // if (mStateSeq <= 0) {
    //     mStateSeq = 0;
    // }
    // mNextSavedIdx = 0;
    // p.writeInt(mHistorySize);
    // p.writeInt(mOwners.size());
    // // XXX eventually we need to be smart here about limiting the
    // // number of undo states we write to not exceed X bytes.
    // int i = mUndos.size();
    // while (i > 0) {
    //     p.writeInt(1);
    //     i--;
    //     mUndos.get(i).writeToParcel(p);
    // }
    // i = mRedos.size();
    // p.writeInt(i);
    // while (i > 0) {
    //     p.writeInt(2);
    //     i--;
    //     mRedos.get(i).writeToParcel(p);
    // }
    // p.writeInt(0);
    // return pp;
    return NOERROR;
}

ECode UndoManager::SaveOwner(
    /* [in] */ IUndoOwner* owner,
    /* [in] */ IParcel* p)
{
    // if (owner.mStateSeq == mStateSeq) {
    //     out.writeInt(owner.mSavedIdx);
    // } else {
    //     owner.mStateSeq = mStateSeq;
    //     owner.mSavedIdx = mNextSavedIdx;
    //     out.writeInt(owner.mSavedIdx);
    //     out.writeString(owner.mTag);
    //     mNextSavedIdx++;
    // }
    return NOERROR;
}

ECode UndoManager::RestoreInstanceState(
    /* [in] */ IParcelable* p)
{
    // if (mUpdateCount > 0) {
    //     throw new IllegalStateException("Can't save state while updating");
    // }
    // forgetUndos(null, -1);
    // forgetRedos(null, -1);
    // ParcelableParcel pp = (ParcelableParcel)state;
    // Parcel p = pp.getParcel();
    // mHistorySize = p->ReadInt32();
    // mStateOwners = new UndoOwner[p->ReadInt32()];

    // int stype;
    // while ((stype=p->ReadInt32()) != 0) {
    //     UndoState ustate = new UndoState(this, p, pp.getClassLoader());
    //     if (stype == 1) {
    //         mUndos.add(0, ustate);
    //     } else {
    //         mRedos.add(0, ustate);
    //     }
    // }
    return NOERROR;
}

ECode UndoManager::RestoreOwner(
    /* [in] */ IParcel* p,
    /* [out] */ IUndoOwner** owner)
{
    // int idx = in->ReadInt32();
    // UndoOwner owner = mStateOwners[idx];
    // if (owner == null) {
    //     String tag = in.readString();
    //     owner = new UndoOwner(tag);
    //     mStateOwners[idx] = owner;
    //     mOwners.put(tag, owner);
    // }
    // return owner;
    return NOERROR;
}

ECode UndoManager::SetHistorySize(
    /* [in] */ Int32 size)
{
    // mHistorySize = size;
    // if (mHistorySize >= 0 && countUndos(null) > mHistorySize) {
    //     forgetUndos(null, countUndos(null) - mHistorySize);
    // }
    return NOERROR;
}

ECode UndoManager::GetHistorySize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mHistorySize;
    return NOERROR;
}

ECode UndoManager::Undo(
    /* [in] */ ArrayOf<IUndoOwner*> * owners,
    /* [in] */ Int32 count,
    /* [out] */ Int32* res)
{
    // if (mWorking != null) {
    //     throw new IllegalStateException("Can't be called during an update");
    // }

    // int num = 0;
    // int i = -1;

    // mInUndo = true;

    // UndoState us = getTopUndo(null);
    // if (us != null) {
    //     us.makeExecuted();
    // }

    // while (count > 0 && (i=findPrevState(mUndos, owners, i)) >= 0) {
    //     UndoState state = mUndos.remove(i);
    //     state.undo();
    //     mRedos.add(state);
    //     count--;
    //     num++;
    // }

    // mInUndo = false;

    // return num;
    return NOERROR;
}

ECode UndoManager::Redo(
    /* [in] */ ArrayOf<IUndoOwner*> * owners,
    /* [in] */ Int32 count,
    /* [out] */ Int32* res)
{
    // if (mWorking != null) {
    //     throw new IllegalStateException("Can't be called during an update");
    // }

    // int num = 0;
    // int i = -1;

    // mInUndo = true;

    // while (count > 0 && (i=findPrevState(mRedos, owners, i)) >= 0) {
    //     UndoState state = mRedos.remove(i);
    //     state.redo();
    //     mUndos.add(state);
    //     count--;
    //     num++;
    // }

    // mInUndo = false;

    // return num;
    return NOERROR;
}

ECode UndoManager::IsInUndo(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mInUndo;
    return NOERROR;
}

ECode UndoManager::ForgetUndos(
    /* [in] */ ArrayOf<IUndoOwner*> * owners,
    /* [in] */ Int32 count,
    /* [out] */ Int32* res)
{
    // if (count < 0) {
    //     count = mUndos.size();
    // }

    // int removed = 0;
    // for (int i=0; i<mUndos.size() && removed < count; i++) {
    //     UndoState state = mUndos.get(i);
    //     if (count > 0 && matchOwners(state, owners)) {
    //         state.destroy();
    //         mUndos.remove(i);
    //         removed++;
    //     }
    // }

    // return removed;
    return NOERROR;
}

ECode UndoManager::ForgetRedos(
    /* [in] */ ArrayOf<IUndoOwner*> * owners,
    /* [in] */ Int32 count,
    /* [out] */ Int32* res)
{
    // if (count < 0) {
    //     count = mRedos.size();
    // }

    // int removed = 0;
    // for (int i=0; i<mRedos.size() && removed < count; i++) {
    //     UndoState state = mRedos.get(i);
    //     if (count > 0 && matchOwners(state, owners)) {
    //         state.destroy();
    //         mRedos.remove(i);
    //         removed++;
    //     }
    // }

    // return removed;
    return NOERROR;
}

ECode UndoManager::CountUndos(
    /* [in] */ ArrayOf<IUndoOwner*> * owners,
    /* [out] */ Int32* res)
{
    // if (owners == null) {
    //     return mUndos.size();
    // }

    // int count=0;
    // int i=0;
    // while ((i=findNextState(mUndos, owners, i)) >= 0) {
    //     count++;
    //     i++;
    // }
    // return count;
    return NOERROR;
}

ECode UndoManager::CountRedos(
    /* [in] */ ArrayOf<IUndoOwner*> * owners,
    /* [out] */ Int32* res)
{
    // if (owners == null) {
    //     return mRedos.size();
    // }

    // int count=0;
    // int i=0;
    // while ((i=findNextState(mRedos, owners, i)) >= 0) {
    //     count++;
    //     i++;
    // }
    // return count;
    return NOERROR;
}

ECode UndoManager::GetUndoLabel(
    /* [in] */ ArrayOf<IUndoOwner*> * owners,
    /* [out] */ ICharSequence** csq)
{
    // UndoState state = getTopUndo(owners);
    // return state != null ? state.getLabel() : null;
    return NOERROR;
}

ECode UndoManager::GetRedoLabel(
    /* [in] */ ArrayOf<IUndoOwner*> * owners,
    /* [out] */ ICharSequence** csq)
{
    // UndoState state = getTopRedo(owners);
    // return state != null ? state.getLabel() : null;
    return NOERROR;
}

ECode UndoManager::BeginUpdate(
    /* [in] */ ICharSequence* label)
{
    // if (mInUndo) {
    //     throw new IllegalStateException("Can't being update while performing undo/redo");
    // }
    // if (mUpdateCount <= 0) {
    //     createWorkingState();
    //     mMerged = false;
    //     mUpdateCount = 0;
    // }

    // mWorking.updateLabel(label);
    // mUpdateCount++;
    return NOERROR;
}

ECode UndoManager::CreateWorkingState()
{
    // mWorking = new UndoState(this, mCommitId++);
    // if (mCommitId < 0) {
    //     mCommitId = 1;
    // }
    return NOERROR;
}

ECode UndoManager::IsInUpdate(
    /* [out] */ Boolean* res)
{
    // return mUpdateCount > 0;
    return NOERROR;
}

ECode UndoManager::SetUndoLabel(
    /* [in] */ ICharSequence* label)
{
    // if (mWorking == null) {
    //     throw new IllegalStateException("Must be called during an update");
    // }
    // mWorking.setLabel(label);
    return NOERROR;
}

ECode UndoManager::SuggestUndoLabel(
    /* [in] */ ICharSequence* label)
{
    // if (mWorking == null) {
    //     throw new IllegalStateException("Must be called during an update");
    // }
    // mWorking.updateLabel(label);
    return NOERROR;
}

ECode UndoManager::GetUpdateNestingLevel(
    /* [out] */ Int32* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mUpdateCount;
    return NOERROR;
}

ECode UndoManager::HasOperation(
    /* [in] */ IUndoOwner* owner,
    /* [out] */ Boolean* res)
{
    // if (mWorking == null) {
    //     throw new IllegalStateException("Must be called during an update");
    // }
    // return mWorking.hasOperation(owner);
    return NOERROR;
}

ECode UndoManager::GetLastOperation(
    /* [in] */ Int32 mergeMode,
    /* [out] */ IUndoOperation** res)
{
    //return getLastOperation(null, null, mergeMode);
    return NOERROR;
}

ECode UndoManager::GetLastOperation(
    /* [in] */ IUndoOwner* owner,
    /* [in] */ Int32 mergeMode,
    /* [out] */ IUndoOperation** res)
{
    //return getLastOperation(null, owner, mergeMode);
    return NOERROR;
}

ECode UndoManager::GetLastOperation(
    /* [in] */ const ClassID& clazz,
    /* [in] */ IUndoOwner* owner,
    /* [in] */ Int32 mergeMode,
    /* [out] */ IInterface** res)
{
    // if (mWorking == null) {
    //     throw new IllegalStateException("Must be called during an update");
    // }
    // if (mergeMode != MERGE_MODE_NONE && !mMerged && !mWorking.hasData()) {
    //     UndoState state = getTopUndo(null);
    //     UndoOperation<?> last;
    //     if (state != null && (mergeMode == MERGE_MODE_ANY || !state.hasMultipleOwners())
    //             && state.canMerge() && (last=state.getLastOperation(clazz, owner)) != null) {
    //         if (last.allowMerge()) {
    //             mWorking.destroy();
    //             mWorking = state;
    //             mUndos.remove(state);
    //             mMerged = true;
    //             return (T)last;
    //         }
    //     }
    // }

    // return mWorking.getLastOperation(clazz, owner);
    return NOERROR;
}

ECode UndoManager::AddOperation(
    /* [in] */ IUndoOperation* op,
    /* [in] */ Int32 mergeMode)
{
    // if (mWorking == null) {
    //     throw new IllegalStateException("Must be called during an update");
    // }
    // UndoOwner owner = op.getOwner();
    // if (owner.mManager != this) {
    //     throw new IllegalArgumentException(
    //             "Given operation's owner is not in this undo manager.");
    // }
    // if (mergeMode != MERGE_MODE_NONE && !mMerged && !mWorking.hasData()) {
    //     UndoState state = getTopUndo(null);
    //     if (state != null && (mergeMode == MERGE_MODE_ANY || !state.hasMultipleOwners())
    //             && state.canMerge() && state.hasOperation(op.getOwner())) {
    //         mWorking.destroy();
    //         mWorking = state;
    //         mUndos.remove(state);
    //         mMerged = true;
    //     }
    // }
    // mWorking.addOperation(op);
    return NOERROR;
}

ECode UndoManager::EndUpdate()
{
    // if (mWorking == null) {
    //     throw new IllegalStateException("Must be called during an update");
    // }
    // mUpdateCount--;

    // if (mUpdateCount == 0) {
    //     pushWorkingState();
    // }
    return NOERROR;
}

ECode UndoManager::PushWorkingState()
{
    // int N = mUndos.size() + 1;

    // if (mWorking.hasData()) {
    //     mUndos.add(mWorking);
    //     forgetRedos(null, -1);
    //     mWorking.commit();
    //     if (N >= 2) {
    //         // The state before this one can no longer be merged, ever.
    //         // The only way to get back to it is for the user to perform
    //         // an undo.
    //         mUndos.get(N-2).makeExecuted();
    //     }
    // } else {
    //     mWorking.destroy();
    // }
    // mWorking = null;

    // if (mHistorySize >= 0 && N > mHistorySize) {
    //     forgetUndos(null, N - mHistorySize);
    // }
    return NOERROR;
}

ECode UndoManager::CommitState(
    /* [in] */ IUndoOwner* owner,
    /* [out] */ Int32* res)
{
    // if (mWorking != null && mWorking.hasData()) {
    //     if (owner == null || mWorking.hasOperation(owner)) {
    //         mWorking.setCanMerge(false);
    //         int commitId = mWorking.getCommitId();
    //         pushWorkingState();
    //         createWorkingState();
    //         mMerged = true;
    //         return commitId;
    //     }
    // } else {
    //     UndoState state = getTopUndo(null);
    //     if (state != null && (owner == null || state.hasOperation(owner))) {
    //         state.setCanMerge(false);
    //         return state.getCommitId();
    //     }
    // }
    // return -1;
    return NOERROR;
}

ECode UndoManager::UncommitState(
    /* [in] */ Int32 commitId,
    /* [in] */ IUndoOwner* owner,
    /* [out] */ Boolean* res)
{
    // if (mWorking != null && mWorking.getCommitId() == commitId) {
    //     if (owner == null || mWorking.hasOperation(owner)) {
    //         return mWorking.setCanMerge(true);
    //     }
    // } else {
    //     UndoState state = getTopUndo(null);
    //     if (state != null && (owner == null || state.hasOperation(owner))) {
    //         if (state.getCommitId() == commitId) {
    //             return state.setCanMerge(true);
    //         }
    //     }
    // }
    // return false;
    return NOERROR;
}

AutoPtr<UndoManager::UndoState> UndoManager::GetTopUndo(
    /* [in] */ ArrayOf<IUndoOwner*>* owners)
{
    // if (mUndos.size() <= 0) {
    //     return null;
    // }
    // int i = findPrevState(mUndos, owners, -1);
    // return i >= 0 ? mUndos.get(i) : null;
    return NULL;
}

AutoPtr<UndoManager::UndoState> UndoManager::GetTopRedo(
    /* [in] */ ArrayOf<IUndoOwner*>* owners)
{
    // if (mRedos.size() <= 0) {
    //     return null;
    // }
    // int i = findPrevState(mRedos, owners, -1);
    // return i >= 0 ? mRedos.get(i) : null;
    return NULL;
}

Boolean UndoManager::MatchOwners(
    /* [in] */ UndoState* state,
    /* [in] */ ArrayOf<IUndoOwner*>*  owners)
{
    // if (owners == null) {
    //     return true;
    // }
    // for (int i=0; i<owners.length; i++) {
    //     if (state.matchOwner(owners[i])) {
    //         return true;
    //     }
    // }
    // return false;
    return FALSE;
}

Int32 UndoManager::FindPrevState(
    /* [in] */ IArrayList* states,
    /* [in] */ ArrayOf<IUndoOwner*>* owners,
    /* [in] */ Int32 from)
{
    // final int N = states.size();

    // if (from == -1) {
    //     from = N-1;
    // }
    // if (from >= N) {
    //     return -1;
    // }
    // if (owners == null) {
    //     return from;
    // }

    // while (from >= 0) {
    //     UndoState state = states.get(from);
    //     if (matchOwners(state, owners)) {
    //         return from;
    //     }
    //     from--;
    // }

    // return -1;
    return 0;
}

Int32 UndoManager::FindNextState(
    /* [in] */ IArrayList* states,
    /* [in] */ ArrayOf<IUndoOwner*>* owners,
    /* [in] */ Int32 from)
{
    // final int N = states.size();

    // if (from < 0) {
    //     from = 0;
    // }
    // if (from >= N) {
    //     return -1;
    // }
    // if (owners == null) {
    //     return from;
    // }

    // while (from < N) {
    //     UndoState state = states.get(from);
    //     if (matchOwners(state, owners)) {
    //         return from;
    //     }
    //     from++;
    // }

    // return -1;
    return 0;
}


}
}
}
