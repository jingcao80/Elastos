
#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_CDISPLAYMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_CDISPLAYMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Hardware_Display_CDisplayManager.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Media::Projection::IMediaProjection;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

/**
 * Manages the properties of attached displays.
 * <p>
 * Get an instance of this class by calling
 * {@link android.content.Context#getSystemService(java.lang.String)
 * Context.getSystemService()} with the argument
 * {@link android.content.Context#DISPLAY_SERVICE}.
 * </p>
 */
CarClass(CDisplayManager)
    , public Object
    , public IDisplayManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDisplayManager();

    ~CDisplayManager();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Gets information about a logical display.
     *
     * The display metrics may be adjusted to provide compatibility
     * for legacy applications.
     *
     * @param displayId The logical display id.
     * @return The display object, or NULL if there is no valid display with the given id.
     */
    CARAPI GetDisplay(
        /* [in] */ Int32 displayId,
        /* [out] */ IDisplay** display);

    /**
     * Gets all currently valid logical displays.
     *
     * @return An array containing all displays.
     */
    CARAPI GetDisplays(
        /* [out] */ ArrayOf<IDisplay*>** displays);

    /**
     * Gets all currently valid logical displays of the specified category.
     * <p>
     * When there are multiple displays in a category the returned displays are sorted
     * of preference.  For example, if the requested category is
     * {@link #DISPLAY_CATEGORY_PRESENTATION} and there are multiple presentation displays
     * then the displays are sorted so that the first display in the returned array
     * is the most preferred presentation display.  The application may simply
     * use the first display or allow the user to choose.
     * </p>
     *
     * @param category The requested display category or NULL to return all displays.
     * @return An array containing all displays sorted by order of preference.
     *
     * @see #DISPLAY_CATEGORY_PRESENTATION
     */
    CARAPI GetDisplays(
        /* [in] */ const String& category,
        /* [out] */ ArrayOf<IDisplay*>** displays);

    /**
     * Registers an display listener to receive notifications about when
     * displays are added, removed or changed.
     *
     * @param listener The listener to register.
     * @param handler The handler on which the listener should be invoked, or NULL
     * if the listener should be invoked on the calling thread's looper.
     *
     * @see #unregisterDisplayListener
     */
    CARAPI RegisterDisplayListener(
        /* [in] */ IDisplayListener* listener,
        /* [in] */ IHandler* handler);

    /**
     * Unregisters a display listener.
     *
     * @param listener The listener to unregister.
     *
     * @see #registerDisplayListener
     */
    CARAPI UnregisterDisplayListener(
        /* [in] */ IDisplayListener* listener);

    /**
     * Initiates a fresh scan of availble Wifi displays.
     * The results are sent as a {@link #ACTION_WIFI_DISPLAY_STATUS_CHANGED} broadcast.
     * @hide
     */
    CARAPI StartWifiDisplayScan();

    /**
     * Stops scanning for available Wifi displays.
     * <p>
     * Requires {@link android.Manifest.permission#CONFIGURE_WIFI_DISPLAY}.
     * </p>
     *
     * @hide
     */
    CARAPI StopWifiDisplayScan();

    /**
     * Connects to a Wifi display.
     * The results are sent as a {@link #ACTION_WIFI_DISPLAY_STATUS_CHANGED} broadcast.
     * <p>
     * Automatically remembers the display after a successful connection, if not
     * already remembered.
     * </p><p>
     * Requires {@link android.Manifest.permission#CONFIGURE_WIFI_DISPLAY}.
     * </p>
     *
     * @param deviceAddress The MAC address of the device to which we should connect.
     * @hide
     */
    CARAPI ConnectWifiDisplay(
        /* [in] */ const String& deviceAddress);

    /** @hide */
    CARAPI PauseWifiDisplay();

    /** @hide */
    CARAPI ResumeWifiDisplay();


    /**
     * Disconnects from the current Wifi display.
     * The results are sent as a {@link #ACTION_WIFI_DISPLAY_STATUS_CHANGED} broadcast.
     * @hide
     */
    CARAPI DisconnectWifiDisplay();

    /**
     * Renames a Wifi display.
     * <p>
     * The display must already be remembered for this call to succeed.  In other words,
     * we must already have successfully connected to the display at least once and then
     * not forgotten it.
     * </p><p>
     * Requires {@link android.Manifest.permission#CONFIGURE_WIFI_DISPLAY}.
     * </p>
     *
     * @param deviceAddress The MAC address of the device to rename.
     * @param alias The alias name by which to remember the device, or NULL
     * or empty if no alias should be used.
     * @hide
     */
    CARAPI RenameWifiDisplay(
        /* [in] */ const String& deviceAddress,
        /* [in] */ const String& alias);

    /**
     * Forgets a previously remembered Wifi display.
     * <p>
     * Automatically disconnects from the display if currently connected to it.
     * </p><p>
     * Requires {@link android.Manifest.permission#CONFIGURE_WIFI_DISPLAY}.
     * </p>
     *
     * @param deviceAddress The MAC address of the device to forget.
     * @hide
     */
    CARAPI ForgetWifiDisplay(
        /* [in] */ const String& deviceAddress);

    /**
     * Gets the current Wifi display status.
     * Watch for changes in the status by registering a broadcast receiver for
     * {@link #ACTION_WIFI_DISPLAY_STATUS_CHANGED}.
     *
     * @return The current Wifi display status.
     * @hide
     */
    CARAPI GetWifiDisplayStatus(
        /* [out] */ IWifiDisplayStatus** status);

    /**
     * Creates a virtual display.
     *
     * @see #createVirtualDisplay(String, int, int, int, Surface, int, VirtualDisplay.Callback)
     */
    CARAPI CreateVirtualDisplay(
        /* [in] */ const String& name,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi,
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 flags,
        /* [out] */ IVirtualDisplay** result);

    /**
     * Creates a virtual display.
     * <p>
     * The content of a virtual display is rendered to a {@link Surface} provided
     * by the application.
     * </p><p>
     * The virtual display should be {@link VirtualDisplay#release released}
     * when no longer needed.  Because a virtual display renders to a surface
     * provided by the application, it will be released automatically when the
     * process terminates and all remaining windows on it will be forcibly removed.
     * </p><p>
     * The behavior of the virtual display depends on the flags that are provided
     * to this method.  By default, virtual displays are created to be private,
     * non-presentation and unsecure.  Permissions may be required to use certain flags.
     * </p><p>
     * As of {@link android.os.Build.VERSION_CODES#KITKAT_WATCH}, the surface may
     * be attached or detached dynamically using {@link VirtualDisplay#setSurface}.
     * Previously, the surface had to be non-null when {@link #createVirtualDisplay}
     * was called and could not be changed for the lifetime of the display.
     * </p><p>
     * Detaching the surface that backs a virtual display has a similar effect to
     * turning off the screen.
     * </p>
     *
     * @param name The name of the virtual display, must be non-empty.
     * @param width The width of the virtual display in pixels, must be greater than 0.
     * @param height The height of the virtual display in pixels, must be greater than 0.
     * @param densityDpi The density of the virtual display in dpi, must be greater than 0.
     * @param surface The surface to which the content of the virtual display should
     * be rendered, or null if there is none initially.
     * @param flags A combination of virtual display flags:
     * {@link #VIRTUAL_DISPLAY_FLAG_PUBLIC}, {@link #VIRTUAL_DISPLAY_FLAG_PRESENTATION},
     * {@link #VIRTUAL_DISPLAY_FLAG_SECURE}, {@link #VIRTUAL_DISPLAY_FLAG_OWN_CONTENT_ONLY},
     * or {@link #VIRTUAL_DISPLAY_FLAG_AUTO_MIRROR}.
     * @param callback Callback to call when the state of the {@link VirtualDisplay} changes
     * @param handler The handler on which the listener should be invoked, or null
     * if the listener should be invoked on the calling thread's looper.
     * @return The newly created virtual display, or null if the application could
     * not create the virtual display.
     *
     * @throws SecurityException if the caller does not have permission to create
     * a virtual display with the specified flags.
     */
    CARAPI CreateVirtualDisplay(
        /* [in] */ const String& name,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi,
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 flags,
        /* [in] */ IVirtualDisplayCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IVirtualDisplay** result);

    /** @hide */
    CARAPI CreateVirtualDisplay(
        /* [in] */ IMediaProjection* projection,
        /* [in] */ const String& name,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 densityDpi,
        /* [in] */ ISurface* surface,
        /* [in] */ Int32 flags,
        /* [in] */ IVirtualDisplayCallback* cb,
        /* [in] */ IHandler* handler,
        /* [out] */ IVirtualDisplay** result);

private:
    CARAPI_(void) AddAllDisplaysLocked(
        /* [in] */ List<AutoPtr<IDisplay> >& displays,
        /* [in] */ ArrayOf<Int32>* displayIds);

    CARAPI_(void) AddPresentationDisplaysLocked(
    /* [in] */ List<AutoPtr<IDisplay> >& displays,
    /* [in] */ ArrayOf<Int32>* displayIds,
    /* [in] */ Int32 matchType);

    CARAPI_(AutoPtr<IDisplay>) GetOrCreateDisplayLocked(
        /* [in] */ Int32 displayId,
        /* [in] */ Boolean assumeValid);

private:
    static const char* TAG;
    static const Boolean DEBUG = FALSE;

    IContext* mContext;
    AutoPtr<IDisplayManagerGlobal> mGlobal;

    Object mLock;
    HashMap<Int32, AutoPtr<IDisplay> > mDisplays;

    List<AutoPtr<IDisplay> > mTempDisplays;
};

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_DISPLAY_CDISPLAYMANAGER_H__
