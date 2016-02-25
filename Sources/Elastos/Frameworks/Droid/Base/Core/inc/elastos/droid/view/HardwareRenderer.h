
#ifndef __ELASTOS_DROID_VIEW_HARDWARERENDERER_H__
#define __ELASTOS_DROID_VIEW_HARDWARERENDERER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::View;
using Elastos::IO::IFile;
using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace View {

class HardwareRenderer
    : public Object
    , public IHardwareRenderer
{
public:
    CAR_INTERFACE_DECL()

    HardwareRenderer();

    virtual ~HardwareRenderer();

    /**
     * Invoke this method to disable hardware rendering in the current process.
     *
     * @hide
     */
    static CARAPI Disable(
        /* [in] */ Boolean system);

    /**
     * Controls whether or not the hardware renderer should aggressively
     * trim memory. Note that this must not be set for any process that
     * uses WebView! This should be only used by system_process or similar
     * that do not go into the background.
     */
    static CARAPI EnableForegroundTrimming();

    /**
     * Indicates whether hardware acceleration is available under any form for
     * the view hierarchy.
     *
     * @return True if the view hierarchy can potentially be hardware accelerated,
     *         false otherwise
     */
    static CARAPI IsAvailable(
        /* [out] */ Boolean* res);

    /**
     * Sets the directory to use as a persistent storage for hardware rendering
     * resources.
     *
     * @param cacheDir A directory the current process can write to
     *
     * @hide
     */
    static CARAPI SetupDiskCache(
        /* [in] */ IFile* cacheDir);

    /**
     * Creates a hardware renderer using OpenGL.
     *
     * @param translucent True if the surface is translucent, false otherwise
     *
     * @return A hardware renderer backed by OpenGL.
     */
    static CARAPI_(AutoPtr<HardwareRenderer>) Create(
        /* [in] */ IContext* context,
        /* [in] */ Boolean translucent);

    /**
     * Invoke this method when the system is running out of memory. This
     * method will attempt to recover as much memory as possible, based on
     * the specified hint.
     *
     * @param level Hint about the amount of memory that should be trimmed,
     *              see {@link android.content.ComponentCallbacks}
     */
    static CARAPI TrimMemory(
        /* [in] */ Int32 level);

    /**
     * Initializes the hardware renderer for the specified surface and setup the
     * renderer for drawing, if needed. This is invoked when the ViewAncestor has
     * potentially lost the hardware renderer. The hardware renderer should be
     * reinitialized and setup when the render {@link #isRequested()} and
     * {@link #isEnabled()}.
     *
     * @param width The width of the drawing surface.
     * @param height The height of the drawing surface.
     * @param surface The surface to hardware accelerate
     * @param surfaceInsets The drawing surface insets to apply
     *
     * @return true if the surface was initialized, false otherwise. Returning
     *         false might mean that the surface was already initialized.
     */
    CARAPI InitializeIfNeeded(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ ISurface* surface,
        /* [in] */ IRect* surfaceInsets,
        /* [out] */ Boolean* result);

    /**
     * Indicates whether hardware acceleration is currently enabled.
     *
     * @return True if hardware acceleration is in use, false otherwise.
     */
    CARAPI IsEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Indicates whether hardware acceleration is currently enabled.
     *
     * @param enabled True if the hardware renderer is in use, false otherwise.
     */
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Indicates whether hardware acceleration is currently request but not
     * necessarily enabled yet.
     *
     * @return True if requested, false otherwise.
     */
    CARAPI IsRequested(
        /* [out] */ Boolean* requested);

    /**
     * Indicates whether hardware acceleration is currently requested but not
     * necessarily enabled yet.
     *
     * @return True to request hardware acceleration, false otherwise.
     */
    CARAPI SetRequested(
        /* [in] */ Boolean requested);


    /**
     * Draws the specified view.
     *
     * @param view The view to draw.
     * @param attachInfo AttachInfo tied to the specified view.
     * @param callbacks Callbacks invoked when drawing happens.
     */
    virtual CARAPI Draw(
        /* [in] */ IView* view,
        /* [in] */ View::AttachInfo* attachInfo,
        /* [in] */ IHardwareDrawCallbacks* callbacks) = 0;

public:
    /**
     * System property used to enable or disable hardware rendering profiling.
     * The default value of this property is assumed to be false.
     *
     * When profiling is enabled, the adb shell dumpsys gfxinfo command will
     * output extra information about the time taken to execute by the last
     * frames.
     *
     * Possible values:
     * "true", to enable profiling
     * "visual_bars", to enable profiling and visualize the results on screen
     * "false", to disable profiling
     *
     * @see #PROFILE_PROPERTY_VISUALIZE_BARS
     *
     * @hide
     */
    static const String PROFILE_PROPERTY;

    /**
     * Value for {@link #PROFILE_PROPERTY}. When the property is set to this
     * value, profiling data will be visualized on screen as a bar chart.
     *
     * @hide
     */
    static const String PROFILE_PROPERTY_VISUALIZE_BARS;

    /**
     * Turn on to draw dirty regions every other frame.
     *
     * Possible values:
     * "true", to enable dirty regions debugging
     * "false", to disable dirty regions debugging
     *
     * @hide
     */
    static const String DEBUG_DIRTY_REGIONS_PROPERTY;

    /**
     * Turn on to flash hardware layers when they update.
     *
     * Possible values:
     * "true", to enable hardware layers updates debugging
     * "false", to disable hardware layers updates debugging
     *
     * @hide
     */
    static const String DEBUG_SHOW_LAYERS_UPDATES_PROPERTY;

    /**
     * Controls overdraw debugging.
     *
     * Possible values:
     * "false", to disable overdraw debugging
     * "show", to show overdraw areas on screen
     * "count", to display an overdraw counter
     *
     * @hide
     */
    static const String DEBUG_OVERDRAW_PROPERTY;

    /**
     * Value for {@link #DEBUG_OVERDRAW_PROPERTY}. When the property is set to this
     * value, overdraw will be shown on screen by coloring pixels.
     *
     * @hide
     */
    static const String OVERDRAW_PROPERTY_SHOW;

    /**
     * Turn on to debug non-rectangular clip operations.
     *
     * Possible values:
     * "hide", to disable this debug mode
     * "highlight", highlight drawing commands tested against a non-rectangular clip
     * "stencil", renders the clip region on screen when set
     *
     * @hide
     */
    static const String DEBUG_SHOW_NON_RECTANGULAR_CLIP_PROPERTY;

    /**
     * A process can set this flag to false to prevent the use of hardware
     * rendering.
     *
     * @hide
     */
    static Boolean sRendererDisabled;

    /**
     * Further hardware renderer disabling for the system process.
     *
     * @hide
     */
    static Boolean sSystemRendererDisabled;

    static Boolean sTrimForeground;

protected:
//    static String LOG_TAG;

    /**
     * System property used to enable or disable dirty regions invalidation.
     * This property is only queried if {@link #RENDER_DIRTY_REGIONS} is true.
     * The default value of this property is assumed to be true.
     *
     * Possible values:
     * "true", to enable partial invalidates
     * "false", to disable partial invalidates
     */
    static const String RENDER_DIRTY_REGIONS_PROPERTY;

    /**
     * System property used to specify the number of frames to be used
     * when doing hardware rendering profiling.
     * The default value of this property is #PROFILE_MAX_FRAMES.
     *
     * When profiling is enabled, the adb shell dumpsys gfxinfo command will
     * output extra information about the time taken to execute by the last
     * frames.
     *
     * Possible values:
     * "60", to set the limit of frames to 60
     */
    static const String PROFILE_MAXFRAMES_PROPERTY;

    /**
     * System property used to debug EGL configuration choice.
     *
     * Possible values:
     * "choice", print the chosen configuration only
     * "all", print all possible configurations
     */
    static const String PRINT_CONFIG_PROPERTY;

private:
    /**
     * Name of the file that holds the shaders cache.
     */
    static const String CACHE_PATH_SHADERS;

    Boolean mEnabled;
    Boolean mRequested;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_HARDWARERENDERER_H__
