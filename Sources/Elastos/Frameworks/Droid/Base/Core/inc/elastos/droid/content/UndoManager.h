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

#ifndef __ELASTOS_DROID_CONTENT_UNDOMANAGER_H__
#define __ELASTOS_DROID_CONTENT_UNDOMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::IClassLoader;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Top-level class for managing and interacting with the global undo state for
 * a document or application.  This class supports both undo and redo and has
 * helpers for merging undoable operations together as they are performed.
 *
 * <p>A single undoable operation is represented by {@link UndoOperation} which
 * apps implement to define their undo/redo behavior.  The UndoManager keeps
 * a stack of undo states; each state can have one or more undo operations
 * inside of it.</p>
 *
 * <p>Updates to the stack must be done inside of a {@link #beginUpdate}/{@link #endUpdate()}
 * pair.  During this time you can add new operations to the stack with
 * {@link #addOperation}, retrieve and modify existing operations with
 * {@link #getLastOperation}, control the label shown to the user for this operation
 * with {@link #setUndoLabel} and {@link #suggestUndoLabel}, etc.</p>
 *
 * <p>Every {link UndoOperation} is associated with an {@link UndoOwner}, which identifies
 * the data it belongs to.  The owner is used to indicate how operations are dependent
 * on each other -- operations with the same owner are dependent on others with the
 * same owner.  For example, you may have a document with multiple embedded objects.  If the
 * document itself and each embedded object use different owners, then you
 * can provide undo semantics appropriate to the user's context: while within
 * an embedded object, only edits to that object are seen and the user can
 * undo/redo them without needing to impact edits in other objects; while
 * within the larger document, all edits can be seen and the user must
 * undo/redo them as a single stream.</p>
 *
 * @hide
 */
class UndoManager
    : public Object
    , public IUndoManager
{
private:
    class UndoState : public Object {
    public:
        UndoState(
            /* [in] */ UndoManager* manager,
            /* [in] */ Int32 commitId);

        UndoState(
            /* [in] */ UndoManager* manager,
            /* [in] */ IParcel* p,
            /* [in] */ IClassLoader* loader);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* p);

        Int32 GetCommitId();

        CARAPI SetLabel(
            /* [in] */ ICharSequence* label);

        CARAPI UpdateLabel(
            /* [in] */ ICharSequence* label);

        AutoPtr<ICharSequence> GetLabel();

        Boolean SetCanMerge(
            /* [in] */ Boolean state);

        CARAPI MakeExecuted();

        Boolean CanMerge();

        Int32 CountOperations();

        Boolean HasOperation(
            /* [in] */ IUndoOwner* owner);

        Boolean HasMultipleOwners();

        CARAPI AddOperation(
            /* [in] */ IUndoOperation* op);

        CARAPI GetLastOperation(
            /* [in] */ ClassID clazz,
            /* [inj] */ IUndoOwner* owner);

        Boolean MatchOwner(
            /* [in] */ IUndoOwner* owner);

        Boolean HasData();

        CARAPI Commit();

        CARAPI Undo();

        CARAPI Redo();

        CARAPI Destroy();

    private:
        UndoManager* mManager;
        Int32 mCommitId;
        AutoPtr<IArrayList> mOperations;// = new ArrayList<UndoOperation<?>>();
        AutoPtr<IArrayList> mRecent;
        AutoPtr<ICharSequence> mLabel;
        Boolean mCanMerge;// = true;
        Boolean mExecuted;

    };

public:
    CAR_INTERFACE_DECL()

    UndoManager();

    virtual ~UndoManager();

    CARAPI GetOwner(
        /* [in] */ const String& tag,
        /* [in] */ IInterface* data,
        /* [out] */ IUndoOwner** owner);

    CARAPI RemoveOwner(
        /* [in] */ IUndoOwner* owner);

    /**
     * Flatten the current undo state into a Parcelable object, which can later be restored
     * with {@link #restoreInstanceState(android.os.Parcelable)}.
     */
    CARAPI SaveInstanceState(
        /* [out] */ IParcelable** p);

    CARAPI SaveOwner(
        /* [in] */ IUndoOwner* owner,
        /* [in] */ IParcel* p);

    /**
     * Restore an undo state previously created with {@link #saveInstanceState()}.  This will
     * restore the UndoManager's state to almost exactly what it was at the point it had
     * been previously saved; the only information not restored is the data object
     * associated with each {@link UndoOwner}, which requires separate calls to
     * {@link #getOwner(String, Object)} to re-associate the owner with its data.
     */
    CARAPI RestoreInstanceState(
        /* [in] */ IParcelable* p);

    CARAPI RestoreOwner(
        /* [in] */ IParcel* p,
        /* [out] */ IUndoOwner** owner);

    /**
     * Set the maximum number of undo states that will be retained.
     */
    CARAPI SetHistorySize(
        /* [in] */ Int32 size);

    CARAPI GetHistorySize(
        /* [out] */ Int32* size);

    /**
     * Perform undo of last/top <var>count</var> undo states.  The states impacted
     * by this can be limited through <var>owners</var>.
     * @param owners Optional set of owners that should be impacted.  If null, all
     * undo states will be visible and available for undo.  If non-null, only those
     * states that contain one of the owners specified here will be visible.
     * @param count Number of undo states to pop.
     * @return Returns the number of undo states that were actually popped.
     */
    CARAPI Undo(
        /* [in] */ ArrayOf<IUndoOwner*> * owners,
        /* [in] */ Int32 count,
        /* [out] */ Int32* res);

    /**
     * Perform redo of last/top <var>count</var> undo states in the transient redo stack.
     * The states impacted by this can be limited through <var>owners</var>.
     * @param owners Optional set of owners that should be impacted.  If null, all
     * undo states will be visible and available for undo.  If non-null, only those
     * states that contain one of the owners specified here will be visible.
     * @param count Number of undo states to pop.
     * @return Returns the number of undo states that were actually redone.
     */
    CARAPI Redo(
        /* [in] */ ArrayOf<IUndoOwner*> * owners,
        /* [in] */ Int32 count,
        /* [out] */ Int32* res);

    /**
     * Returns true if we are currently inside of an undo/redo operation.  This is
     * useful for editors to know whether they should be generating new undo state
     * when they see edit operations happening.
     */
    CARAPI IsInUndo(
        /* [out] */ Boolean* res);

    CARAPI ForgetUndos(
        /* [in] */ ArrayOf<IUndoOwner*> * owners,
        /* [in] */ Int32 count,
        /* [out] */ Int32* res);

    CARAPI ForgetRedos(
        /* [in] */ ArrayOf<IUndoOwner*> * owners,
        /* [in] */ Int32 count,
        /* [out] */ Int32* res);

    /**
     * Return the number of undo states on the undo stack.
     * @param owners If non-null, only those states containing an operation with one of
     * the owners supplied here will be counted.
     */
    CARAPI CountUndos(
        /* [in] */ ArrayOf<IUndoOwner*> * owners,
        /* [out] */ Int32* res);

    /**
     * Return the number of redo states on the undo stack.
     * @param owners If non-null, only those states containing an operation with one of
     * the owners supplied here will be counted.
     */
    CARAPI CountRedos(
        /* [in] */ ArrayOf<IUndoOwner*> * owners,
        /* [out] */ Int32* res);

    /**
     * Return the user-visible label for the top undo state on the stack.
     * @param owners If non-null, will select the top-most undo state containing an
     * operation with one of the owners supplied here.
     */
    CARAPI GetUndoLabel(
        /* [in] */ ArrayOf<IUndoOwner*> * owners,
        /* [out] */ ICharSequence** csq);

    /**
     * Return the user-visible label for the top redo state on the stack.
     * @param owners If non-null, will select the top-most undo state containing an
     * operation with one of the owners supplied here.
     */
    CARAPI GetRedoLabel(
        /* [in] */ ArrayOf<IUndoOwner*> * owners,
        /* [out] */ ICharSequence** csq);

    /**
     * Start creating a new undo state.  Multiple calls to this function will nest until
     * they are all matched by a later call to {@link #endUpdate}.
     * @param label Optional user-visible label for this new undo state.
     */
    CARAPI BeginUpdate(
        /* [in] */ ICharSequence* label);

    /**
     * Returns true if currently inside of a {@link #beginUpdate}.
     */
    CARAPI IsInUpdate(
        /* [out] */ Boolean* res);

    /**
     * Forcibly set a new for the new undo state being built within a {@link #beginUpdate}.
     * Any existing label will be replaced with this one.
     */
    CARAPI SetUndoLabel(
        /* [in] */ ICharSequence* label);

    /**
     * Set a new for the new undo state being built within a {@link #beginUpdate}, but
     * only if there is not a label currently set for it.
     */
    CARAPI SuggestUndoLabel(
        /* [in] */ ICharSequence* label);

    /**
     * Return the number of times {@link #beginUpdate} has been called without a matching
     * {@link #endUpdate} call.
     */
    CARAPI GetUpdateNestingLevel(
        /* [out] */ Int32* res);

    /**
     * Check whether there is an {@link UndoOperation} in the current {@link #beginUpdate}
     * undo state.
     * @param owner Optional owner of the operation to look for.  If null, will succeed
     * if there is any operation; if non-null, will only succeed if there is an operation
     * with the given owner.
     * @return Returns true if there is a matching operation in the current undo state.
     */
    CARAPI HasOperation(
        /* [in] */ IUndoOwner* owner,
        /* [out] */ Boolean* res);

    /**
     * Return the most recent {@link UndoOperation} that was added to the update.
     * @param mergeMode May be either {@link #MERGE_MODE_NONE} or {@link #MERGE_MODE_ANY}.
     */
    CARAPI GetLastOperation(
        /* [in] */ Int32 mergeMode,
        /* [out] */ IUndoOperation** res);

    /**
     * Return the most recent {@link UndoOperation} that was added to the update and
     * has the given owner.
     * @param owner Optional owner of last operation to retrieve.  If null, the last
     * operation regardless of owner will be retrieved; if non-null, the last operation
     * matching the given owner will be retrieved.
     * @param mergeMode May be either {@link #MERGE_MODE_NONE}, {@link #MERGE_MODE_UNIQUE},
     * or {@link #MERGE_MODE_ANY}.
     */
    CARAPI GetLastOperation(
        /* [in] */ IUndoOwner* owner,
        /* [in] */ Int32 mergeMode,
        /* [out] */ IUndoOperation** res);

    /**
     * Return the most recent {@link UndoOperation} that was added to the update and
     * has the given owner.
     * @param clazz Optional class of the last operation to retrieve.  If null, the
     * last operation regardless of class will be retrieved; if non-null, the last
     * operation whose class is the same as the given class will be retrieved.
     * @param owner Optional owner of last operation to retrieve.  If null, the last
     * operation regardless of owner will be retrieved; if non-null, the last operation
     * matching the given owner will be retrieved.
     * @param mergeMode May be either {@link #MERGE_MODE_NONE}, {@link #MERGE_MODE_UNIQUE},
     * or {@link #MERGE_MODE_ANY}.
     */
    CARAPI GetLastOperation(
        /* [in] */ const ClassID& clazz,
        /* [in] */ IUndoOwner* owner,
        /* [in] */ Int32 mergeMode,
        /* [out] */ IInterface** res);

    /**
     * Add a new UndoOperation to the current update.
     * @param op The new operation to add.
     * @param mergeMode May be either {@link #MERGE_MODE_NONE}, {@link #MERGE_MODE_UNIQUE},
     * or {@link #MERGE_MODE_ANY}.
     */
    CARAPI AddOperation(
        /* [in] */ IUndoOperation* op,
        /* [in] */ Int32 mergeMode);

    /**
     * Finish the creation of an undo state, matching a previous call to
     * {@link #beginUpdate}.
     */
    CARAPI EndUpdate();

    /**
     * Commit the last finished undo state.  This undo state can no longer be
     * modified with further {@link #MERGE_MODE_UNIQUE} or
     * {@link #MERGE_MODE_ANY} merge modes.  If called while inside of an update,
     * this will push any changes in the current update on to the undo stack
     * and result with a fresh undo state, behaving as if {@link #endUpdate()}
     * had been called enough to unwind the current update, then the last state
     * committed, and {@link #beginUpdate} called to restore the update nesting.
     * @param owner The optional owner to determine whether to perform the commit.
     * If this is non-null, the commit will only execute if the current top undo
     * state contains an operation with the given owner.
     * @return Returns an integer identifier for the committed undo state, which
     * can later be used to try to uncommit the state to perform further edits on it.
     */
    CARAPI CommitState(
        /* [in] */ IUndoOwner* owner,
        /* [out] */ Int32* res);

    /**
     * Attempt to undo a previous call to {@link #commitState}.  This will work
     * if the undo state at the top of the stack has the given id, and has not been
     * involved in an undo operation.  Otherwise false is returned.
     * @param commitId The identifier for the state to be uncommitted, as returned
     * by {@link #commitState}.
     * @param owner Optional owner that must appear in the committed state.
     * @return Returns true if the uncommit is successful, else false.
     */
    CARAPI UncommitState(
        /* [in] */ Int32 commitId,
        /* [in] */ IUndoOwner* owner,
        /* [out] */ Boolean* res);

protected:
    AutoPtr<UndoState> GetTopUndo(
        /* [in] */ ArrayOf<IUndoOwner*>* owners);

    AutoPtr<UndoState> GetTopRedo(
        /* [in] */ ArrayOf<IUndoOwner*>* owners);

    Boolean MatchOwners(
        /* [in] */ UndoState* state,
        /* [in] */ ArrayOf<IUndoOwner*>*  owners);

    Int32 FindPrevState(
        /* [in] */ IArrayList* states,
        /* [in] */ ArrayOf<IUndoOwner*>* owners,
        /* [in] */ Int32 from);

    Int32 FindNextState(
        /* [in] */ IArrayList* states,
        /* [in] */ ArrayOf<IUndoOwner*>* owners,
        /* [in] */ Int32 from);

private:
    CARAPI CreateWorkingState();

    CARAPI PushWorkingState();

private:
    AutoPtr<HashMap<String, AutoPtr<IUndoOwner> > > mOwners;// = new HashMap<String, UndoOwner>();
    AutoPtr<IArrayList> mUndos;// = new ArrayList<UndoState>();
    AutoPtr<IArrayList> mRedos;// = new ArrayList<UndoState>();
    Int32 mUpdateCount;
    Int32 mHistorySize;// = 20;
    AutoPtr<UndoState> mWorking;
    Int32 mCommitId;// = 1;
    Boolean mInUndo;
    Boolean mMerged;

    Int32 mStateSeq;
    Int32 mNextSavedIdx;
    AutoPtr<ArrayOf<IUndoOwner*> > mStateOwners;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_UNDOMANAGER_H__
