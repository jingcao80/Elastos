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

#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_CVIRTUALDISPLAYCALLBACK_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_CVIRTUALDISPLAYCALLBACK_H__

#include "_Elastos_Droid_Hardware_Display_CVirtualDisplayCallback.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

/**
 * Interface for receiving information about a {@link VirtualDisplay}'s state changes.
 */
CarClass(CVirtualDisplayCallback)
    , public Object
    , public IVirtualDisplayCallback
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Called when the virtual display video projection has been
     * paused by the system or when the surface has been detached
     * by the application by calling setSurface(null).
     * The surface will not receive any more buffers while paused.
     */
    CARAPI OnPaused();

    /**
     * Called when the virtual display video projection has been
     * resumed after having been paused.
     */
    CARAPI OnResumed();

    /**
     * Called when the virtual display video projection has been
     * stopped by the system.  It will no longer receive frames
     * and it will never be resumed.  It is still the responsibility
     * of the application to release() the virtual display.
     */
    CARAPI OnStopped();
};

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_DISPLAY_CVIRTUALDISPLAYCALLBACK_H__
