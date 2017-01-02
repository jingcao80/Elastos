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

#ifndef __ELASTOS_DROID_CONTENT_UNDOOPERATION_H__
#define __ELASTOS_DROID_CONTENT_UNDOOPERATION_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * A single undoable operation.  You must subclass this to implement the state
 * and behavior for your operation.  Instances of this class are placed and
 * managed in an {@link UndoManager}.
 *
 * @hide
 */
class UndoOperation
    : public Object
    , public IUndoOperation
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    UndoOperation();

    virtual ~UndoOperation();

    CARAPI constructor();

    /**
     * Create a new instance of the operation.
     * @param owner Who owns the data being modified by this undo state; must be
     * returned by {@link UndoManager#getOwner(String, Object) UndoManager.getOwner}.
     */
    CARAPI constructor(
        /* [in] */ IUndoOwner* owner);

    /**
     * Owning object as given to {@link #UndoOperation(UndoOwner)}.
     */
    CARAPI GetOwner(
        /* [out] */ IUndoOwner** owner);

    /**
     * Synonym for {@link #getOwner()}.{@link android.content.UndoOwner#getData()}.
     */
    CARAPI GetOwnerData(
        /* [out] */ IInterface** obj);

    /**
     * Return true if this undo operation is a member of the given owner.
     * The default implementation is <code>owner == getOwner()</code>.  You
     * can override this to provide more sophisticated dependencies between
     * owners.
     */
    CARAPI MatchOwner(
        /* [in] */ IUndoOwner* owner,
        /* [out] */ Boolean* result);

    /**
     * Return true if this operation actually contains modification data.  The
     * default implementation always returns true.  If you return false, the
     * operation will be dropped when the final undo state is being built.
     */
    CARAPI HasData(
        /* [out] */ Boolean* result);

    /**
     * Return true if this operation can be merged with a later operation.
     * The default implementation always returns true.
     */
    CARAPI AllowMerge(
        /* [out] */ Boolean* result);

    /**
     * Called when this undo state is being committed to the undo stack.
     * The implementation should perform the initial edits and save any state that
     * may be needed to undo them.
     */
    virtual CARAPI Commit() = 0;

    /**
     * Called when this undo state is being popped off the undo stack (in to
     * the temporary redo stack).  The implementation should remove the original
     * edits and thus restore the target object to its prior value.
     */
    virtual CARAPI Undo() = 0;
    /**
     * Called when this undo state is being pushed back from the transient
     * redo stack to the main undo stack.  The implementation should re-apply
     * the edits that were previously removed by {@link #undo}.
     */
    virtual CARAPI Redo() = 0;

protected:
    AutoPtr<IUndoOwner> mOwner;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_UNDOOPERATION_H__
