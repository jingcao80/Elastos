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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NATIVE_API_AWBYTEBUFFER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NATIVE_API_AWBYTEBUFFER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Api {

class AwMotionEvent
{
public:
    static CARAPI_(void*) ElaMotionEventCallback_Init();

private:
    static CARAPI_(void) Finalize(
            /* [in] */ IInterface* obj);

    static CARAPI_(AutoPtr<IInterface>) ObtainAVME_J_J_I_I_LAVMEPP_LAVMEPC_I_I_F_F_I_I_I_I(
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
            /* [in] */ Int32 p13);

    static CARAPI_(AutoPtr<IInterface>) ObtainAVME_J_J_I_I_AI_LAVMEPC_I_F_F_I_I_I_I(
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
            /* [in] */ Int32 p12);

    static CARAPI_(AutoPtr<IInterface>) ObtainAVME_J_J_I_F_F_F_F_I_F_F_I_I(
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
            /* [in] */ Int32 p11);

    static CARAPI_(AutoPtr<IInterface>) ObtainAVME_J_J_I_I_F_F_F_F_I_F_F_I_I(
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
            /* [in] */ Int32 p12);

    static CARAPI_(AutoPtr<IInterface>) ObtainAVME_J_J_I_F_F_I(
            /* [in] */ Int64 p0,
            /* [in] */ Int64 p1,
            /* [in] */ Int32 p2,
            /* [in] */ Float p3,
            /* [in] */ Float p4,
            /* [in] */ Int32 p5);

    static CARAPI_(AutoPtr<IInterface>) ObtainAVME_AVME(
            /* [in] */ IInterface* p0);

    static CARAPI_(AutoPtr<IInterface>) ObtainNoHistory(
            /* [in] */ IInterface* p0);

    static CARAPI_(void) Recycle(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetDeviceId(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetSource(
            /* [in] */ IInterface* obj);

    static CARAPI_(void) SetSource(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Int32) GetAction(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetActionMasked(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetActionIndex(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetFlags(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int64) GetDownTime(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int64) GetEventTime(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetXF(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetYF(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetPressureF(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetSizeF(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetTouchMajorF(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetTouchMinorF(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetToolMajorF(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetToolMinorF(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetOrientationF(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetAxisValueF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Int32) GetPointerCount(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetPointerId(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Int32) GetToolType(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Int32) FindPointerIndex(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetXF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetYF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetPressureF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetSizeF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetTouchMajorF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetTouchMinorF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetToolMajorF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetToolMinorF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetOrientationF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetAxisValueF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(void) GetPointerCoords(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ IInterface* p1);

    static CARAPI_(void) GetPointerProperties(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ IInterface* p1);

    static CARAPI_(Int32) GetMetaState(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetButtonState(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetRawX(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetRawY(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetXPrecision(
            /* [in] */ IInterface* obj);

    static CARAPI_(Float) GetYPrecision(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int32) GetHistorySize(
            /* [in] */ IInterface* obj);

    static CARAPI_(Int64) GetHistoricalEventTime(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalXF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalYF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalPressureF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalSizeF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalTouchMajorF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalTouchMinorF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalToolMajorF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalToolMinorF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalOrientationF_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(Float) GetHistoricalAxisValueF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalXF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalYF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalPressureF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalSizeF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalTouchMajorF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalTouchMinorF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalToolMajorF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalToolMinorF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalOrientationF_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1);

    static CARAPI_(Float) GetHistoricalAxisValueF_I_I_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1,
            /* [in] */ Int32 p2);

    static CARAPI_(void) GetHistoricalPointerCoords(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0,
            /* [in] */ Int32 p1,
            /* [in] */ IInterface* p2);

    static CARAPI_(Int32) GetEdgeFlags(
            /* [in] */ IInterface* obj);

    static CARAPI_(void) SetEdgeFlags(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(void) SetAction(
            /* [in] */ IInterface* obj,
            /* [in] */ Int32 p0);

    static CARAPI_(void) OffsetLocation(
            /* [in] */ IInterface* obj,
            /* [in] */ Float p0,
            /* [in] */ Float p1);

    static CARAPI_(void) SetLocation(
            /* [in] */ IInterface* obj,
            /* [in] */ Float p0,
            /* [in] */ Float p1);

    static CARAPI_(void) Transform(
            /* [in] */ IInterface* obj,
            /* [in] */ IInterface* p0);

    static CARAPI_(void) AddBatchV_J_F_F_F_F_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int64 p0,
            /* [in] */ Float p1,
            /* [in] */ Float p2,
            /* [in] */ Float p3,
            /* [in] */ Float p4,
            /* [in] */ Int32 p5);

    static CARAPI_(void) AddBatchV_J_LAVMEPC_I(
            /* [in] */ IInterface* obj,
            /* [in] */ Int64 p0,
            /* [in] */ ArrayOf<IInterface*>* p1,
            /* [in] */ Int32 p2);

    static CARAPI_(String) ToString(
            /* [in] */ IInterface* obj);

    static CARAPI_(String) AxisToString(
            /* [in] */ Int32 p0);

    static CARAPI_(Int32) AxisFromString(
            /* [in] */ const String& p0);

    static CARAPI_(void) WriteToParcel(
            /* [in] */ IInterface* obj,
            /* [in] */ IInterface* p0,
            /* [in] */ Int32 p1);
};

} // namespace Api
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_NATIVE_API_AWBYTEBUFFER_H__
