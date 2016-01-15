
#include "CBluetoothClass.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

ECode CBluetoothClass::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code)
    *code = mClass;
    return NOERROR;
}

ECode CBluetoothClass::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IBluetoothClass> bc = (IBluetoothClass*)o->Probe(EIID_IBluetoothClass);
    if (bc != NULL) {
        Int32 value;
        bc->GetClass(&value);
        *result = mClass == value;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CBluetoothClass::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = StringUtils::Int32ToHexString(mClass);
    return NOERROR;
}

ECode CBluetoothClass::HasService(
    /* [in] */ Int32 service,
    /* [out] */ Boolean* hasService)
{
    VALIDATE_NOT_NULL(service)
    *hasService = (mClass & IBluetoothClassService::BITMASK & service) != 0;
    return NOERROR;
}

ECode CBluetoothClass::GetMajorDeviceClass(
    /* [out] */ Int32* deviceClass)
{
    VALIDATE_NOT_NULL(deviceClass)
    *deviceClass = mClass & IBluetoothClassDeviceMajor::BITMASK;
    return NOERROR;
}

ECode CBluetoothClass::GetDeviceClass(
    /* [out] */ Int32* deviceClass)
{
    VALIDATE_NOT_NULL(deviceClass)
    *deviceClass = mClass & IBluetoothClassDevice::BITMASK;
    return NOERROR;
}

ECode CBluetoothClass::DoesClassMatch(
    /* [in] */ Int32 profile,
    /* [out] */ Boolean* isMatch)
{
    VALIDATE_NOT_NULL(isMatch)

    if (profile == IBluetoothClass::PROFILE_A2DP) {
        Boolean hasService;
        if (HasService(IBluetoothClassService::RENDER, &hasService), hasService) {
            *isMatch = TRUE;
            return NOERROR;
        }

        // By the A2DP spec, sinks must indicate the RENDER service.
        // However we found some that do not (Chordette). So lets also
        // match on some other class bits.
        Int32 deviceClass;
        GetDeviceClass(&deviceClass);
        switch (deviceClass) {
            case IBluetoothClassDevice::AUDIO_VIDEO_HIFI_AUDIO:
            case IBluetoothClassDevice::AUDIO_VIDEO_HEADPHONES:
            case IBluetoothClassDevice::AUDIO_VIDEO_LOUDSPEAKER:
            case IBluetoothClassDevice::AUDIO_VIDEO_CAR_AUDIO:
                *isMatch = TRUE;
                return NOERROR;
            default:
                *isMatch = FALSE;
                return NOERROR;
        }
    }
    else if (profile == IBluetoothClass::PROFILE_HEADSET) {
        // The render service class is required by the spec for HFP, so is a
        // pretty good signal
        Boolean hasService;
        if (HasService(IBluetoothClassService::RENDER, &hasService), hasService) {
            *isMatch = TRUE;
            return NOERROR;
        }

        // Just in case they forgot the render service class
        Int32 deviceClass;
        GetDeviceClass(&deviceClass);
        switch (deviceClass) {
            case IBluetoothClassDevice::AUDIO_VIDEO_HANDSFREE:
            case IBluetoothClassDevice::AUDIO_VIDEO_WEARABLE_HEADSET:
            case IBluetoothClassDevice::AUDIO_VIDEO_CAR_AUDIO:
                *isMatch = TRUE;
                return NOERROR;
            default:
                *isMatch = FALSE;
                return NOERROR;
        }
    }
    else if (profile == IBluetoothClass::PROFILE_OPP) {
        Boolean hasService;
        if (HasService(IBluetoothClassService::OBJECT_TRANSFER, &hasService), hasService) {
            *isMatch = TRUE;
            return NOERROR;
        }

        Int32 deviceClass;
        GetDeviceClass(&deviceClass);
        switch (deviceClass) {
            case IBluetoothClassDevice::COMPUTER_UNCATEGORIZED:
            case IBluetoothClassDevice::COMPUTER_DESKTOP:
            case IBluetoothClassDevice::COMPUTER_SERVER:
            case IBluetoothClassDevice::COMPUTER_LAPTOP:
            case IBluetoothClassDevice::COMPUTER_HANDHELD_PC_PDA:
            case IBluetoothClassDevice::COMPUTER_PALM_SIZE_PC_PDA:
            case IBluetoothClassDevice::COMPUTER_WEARABLE:
            case IBluetoothClassDevice::PHONE_UNCATEGORIZED:
            case IBluetoothClassDevice::PHONE_CELLULAR:
            case IBluetoothClassDevice::PHONE_CORDLESS:
            case IBluetoothClassDevice::PHONE_SMART:
            case IBluetoothClassDevice::PHONE_MODEM_OR_GATEWAY:
            case IBluetoothClassDevice::PHONE_ISDN:
                *isMatch = TRUE;
                return NOERROR;
            default:
                *isMatch = FALSE;
                return NOERROR;
        }
    }
    else if (profile == IBluetoothClass::PROFILE_HID) {
        Int32 deviceClass;
        GetDeviceClass(&deviceClass);
        *isMatch = (deviceClass & IBluetoothClassDeviceMajor::PERIPHERAL) == IBluetoothClassDeviceMajor::PERIPHERAL;
        return NOERROR;

    }
    else if (profile == IBluetoothClass::PROFILE_PANU || profile == IBluetoothClass::PROFILE_NAP){
        // No good way to distinguish between the two, based on class bits.
        Boolean hasService;
        if (HasService(IBluetoothClassService::NETWORKING, &hasService), hasService) {
            *isMatch = TRUE;
            return NOERROR;
        }

        Int32 deviceClass;
        GetDeviceClass(&deviceClass);
        *isMatch = (deviceClass & IBluetoothClassDeviceMajor::NETWORKING) == IBluetoothClassDeviceMajor::NETWORKING;
        return NOERROR;
    }
    else {
        *isMatch = FALSE;
        return NOERROR;
    }
}

ECode CBluetoothClass::GetClass(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mClass;
    return NOERROR;
}

ECode CBluetoothClass::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source)
    source->ReadInt32(&mClass);
    return NOERROR;
}

ECode CBluetoothClass::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mClass);
    return NOERROR;
}

ECode CBluetoothClass::constructor()
{
    return NOERROR;
}

ECode CBluetoothClass::constructor(
    /* [in] */ Int32 classInt)
{
    mClass = classInt;
    return NOERROR;
}

}
}
}

