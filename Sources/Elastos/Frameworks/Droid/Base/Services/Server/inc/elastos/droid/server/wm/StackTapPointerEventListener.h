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

#ifndef __ELASTOS_DROID_SERVER_WM_STACKTAPPOINTEREVENTLISTENER_H__
#define __ELASTOS_DROID_SERVER_WM_STACKTAPPOINTEREVENTLISTENER_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/wm/CWindowManagerService.h"

using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::View::IPointerEventListener;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class DisplayContent;

class StackTapPointerEventListener
    : public Object
    , public IPointerEventListener
{
public:
    CAR_INTERFACE_DECL()

    StackTapPointerEventListener(
        /* [in] */ CWindowManagerService* service,
        /* [in] */ DisplayContent* displayContent);

    CARAPI OnPointerEvent(
        /* [in] */ IMotionEvent* motionEvent);

    CARAPI_(void) SetTouchExcludeRegion(
        /* [in] */ IRegion* newRegion);

public:
    static const Int32 TAP_TIMEOUT_MSEC = 300;
    static const Float TAP_MOTION_SLOP_INCHES = 0.125f;

    Int32 mMotionSlop;
    Float mDownX;
    Float mDownY;
    Int32 mPointerId;
    AutoPtr<IRegion> mTouchExcludeRegion;
    AutoPtr<CWindowManagerService> mService;
    AutoPtr<DisplayContent> mDisplayContent;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_STACKTAPPOINTEREVENTLISTENER_H__
