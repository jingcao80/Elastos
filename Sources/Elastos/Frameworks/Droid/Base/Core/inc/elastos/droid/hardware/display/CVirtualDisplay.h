
#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_CVIRTUALDISPLAY_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_CVIRTUALDISPLAY_H__

#include "_Elastos_Droid_Hardware_Display_CVirtualDisplay.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IDisplay;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

/**
 * Represents a virtual display. The content of a virtual display is rendered to a
 * {@link android.view.Surface} that you must provide to {@link DisplayManager#createVirtualDisplay
 * createVirtualDisplay()}.
 * <p>
 * Because a virtual display renders to a surface provided by the application, it will be
 * released automatically when the process terminates and all remaining windows on it will
 * be forcibly removed.  However, you should also explicitly call {@link #release} when
 * you're done with it.
 * </p>
 *
 * @see DisplayManager#createVirtualDisplay
 */
CarClass(CVirtualDisplay)
    , public Object
    , public IVirtualDisplay
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CVirtualDisplay();

    ~CVirtualDisplay();

    CARAPI constructor(
        /* [in] */ IDisplayManagerGlobal* global,
        /* [in] */ IDisplay* display,
        /* [in] */ IIVirtualDisplayCallback* token,
        /* [in] */ ISurface* surface);

    /**
     * Gets the virtual display.
     */
    CARAPI GetDisplay(
        /* [out] */ IDisplay** result);

    /**
     * Gets the surface that backs the virtual display.
     */
    CARAPI GetSurface(
        /* [out] */ ISurface** result);

    /**
     * Sets the surface that backs the virtual display.
     * <p>
     * Detaching the surface that backs a virtual display has a similar effect to
     * turning off the screen.
     * </p><p>
     * It is still the caller's responsibility to destroy the surface after it has
     * been detached.
     * </p>
     *
     * @param surface The surface to set, or null to detach the surface from the virtual display.
     */
    CARAPI SetSurface(
        /* [in] */ ISurface* surface);

    /**
     * Asks the virtual display to resize.
     *<p>
     * This is really just a convenience to allow applications using
     * virtual displays to adapt to changing conditions without having
     * to tear down and recreate the display.
     * </p>
     */
    CARAPI Resize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    AutoPtr<IDisplayManagerGlobal> mGlobal;
    AutoPtr<IDisplay> mDisplay;
    AutoPtr<IIVirtualDisplayCallback> mToken;
    AutoPtr<ISurface> mSurface;
};

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_DISPLAY_CVIRTUALDISPLAY_H__
