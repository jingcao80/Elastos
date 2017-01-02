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

#include "elastos/droid/internal/utility/gesture/CEdgeGesturePosition.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {
namespace Gesture {

static AutoPtr<IEdgeGesturePosition> Init(Int32 index, Int32 factor)
{
    AutoPtr<IEdgeGesturePosition> obj;
    CEdgeGesturePosition::New(index, factor, (IEdgeGesturePosition**)&obj);
    return obj;
}

const AutoPtr<IEdgeGesturePosition> CEdgeGesturePosition::LEFT = Init(0, 0);
const AutoPtr<IEdgeGesturePosition> CEdgeGesturePosition::BOTTOM = Init(1, 1);
const AutoPtr<IEdgeGesturePosition> CEdgeGesturePosition::RIGHT = Init(2, 1);
const AutoPtr<IEdgeGesturePosition> CEdgeGesturePosition::TOP = Init(3, 0);

CAR_INTERFACE_IMPL(CEdgeGesturePosition, Object, IEdgeGesturePosition)

CAR_OBJECT_IMPL(CEdgeGesturePosition)

CEdgeGesturePosition::CEdgeGesturePosition()
    : mIndex(0)
    , mFlag(0)
    , mFactor(0)
{
}

ECode CEdgeGesturePosition::constructor()
{
    return NOERROR;
}

ECode CEdgeGesturePosition::constructor(
    /* [in] */ Int32 index,
    /* [in] */ Int32 factor)
{
    mIndex = index;
    mFlag = (0x01 << index);
    mFactor = factor;
    return NOERROR;
}

ECode CEdgeGesturePosition::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    *index = mIndex;
    return NOERROR;
}

ECode CEdgeGesturePosition::GetFlag(
    /* [out] */ Int32* flag)
{
    VALIDATE_NOT_NULL(flag)
    *flag = mFlag;
    return NOERROR;
}

ECode CEdgeGesturePosition::GetFactor(
    /* [out] */ Int32* factor)
{
    VALIDATE_NOT_NULL(factor)
    *factor = mFactor;
    return NOERROR;
}

AutoPtr<ArrayOf<IEdgeGesturePosition*> > CEdgeGesturePosition::Values()
{
    AutoPtr<ArrayOf<IEdgeGesturePosition*> > array = ArrayOf<IEdgeGesturePosition*>::Alloc(4);
    array->Set(0, LEFT.Get());
    array->Set(0, BOTTOM.Get());
    array->Set(0, RIGHT.Get());
    array->Set(0, TOP.Get());
    return array;
}

} // namespace Gesture
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
