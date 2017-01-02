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

#include "elastos/droid/server/wm/StackTapPointerEventListener.h"
#include "elastos/droid/server/wm/DisplayContent.h"
#include <elastos/core/Math.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::View::EIID_IPointerEventListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

const Int32 StackTapPointerEventListener::TAP_TIMEOUT_MSEC;
const Float StackTapPointerEventListener::TAP_MOTION_SLOP_INCHES;

StackTapPointerEventListener::StackTapPointerEventListener(
    /* [in] */ CWindowManagerService* service,
    /* [in] */ DisplayContent* displayContent)
    : mMotionSlop(0)
    , mDownX(0)
    , mDownY(0)
    , mPointerId(0)
    , mService(service)
    , mDisplayContent(displayContent)
{
    CRegion::New((IRegion**)&mTouchExcludeRegion);
    AutoPtr<IDisplayInfo> info = displayContent->GetDisplayInfo();
    Int32 dpi;
    info->GetLogicalDensityDpi(&dpi);
    mMotionSlop = (Int32)(dpi * TAP_MOTION_SLOP_INCHES);
}

CAR_INTERFACE_IMPL(StackTapPointerEventListener, Object, IPointerEventListener)

ECode StackTapPointerEventListener::OnPointerEvent(
    /* [in] */ IMotionEvent* motionEvent)
{
    Int32 action;
    motionEvent->GetAction(&action);
    switch (action & IMotionEvent::ACTION_MASK) {
        case IMotionEvent::ACTION_DOWN:
            motionEvent->GetPointerId(0, &mPointerId);
            motionEvent->GetX(&mDownX);
            motionEvent->GetY(&mDownY);
            break;
        case IMotionEvent::ACTION_MOVE:
            if (mPointerId >= 0) {
                Int32 index;
                motionEvent->FindPointerIndex(mPointerId, &index);
                if (index < 0) {
                    mPointerId = -1;
                    break;
                }

                Int64 eventTime, downTime;
                Float x, y;
                IInputEvent::Probe(motionEvent)->GetEventTime(&eventTime);
                motionEvent->GetDownTime(&downTime);
                if ((eventTime - downTime) > TAP_TIMEOUT_MSEC
                        || index < 0
                        || (motionEvent->GetX(index, &x), Elastos::Core::Math::Abs(x - mDownX) > mMotionSlop)
                        || (motionEvent->GetY(index, &y), Elastos::Core::Math::Abs(y - mDownY) > mMotionSlop)) {
                    mPointerId = -1;
                }
            }
            break;
        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_POINTER_UP: {
            Int32 index = (action & IMotionEvent::ACTION_POINTER_INDEX_MASK)
                    >> IMotionEvent::ACTION_POINTER_INDEX_SHIFT;
            // Extract the index of the pointer that left the touch sensor
            Int32 id;
            if (motionEvent->GetPointerId(index, &id), mPointerId == id) {
                Float ex, ey;
                motionEvent->GetX(index, &ex);
                motionEvent->GetY(index, &ey);
                Int32 x = (Int32)ex;
                Int32 y = (Int32)ey;
                {    AutoLock syncLock(this);
                    Int64 eventTime, downTime;
                    IInputEvent::Probe(motionEvent)->GetEventTime(&eventTime);
                    motionEvent->GetDownTime(&downTime);
                    Boolean contains;
                    if ((eventTime - downTime) < TAP_TIMEOUT_MSEC
                            && Elastos::Core::Math::Abs(x - mDownX) < mMotionSlop
                            && Elastos::Core::Math::Abs(y - mDownY) < mMotionSlop
                            && (mTouchExcludeRegion->Contains(x, y, &contains), !contains)) {
                        AutoPtr<IMessage> msg;
                        mService->mH->ObtainMessage(CWindowManagerService::H::TAP_OUTSIDE_STACK, x, y,
                                (IObject*)mDisplayContent.Get(), (IMessage**)&msg);
                        msg->SendToTarget();
                    }
                }
                mPointerId = -1;
            }
            break;
        }
    }
    return NOERROR;
}

void StackTapPointerEventListener::SetTouchExcludeRegion(
    /* [in] */ IRegion* newRegion)
{
    {    AutoLock syncLock(this);
        Boolean result;
        mTouchExcludeRegion->Set(newRegion, &result);
    }
}

} // Wm
} // Server
} // Droid
} // Elastos
