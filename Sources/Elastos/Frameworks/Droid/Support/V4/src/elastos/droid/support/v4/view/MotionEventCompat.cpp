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

#include "elastos/droid/support/v4/view/MotionEventCompat.h"
#include "elastos/droid/support/v4/view/MotionEventCompatEclair.h"
#include "elastos/droid/os/Build.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Support::V4::View::EIID_IMotionEventVersionImpl;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

//=================================================================
// MotionEventCompat::StaticInitializer
//=================================================================
MotionEventCompat::StaticInitializer::StaticInitializer()
{
    if (Build::VERSION::SDK_INT >= 5) {
        IMPL = new EclairMotionEventVersionImpl();
    }
    else {
        IMPL = new BaseMotionEventVersionImpl();
    }
}


//=================================================================
// MotionEventCompat::BaseMotionEventVersionImpl
//=================================================================
CAR_INTERFACE_IMPL(MotionEventCompat::BaseMotionEventVersionImpl, Object, IMotionEventVersionImpl)

ECode MotionEventCompat::BaseMotionEventVersionImpl::FindPointerIndex(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerId,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    *index = -1;
    if (pointerId == 0) {
        // id 0 == index 0 and vice versa.
        *index = 0;
    }
    return NOERROR;
}

ECode MotionEventCompat::BaseMotionEventVersionImpl::GetPointerId(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    if (pointerIndex == 0) {
        // index 0 == id 0 and vice versa.
        *id = 0;
        return NOERROR;
    }
    *id = -1;
    Logger::E("BaseMotionEventVersionImpl", "Pre-Eclair does not support multiple pointers");
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode MotionEventCompat::BaseMotionEventVersionImpl::GetX(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x)
    if (pointerIndex == 0) {
        return event->GetX(x);
    }
    *x = -1;
    Logger::E("BaseMotionEventVersionImpl", "Pre-Eclair does not support multiple pointers");
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode MotionEventCompat::BaseMotionEventVersionImpl::GetY(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y)
    if (pointerIndex == 0) {
        return event->GetY(y);
    }
    *y = -1;
    Logger::E("BaseMotionEventVersionImpl", "Pre-Eclair does not support multiple pointers");
    return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
}

ECode MotionEventCompat::BaseMotionEventVersionImpl::GetPointerCount(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 1;
    return NOERROR;
}


//=================================================================
// MotionEventCompat::EclairMotionEventVersionImpl
//=================================================================
CAR_INTERFACE_IMPL(MotionEventCompat::EclairMotionEventVersionImpl, Object, IMotionEventVersionImpl)

ECode MotionEventCompat::EclairMotionEventVersionImpl::FindPointerIndex(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerId,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    return MotionEventCompatEclair::FindPointerIndex(event, pointerId, index);
}

ECode MotionEventCompat::EclairMotionEventVersionImpl::GetPointerId(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    return MotionEventCompatEclair::GetPointerId(event, pointerIndex, id);
}

ECode MotionEventCompat::EclairMotionEventVersionImpl::GetX(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x)
    return MotionEventCompatEclair::GetX(event, pointerIndex, x);
}

ECode MotionEventCompat::EclairMotionEventVersionImpl::GetY(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex,
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y)
    return MotionEventCompatEclair::GetY(event, pointerIndex, y);
}

ECode MotionEventCompat::EclairMotionEventVersionImpl::GetPointerCount(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    return MotionEventCompatEclair::GetPointerCount(event, count);
}


//=================================================================
// MotionEventCompat
//=================================================================
AutoPtr<IMotionEventVersionImpl> MotionEventCompat::IMPL;
MotionEventCompat::StaticInitializer MotionEventCompat::sInitializer;
const Int32 MotionEventCompat::ACTION_MASK;
const Int32 MotionEventCompat::ACTION_POINTER_DOWN;
const Int32 MotionEventCompat::ACTION_POINTER_UP;
const Int32 MotionEventCompat::ACTION_HOVER_MOVE;
const Int32 MotionEventCompat::ACTION_SCROLL;
const Int32 MotionEventCompat::ACTION_POINTER_INDEX_MASK;
const Int32 MotionEventCompat::ACTION_POINTER_INDEX_SHIFT;
const Int32 MotionEventCompat::ACTION_HOVER_ENTER;
const Int32 MotionEventCompat::ACTION_HOVER_EXIT;

Int32 MotionEventCompat::GetActionMasked(
    /* [in] */ IMotionEvent* event)
{
    Int32 action;
    event->GetAction(&action);
    return action & ACTION_MASK;
}

Int32 MotionEventCompat::GetActionIndex(
    /* [in] */ IMotionEvent* event)
{
    Int32 action;
    event->GetAction(&action);
    return (action & ACTION_POINTER_INDEX_MASK) >> ACTION_POINTER_INDEX_SHIFT;
}

Int32 MotionEventCompat::FindPointerIndex(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerId)
{
    Int32 index;
    IMPL->FindPointerIndex(event, pointerId, &index);
    return index;
}

Int32 MotionEventCompat::GetPointerId(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex)
{
    Int32 id;
    IMPL->GetPointerId(event, pointerIndex, &id);
    return id;
}

Float MotionEventCompat::GetX(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex)
{
    Float x;
    IMPL->GetX(event, pointerIndex, &x);
    return x;
}

Float MotionEventCompat::GetY(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Int32 pointerIndex)
{
    Float y;
    IMPL->GetY(event, pointerIndex, &y);
    return y;
}

Int32 MotionEventCompat::GetPointerCount(
    /* [in] */ IMotionEvent* event)
{
    Int32 count;
    IMPL->GetPointerCount(event, &count);
    return count;
}

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos
