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

#ifndef __ELASTOS_DROID_UTILITY_CDISPLAYMETRICS_H__
#define __ELASTOS_DROID_UTILITY_CDISPLAYMETRICS_H__

#include "_Elastos_Droid_Utility_CDisplayMetrics.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CDisplayMetrics)
    , public Object
    , public IDisplayMetrics
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDisplayMetrics();

    CARAPI SetTo(
        /* [in] */ IDisplayMetrics* o);

    CARAPI SetToDefaults();

    CARAPI GetWidthPixels(
        /* [out] */ Int32* value);

    CARAPI SetWidthPixels(
        /* [in] */ Int32 value);

    CARAPI GetHeightPixels(
        /* [out] */ Int32* value);

    CARAPI SetHeightPixels(
        /* [in] */ Int32 value);

    CARAPI GetDensity(
        /* [out] */ Float* density);

    CARAPI SetDensity(
        /* [in] */ Float density);

    CARAPI GetDensityDpi(
        /* [out] */ Int32* value);

    CARAPI SetDensityDpi(
        /* [in] */ Int32 value);

    CARAPI GetScaledDensity(
        /* [out] */ Float* value);

    CARAPI SetScaledDensity(
        /* [in] */ Float value);

    CARAPI GetXdpi(
        /* [out] */ Float* value);

    CARAPI SetXdpi(
        /* [in] */ Float value);

    CARAPI GetYdpi(
        /* [out] */ Float* value);

    CARAPI SetYdpi(
        /* [in] */ Float value);

    CARAPI GetNoncompatWidthPixels(
        /* [out] */ Int32* value);

    CARAPI SetNoncompatWidthPixels(
        /* [in] */ Int32 value);

    CARAPI GetNoncompatHeightPixels(
        /* [out] */ Int32* value);

    CARAPI SetNoncompatHeightPixels(
        /* [in] */ Int32 value);

    CARAPI GetNoncompatDensity(
        /* [out] */ Float* value);

    CARAPI SetNoncompatDensity(
        /* [in] */ Float value);

    CARAPI GetNoncompatDensityDpi(
        /* [out] */ Int32* value);

    CARAPI SetNoncompatDensityDpi(
        /* [in] */ Int32 value);

    CARAPI GetNoncompatScaledDensity(
        /* [out] */ Float* value);

    CARAPI SetNoncompatScaledDensity(
        /* [in] */ Float value);

    CARAPI GetNoncompatXdpi(
        /* [out] */ Float* value);

    CARAPI SetNoncompatXdpi(
        /* [in] */ Float value);

    CARAPI GetNoncompatYdpi(
        /* [out] */ Float* value);

    CARAPI SetNoncompatYdpi(
        /* [in] */ Float value);

    /**
     * Returns true if these display metrics equal the other display metrics.
     *
     * @param other The display metrics with which to compare.
     * @return True if the display metrics are equal.
     */
    CARAPI Equals(
        /* [in] */ IDisplayMetrics* other,
        /* [out] */ Boolean* result);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean * result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    /**
     * Returns true if the physical aspects of the two display metrics
     * are equal.  This ignores the scaled density, which is a logical
     * attribute based on the current desired font size.
     *
     * @param other The display metrics with which to compare.
     * @return True if the display metrics are equal.
     * @hide
     */
    CARAPI EqualsPhysical(
        /* [in] */ IDisplayMetrics* other,
        /* [out] */ Boolean* resutl);

private:
    static CARAPI_(Int32) GetDeviceDensity();

public:
    /**
     * The device's density.
     * @hide because eventually this should be able to change while
     * running, so shouldn't be a constant.
     * @deprecated There is no longer a static density; you can find the
     * density for a display in {@link #densityDpi}.
     */
    //@Deprecated
    static Int32 DENSITY_DEVICE;

    /**
    * The absolute width of the display in pixels.
    */
    Int32 mWidthPixels;

    /**
    * The absolute height of the display in pixels.
    */
    Int32 mHeightPixels;

    /**
    * The logical density of the display.  This is a scaling factor for the
    * Density Independent Pixel unit, where one DIP is one pixel on an
    * approximately 160 dpi screen (for example a 240x320, 1.5"x2" screen),
    * providing the baseline of the system's display. Thus on a 160dpi screen
    * this density value will be 1; on a 120 dpi screen it would be .75; etc.
    *
    * <p>This value does not exactly follow the real screen size (as given by
    * {@link #xdpi} and {@link #ydpi}, but rather is used to scale the size of
    * the overall UI in steps based on gross changes in the display dpi.  For
    * example, a 240x320 screen will have a density of 1 even if its width is
    * 1.8", 1.3", etc. However, if the screen resolution is increased to
    * 320x480 but the screen size remained 1.5"x2" then the density would be
    * increased (probably to 1.5).
    *
    * @see #DENSITY_DEFAULT
    */
    Float mDensity;

    /**
    * The screen density expressed as dots-per-inch.  May be either
    * {@link #DENSITY_LOW}, {@link #DENSITY_MEDIUM}, or {@link #DENSITY_HIGH}.
    */
    Int32 mDensityDpi;

    /**
    * A scaling factor for fonts displayed on the display.  This is the same
    * as {@link #density}, except that it may be adjusted in smaller
    * increments at runtime based on a user preference for the font size.
    */
    Float mScaledDensity;

    /**
    * The exact physical pixels per inch of the screen in the X dimension.
    */
    Float mXdpi;

    /**
    * The exact physical pixels per inch of the screen in the Y dimension.
    */
    Float mYdpi;

    /**
     * The reported display width prior to any compatibility mode scaling
     * being applied.
     * @hide
     */
    Int32 mNoncompatWidthPixels;
    /**
     * The reported display height prior to any compatibility mode scaling
     * being applied.
     * @hide
     */
    Int32 mNoncompatHeightPixels;
    /**
     * The reported display density prior to any compatibility mode scaling
     * being applied.
     * @hide
     */
    Float mNoncompatDensity;
    /**
     * The reported display density prior to any compatibility mode scaling
     * being applied.
     * @hide
     */
    Int32 mNoncompatDensityDpi;
    /**
     * The reported scaled density prior to any compatibility mode scaling
     * being applied.
     * @hide
     */
    Float mNoncompatScaledDensity;
    /**
     * The reported display xdpi prior to any compatibility mode scaling
     * being applied.
     * @hide
     */
    Float mNoncompatXdpi;
    /**
     * The reported display ydpi prior to any compatibility mode scaling
     * being applied.
     * @hide
     */
    Float mNoncompatYdpi;
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_UTILITY_CDISPLAYMETRICS_H__
