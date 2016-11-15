
#include "elastos/droid/hardware/CCmHardwareManager.h"
#include "elastos/droid/os/ServiceManager.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::ServiceManager;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {

const String CCmHardwareManager::TAG("CmHardwareManager");

static AutoPtr<IList> InitBOOLEAN_FEATURES()
{
    Int32 _array[] = {
        ICmHardwareManager::FEATURE_ADAPTIVE_BACKLIGHT,
        ICmHardwareManager::FEATURE_COLOR_ENHANCEMENT,
        ICmHardwareManager::FEATURE_HIGH_TOUCH_SENSITIVITY,
        ICmHardwareManager::FEATURE_KEY_DISABLE,
        ICmHardwareManager::FEATURE_SUNLIGHT_ENHANCEMENT,
        ICmHardwareManager::FEATURE_TAP_TO_WAKE,
        ICmHardwareManager::FEATURE_TOUCH_HOVERING
    };
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(7);
    array->Copy(_array, 7);
    AutoPtr<IList> list;
    Arrays::AsList(array, (IList**)&list);
    return list;
}
const AutoPtr<IList> CCmHardwareManager::BOOLEAN_FEATURES = InitBOOLEAN_FEATURES();

CAR_INTERFACE_IMPL(CCmHardwareManager, Object, ICmHardwareManager)

CAR_OBJECT_IMPL(CCmHardwareManager)

ECode CCmHardwareManager::constructor(
    /* [in] */ IContext* context)
{
    mService = IICmHardwareService::Probe(ServiceManager::GetService(IContext::CMHW_SERVICE));
    return NOERROR;
}

ECode CCmHardwareManager::GetSupportedFeatures(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        *result = 0;
        return NOERROR;
    }

    if (FAILED(mService->GetSupportedFeatures(result))) {
        *result = 0;
    }
    return NOERROR;
}

ECode CCmHardwareManager::IsSupported(
    /* [in] */ Int32 feature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 features;
    GetSupportedFeatures(&features);
    *result = feature == (features & feature);
    return NOERROR;
}

ECode CCmHardwareManager::Get(
    /* [in] */ Int32 feature,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Boolean contains;
    BOOLEAN_FEATURES->Contains(CoreUtils::Convert(feature), &contains);
    if (!contains) {
        Logger::E(TAG, "%d is not a Boolean", feature);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->Get(feature, result);
    return NOERROR;
}

ECode CCmHardwareManager::Set(
    /* [in] */ Int32 feature,
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Boolean contains;
    BOOLEAN_FEATURES->Contains(CoreUtils::Convert(feature), &contains);
    if (!contains) {
        Logger::E(TAG, "%d is not a Boolean", feature);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->Set(feature, enable, result);
    return NOERROR;
}

ECode CCmHardwareManager::GetArrayValue(
    /* [in] */ ArrayOf<Int32>* arr,
    /* [in] */ Int32 idx,
    /* [in] */ Int32 defaultValue,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (arr == NULL || arr->GetLength() <= idx) {
        *result = defaultValue;
        return NOERROR;
    }

    *result = (*arr)[idx];
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > CCmHardwareManager::GetVibratorIntensityArray()
{
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NULL;
    }

    AutoPtr<ArrayOf<Int32> > array;
    mService->GetVibratorIntensity((ArrayOf<Int32>**)&array);
    return array;
}

ECode CCmHardwareManager::GetVibratorIntensity(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetVibratorIntensityArray(), VIBRATOR_INTENSITY_INDEX, 0, result);
}

ECode CCmHardwareManager::GetVibratorDefaultIntensity(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetVibratorIntensityArray(), VIBRATOR_DEFAULT_INDEX, 0, result);
}

ECode CCmHardwareManager::GetVibratorMinIntensity(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetVibratorIntensityArray(), VIBRATOR_MIN_INDEX, 0, result);
}

ECode CCmHardwareManager::GetVibratorMaxIntensity(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetVibratorIntensityArray(), VIBRATOR_MAX_INDEX, 0, result);
}

ECode CCmHardwareManager::GetVibratorWarningIntensity(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetVibratorIntensityArray(), VIBRATOR_WARNING_INDEX, 0, result);
}

ECode CCmHardwareManager::SetVibratorIntensity(
    /* [in] */ Int32 intensity,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->SetVibratorIntensity(intensity, result);
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > CCmHardwareManager::GetDisplayColorCalibrationArray()
{
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NULL;
    }

    AutoPtr<ArrayOf<Int32> > array;
    mService->GetDisplayColorCalibration((ArrayOf<Int32>**)&array);
    return array;
}

ECode CCmHardwareManager::GetDisplayColorCalibration(
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<ArrayOf<Int32> > arr = GetDisplayColorCalibrationArray();
    if (arr == NULL || arr->GetLength() < 3) {
        return NOERROR;
    }
    return Arrays::CopyOf(arr, 3, result);
}

ECode CCmHardwareManager::GetDisplayColorCalibrationDefault(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetDisplayColorCalibrationArray(), COLOR_CALIBRATION_DEFAULT_INDEX, 0, result);
}

ECode CCmHardwareManager::GetDisplayColorCalibrationMin(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetDisplayColorCalibrationArray(), COLOR_CALIBRATION_MIN_INDEX, 0, result);
}

ECode CCmHardwareManager::GetDisplayColorCalibrationMax(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetDisplayColorCalibrationArray(), COLOR_CALIBRATION_MAX_INDEX, 0, result);
}

ECode CCmHardwareManager::SetDisplayColorCalibration(
    /* [in] */ ArrayOf<Int32>* rgb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->SetDisplayColorCalibration(rgb, result);
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > CCmHardwareManager::GetDisplayGammaCalibrationArray(
    /* [in] */ Int32 idx)
{
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NULL;
    }

    AutoPtr<ArrayOf<Int32> > array;
    mService->GetDisplayGammaCalibration(idx, (ArrayOf<Int32>**)&array);
    return array;
}

ECode CCmHardwareManager::GetNumGammaControls(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->GetNumGammaControls(result);
    return NOERROR;
}

ECode CCmHardwareManager::GetDisplayGammaCalibration(
    /* [in] */ Int32 idx,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    AutoPtr<ArrayOf<Int32> > arr = GetDisplayGammaCalibrationArray(idx);
    if (arr == NULL || arr->GetLength() < 3) {
        return NOERROR;
    }
    return Arrays::CopyOf(arr, 3, result);
}

ECode CCmHardwareManager::GetDisplayGammaCalibrationMin(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetDisplayGammaCalibrationArray(0), GAMMA_CALIBRATION_MIN_INDEX, 0, result);
}

ECode CCmHardwareManager::GetDisplayGammaCalibrationMax(
    /* [out] */ Int32* result)
{
    return GetArrayValue(GetDisplayGammaCalibrationArray(0), GAMMA_CALIBRATION_MAX_INDEX, 0, result);
}

ECode CCmHardwareManager::SetDisplayGammaCalibration(
    /* [in] */ Int32 idx,
    /* [in] */ ArrayOf<Int32>* rgb,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->SetDisplayGammaCalibration(idx, rgb, result);
    return NOERROR;
}

ECode CCmHardwareManager::GetLtoSource(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->GetLtoSource(result);
    return NOERROR;
}

ECode CCmHardwareManager::GetLtoDestination(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->GetLtoDestination(result);
    return NOERROR;
}

ECode CCmHardwareManager::GetLtoDownloadInterval(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->GetLtoDownloadInterval(result);
    return NOERROR;
}

ECode CCmHardwareManager::GetSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->GetSerialNumber(result);
    return NOERROR;
}

ECode CCmHardwareManager::RequireAdaptiveBacklightForSunlightEnhancement(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (mService == NULL) {
        Logger::W(TAG, "no cmhw service.");
        return NOERROR;
    }

    mService->RequireAdaptiveBacklightForSunlightEnhancement(result);
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
