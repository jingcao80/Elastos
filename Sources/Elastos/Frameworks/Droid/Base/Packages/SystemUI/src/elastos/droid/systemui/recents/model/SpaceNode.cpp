#include "elastos/droid/systemui/recents/model/SpaceNode.h"

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

SpaceNode::SpaceNode() {
    // Do nothing
}

CAR_INTERFACE_IMPL(SpaceNode, Object, ISpaceNode)

ECode SpaceNode::SetStack(
    /* [in] */ ITaskStack* stack)
{
    mStack = (TaskStack*)stack;
    return NOERROR;
}

ECode SpaceNode::GetStack(
    /* [out] */ ITaskStack** stack)
{
    VALIDATE_NOT_NULL(stack)
    *stack = mStack;
    REFCOUNT_ADD(*stack)
    return NOERROR;
}

ECode SpaceNode::HasTasks(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 tc = mStack->GetTaskCount();
    Boolean b1,b2;
    mStartNode->HasTasks(&b1);
    mEndNode->HasTasks(&b2);
    *result = (tc > 0) ||
        (mStartNode != NULL && b1) ||
        (mEndNode != NULL && b2);
    return NOERROR;
}

ECode SpaceNode::IsLeafNode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mStartNode == NULL) && (mEndNode == NULL);
    return NOERROR;
}

void SpaceNode::GetStacksRec(
    /* [in] */ IArrayList* stacks)
{
    Boolean isLeafNode;
    IsLeafNode(&isLeafNode);
    if (isLeafNode) {
        stacks->Add((ITaskStack*)mStack);
    }
    else {
        mStartNode->GetStacksRec(stacks);
        mEndNode->GetStacksRec(stacks);
    }
}

ECode SpaceNode::GetStacks(
    /* [out] */ IArrayList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IArrayList> stacks;
    CArrayList::New((IArrayList**)&stacks);
    GetStacksRec(stacks);
    *result = stacks;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos