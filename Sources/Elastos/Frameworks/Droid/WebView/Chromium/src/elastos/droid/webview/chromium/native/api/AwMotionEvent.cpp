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

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/webkit/webview/chromium/native/api/AwMotionEvent.h"
#include "elastos/droid/webkit/webview/chromium/native/api/MotionEvent_dec.h"
//TODO #include "elastos/droid/view/CMotionEventHelper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::IPointerProperties;
using Elastos::Droid::View::IPointerCoords;
using Elastos::Droid::View::IMotionEvent;
//TODO using Elastos::Droid::View::CMotionEventHelper;
using Elastos::Droid::View::IMotionEventHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Api {

void AwMotionEvent::Finalize(
    /* [in] */ IInterface* obj)
{
    Logger::E("AwMotionEvent", "AwMotionEvent::Finalize, no implementation");
    //this method will not be called, if called, then implement
    return;
    /*
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::Finalize, mObj is NULL");
        return;
    }
    mObj->Finalize();
    */
}

AutoPtr<IInterface> AwMotionEvent::ObtainAVME_J_J_I_I_LAVMEPP_LAVMEPC_I_I_F_F_I_I_I_I(
    /* [in] */ Int64 p0,
    /* [in] */ Int64 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ ArrayOf<IInterface*>* p4,
    /* [in] */ ArrayOf<IInterface*>* p5,
    /* [in] */ Int32 p6,
    /* [in] */ Int32 p7,
    /* [in] */ Float p8,
    /* [in] */ Float p9,
    /* [in] */ Int32 p10,
    /* [in] */ Int32 p11,
    /* [in] */ Int32 p12,
    /* [in] */ Int32 p13)
{
    Logger::E("AwMotionEvent", "AwMotionEvent::ObtainAVME_J_J_I_I_AI_LAVMEPC_I_F_F_I_I_I_I, no implementation");
    //this method will not be called, if called, then implement
    return NULL;
    /*
    AutoPtr<IMotionEventHelper> meHelper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&meHelper);
    AutoPtr<IMotionEvent> me;
    //p4, p5 should convert to ArrayOf<IPointerProperties*> and ArrayOf<IPointerCoords*>
    meHelper->Obtain(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, (IMotionEvent**)&me);
    return me;
    */
}

AutoPtr<IInterface> AwMotionEvent::ObtainAVME_J_J_I_I_AI_LAVMEPC_I_F_F_I_I_I_I(
    /* [in] */ Int64 p0,
    /* [in] */ Int64 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ ArrayOf<Int32>* p4,
    /* [in] */ ArrayOf<IInterface*>* p5,
    /* [in] */ Int32 p6,
    /* [in] */ Float p7,
    /* [in] */ Float p8,
    /* [in] */ Int32 p9,
    /* [in] */ Int32 p10,
    /* [in] */ Int32 p11,
    /* [in] */ Int32 p12)
{
    Logger::E("AwMotionEvent", "AwMotionEvent::ObtainAVME_J_J_I_I_AI_LAVMEPC_I_F_F_I_I_I_I, no implementation");
    //this method will not be called, if called, then implement
    return NULL;
    /*
    AutoPtr<IMotionEventHelper> meHelper;
    CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&meHelper);
    AutoPtr<IMotionEvent> me;
    //p5 should convert to ArrayOf<IPointerCoords*>
    meHelper->Obtain(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, (IMotionEvent**)&me);
    return me;
    */
}

AutoPtr<IInterface> AwMotionEvent::ObtainAVME_J_J_I_F_F_F_F_I_F_F_I_I(
    /* [in] */ Int64 p0,
    /* [in] */ Int64 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Float p3,
    /* [in] */ Float p4,
    /* [in] */ Float p5,
    /* [in] */ Float p6,
    /* [in] */ Int32 p7,
    /* [in] */ Float p8,
    /* [in] */ Float p9,
    /* [in] */ Int32 p10,
    /* [in] */ Int32 p11)
{
    AutoPtr<IMotionEventHelper> meHelper;
    //TODO CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&meHelper);
    AutoPtr<IMotionEvent> me;
    meHelper->Obtain(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, (IMotionEvent**)&me);
    return me;
}

AutoPtr<IInterface> AwMotionEvent::ObtainAVME_J_J_I_I_F_F_F_F_I_F_F_I_I(
    /* [in] */ Int64 p0,
    /* [in] */ Int64 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ Float p4,
    /* [in] */ Float p5,
    /* [in] */ Float p6,
    /* [in] */ Float p7,
    /* [in] */ Int32 p8,
    /* [in] */ Float p9,
    /* [in] */ Float p10,
    /* [in] */ Int32 p11,
    /* [in] */ Int32 p12)
{
    AutoPtr<IMotionEventHelper> meHelper;
    //TODO CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&meHelper);
    AutoPtr<IMotionEvent> me;
    meHelper->Obtain(p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, (IMotionEvent**)&me);
    return me;
}

AutoPtr<IInterface> AwMotionEvent::ObtainAVME_J_J_I_F_F_I(
    /* [in] */ Int64 p0,
    /* [in] */ Int64 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Float p3,
    /* [in] */ Float p4,
    /* [in] */ Int32 p5)
{
    AutoPtr<IMotionEventHelper> meHelper;
    //TODO CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&meHelper);
    AutoPtr<IMotionEvent> me;
    meHelper->Obtain(p0, p1, p2, p3, p4, p5, (IMotionEvent**)&me);
    return me;
}

AutoPtr<IInterface> AwMotionEvent::ObtainAVME_AVME(
    /* [in] */ IInterface* p0)
{
    AutoPtr<IMotionEvent> me;
    AutoPtr<IMotionEvent> om = IMotionEvent::Probe(p0);
    AutoPtr<IMotionEventHelper> meHelper;
    //TODO CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&meHelper);
    meHelper->Obtain(om, (IMotionEvent**)&me);
    return me;
}

AutoPtr<IInterface> AwMotionEvent::ObtainNoHistory(
    /* [in] */ IInterface* p0)
{
    AutoPtr<IMotionEvent> me;
    AutoPtr<IMotionEvent> om = IMotionEvent::Probe(p0);
    AutoPtr<IMotionEventHelper> meHelper;
    //TODO CMotionEventHelper::AcquireSingleton((IMotionEventHelper**)&meHelper);
    meHelper->ObtainNoHistory(om, (IMotionEvent**)&me);
    return me;
}

void AwMotionEvent::Recycle(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::Recycle, mObj is NULL");
        return;
    }
    AutoPtr<IInputEvent> ie = IInputEvent::Probe(mObj);
    ie->Recycle();
}

Int32 AwMotionEvent::GetDeviceId(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetDeviceId, mObj is NULL");
        return 0;
    }
    AutoPtr<IInputEvent> ie = IInputEvent::Probe(mObj);
    Int32 deviceId;
    ie->GetDeviceId(&deviceId);
    return deviceId;
}

Int32 AwMotionEvent::GetSource(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetSource, mObj is NULL");
        return IInputDevice::SOURCE_UNKNOWN;//0
    }
    AutoPtr<IInputEvent> ie = IInputEvent::Probe(mObj);
    Int32 source;
    ie->GetSource(&source);
    return source;
}

void AwMotionEvent::SetSource(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::SetSource, mObj is NULL");
        return;
    }
    AutoPtr<IInputEvent> ie = IInputEvent::Probe(mObj);
    ie->SetSource(p0);
    return;
}

Int32 AwMotionEvent::GetAction(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetAction, mObj is NULL, return -1");
        return -1;
    }
    Int32 action;
    mObj->GetAction(&action);
    return action;
}

Int32 AwMotionEvent::GetActionMasked(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetActionMasked, mObj is NULL, return -1");
        return -1;//is this value ok??
    }
    Int32 actiionMasked;
    mObj->GetActionMasked(&actiionMasked);
    return actiionMasked;
}

Int32 AwMotionEvent::GetActionIndex(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetActionIndex, mObj is NULL, return 0");
        return 0;//is this value ok??
    }
    Int32 actionIndex;
    mObj->GetActionIndex(&actionIndex);
    return actionIndex;
}

Int32 AwMotionEvent::GetFlags(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetFlags, mObj is NULL, return 0");
        return 0;//is this ok??
    }
    Int32 flags;
    mObj->GetFlags(&flags);
    return flags;
}

Int64 AwMotionEvent::GetDownTime(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetDownTime, mObj is NULL");
        return 0;
    }
    Int64 downTime;
    mObj->GetDownTime(&downTime);
    return downTime;
}

Int64 AwMotionEvent::GetEventTime(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetEventTime, mObj is NULL");
        return 0;
    }
    AutoPtr<IInputEvent> ie = IInputEvent::Probe(mObj);
    Int64 eventTime;
    ie->GetEventTime(&eventTime);
    return eventTime;
}

Float AwMotionEvent::GetXF(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetXF, mObj is NULL");
        return 0;
    }
    Float x;
    mObj->GetX(&x);
    return x;
}

Float AwMotionEvent::GetYF(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetYF, mObj is NULL");
        return 0;
    }
    Float y;
    mObj->GetY(&y);
    return y;
}

Float AwMotionEvent::GetPressureF(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetPressureF, mObj is NULL");
        return 0;
    }
    Float pressure;
    mObj->GetPressure(&pressure);
    return pressure;
}

Float AwMotionEvent::GetSizeF(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetSizeF, mObj is NULL");
        return 0;
    }
    Float size;
    mObj->GetSize(&size);
    return size;
}

Float AwMotionEvent::GetTouchMajorF(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetTouchMajorF, mObj is NULL");
        return 0;
    }
    Float touchMajor;
    mObj->GetTouchMajor(&touchMajor);
    return touchMajor;
}

Float AwMotionEvent::GetTouchMinorF(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetTouchMinorF, mObj is NULL");
        return 0;
    }
    Float touchMinor;
    mObj->GetTouchMinor(&touchMinor);
    return touchMinor;
}

Float AwMotionEvent::GetToolMajorF(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetToolMajorF, mObj is NULL");
        return 0;
    }
    Float toolMajor;
    mObj->GetToolMajor(&toolMajor);
    return toolMajor;
}

Float AwMotionEvent::GetToolMinorF(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetToolMinorF, mObj is NULL");
        return 0;
    }
    Float toolMinor;
    mObj->GetToolMinor(&toolMinor);
    return toolMinor;
}

Float AwMotionEvent::GetOrientationF(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetOrientationF, mObj is NULL");
        return 0;
    }
    Float orientation;
    mObj->GetOrientation(&orientation);
    return orientation;
}

Float AwMotionEvent::GetAxisValueF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::AwMotionEvent_getAxisValueF::I, mObj is NULL");
        return 0;
    }
    Float value;
    mObj->GetAxisValue(p0, &value);
    return value;
}

Int32 AwMotionEvent::GetPointerCount(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetPointerCount, mObj is NULL");
        return 0;
    }
    Int32 count;
    mObj->GetPointerCount(&count);
    return count;
}

Int32 AwMotionEvent::GetPointerId(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetPointerId, mObj is NULL");
        return 0;
    }
    Int32 pointerId;
    mObj->GetPointerId(p0, &pointerId);
    return pointerId;
}

Int32 AwMotionEvent::GetToolType(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetToolType, mObj is NULL");
        return 0;
    }
    Int32 toolType;
    mObj->GetToolType(p0, &toolType);
    return toolType;
}

Int32 AwMotionEvent::FindPointerIndex(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::FindPointerIndex, mObj is NULL");
        return 0;
    }
    Int32 pointerIndex;
    mObj->FindPointerIndex(p0, &pointerIndex);
    return pointerIndex;
}

Float AwMotionEvent::GetXF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetXF_I, mObj is NULL");
        return 0;
    }
    Float x;
    mObj->GetX(p0, &x);
    return x;
}

Float AwMotionEvent::GetYF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetYF_I, mObj is NULL");
        return 0;
    }
    Float y;
    mObj->GetY(p0, &y);
    return y;
}

Float AwMotionEvent::GetPressureF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetPressureF_I, mObj is NULL");
        return 0;
    }
    Float pressure;
    mObj->GetPressure(p0, &pressure);
    return pressure;
}

Float AwMotionEvent::GetSizeF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetSizeF_I, mObj is NULL");
        return 0;
    }
    Float size;
    mObj->GetSize(p0, &size);
    return size;
}

Float AwMotionEvent::GetTouchMajorF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetTouchMajorF_I, mObj is NULL");
        return 0;
    }
    Float touchMajor;
    mObj->GetTouchMajor(p0, &touchMajor);
    return touchMajor;
}

Float AwMotionEvent::GetTouchMinorF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetTouchMinorF_I, mObj is NULL");
        return 0;
    }
    Float touchMinor;
    mObj->GetTouchMinor(p0, &touchMinor);
    return touchMinor;
}

Float AwMotionEvent::GetToolMajorF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetToolMajorF_I, mObj is NULL");
        return 0;
    }
    Float toolMajor;
    mObj->GetToolMajor(p0, &toolMajor);
    return toolMajor;
}

Float AwMotionEvent::GetToolMinorF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetToolMinorF_I, mObj is NULL");
        return 0;
    }
    Float toolMinor;
    mObj->GetToolMinor(p0, &toolMinor);
    return toolMinor;
}

Float AwMotionEvent::GetOrientationF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetOrientationF_I, mObj is NULL");
        return 0;
    }
    Float orientation;
    mObj->GetOrientation(p0, &orientation);
    return orientation;
}

Float AwMotionEvent::GetAxisValueF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::AwMotionEvent_getAxisValueF_I::I, mObj is NULL");
        return 0;
    }
    Float value;
    mObj->GetAxisValue(p0, p1, &value);
    return value;
}

void AwMotionEvent::GetPointerCoords(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ IInterface* p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetPointerCoords, mObj is NULL");
        return;
    }
    AutoPtr<IPointerCoords> pc = IPointerCoords::Probe(p1);
    mObj->GetPointerCoords(p0, pc);
}

void AwMotionEvent::GetPointerProperties(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ IInterface* p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetPointerProperties, mObj is NULL");
        return;
    }
    AutoPtr<IPointerProperties> pp = IPointerProperties::Probe(p1);
    mObj->GetPointerProperties(p0, pp);
}

Int32 AwMotionEvent::GetMetaState(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetMetaState, mObj is NULL");
        return 0;
    }
    Int32 metaState;
    mObj->GetMetaState(&metaState);
    return metaState;
}

Int32 AwMotionEvent::GetButtonState(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetButtonState, mObj is NULL, return -1");
        return -1;//is this ok??
    }
    Int32 buttonState;
    mObj->GetButtonState(&buttonState);
    return buttonState;
}

Float AwMotionEvent::GetRawX(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetRawX, mObj is NULL");
        return 0;
    }
    Float rawX;
    mObj->GetRawX(&rawX);
    return rawX;
}

Float AwMotionEvent::GetRawY(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetRawY, mObj is NULL");
        return 0;
    }
    Float rawY;
    mObj->GetRawY(&rawY);
    return rawY;
}

Float AwMotionEvent::GetXPrecision(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetXPrecision, mObj is NULL");
        return 0;
    }
    Float xPrecision;
    mObj->GetXPrecision(&xPrecision);
    return xPrecision;
}

Float AwMotionEvent::GetYPrecision(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetYPrecision, mObj is NULL");
        return 0;
    }
    Float yPrecision;
    mObj->GetYPrecision(&yPrecision);
    return yPrecision;
}

Int32 AwMotionEvent::GetHistorySize(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistorySize, mObj is NULL");
        return 0;
    }
    Int32 historySize;
    mObj->GetHistorySize(&historySize);
    return historySize;
}

Int64 AwMotionEvent::GetHistoricalEventTime(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalEventTime, mObj is NULL");
        return 0;
    }
    Int64 hEventTime;
    mObj->GetHistoricalEventTime(p0, &hEventTime);
    return hEventTime;
}

Float AwMotionEvent::GetHistoricalXF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalXF_I, mObj is NULL");
        return 0;
    }
    Float hX;
    mObj->GetHistoricalX(p0, &hX);
    return hX;
}

Float AwMotionEvent::GetHistoricalYF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::AwMotionEvent_getHistoricalYF::I, mObj is NULL");
        return 0;
    }
    Float hY;
    mObj->GetHistoricalY(p0, &hY);
    return hY;
}

Float AwMotionEvent::GetHistoricalPressureF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalPressureF_I, mObj is NULL");
        return 0;
    }
    Float hP;
    mObj->GetHistoricalPressure(p0, &hP);
    return hP;
}

Float AwMotionEvent::GetHistoricalSizeF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalSizeF_I, mObj is NULL");
        return 0;
    }
    Float hSize;
    mObj->GetHistoricalSize(p0, &hSize);
    return hSize;
}

Float AwMotionEvent::GetHistoricalTouchMajorF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalTouchMajorF_I, mObj is NULL");
        return 0;
    }
    Float hTouchMajor;
    mObj->GetHistoricalTouchMajor(p0, &hTouchMajor);
    return hTouchMajor;
}

Float AwMotionEvent::GetHistoricalTouchMinorF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalTouchMinorF_I, mObj is NULL");
        return 0;
    }
    Float hTouchMinor;
    mObj->GetHistoricalTouchMinor(p0, &hTouchMinor);
    return hTouchMinor;
}

Float AwMotionEvent::GetHistoricalToolMajorF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalToolMajorF_I, mObj is NULL");
        return 0;
    }
    Float hToolMajor;
    mObj->GetHistoricalToolMajor(p0, &hToolMajor);
    return hToolMajor;
}

Float AwMotionEvent::GetHistoricalToolMinorF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalToolMinorF_I, mObj is NULL");
        return 0;
    }
    Float hToolMinor;
    mObj->GetHistoricalToolMinor(p0, &hToolMinor);
    return hToolMinor;
}

Float AwMotionEvent::GetHistoricalOrientationF_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalOrientationF_I, mObj is NULL");
        return 0;
    }
    Float hOrientation;
    mObj->GetHistoricalOrientation(p0, &hOrientation);
    return hOrientation;
}

Float AwMotionEvent::GetHistoricalAxisValueF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalAxisValueF_I_I, mObj is NULL");
        return 0;
    }
    Float value;
    mObj->GetHistoricalAxisValue(p0, p1, &value);
    return value;
}

Float AwMotionEvent::GetHistoricalXF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalXF_I_I, mObj is NULL");
        return 0;
    }
    Float hX;
    mObj->GetHistoricalX(p0, p1, &hX);
    return hX;
}

Float AwMotionEvent::GetHistoricalYF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalYF_I_I, mObj is NULL");
        return 0;
    }
    Float hY;
    mObj->GetHistoricalY(p0, p1, &hY);
    return hY;
}

Float AwMotionEvent::GetHistoricalPressureF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalPressureF_I_I, mObj is NULL");
        return 0;
    }
    Float hP;
    mObj->GetHistoricalPressure(p0, p1, &hP);
    return hP;
}

Float AwMotionEvent::GetHistoricalSizeF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalSizeF_I_I, mObj is NULL");
        return 0;
    }
    Float hSize;
    mObj->GetHistoricalSize(p0, p1, &hSize);
    return hSize;
}

Float AwMotionEvent::GetHistoricalTouchMajorF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalTouchMajorF_I_I, mObj is NULL");
        return 0;
    }
    Float hTouchMajor;
    mObj->GetHistoricalTouchMajor(p0, p1, &hTouchMajor);
    return hTouchMajor;
}

Float AwMotionEvent::GetHistoricalTouchMinorF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalTouchMinorF_I_I, mObj is NULL");
        return 0;
    }
    Float hTouchMinor;
    mObj->GetHistoricalTouchMinor(p0, p1, &hTouchMinor);
    return hTouchMinor;
}

Float AwMotionEvent::GetHistoricalToolMajorF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalToolMajorF_I_I, mObj is NULL");
        return 0;
    }
    Float hToolMajor;
    mObj->GetHistoricalToolMajor(p0, p1, &hToolMajor);
    return hToolMajor;
}

Float AwMotionEvent::GetHistoricalToolMinorF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalToolMinorF_I_I, mObj is NULL");
        return 0;
    }
    Float hToolMinor;
    mObj->GetHistoricalToolMinor(p0, p1, &hToolMinor);
    return hToolMinor;
}

Float AwMotionEvent::GetHistoricalOrientationF_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalOrientationF_I_I, mObj is NULL");
        return 0;
    }
    Float hOrientation;
    mObj->GetHistoricalOrientation(p0, p1, &hOrientation);
    return hOrientation;
}

Float AwMotionEvent::GetHistoricalAxisValueF_I_I_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalAxisValueF_I_I_I, mObj is NULL");
        return 0;
    }
    Float hAxisValue;
    mObj->GetHistoricalAxisValue(p0, p1, p2, &hAxisValue);
    return hAxisValue;
}

void AwMotionEvent::GetHistoricalPointerCoords(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0,
    /* [in] */ Int32 p1,
    /* [in] */ IInterface* p2)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetHistoricalPointerCoords, mObj is NULL");
        return;
    }
    AutoPtr<IPointerCoords> pc = IPointerCoords::Probe(p2);
    mObj->GetHistoricalPointerCoords(p0, p1, pc);
}

Int32 AwMotionEvent::GetEdgeFlags(
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetEdgeFlags, mObj is NULL");
        return 0;
    }
    Int32 edgeFlags;
    mObj->GetEdgeFlags(&edgeFlags);
    return edgeFlags;
}

void AwMotionEvent::SetEdgeFlags(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::SetEdgeFlags, mObj is NULL");
        return;
    }
    mObj->SetEdgeFlags(p0);
}

void AwMotionEvent::SetAction(
    /* [in] */ IInterface* obj,
    /* [in] */ Int32 p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::SetAction, mObj is NULL");
        return;
    }
    mObj->SetAction(p0);
}

void AwMotionEvent::OffsetLocation(
    /* [in] */ IInterface* obj,
    /* [in] */ Float p0,
    /* [in] */ Float p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::OffsetLocation, mObj is NULL");
        return;
    }
    mObj->OffsetLocation(p0, p1);
}

void AwMotionEvent::SetLocation(
    /* [in] */ IInterface* obj,
    /* [in] */ Float p0,
    /* [in] */ Float p1)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::SetLocation, mObj is NULL");
        return;
    }
    mObj->SetLocation(p0, p1);
}

void AwMotionEvent::Transform(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::Transform, mObj is NULL");
        return;
    }
    AutoPtr<IMatrix> m = IMatrix::Probe(p0);
    mObj->Transform(m);
}

void AwMotionEvent::AddBatchV_J_F_F_F_F_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 p0,
    /* [in] */ Float p1,
    /* [in] */ Float p2,
    /* [in] */ Float p3,
    /* [in] */ Float p4,
    /* [in] */ Int32 p5)
{
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::AddBatchV_J_F_F_F_F_I, mObj is NULL");
        return;
    }
    mObj->AddBatch(p0, p1, p2, p3, p4, p5);
}

void AwMotionEvent::AddBatchV_J_LAVMEPC_I(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 p0,
    /* [in] */ ArrayOf<IInterface*>* p1,
    /* [in] */ Int32 p2)
{
    Logger::E("AwMotionEvent", "AwMotionEvent::AddBatchV_J_LAVMEPC_I, no implementation");
    //this method will not be called, if called, then implement
    return;
    /*
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::GetEdgeFlags, mObj is NULL");
        return;
    }
    //p1 should be converted to ArrayOf<IPointerCoords*>
    mObj->AddBatch(p0, p1, p2);
    */
}

String AwMotionEvent::ToString(
    /* [in] */ IInterface* obj)
{
    Logger::E("AwMotionEvent", "AwMotionEvent::ToString, no implementation");
    //this method will not be called, if called, then implement
    return String(NULL);
    /*
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::ToString, mObj is NULL");
        return String(NULL);
    }
    return mObj->ToString();
    */
}

String AwMotionEvent::AxisToString(
    /* [in] */ Int32 p0)
{
    Logger::E("AwMotionEvent", "AwMotionEvent::AxisToString, no implementation");
    //this method will not be called, if called, then implement
    return String(NULL);
}

Int32 AwMotionEvent::AxisFromString(
    /* [in] */ const String& p0)
{
    Logger::E("AwMotionEvent", "AwMotionEvent::AxisFromString, no implementation");
    //this method will not be called, if called, then implement
    return 0;
}

void AwMotionEvent::WriteToParcel(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* p0,
    /* [in] */ Int32 p1)
{
    Logger::E("AwMotionEvent", "AwMotionEvent::WriteToParcel, no implementation");
    //this method will not be called, if called, then implement
    return;
    /*
    AutoPtr<IMotionEvent> mObj = IMotionEvent::Probe(obj);
    if (NULL == mObj)
    {
        Logger::E("AwMotionEvent", "AwMotionEvent::WriteToParcel, mObj is NULL");
        return;
    }
    mObj->WriteToParcel(p0, p1);
    */
}

} // namespace Api
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
