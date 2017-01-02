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