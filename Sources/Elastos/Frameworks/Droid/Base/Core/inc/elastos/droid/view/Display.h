
#ifndef __ELASTOS_DROID_VIEW_DISPLAY_H__
#define __ELASTOS_DROID_VIEW_DISPLAY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Hardware::Display::IDisplayManagerGlobal;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

class Display
    : public Object
    , public IDisplay
{
public:
    /**
     * @hide
     */
    static CARAPI_(String) TypeToString(
        /* [in] */ Int32 type);

    static CARAPI_(String) StateToString(
        /* [in] */ Int32 state);

public:
    CAR_INTERFACE_DECL();

    /**
     * Internal method to create a display.
     * Applications should use {@link android.view.WindowManager#getDefaultDisplay()}
     * or {@link android.hardware.display.DisplayManager#getDisplay}
     * to get a display object.
     *
     * @hide
     */
    Display();

    CARAPI constructor(
        /* [in] */ IDisplayManagerGlobal* global,
        /* [in] */ Int32 displayId,
        /* [in] */ IDisplayInfo* displayInfo /*not NULL*/,
        /* [in] */ IDisplayAdjustments* daj);

    /**
     * Gets the display id.
     * <p>
     * Each logical display has a unique id.
     * The default display has id {@link #DEFAULT_DISPLAY}.
     * </p>
     */
    CARAPI GetDisplayId(
        /* [out] */ Int32* displayId);

    /**
     * Returns true if this display is still valid, FALSE if the display has been removed.
     *
     * If the display is invalid, then the methods of this class will
     * continue to report the most recently observed display information.
     * However, it is unwise (and rather fruitless) to continue using a
     * {@link Display} object after the display's demise.
     *
     * It's possible for a display that was previously invalid to become
     * valid again if a display with the same id is reconnected.
     *
     * @return True if the display is still valid.
     */
    CARAPI IsValid(
        /* [out] */ Boolean* isValid);

    /**
     * Gets a full copy of the display information.
     *
     * @param outDisplayInfo The object to receive the copy of the display information.
     * @return True if the display is still valid.
     * @hide
     */
    CARAPI GetDisplayInfo(
        /* [in] */ IDisplayInfo* outDisplayInfo,
        /* [out] */ Boolean* isValid);

    /**
     * Gets the display's layer stack.
     *
     * Each display has its own independent layer stack upon which surfaces
     * are placed to be managed by surface flinger.
     *
     * @return The display's layer stack number.
     * @hide
     */
    CARAPI GetLayerStack(
        /* [out] */ Int32* layerStack);

    /**
     * Returns a combination of flags that describe the capabilities of the display.
     *
     * @return The display flags.
     *
     * @see #FLAG_SUPPORTS_PROTECTED_BUFFERS
     * @see #FLAG_SECURE
     */
    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    /**
     * Gets the display type.
     *
     * @return The display type.
     *
     * @see #TYPE_UNKNOWN
     * @see #TYPE_BUILT_IN
     * @see #TYPE_HDMI
     * @see #TYPE_WIFI
     * @see #TYPE_OVERLAY
     * @hide
     */
    CARAPI GetType(
        /* [out] */ Int32* type);

    /**
     * Gets the display address, or NULL if none.
     * Interpretation varies by display type.
     *
     * @return The display address.
     * @hide
     */
    CARAPI GetAddress(
        /* [out] */ String* address);

    /**
     * Gets the UID of the application that owns this display, or zero if it is
     * owned by the system.
     * <p>
     * If the display is private, then only the owner can use it.
     * </p>
     *
     * @hide
     */
    CARAPI GetOwnerUid(
        /* [out] */ Int32* uid);

    /**
     * Gets the package name of the application that owns this display, or null if it is
     * owned by the system.
     * <p>
     * If the display is private, then only the owner can use it.
     * </p>
     *
     * @hide
     */
    CARAPI GetOwnerPackageName(
        /* [out] */ String* name);

    /**
     * Gets the compatibility info used by this display instance.
     *
     * @return The display adjustments holder, or null if none is required.
     * @hide
     */
    CARAPI GetDisplayAdjustments(
        /* [out] */ IDisplayAdjustments** daj);

    /**
     * Gets the name of the display.
     * <p>
     * Note that some displays may be renamed by the user.
     * </p>
     *
     * @return The display's name.
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Gets the size of the display, in pixels.
     * <p>
     * Note that this value should <em>not</em> be used for computing layouts,
     * since a device will typically have screen decoration (such as a status bar)
     * along the edges of the display that reduce the amount of application
     * space available from the size returned here.  Layouts should instead use
     * the window size.
     * </p><p>
     * The size is adjusted based on the current rotation of the display.
     * </p><p>
     * The size returned by this method does not necessarily represent the
     * actual raw size (native resolution) of the display.  The returned size may
     * be adjusted to exclude certain system decoration elements that are always visible.
     * It may also be scaled to provide compatibility with older applications that
     * were originally designed for smaller displays.
     * </p>
     *
     * @param outSize A {@link Point} object to receive the size information.
     */
    CARAPI GetSize(
        /* [in] */ IPoint* outSize);

    /**
     * Gets the size of the display as a rectangle, in pixels.
     *
     * @param outSize A {@link Rect} object to receive the size information.
     * @see #getSize(Point)
     */
    CARAPI GetRectSize(
        /* [in] */ IRect* outSize);

    /**
     * Return the range of display sizes an application can expect to encounter
     * under normal operation, as Int64 as there is no physical change in screen
     * size.  This is basically the sizes you will see as the orientation
     * changes, taking into account whatever screen decoration there is in
     * each rotation.  For example, the status bar is always at the top of the
     * screen, so it will reduce the height both in landscape and portrait, and
     * the smallest height returned here will be the smaller of the two.
     *
     * This is intended for applications to get an idea of the range of sizes
     * they will encounter while going through device rotations, to provide a
     * stable UI through rotation.  The sizes here take into account all standard
     * system decorations that reduce the size actually available to the
     * application: the status bar, navigation bar, system bar, etc.  It does
     * <em>not</em> take into account more transient elements like an IME
     * soft keyboard.
     *
     * @param outSmallestSize Filled in with the smallest width and height
     * that the application will encounter, in pixels (not dp units).  The x
     * (width) dimension here directly corresponds to
     * {@link android.content.res.Configuration#smallestScreenWidthDp
     * Configuration.smallestScreenWidthDp}, except the value here is in raw
     * screen pixels rather than dp units.  Your application may of course
     * still get smaller space yet if, for example, a soft keyboard is
     * being displayed.
     * @param outLargestSize Filled in with the largest width and height
     * that the application will encounter, in pixels (not dp units).  Your
     * application may of course still get larger space than this if,
     * for example, screen decorations like the status bar are being hidden.
     */
    CARAPI GetCurrentSizeRange(
        /* [in] */ IPoint* outSmallestSize,
        /* [in] */ IPoint* outLargestSize);

    /**
     * Return the maximum screen size dimension that will happen.  This is
     * mostly for wallpapers.
     * @hide
     */
    CARAPI GetMaximumSizeDimension(
        /* [out] */ Int32* maximumSizeDimension);

    /**
     * @deprecated Use {@link #getSize(Point)} instead.
     */
    //@Deprecated
    CARAPI GetWidth(
        /* [out] */ Int32* width);

    /**
     * @deprecated Use {@link #getSize(Point)} instead.
     */
    //@Deprecated
    CARAPI GetHeight(
        /* [out] */ Int32* height);

    /**
     * @hide
     * Return a rectangle defining the insets of the overscan region of the display.
     * Each field of the rectangle is the number of pixels the overscan area extends
     * into the display on that side.
     */
    CARAPI GetOverscanInsets(
        /* [in] */ IRect* outRect);

    /**
     * Returns the rotation of the screen from its "natural" orientation.
     * The returned value may be {@link Surface#ROTATION_0 Surface.ROTATION_0}
     * (no rotation), {@link Surface#ROTATION_90 Surface.ROTATION_90},
     * {@link Surface#ROTATION_180 Surface.ROTATION_180}, or
     * {@link Surface#ROTATION_270 Surface.ROTATION_270}.  For
     * example, if a device has a naturally tall screen, and the user has
     * turned it on its side to go into a landscape orientation, the value
     * returned here may be either {@link Surface#ROTATION_90 Surface.ROTATION_90}
     * or {@link Surface#ROTATION_270 Surface.ROTATION_270} depending on
     * the direction it was turned.  The angle is the rotation of the drawn
     * graphics on the screen, which is the opposite direction of the physical
     * rotation of the device.  For example, if the device is rotated 90
     * degrees counter-clockwise, to compensate rendering will be rotated by
     * 90 degrees clockwise and thus the returned value here will be
     * {@link Surface#ROTATION_90 Surface.ROTATION_90}.
     */
    CARAPI GetRotation(
        /* [out] */ Int32* rotation);

    /**
     * @deprecated use {@link #getRotation}
     * @return orientation of this display.
     */
    //@Deprecated
    CARAPI GetOrientation(
        /* [out] */ Int32* orientation);

    /**
     * Gets the pixel format of the display.
     * @return One of the constants defined in {@link android.graphics.PixelFormat}.
     *
     * @deprecated This method is no longer supported.
     * The result is always {@link PixelFormat#RGBA_8888}.
     */
    //@Deprecated
    CARAPI GetPixelFormat(
        /* [out] */ Int32* pixelFormat);

    /**
     * Gets the refresh rate of this display in frames per second.
     */
    CARAPI GetRefreshRate(
        /* [out] */ Float* refreshRate);

    /**
     * Get the supported refresh rates of this display in frames per second.
     */
    CARAPI GetSupportedRefreshRates(
        /* [out, callee] */ ArrayOf<Float>** rates);

    /**
     * Gets the app VSYNC offset, in nanoseconds.  This is a positive value indicating
     * the phase offset of the VSYNC events provided by Choreographer relative to the
     * display refresh.  For example, if Choreographer reports that the refresh occurred
     * at time N, it actua`lly occurred at (N - appVsyncOffset).
     * <p>
     * Apps generally do not need to be aware of this.  It's only useful for fine-grained
     * A/V synchronization.
     */
    CARAPI GetAppVsyncOffsetNanos(
        /* [out] */ Int64* nanos);

    /**
     * This is how far in advance a buffer must be queued for presentation at
     * a given time.  If you want a buffer to appear on the screen at
     * time N, you must submit the buffer before (N - presentationDeadline).
     * <p>
     * The desired presentation time for GLES rendering may be set with
     * {@link android.opengl.EGLExt#eglPresentationTimeANDROID}.  For video decoding, use
     * {@link android.media.MediaCodec#releaseOutputBuffer(int, long)}.  Times are
     * expressed in nanoseconds, using the system monotonic clock
     * ({@link System#nanoTime}).
     */
    CARAPI GetPresentationDeadlineNanos(
        /* [out] */ Int64* result);


    /**
     * Gets display metrics that describe the size and density of this display.
     * <p>
     * The size is adjusted based on the current rotation of the display.
     * </p><p>
     * The size returned by this method does not necessarily represent the
     * actual raw size (native resolution) of the display.  The returned size may
     * be adjusted to exclude certain system decor elements that are always visible.
     * It may also be scaled to provide compatibility with older applications that
     * were originally designed for smaller displays.
     * </p>
     *
     * @param outMetrics A {@link DisplayMetrics} object to receive the metrics.
     */
    CARAPI GetMetrics(
        /* [in] */ IDisplayMetrics* outMetrics);

    /**
     * Gets the real size of the display without subtracting any window decor or
     * applying any compatibility scale factors.
     * <p>
     * The size is adjusted based on the current rotation of the display.
     * </p><p>
     * The real size may be smaller than the physical size of the screen when the
     * window manager is emulating a smaller display (using adb shell am display-size).
     * </p>
     *
     * @param outSize Set to the real size of the display.
     */
    CARAPI GetRealSize(
        /* [in] */ IPoint* outSize);

    /**
     * Gets the state of the display, such as whether it is on or off.
     *
     * @return The state of the display: one of {@link #STATE_OFF}, {@link #STATE_ON},
     * {@link #STATE_DOZE}, {@link #STATE_DOZE_SUSPEND}, or {@link #STATE_UNKNOWN}.
     */
    CARAPI GetState(
        /* [out] */ Int32* state);

    /**
     * Returns true if the specified UID has access to this display.
     * @hide
     */
    CARAPI HasAccess(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* result);

    /** @hide */
    static Boolean HasAccess(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 ownerUid);

    CARAPI IsPublicPresentation(
        /* [out] */ Boolean* result);

    /**
     * Gets display metrics based on the real size of this display.
     * <p>
     * The size is adjusted based on the current rotation of the display.
     * </p><p>
     * The real size may be smaller than the physical size of the screen when the
     * window manager is emulating a smaller display (using adb shell am display-size).
     * </p>
     *
     * @param outMetrics A {@link DisplayMetrics} object to receive the metrics.
     */
    CARAPI GetRealMetrics(
        /* [in] */ IDisplayMetrics* outMetrics);

private:
    CARAPI_(void) UpdateDisplayInfoLocked();

    CARAPI_(void) UpdateCachedAppSizeIfNeededLocked();

private:
    static const char* TAG;
    static const Boolean DEBUG = FALSE;

    // We cache the app width and height properties briefly between calls
    // to getHeight() and getWidth() to ensure that applications perceive
    // consistent results when the size changes (most of the time).
    // Applications should now be using getSize() instead.
    static const Int32 CACHED_APP_SIZE_DURATION_MILLIS = 20;

    AutoPtr<IDisplayManagerGlobal> mGlobal;
    Int32 mDisplayId;
    Int32 mLayerStack;
    Int32 mFlags;
    Int32 mType;
    String mAddress;
    // AutoPtr<ICompatibilityInfoHolder> mCompatibilityInfo;
    Int32 mOwnerUid;
    String mOwnerPackageName;
    AutoPtr<IDisplayAdjustments> mDisplayAdjustments;

    AutoPtr<IDisplayInfo> mDisplayInfo; // never NULL
    Boolean mIsValid;

    // Temporary display metrics structure used for compatibility mode.
    AutoPtr<IDisplayMetrics> mTempMetrics;
    Int64 mLastCachedAppSizeUpdate;
    Int32 mCachedAppWidthCompat;
    Int32 mCachedAppHeightCompat;

    Object mSelfLock;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_DISPLAY_H__
