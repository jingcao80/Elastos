
#ifndef __ELASTOS_DROID_HARDWARE_CCMHARDWAREMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_CCMHARDWAREMANAGER_H__

#include "_Elastos_Droid_Hardware_CCmHardwareManager.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {

CarClass(CCmHardwareManager)
    , public Object
    , public ICmHardwareManager
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * @hide to prevent subclassing from outside of the framework
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * @return the supported features bitmask
     */
    CARAPI GetSupportedFeatures(
        /* [out] */ Int32* result);

    /**
     * Determine if a CM Hardware feature is supported on this device
     *
     * @param feature The CM Hardware feature to query
     *
     * @return true if the feature is supported, FALSE otherwise.
     */
    CARAPI IsSupported(
        /* [in] */ Int32 feature,
        /* [out] */ Boolean* result);

    /**
     * Determine if the given feature is enabled or disabled.
     *
     * Only used for features which have simple enable/disable controls.
     *
     * @param feature the CM Hardware feature to query
     *
     * @return true if the feature is enabled, FALSE otherwise.
     */
    CARAPI Get(
        /* [in] */ Int32 feature,
        /* [out] */ Boolean* result);

    /**
     * Enable or disable the given feature
     *
     * Only used for features which have simple enable/disable controls.
     *
     * @param feature the CM Hardware feature to set
     * @param enable true to enable, FALSE to disale
     *
     * @return true if the feature is enabled, FALSE otherwise.
     */
    CARAPI Set(
        /* [in] */ Int32 feature,
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    /**
     * @return The current vibrator intensity.
     */
    CARAPI GetVibratorIntensity(
        /* [out] */ Int32* result);

    /**
     * @return The default vibrator intensity.
     */
    CARAPI GetVibratorDefaultIntensity(
        /* [out] */ Int32* result);

    /**
     * @return The minimum vibrator intensity.
     */
    CARAPI GetVibratorMinIntensity(
        /* [out] */ Int32* result);

    /**
     * @return The maximum vibrator intensity.
     */
    CARAPI GetVibratorMaxIntensity(
        /* [out] */ Int32* result);

    /**
     * @return The warning threshold vibrator intensity.
     */
    CARAPI GetVibratorWarningIntensity(
        /* [out] */ Int32* result);

    /**
     * Set the current vibrator intensity
     *
     * @param intensity the intensity to set, between {@link #getVibratorMinIntensity()} and
     * {@link #getVibratorMaxIntensity()} inclusive.
     *
     * @return true on success, FALSE otherwise.
     */
    CARAPI SetVibratorIntensity(
        /* [in] */ Int32 intensity,
        /* [out] */ Boolean* result);

    /**
     * @return the current RGB calibration, where Int32[0] = R, Int32[1] = G, Int32[2] = B.
     */
    CARAPI GetDisplayColorCalibration(
        /* [out] */ ArrayOf<Int32>** result);

    /**
     * @return the default value for all colors
     */
    CARAPI GetDisplayColorCalibrationDefault(
        /* [out] */ Int32* result);

    /**
     * @return The minimum value for all colors
     */
    CARAPI GetDisplayColorCalibrationMin(
        /* [out] */ Int32* result);

    /**
     * @return The minimum value for all colors
     */
    CARAPI GetDisplayColorCalibrationMax(
        /* [out] */ Int32* result);

    /**
     * Set the display color calibration to the given rgb triplet
     *
     * @param rgb RGB color calibration.  Each value must be between
     * {@link getDisplayColorCalibrationMin()} and {@link getDisplayColorCalibrationMax()},
     * inclusive.
     *
     * @return true on success, FALSE otherwise.
     */
    CARAPI SetDisplayColorCalibration(
        /* [in] */ ArrayOf<Int32>* rgb,
        /* [out] */ Boolean* result);

    /**
     * @return the number of RGB controls the device supports
     */
    CARAPI GetNumGammaControls(
        /* [out] */ Int32* result);

    /**
     * @param the control to query
     *
     * @return the current RGB gamma calibration for the given control
     */
    CARAPI GetDisplayGammaCalibration(
        /* [in] */ Int32 idx,
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * @return the minimum value for all colors
     */
    CARAPI GetDisplayGammaCalibrationMin(
        /* [out] */ Int32* result);

    /**
     * @return the maximum value for all colors
     */
    CARAPI GetDisplayGammaCalibrationMax(
        /* [out] */ Int32* result);

    /**
     * Set the display gamma calibration for a specific control
     *
     * @param idx the control to set
     * @param rgb RGB color calibration.  Each value must be between
     * {@link getDisplayGammaCalibrationMin()} and {@link getDisplayGammaCalibrationMax()},
     * inclusive.
     *
     * @return true on success, FALSE otherwise.
     */
    CARAPI SetDisplayGammaCalibration(
        /* [in] */ Int32 idx,
        /* [in] */ ArrayOf<Int32>* rgb,
        /* [out] */ Boolean* result);

    /**
     * @return the source location of LTO data, or NULL on failure
     */
    CARAPI GetLtoSource(
        /* [out] */ String* result);

    /**
     * @return the destination location of LTO data, or NULL on failure
     */
    CARAPI GetLtoDestination(
        /* [out] */ String* result);

    /**
     * @return the interval, in milliseconds, to trigger LTO data download
     */
    CARAPI GetLtoDownloadInterval(
        /* [out] */ Int64* result);

    /**
     * @return the serial number to display instead of ro.serialno, or NULL on failure
     */
    CARAPI GetSerialNumber(
        /* [out] */ String* result);

    /**
     * @return true if adaptive backlight should be enabled when sunlight enhancement
     * is enabled.
     */
    CARAPI RequireAdaptiveBacklightForSunlightEnhancement(
        /* [out] */ Boolean* result);

private:
    CARAPI GetArrayValue(
        /* [in] */ ArrayOf<Int32>* arr,
        /* [in] */ Int32 idx,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* result);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetVibratorIntensityArray();

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetDisplayColorCalibrationArray();

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetDisplayGammaCalibrationArray(
        /* [in] */ Int32 idx);

private:
    static const String TAG;

    static const AutoPtr<IList> BOOLEAN_FEATURES ;

    AutoPtr<IICmHardwareService> mService;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CCMHARDWAREMANAGER_H__
