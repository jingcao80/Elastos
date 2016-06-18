
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/hardware/display/CDisplayPowerRequest.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

static const String TAG("CDisplayPowerRequest");

CAR_INTERFACE_IMPL(CDisplayPowerRequest, Object, IDisplayPowerRequest)

CAR_OBJECT_IMPL(CDisplayPowerRequest)

ECode CDisplayPowerRequest::constructor()
{
    mPolicy = IDisplayPowerRequest::POLICY_BRIGHT;
    mUseProximitySensor = FALSE;
    mScreenBrightness = IPowerManager::BRIGHTNESS_ON;
    mScreenAutoBrightnessAdjustment = 0.0F;
    mUseAutoBrightness = FALSE;
    mBlockScreenOn = FALSE;
    mDozeScreenBrightness = IPowerManager::BRIGHTNESS_DEFAULT;
    mDozeScreenState = IDisplay::STATE_UNKNOWN;
    return NOERROR;
}

ECode CDisplayPowerRequest::constructor(
    /* [in] */ IDisplayPowerRequest* other)
{
    return CopyFrom(other);
}

ECode CDisplayPowerRequest::IsBrightOrDim(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (mPolicy == IDisplayPowerRequest::POLICY_BRIGHT || mPolicy == IDisplayPowerRequest::POLICY_DIM);
    return NOERROR;
}

ECode CDisplayPowerRequest::CopyFrom(
    /* [in] */ IDisplayPowerRequest* other)
{
    mPolicy = ((CDisplayPowerRequest*)other)->mPolicy;
    mUseProximitySensor = ((CDisplayPowerRequest*)other)->mUseProximitySensor;
    mScreenBrightness = ((CDisplayPowerRequest*)other)->mScreenBrightness;
    mScreenAutoBrightnessAdjustment = ((CDisplayPowerRequest*)other)->mScreenAutoBrightnessAdjustment;
    mUseAutoBrightness = ((CDisplayPowerRequest*)other)->mUseAutoBrightness;
    mBlockScreenOn = ((CDisplayPowerRequest*)other)->mBlockScreenOn;
    mLowPowerMode = ((CDisplayPowerRequest*)other)->mLowPowerMode;
    mDozeScreenBrightness = ((CDisplayPowerRequest*)other)->mDozeScreenBrightness;
    mDozeScreenState = ((CDisplayPowerRequest*)other)->mDozeScreenState;
    return NOERROR;
}

ECode CDisplayPowerRequest::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if(IDisplayPowerRequest::Probe(o) != NULL) {
        return Equals(IDisplayPowerRequest::Probe(o), result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode CDisplayPowerRequest::Equals(
    /* [in] */ IDisplayPowerRequest* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = (other != NULL
            && mPolicy == ((CDisplayPowerRequest*)other)->mPolicy
            && mUseProximitySensor == ((CDisplayPowerRequest*)other)->mUseProximitySensor
            && mScreenBrightness == ((CDisplayPowerRequest*)other)->mScreenBrightness
            && mScreenAutoBrightnessAdjustment == ((CDisplayPowerRequest*)other)->mScreenAutoBrightnessAdjustment
            && mUseAutoBrightness == ((CDisplayPowerRequest*)other)->mUseAutoBrightness
            && mBlockScreenOn == ((CDisplayPowerRequest*)other)->mBlockScreenOn
            && mLowPowerMode == ((CDisplayPowerRequest*)other)->mLowPowerMode
            && mDozeScreenBrightness == ((CDisplayPowerRequest*)other)->mDozeScreenBrightness
            && mDozeScreenState == ((CDisplayPowerRequest*)other)->mDozeScreenState);
    return NOERROR;
}

ECode CDisplayPowerRequest::GetHashCode(
    /* [out] */ Int32* hc)
{
    VALIDATE_NOT_NULL(hc);

    *hc = 0; // don't care
    return NOERROR;
}

static String StateToString(
    /* [in] */ Int32 state)
{
    switch (state) {
        case IDisplay::STATE_UNKNOWN:
            return String("UNKNOWN");
        case IDisplay::STATE_OFF:
            return String("OFF");
        case IDisplay::STATE_ON:
            return String("ON");
        case IDisplay::STATE_DOZE:
            return String("DOZE");
        case IDisplay::STATE_DOZE_SUSPEND:
            return String("DOZE_SUSPEND");
        default:
            return StringUtils::ToString(state);
    }
}

ECode CDisplayPowerRequest::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb;
    sb += "policy=";
    String tmp;
    PolicyToString(mPolicy, &tmp);
    sb += tmp;
    sb += ", useProximitySensor=";
    sb += StringUtils::BooleanToString(mUseProximitySensor);
    sb += ", screenBrightness=";
    sb += StringUtils::ToString(mScreenBrightness);
    sb += ", screenAutoBrightnessAdjustment=";
    sb += StringUtils::ToString(mScreenAutoBrightnessAdjustment);
    sb += ", useAutoBrightness=";
    sb += StringUtils::BooleanToString(mUseAutoBrightness);
    sb += ", blockScreenOn=";
    sb += StringUtils::BooleanToString(mBlockScreenOn);
    sb += ", lowPowerMode=";
    sb += StringUtils::BooleanToString(mLowPowerMode);
    sb += ", dozeScreenBrightness=";
    sb += StringUtils::ToString(mDozeScreenBrightness);
    sb += ", dozeScreenState=";
    sb += StateToString(mDozeScreenState);

    return sb.ToString(result);
}

ECode CDisplayPowerRequest::PolicyToString(
    /* [in] */ Int32 policy,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    switch (policy) {
        case IDisplayPowerRequest::POLICY_OFF:
            *result = String("OFF");
            return NOERROR;
        case IDisplayPowerRequest::POLICY_DOZE:
            *result = String("DOZE");
            return NOERROR;
        case IDisplayPowerRequest::POLICY_DIM:
            *result = String("DIM");
            return NOERROR;
        case IDisplayPowerRequest::POLICY_BRIGHT:
            *result = String("BRIGHT");
            return NOERROR;
        default:
            *result = StringUtils::ToString(policy);
            return NOERROR;
    }
}

ECode CDisplayPowerRequest::SetPolicy(
    /* [in] */ Int32 value)
{
    mPolicy = value;
    return NOERROR;
}

ECode CDisplayPowerRequest::SetUseProximitySensor(
    /* [in] */ Boolean value)
{
    mUseProximitySensor = value;
    return NOERROR;
}

ECode CDisplayPowerRequest::SetScreenBrightness(
    /* [in] */ Int32 value)
{
    Logger::I(TAG, " >> SetScreenBrightness: %d", value);
    mScreenBrightness = value;
    return NOERROR;
}

ECode CDisplayPowerRequest::SetScreenAutoBrightnessAdjustment(
    /* [in] */ Float value)
{
    Logger::I(TAG, " >> SetScreenAutoBrightnessAdjustment: %d", value);
    mScreenAutoBrightnessAdjustment = value;
    return NOERROR;
}

ECode CDisplayPowerRequest::SetUseAutoBrightness(
    /* [in] */ Boolean value)
{
    Logger::I(TAG, " >> SetUseAutoBrightness: %d", value);
    mUseAutoBrightness = value;
    return NOERROR;
}

ECode CDisplayPowerRequest::SetLowPowerMode(
    /* [in] */ Boolean value)
{
    mLowPowerMode = value;
    return NOERROR;
}

ECode CDisplayPowerRequest::SetBlockScreenOn(
    /* [in] */ Boolean value)
{
    mBlockScreenOn = value;
    return NOERROR;
}

ECode CDisplayPowerRequest::SetDozeScreenBrightness(
    /* [in] */ Int32 value)
{
    mDozeScreenBrightness = value;
    return NOERROR;
}

ECode CDisplayPowerRequest::SetDozeScreenState(
    /* [in] */ Int32 value)
{
    mDozeScreenState = value;
    return NOERROR;
}

ECode CDisplayPowerRequest::GetPolicy(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mPolicy;
    return NOERROR;
}

ECode CDisplayPowerRequest::GetUseProximitySensor(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mUseProximitySensor;
    return NOERROR;
}

ECode CDisplayPowerRequest::GetScreenBrightness(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mScreenBrightness;
    return NOERROR;
}

ECode CDisplayPowerRequest::GetScreenAutoBrightnessAdjustment(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mScreenAutoBrightnessAdjustment;
    return NOERROR;
}

ECode CDisplayPowerRequest::GetUseAutoBrightness(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mUseAutoBrightness;
    return NOERROR;
}

ECode CDisplayPowerRequest::GetLowPowerMode(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mLowPowerMode;
    return NOERROR;
}

ECode CDisplayPowerRequest::GetBlockScreenOn(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mBlockScreenOn;
    return NOERROR;
}

ECode CDisplayPowerRequest::GetDozeScreenBrightness(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mDozeScreenBrightness;
    return NOERROR;
}

ECode CDisplayPowerRequest::GetDozeScreenState(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mDozeScreenState;
    return NOERROR;
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos