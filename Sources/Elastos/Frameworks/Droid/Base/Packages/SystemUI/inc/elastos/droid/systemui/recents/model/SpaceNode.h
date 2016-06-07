
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_SPACENODE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_SPACENODE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/**
 * The full recents space is partitioned using a BSP into various nodes that define where task
 * stacks should be placed.
 */
class SpaceNode
    : public Object
    , public ISpaceNode
{
public:
    SpaceNode();

    CAR_INTERFACE_DECL()

    /** Sets the current stack for this space node */
    CARAPI SetStack(
        /* [in] */ ITaskStack* stack);

    /** Returns the task stack (not null if this is a leaf) */
    CARAPI GetStack(
        /* [out] */ ITaskStack** stack);

    /** Returns whether there are any tasks in any stacks below this node. */
    CARAPI HasTasks(
        /* [out] */ Boolean* result);

    /** Returns whether this is a leaf node */
    CARAPI IsLeafNode(
        /* [out] */ Boolean* result);

    CARAPI GetStacks(
        /* [out] */ IArrayList** result);

private:
    /** Returns all the descendent task stacks */
    CARAPI_(void) GetStacksRec(
        /* [in] */ IArrayList* stacks);

public:
    SpaceNode* mStartNode;
    SpaceNode* mEndNode;

    AutoPtr<ITaskStack> mStack;
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_SPACENODE_H__
