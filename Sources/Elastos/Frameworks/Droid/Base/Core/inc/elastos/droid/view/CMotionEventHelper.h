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

#ifndef __ELASTOS_DROID_VIEW_CMOTIONEVENTHELPER_H__
#define __ELASTOS_DROID_VIEW_CMOTIONEVENTHELPER_H__

#include "_Elastos_Droid_View_CMotionEventHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CMotionEventHelper)
    , public Singleton
    , public IMotionEventHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI Obtain(
        /* [out] */ IMotionEvent** o);

    CARAPI Obtain(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 pointerCount,
        /* [in] */ ArrayOf<IPointerProperties*>* pointerProperties,
        /* [in] */ ArrayOf<IPointerCoords*>* pointerCoords,
        /* [in] */ Int32 metaState,
        /* [in] */ Int32 buttonState,
        /* [in] */ Float xPrecision,
        /* [in] */ Float yPrecision,
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 edgeFlags,
        /* [in] */ Int32 source,
        /* [in] */ Int32 flags,
        /* [out] */ IMotionEvent** event);

    CARAPI Obtain(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 pointerCount,
        /* [in] */ ArrayOf<Int32>* pointerIds,
        /* [in] */ ArrayOf<IPointerCoords*>* pointerCoords,
        /* [in] */ Int32 metaState,
        /* [in] */ Float xPrecision,
        /* [in] */ Float yPrecision,
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 edgeFlags,
        /* [in] */ Int32 source,
        /* [in] */ Int32 flags,
        /* [out] */ IMotionEvent** event);

    CARAPI Obtain(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float pressure,
        /* [in] */ Float size,
        /* [in] */ Int32 metaState,
        /* [in] */ Float xPrecision,
        /* [in] */ Float yPrecision,
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 edgeFlags,
        /* [out] */ IMotionEvent** event);

    CARAPI Obtain(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Int32 pointerCount,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float pressure,
        /* [in] */ Float size,
        /* [in] */ Int32 metaState,
        /* [in] */ Float xPrecision,
        /* [in] */ Float yPrecision,
        /* [in] */ Int32 deviceId,
        /* [in] */ Int32 edgeFlags,
        /* [out] */ IMotionEvent** event);

    CARAPI Obtain(
        /* [in] */ Int64 downTime,
        /* [in] */ Int64 eventTime,
        /* [in] */ Int32 action,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Int32 metaState,
        /* [out] */ IMotionEvent** event);

    CARAPI Obtain(
        /* [in] */ IMotionEvent* o,
        /* [out] */ IMotionEvent** event);

    CARAPI ObtainNoHistory(
        /* [in] */ IMotionEvent* o,
        /* [out] */ IMotionEvent** event);

    CARAPI ActionToString(
        /* [in] */ Int32 action,
        /* [out] */ String* str);

    CARAPI AxisToString(
        /* [in] */ Int32 axis,
        /* [out] */ String* str);

    CARAPI AxisFromString(
        /* [in] */ const String& symbolicName,
        /* [out] */ Int32* axis);

    CARAPI ButtonStateToString(
        /* [in] */ Int32 buttonState,
        /* [out] */ String* str);

    CARAPI ToolTypeToString(
        /* [in] */ Int32 toolType,
        /* [out] */ String* str);

    CARAPI CreateFromParcelBody(
        /* [in] */ IParcel* in,
        /* [out] */ IMotionEvent** event);
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CMOTIONEVENTHELPER_H__
