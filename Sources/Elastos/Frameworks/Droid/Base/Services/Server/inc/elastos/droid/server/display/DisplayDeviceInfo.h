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

#ifndef __ELASTOS_DROID_SERVER_DISPLAY_DISPLAYDEVICEINFO_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_DISPLAYDEVICEINFO_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * Describes the characteristics of a physical display device.
 */
class DisplayDeviceInfo
    : public Object
{
public:
    DisplayDeviceInfo();

    CARAPI_(void) SetAssumedDensityForExternalDisplay(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI_(Boolean) Equals(
        /* [in] */ DisplayDeviceInfo* other);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI_(void) CopyFrom(
        /* [in] */ DisplayDeviceInfo* other);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    static CARAPI_(String) TouchToString(
        /* [in] */ Int32 touch);

    static CARAPI_(String) FlagsToString(
        /* [in] */ Int32 flags);

public:
    /**
     * Flag: Indicates that this display device should be considered the default display
     * device of the system.
     */
    static const Int32 FLAG_DEFAULT_DISPLAY = 1 << 0;

    /**
     * Flag: Indicates that the orientation of this display device is coupled to the
     * rotation of its associated logical display.
     * <p>
     * This flag should be applied to the default display to indicate that the user
     * physically rotates the display when content is presented in a different orientation.
     * The display manager will apply a coordinate transformation assuming that the
     * physical orientation of the display matches the logical orientation of its content.
     * </p><p>
     * The flag should not be set when the display device is mounted in a fixed orientation
     * such as on a desk.  The display manager will apply a coordinate transformation
     * such as a scale and translation to letterbox or pillarbox format under the
     * assumption that the physical orientation of the display is invariant.
     * </p>
     */
    static const Int32 FLAG_ROTATES_WITH_CONTENT = 1 << 1;

    /**
     * Flag: Indicates that this display device has secure video output, such as HDCP.
     */
    static const Int32 FLAG_SECURE = 1 << 2;

    /**
     * Flag: Indicates that this display device supports compositing
     * from gralloc protected buffers.
     */
    static const Int32 FLAG_SUPPORTS_PROTECTED_BUFFERS = 1 << 3;

    /**
     * Flag: Indicates that the display device is owned by a particular application
     * and that no other application should be able to interact with it.
     * Should typically be used together with {@link #FLAG_OWN_CONTENT_ONLY}.
     */
    static const Int32 FLAG_PRIVATE = 1 << 4;

    /**
     * Flag: Indicates that the display device is not blanked automatically by
     * the power manager.
     */
    static const Int32 FLAG_NEVER_BLANK = 1 << 5;

    /**
     * Flag: Indicates that the display is suitable for presentations.
     */
    static const Int32 FLAG_PRESENTATION = 1 << 6;

    /**
     * Flag: Only show this display's own content; do not mirror
     * the content of another display.
     */
    static const Int32 FLAG_OWN_CONTENT_ONLY = 1 << 7;

    /**
     * Touch attachment: Display does not receive touch.
     */
    static const Int32 TOUCH_NONE = 0;

    /**
     * Touch attachment: Touch input is via the internal interface.
     */
    static const Int32 TOUCH_INTERNAL = 1;

    /**
     * Touch attachment: Touch input is via an external interface, such as USB.
     */
    static const Int32 TOUCH_EXTERNAL = 2;

     /**
     * Gets the name of the display device, which may be derived from
     * EDID or other sources.  The name may be displayed to the user.
     */
    String mName;

    /**
     * The width of the display in its natural orientation, in pixels.
     * This value is not affected by display rotation.
     */
    Int32 mWidth;

    /**
     * The height of the display in its natural orientation, in pixels.
     * This value is not affected by display rotation.
     */
    Int32 mHeight;

    /**
     * The refresh rate of the display, in frames per second.
     */
    Float mRefreshRate;

    /**
     * The supported refresh rates of the display at the current resolution in frames per second.
     */
    AutoPtr<ArrayOf<Float> > mSupportedRefreshRates;


    /**
     * The nominal apparent density of the display in DPI used for layout calculations.
     * This density is sensitive to the viewing distance.  A big TV and a tablet may have
     * the same apparent density even though the pixels on the TV are much bigger than
     * those on the tablet.
     */
    Int32 mDensityDpi;

    /**
     * The physical density of the display in DPI in the X direction.
     * This density should specify the physical size of each pixel.
     */
    Float mXDpi;

    /**
     * The physical density of the display in DPI in the X direction.
     * This density should specify the physical size of each pixel.
     */
    Float mYDpi;

    /**
     * This is a positive value indicating the phase offset of the VSYNC events provided by
     * Choreographer relative to the display refresh.  For example, if Choreographer reports
     * that the refresh occurred at time N, it actually occurred at (N - appVsyncOffsetNanos).
     */
    Int64 mAppVsyncOffsetNanos;

    /**
     * This is how far in advance a buffer must be queued for presentation at
     * a given time.  If you want a buffer to appear on the screen at
     * time N, you must submit the buffer before (N - bufferDeadlineNanos).
     */
    Int64 mPresentationDeadlineNanos;

    /**
     * Display flags.
     */
    Int32 mFlags;

    /**
     * The touch attachment, per {@link DisplayViewport#touch}.
     */
    Int32 mTouch;

    /**
     * The additional rotation to apply to all content presented on the display device
     * relative to its physical coordinate system.  Default is {@link Surface#ROTATION_0}.
     * <p>
     * This field can be used to compensate for the fact that the display has been
     * physically rotated relative to its natural orientation such as an HDMI monitor
     * that has been mounted sideways to appear to be portrait rather than landscape.
     * </p>
     */
    Int32 mRotation;

    /**
     * Display type.
     */
    Int32 mType;

    /**
     * Display address, or null if none.
     * Interpretation varies by display type.
     */
    String mAddress;

    /**
     * Display state.
     */
    Int32 mState;

    /**
     * The UID of the application that owns this display, or zero if it is owned by the system.
     * <p>
     * If the display is private, then only the owner can use it.
     * </p>
     */
    Int32 mOwnerUid;

    /**
     * The package name of the application that owns this display, or null if it is
     * owned by the system.
     * <p>
     * If the display is private, then only the owner can use it.
     * </p>
     */
    String mOwnerPackageName;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_DISPLAYDEVICEINFO_H__
