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

#ifndef __ELASTOS_DROID_SERVER_DISPLAY_DISPLAYDEVICE_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_DISPLAYDEVICE_H__

#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/server/display/DisplayDeviceInfo.h"
#include "elastos/droid/server/display/DisplayAdapter.h"

using Elastos::IO::IPrintWriter;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::Hardware::Display::IDisplayViewport;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * Represents a physical display device such as the built-in display
 * an external monitor, or a WiFi display.
 * <p>
 * Display devices are guarded by the {@link DisplayManagerService.SyncRoot} lock.
 * </p>
 */
class DisplayDevice
    : public Object
{
public:
    DisplayDevice(
        /* [in] */ DisplayAdapter* displayAdapter,
        /* [in] */ IBinder* displayToken);

    /**
     * Gets the display adapter that owns the display device.
     *
     * @return The display adapter.
     */
    CARAPI_(AutoPtr<DisplayAdapter>) GetAdapterLocked();

    /**
     * Gets the Surface Flinger display token for this display.
     *
     * @return The display token, or NULL if the display is not being managed
     * by Surface Flinger.
     */
    CARAPI_(AutoPtr<IBinder>) GetDisplayTokenLocked();

    /**
     * Gets the name of the display device.
     *
     * @return The display device name.
     */
    CARAPI_(String) GetNameLocked();

    /**
     * Gets information about the display device.
     *
     * The information returned should not change between calls unless the display
     * adapter sent a {@link DisplayAdapter#DISPLAY_DEVICE_EVENT_CHANGED} event and
     * {@link #applyPendingDisplayDeviceInfoChangesLocked()} has been called to apply
     * the pending changes.
     *
     * @return The display device info, which should be treated as immutable by the caller.
     * The display device should allocate a new display device info object whenever
     * the data changes.
     */
    virtual CARAPI_(AutoPtr<DisplayDeviceInfo>) GetDisplayDeviceInfoLocked() = 0;

    /**
     * Applies any pending changes to the observable state of the display device
     * if the display adapter sent a {@link DisplayAdapter#DISPLAY_DEVICE_EVENT_CHANGED} event.
     */
    virtual CARAPI_(void) ApplyPendingDisplayDeviceInfoChangesLocked();

    /**
     * Gives the display device a chance to update its properties while in a transaction.
     */
    virtual CARAPI_(void) PerformTraversalInTransactionLocked();

    /**
     * Sets the display state, if supported.
     *
     * @return A runnable containing work to be deferred until after we have
     * exited the critical section, or null if none.
     */
    virtual CARAPI_(AutoPtr<IRunnable>) RequestDisplayStateLocked(
        /* [in] */ Int32 state);

    /**
     * Sets the refresh rate, if supported.
     */
    virtual CARAPI_(void) RequestRefreshRateLocked(
        /* [in] */ Float refreshRate);

    /**
     * Sets the display layer stack while in a transaction.
     */
    CARAPI_(void) SetLayerStackInTransactionLocked(
        /* [in] */ Int32 layerStack);

    /**
     * Sets the display projection while in a transaction.
     *
     * @param orientation defines the display's orientation
     * @param layerStackRect defines which area of the window manager coordinate
     *            space will be used
     * @param displayRect defines where on the display will layerStackRect be
     *            mapped to. displayRect is specified post-orientation, that is
     *            it uses the orientation seen by the end-user
     */
    CARAPI_(void) SetProjectionInTransactionLocked(
        /* [in] */ Int32 orientation,
        /* [in] */ IRect* layerStackRect,
        /* [in] */ IRect* displayRect);

    /**
     * Sets the display surface while in a transaction.
     */
    CARAPI_(void) SetSurfaceInTransactionLocked(
        /* [in] */ ISurface* surface);

    /**
     * Populates the specified viewport object with orientation,
     * physical and logical rects based on the display's current projection.
     */
    CARAPI_(void) PopulateViewportLocked(
        /* [in] */ IDisplayViewport* viewport);

    /**
     * Dumps the local state of the display device.
     * Does not need to dump the display device info because that is already dumped elsewhere.
     */
    virtual CARAPI_(void) DumpLocked(
        /* [in] */ IPrintWriter* pw);

private:
    AutoPtr<DisplayAdapter> mDisplayAdapter;
    AutoPtr<IBinder> mDisplayToken;

    // The display device does not manage these properties itself, they are set by
    // the display manager service.  The display device shouldn't really be looking at these.
    Int32 mCurrentLayerStack;
    Int32 mCurrentOrientation;
    AutoPtr<IRect> mCurrentLayerStackRect;
    AutoPtr<IRect> mCurrentDisplayRect;

    // The display device owns its surface, but it should only set it
    // within a transaction from performTraversalInTransactionLocked.
    AutoPtr<ISurface> mCurrentSurface;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_DISPLAYDEVICE_H__
