#include "elastos/droid/systemui/recents/model/SpaceNode.h"
#include "elastos/droid/systemui/recents/model/TaskStack.h"

using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

CAR_INTERFACE_IMPL(SpaceNode, Object, ISpaceNode)

SpaceNode::SpaceNode()
    : mStartNode(NULL)
    , mEndNode(NULL)
{
    // Do nothing
}

ECode SpaceNode::SetStack(
    /* [in] */ ITaskStack* stack)
{
    mStack = stack;
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
    TaskStack* stack = (TaskStack*)mStack.Get();
    Int32 tc = stack->GetTaskCount();
    Boolean b1,b2;
    *result = (tc > 0)
        || (mStartNode != NULL && (mStartNode->HasTasks(&b1), b1))
        || (mEndNode != NULL && (mEndNode->HasTasks(&b2), b2));
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
        stacks->Add(mStack);
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