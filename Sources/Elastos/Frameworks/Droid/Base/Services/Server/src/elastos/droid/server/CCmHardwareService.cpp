
#include "elastos/droid/server/CCmHardwareService.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::ICmHardwareManager;
using Elastos::Droid::Hardware::EIID_IICmHardwareService;
using Elastos::Droid::Manifest;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {

Boolean CCmHardwareService::DEBUG = TRUE;
String CCmHardwareService::TAG("CCmHardwareService");

//=======================================================================
// CCmHardwareService::LegacyCmHardware::
//=======================================================================
CAR_INTERFACE_IMPL(CCmHardwareService::LegacyCmHardware, Object, ICmHardwareInterface)

CCmHardwareService::LegacyCmHardware::LegacyCmHardware()
    : mSupportedFeatures(0)
{
    assert(0 && "TODO");
    // if (AdaptiveBacklight.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_ADAPTIVE_BACKLIGHT;
    // if (ColorEnhancement.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_COLOR_ENHANCEMENT;
    // if (DisplayColorCalibration.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_DISPLAY_COLOR_CALIBRATION;
    // if (DisplayGammaCalibration.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_DISPLAY_GAMMA_CALIBRATION;
    // if (HighTouchSensitivity.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_HIGH_TOUCH_SENSITIVITY;
    // if (KeyDisabler.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_KEY_DISABLE;
    // if (LongTermOrbits.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_LONG_TERM_ORBITS;
    // if (SerialNumber.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_SERIAL_NUMBER;
    // if (SunlightEnhancement.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_SUNLIGHT_ENHANCEMENT;
    // if (TapToWake.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_TAP_TO_WAKE;
    // if (VibratorHW.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_VIBRATOR;
    // if (TouchscreenHovering.isSupported())
    //     mSupportedFeatures |= CmHardwareManager.FEATURE_TOUCH_HOVERING;
}

ECode CCmHardwareService::LegacyCmHardware::GetSupportedFeatures(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSupportedFeatures;
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::Get(
    /* [in] */ Int32 feature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // switch(feature) {
    //     case CmHardwareManager.FEATURE_ADAPTIVE_BACKLIGHT:
    //         return AdaptiveBacklight.isEnabled();
    //     case CmHardwareManager.FEATURE_COLOR_ENHANCEMENT:
    //         return ColorEnhancement.isEnabled();
    //     case CmHardwareManager.FEATURE_HIGH_TOUCH_SENSITIVITY:
    //         return HighTouchSensitivity.isEnabled();
    //     case CmHardwareManager.FEATURE_KEY_DISABLE:
    //         return KeyDisabler.isActive();
    //     case CmHardwareManager.FEATURE_SUNLIGHT_ENHANCEMENT:
    //         return SunlightEnhancement.isEnabled();
    //     case CmHardwareManager.FEATURE_TAP_TO_WAKE:
    //         return TapToWake.isEnabled();
    //     case CmHardwareManager.FEATURE_TOUCH_HOVERING:
    //         return TouchscreenHovering.isEnabled();
    //     default:
    //         Log.e(TAG, "feature " + feature + " is not a boolean feature");
    //         return false;
    // }
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::Set(
    /* [in] */ Int32 feature,
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // switch(feature) {
    //     case CmHardwareManager.FEATURE_ADAPTIVE_BACKLIGHT:
    //         return AdaptiveBacklight.setEnabled(enable);
    //     case CmHardwareManager.FEATURE_COLOR_ENHANCEMENT:
    //         return ColorEnhancement.setEnabled(enable);
    //     case CmHardwareManager.FEATURE_HIGH_TOUCH_SENSITIVITY:
    //         return HighTouchSensitivity.setEnabled(enable);
    //     case CmHardwareManager.FEATURE_KEY_DISABLE:
    //         return KeyDisabler.setActive(enable);
    //     case CmHardwareManager.FEATURE_SUNLIGHT_ENHANCEMENT:
    //         return SunlightEnhancement.setEnabled(enable);
    //     case CmHardwareManager.FEATURE_TAP_TO_WAKE:
    //         return TapToWake.setEnabled(enable);
    //     case CmHardwareManager.FEATURE_TOUCH_HOVERING:
    //         return TouchscreenHovering.setEnabled(enable);
    //     default:
    //         Log.e(TAG, "feature " + feature + " is not a boolean feature");
    //         return false;
    // }
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::SplitStringToInt32(
    /* [in] */ String input,
    /* [in] */ String delimiter,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    if (input.IsNull() || delimiter.IsNull()) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<String> > strArray;
    StringUtils::Split(input, delimiter, (ArrayOf<String>**)&strArray);
    // try {
    AutoPtr<ArrayOf<Int32> > intArray = ArrayOf<Int32>::Alloc(strArray->GetLength());
    for(Int32 i = 0; i < strArray->GetLength(); i++) {
        (*intArray)[i] = StringUtils::ParseInt32((*strArray)[i]);
    }
    *result = intArray;
    return NOERROR;
    // } catch (NumberFormatException e) {
    //     /* ignore */
    // }
    // return null;
}

ECode CCmHardwareService::LegacyCmHardware::RgbToString(
    /* [in] */ ArrayOf<Int32>* rgb,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder builder;
    builder.Append((*rgb)[ICmHardwareManager::COLOR_CALIBRATION_RED_INDEX]);
    builder.Append(" ");
    builder.Append((*rgb)[ICmHardwareManager::COLOR_CALIBRATION_GREEN_INDEX]);
    builder.Append(" ");
    builder.Append((*rgb)[ICmHardwareManager::COLOR_CALIBRATION_BLUE_INDEX]);
    return builder.ToString(result);
}

ECode CCmHardwareService::LegacyCmHardware::GetDisplayColorCalibration(
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    // AutoPtr<ArrayOf<Int32> > rgb = SplitStringToInt(DisplayColorCalibration.getCurColors(), " ");
    // if (rgb == NULL || rgb->GetLength() != 3) {
    //     Logger::E(TAG, "Invalid color calibration string");
    //     *result = NULL;
    //     return NOERROR;
    // }
    // AutoPtr<ArrayOf<Int32> > currentCalibration = ArrayOf<Int32>::Alloc(6);
    // (*currentCalibration)[ICmHardwareManager::COLOR_CALIBRATION_RED_INDEX] = (*rgb)[0];
    // (*currentCalibration)[ICmHardwareManager::COLOR_CALIBRATION_GREEN_INDEX] = (*rgb)[1];
    // (*currentCalibration)[ICmHardwareManager::COLOR_CALIBRATION_BLUE_INDEX] = (*rgb)[2];
    // (*currentCalibration)[ICmHardwareManager::COLOR_CALIBRATION_DEFAULT_INDEX] =
    //     DisplayColorCalibration.getDefValue();
    // (*currentCalibration)[ICmHardwareManager::COLOR_CALIBRATION_MIN_INDEX] =
    //     DisplayColorCalibration.getMinValue();
    // (*currentCalibration)[ICmHardwareManager::COLOR_CALIBRATION_MAX_INDEX] =
    //     DisplayColorCalibration.getMaxValue();
    // return currentCalibration;
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::SetDisplayColorCalibration(
    /* [in] */ ArrayOf<Int32>* rgb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // return DisplayColorCalibration.setColors(rgbToString(rgb));
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::GetNumGammaControls(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // return DisplayGammaCalibration.getNumberOfControls();
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::GetDisplayGammaCalibration(
    /* [in] */ Int32 idx,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    // int[] rgb = splitStringToInt(DisplayGammaCalibration.getCurGamma(idx), " ");
    // if (rgb == null || rgb.length != 3) {
    //     Log.e(TAG, "Invalid gamma calibration string");
    //     return null;
    // }
    // int[] currentCalibration = new int[5];
    // currentCalibration[CmHardwareManager.GAMMA_CALIBRATION_RED_INDEX] = rgb[0];
    // currentCalibration[CmHardwareManager.GAMMA_CALIBRATION_GREEN_INDEX] = rgb[1];
    // currentCalibration[CmHardwareManager.GAMMA_CALIBRATION_BLUE_INDEX] = rgb[2];
    // currentCalibration[CmHardwareManager.GAMMA_CALIBRATION_MIN_INDEX] =
    //     DisplayGammaCalibration.getMinValue(idx);
    // currentCalibration[CmHardwareManager.GAMMA_CALIBRATION_MAX_INDEX] =
    //     DisplayGammaCalibration.getMaxValue(idx);
    // return currentCalibration;
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::SetDisplayGammaCalibration(
    /* [in] */ Int32 idx,
    /* [in] */ ArrayOf<Int32>* rgb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // return DisplayGammaCalibration.setGamma(idx, rgbToString(rgb));
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::GetVibratorIntensity(
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    // int[] vibrator = new int[5];
    // vibrator[CmHardwareManager.VIBRATOR_INTENSITY_INDEX] = VibratorHW.getCurIntensity();
    // vibrator[CmHardwareManager.VIBRATOR_DEFAULT_INDEX] = VibratorHW.getDefaultIntensity();
    // vibrator[CmHardwareManager.VIBRATOR_MIN_INDEX] = VibratorHW.getMinIntensity();
    // vibrator[CmHardwareManager.VIBRATOR_MAX_INDEX] = VibratorHW.getMaxIntensity();
    // vibrator[CmHardwareManager.VIBRATOR_WARNING_INDEX] = VibratorHW.getWarningThreshold();
    // return vibrator;
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::SetVibratorIntensity(
    /* [in] */ Int32 intensity,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // return VibratorHW.setIntensity(intensity);
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::GetLtoSource(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // return LongTermOrbits.getSourceLocation();
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::GetLtoDestination(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // File file = LongTermOrbits.getDestinationLocation();
    // return file.getAbsolutePath();
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::GetLtoDownloadInterval(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    // return LongTermOrbits.getDownloadInterval();
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::GetSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // return SerialNumber.getSerialNumber();
    return NOERROR;
}

ECode CCmHardwareService::LegacyCmHardware::RequireAdaptiveBacklightForSunlightEnhancement(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // return SunlightEnhancement.isAdaptiveBacklightRequired();
    return NOERROR;
}

//=======================================================================
// CCmHardwareService::
//=======================================================================
CAR_OBJECT_IMPL(CCmHardwareService)

CAR_INTERFACE_IMPL(CCmHardwareService, Object, IICmHardwareService)

AutoPtr<ICmHardwareInterface> CCmHardwareService::GetImpl(
    /* [in] */ IContext* context)
{
    AutoPtr<LegacyCmHardware> p = new LegacyCmHardware();
    return ICmHardwareInterface::Probe(p);
}

ECode CCmHardwareService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mCmHwImpl = GetImpl(context);
    return NOERROR;
}

Boolean CCmHardwareService::IsSupported(
    /* [in] */ Int32 feature)
{
    Int32 ft = 0;
    GetSupportedFeatures(&ft);
    return (ft & feature) == feature;
}

ECode CCmHardwareService::GetSupportedFeatures(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    return mCmHwImpl->GetSupportedFeatures(result);
}

ECode CCmHardwareService::Get(
    /* [in] */ Int32 feature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(feature)) {
        Logger::E(TAG, "feature %d is not supported", feature);
        *result = FALSE;
        return NOERROR;
    }
    return mCmHwImpl->Get(feature, result);
}

ECode CCmHardwareService::Set(
    /* [in] */ Int32 feature,
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(feature)) {
        Logger::E(TAG, "feature %d is not supported", feature);
        *result = FALSE;
        return NOERROR;
    }
    return mCmHwImpl->Set(feature, enable, result);
}

ECode CCmHardwareService::GetDisplayColorCalibration(
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_DISPLAY_COLOR_CALIBRATION)) {
        Logger::E(TAG, "Display color calibration is not supported");
        *result = NULL;
        return NOERROR;
    }
    return mCmHwImpl->GetDisplayColorCalibration(result);
}

ECode CCmHardwareService::SetDisplayColorCalibration(
    /* [in] */ ArrayOf<Int32>* rgb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_DISPLAY_COLOR_CALIBRATION)) {
        Logger::E(TAG, "Display color calibration is not supported");
        *result = FALSE;
        return NOERROR;
    }
    if (rgb->GetLength() < 3) {
        Logger::E(TAG, "Invalid color calibration");
        *result = FALSE;
        return NOERROR;
    }
    return mCmHwImpl->SetDisplayColorCalibration(rgb, result);
}

ECode CCmHardwareService::GetNumGammaControls(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_DISPLAY_GAMMA_CALIBRATION)) {
        Logger::E(TAG, "Display gamma calibration is not supported");
        *result = 0;
        return NOERROR;
    }
    return mCmHwImpl->GetNumGammaControls(result);
}

ECode CCmHardwareService::GetDisplayGammaCalibration(
    /* [in] */ Int32 idx,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_DISPLAY_GAMMA_CALIBRATION)) {
        Logger::E(TAG, "Display gamma calibration is not supported");
        *result = NULL;
        return NOERROR;
    }
    return mCmHwImpl->GetDisplayGammaCalibration(idx, result);
}

ECode CCmHardwareService::SetDisplayGammaCalibration(
    /* [in] */ Int32 idx,
    /* [in] */ ArrayOf<Int32>* rgb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_DISPLAY_GAMMA_CALIBRATION)) {
        Logger::E(TAG, "Display gamma calibration is not supported");
        *result = FALSE;
        return NOERROR;
    }
    return mCmHwImpl->SetDisplayGammaCalibration(idx, rgb, result);
}

ECode CCmHardwareService::GetVibratorIntensity(
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_VIBRATOR)) {
        Logger::E(TAG, "Vibrator is not supported");
        *result = NULL;
        return NOERROR;
    }
    return mCmHwImpl->GetVibratorIntensity(result);
}

ECode CCmHardwareService::SetVibratorIntensity(
    /* [in] */ Int32 intensity,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_VIBRATOR)) {
        Logger::E(TAG, "Vibrator is not supported");
        *result = FALSE;
        return NOERROR;
    }
    return mCmHwImpl->SetVibratorIntensity(intensity, result);
}

ECode CCmHardwareService::GetLtoSource(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_LONG_TERM_ORBITS)) {
        Logger::E(TAG, "Long term orbits is not supported");
        *result = NULL;
        return NOERROR;
    }
    return mCmHwImpl->GetLtoSource(result);
}

ECode CCmHardwareService::GetLtoDestination(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_LONG_TERM_ORBITS)) {
        Logger::E(TAG, "Long term orbits is not supported");
        *result = NULL;
        return NOERROR;
    }
    return mCmHwImpl->GetLtoDestination(result);
}

ECode CCmHardwareService::GetLtoDownloadInterval(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_LONG_TERM_ORBITS)) {
        Logger::E(TAG, "Long term orbits is not supported");
        *result = 0;
        return NOERROR;
    }
    return mCmHwImpl->GetLtoDownloadInterval(result);
}

ECode CCmHardwareService::GetSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_SERIAL_NUMBER)) {
        Logger::E(TAG, "Serial number is not supported");
        *result = NULL;
        return NOERROR;
    }
    return mCmHwImpl->GetSerialNumber(result);
}

ECode CCmHardwareService::RequireAdaptiveBacklightForSunlightEnhancement(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::HARDWARE_ABSTRACTION_ACCESS, String(NULL));
    if (!IsSupported(ICmHardwareManager::FEATURE_SUNLIGHT_ENHANCEMENT)) {
        Logger::E(TAG, "Sunlight enhancement is not supported");
        *result = FALSE;
        return NOERROR;
    }
    return mCmHwImpl->RequireAdaptiveBacklightForSunlightEnhancement(result);
}

} //namespace Server
} //namespace Droid
} //namespace Elastos
