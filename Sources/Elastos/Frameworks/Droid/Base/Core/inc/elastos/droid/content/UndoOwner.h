
#ifndef __ELASTOS_DROID_CONTENT_UNDOOWNER_H__
#define __ELASTOS_DROID_CONTENT_UNDOOWNER_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Representation of an owner of {@link UndoOperation} objects in an {@link UndoManager}.
 *
 * @hide
 */
class UndoOwner
    : public Object
    , public IUndoOwner
{
public:
    CAR_INTERFACE_DECL()

    UndoOwner();

    virtual ~UndoOwner();

    CARAPI constructor(
        /* [in] */ const String& tag);

    CARAPI GetTag(
        /* [out] */ String* tag);

    CARAPI GetData(
        /* [out] */ IInterface** data);

private:
    String mTag;

    AutoPtr<IUndoManager> mManager;
    AutoPtr<IInterface> mData;
    Int32 mOpCount;

    // For saving/restoring state.
    Int32 mStateSeq;
    Int32 mSavedIdx;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_UNDOOWNER_H__
