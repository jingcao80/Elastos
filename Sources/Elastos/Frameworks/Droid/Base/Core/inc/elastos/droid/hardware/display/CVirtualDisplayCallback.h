
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
