#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/media/audiofx/CVirtualizer.h"
#include "elastos/droid/media/audiofx/CVirtualizerSettings.h"
#include "elastos/droid/media/CAudioDevice.h"
#include "elastos/droid/media/CAudioFormatHelper.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::ByteOrder;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

const String CVirtualizer::TAG("Virtualizer");

const Boolean CVirtualizer::DEBUG = FALSE;

CAR_INTERFACE_IMPL(CVirtualizer, AudioEffect, IVirtualizer)

CAR_OBJECT_IMPL(CVirtualizer)

CAR_INTERFACE_IMPL(CVirtualizer::BaseParameterListener, Object, IAudioEffectOnParameterChangeListener)

ECode CVirtualizer::BaseParameterListener::OnParameterChange(
    /* [in] */ IAudioEffect* effect,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* param,
    /* [in] */ ArrayOf<Byte>* value)
{
    AutoPtr<IVirtualizerOnParameterChangeListener> l;
    {
        Object& lock = mHost->mParamListenerLock;
        AutoLock syncLock(lock);
        if (mHost->mParamListener != NULL) {
            l = mHost->mParamListener;
        }
    }
    if (l != NULL) {
        Int32 p = -1;
        Int16 v = -1;

        if (param->GetLength() == 4) {
            mHost->ByteArrayToInt32(param, 0, &p);
        }
        if (value->GetLength() == 2) {
            mHost->ByteArrayToInt16(value, 0, &v);
        }
        if (p != -1 && v != -1) {
            l->OnParameterChange((IVirtualizer*)mHost, status, p, v);
        }
    }
    return NOERROR;
}

CVirtualizer::CVirtualizer()
    : mStrengthSupported(FALSE)
{
}

ECode CVirtualizer::constructor(
    /* [in] */ Int32 priority,
    /* [in] */ Int32 audioSession)
{
    // super(EFFECT_TYPE_VIRTUALIZER, EFFECT_TYPE_NULL, priority, audioSession);
    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> typeVIRTUALIZER;
    AutoPtr<IUUID> typeNULL;
    helper->FromString(IAudioEffect::EFFECT_TYPE_VIRTUALIZER, (IUUID**)&typeVIRTUALIZER);
    helper->FromString(IAudioEffect::EFFECT_TYPE_NULL, (IUUID**)&typeNULL);
    AudioEffect::constructor(typeVIRTUALIZER, typeNULL, priority, audioSession);

    if (audioSession == 0) {
        // Log.w(TAG, "WARNING: attaching a Virtualizer to global output mix is deprecated!");
    }

    AutoPtr<ArrayOf<Int32> > value = ArrayOf<Int32>::Alloc(1);
    Int32 status;
    GetParameter(IVirtualizer::PARAM_STRENGTH_SUPPORTED, value, &status);
    CheckStatus(status);
    mStrengthSupported = (value[0] != 0);
    return NOERROR;
}

ECode CVirtualizer::GetStrengthSupported(
    /* [out] */ Boolean* strengthSupported)
{
    VALIDATE_NOT_NULL(strengthSupported);
    *strengthSupported = mStrengthSupported;
    return NOERROR;
}

ECode CVirtualizer::SetStrength(
    /* [in] */ Int16 strength)
{
    Int32 status;
    SetParameter(IVirtualizer::PARAM_STRENGTH, strength, &status);
    return CheckStatus(status);
}

ECode CVirtualizer::GetRoundedStrength(
    /* [out] */ Boolean* strength)
{
    VALIDATE_NOT_NULL(strength);

    AutoPtr<ArrayOf<Int16> > value = ArrayOf<Int16>::Alloc(1);
    Int32 status;
    GetParameter(IVirtualizer::PARAM_STRENGTH, value, &status);
    CheckStatus(status);
    *strength = (*value)[0];
    return NOERROR;
}

ECode CVirtualizer::CanVirtualize(
    /* [in] */ Int32 inputChannelMask,
    /* [in] */ Int32 virtualizationMode,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    Int32 deviceType;
    GetDeviceForModeQuery(virtualizationMode, &deviceType);
    return GetAnglesInt(inputChannelMask, deviceType, NULL, result);
}

ECode CVirtualizer::GetSpeakerAngles(
    /* [in] */ Int32 inputChannelMask,
    /* [in] */ Int32 virtualizationMode,
    /* [in] */ ArrayOf<Int32> * angles,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    if (angles == NULL) {
        // throw (new IllegalArgumentException(
        //         "Virtualizer: illegal null channel / angle array"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 deviceType;
    GetDeviceForModeQuery(virtualizationMode, &deviceType);
    return GetAnglesInt(inputChannelMask, deviceType, angles, result);
}

ECode CVirtualizer::ForceVirtualizationMode(
    /* [in] */ Int32 virtualizationMode,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    // convert Java device type to internal representation
    Int32 deviceType = GetDeviceForModeForce(virtualizationMode);
    Int32 internalDevice;
    CAudioDevice::ConvertDeviceTypeToInternalDevice(deviceType, &internalDevice);

    Int32 status;
    SetParameter(IVirtualizer::PARAM_FORCE_VIRTUALIZATION_MODE, internalDevice, &status);

    if (status >= 0) {
        *result = TRUE;
        return NOERROR;
    } else if (status == IAudioEffect::ERROR_BAD_VALUE) {
        // a BAD_VALUE return from setParameter indicates the mode can't be forced
        // don't throw an exception, just return false
        *result = FALSE;
        return NOERROR;
    } else {
        // something wrong may have happened
        CheckStatus(status);
    }
    // unexpected virtualizer behavior
    Logger::E(TAG, "unexpected status code %d after"
            " setParameter(PARAM_FORCE_VIRTUALIZATION_MODE)", status);
    *result = FALSE;
    return NOERROR;
}

ECode CVirtualizer::GetVirtualizationMode(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<ArrayOf<Int32> > value = ArrayOf<Int32>::Alloc(1);
    Int32 status;
    GetParameter(IVirtualizer::PARAM_VIRTUALIZATION_MODE, value, &status);
    if (status >= 0) {
        Int32 device;
        CAudioDevice::ConvertDeviceTypeToInternalDevice((*value)[0], &device);
        *result = DeviceToMode(device);
        return NOERROR;
    } else if (status == IAudioEffect::ERROR_BAD_VALUE) {
        *result = IVirtualizer::VIRTUALIZATION_MODE_OFF;
        return NOERROR;
    } else {
        // something wrong may have happened
        CheckStatus(status);
    }
    // unexpected virtualizer behavior
    Logger::E(TAG, "unexpected status code %d after"
            " getParameter(PARAM_VIRTUALIZATION_MODE)", status);
    *result = VIRTUALIZATION_MODE_OFF;
    return NOERROR;
}

ECode CVirtualizer::SetParameterListener(
    /* [in] */ IVirtualizerOnParameterChangeListener* listener)
{
    AutoLock syncLock(mParamListenerLock);
    if (mParamListener != NULL) {
        mParamListener = listener;
        mBaseParamListener = new BaseParameterListener(this);
        AudioEffect::SetParameterListener(IAudioEffectOnParameterChangeListener::Probe(mBaseParamListener));
    }
    return NOERROR;
}

ECode CVirtualizer::GetProperties(
    /* [out] */ IVirtualizerSettings** properties)
{
    VALIDATE_NOT_NULL(properties);

    AutoPtr<IVirtualizerSettings> settings;
    CVirtualizerSettings::New((IVirtualizerSettings**)&settings);
    AutoPtr<ArrayOf<Int16> > value = ArrayOf<Int16>::Alloc(1);
    Int32 status;
    GetParameter(IVirtualizer::PARAM_STRENGTH, value, &status);
    CheckStatus(status);
    settings->SetStrength((*value)[0]);
    *properties = settings;
    REFCOUNT_ADD(*properties);
    return NOERROR;
}

ECode CVirtualizer::SetProperties(
    /* [in] */ IVirtualizerSettings* settings)
{
    Int16 strength;
    settings->GetStrength(&strength);
    Int32 status;
    SetParameter(IVirtualizer::PARAM_STRENGTH, strength, &status);
    return CheckStatus(status);
}

ECode CVirtualizer::GetAnglesInt(
    /* [in] */ Int32 inputChannelMask,
    /* [in] */ Int32 deviceType,
    /* [in] */ ArrayOf<Int32> * angles,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // parameter check
    if (inputChannelMask == IAudioFormat::CHANNEL_INVALID) {
        // throw (new IllegalArgumentException(
        //         "Virtualizer: illegal CHANNEL_INVALID channel mask"));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 channelMask = inputChannelMask == IAudioFormat::CHANNEL_OUT_DEFAULT ?
            IAudioFormat::CHANNEL_OUT_STEREO : inputChannelMask;
    AutoPtr<IAudioFormatHelper> afHelper;
    CAudioFormatHelper::AcquireSingleton((IAudioFormatHelper**)&afHelper);
    Int32 nbChannels;
    afHelper->ChannelCountFromOutChannelMask(channelMask, &nbChannels);

    if ((angles != NULL) && (angles->GetLength() < (nbChannels * 3))) {
        Logger::E(TAG, "Size of array for angles cannot accomodate number of channels in mask (%d)",
                nbChannels);
        // throw (new IllegalArgumentException(
        //         "Virtualizer: array for channel / angle pairs is too small: is " + angles.length
        //         + ", should be " + (nbChannels * 3)));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IByteBuffer> paramsConverter;
    AutoPtr<IByteBufferHelper> bbhelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbhelper);
    bbhelper->Allocate(3 /* param + mask + device*/ * 4, (IByteBuffer**)&paramsConverter);
    AutoPtr<IByteOrderHelper> boHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&boHelper);
    ByteOrder order;
    boHelper->GetNativeOrder(&order);
    paramsConverter->SetOrder(order);
    paramsConverter->PutInt32(IVirtualizer::PARAM_VIRTUAL_SPEAKER_ANGLES);
    // convert channel mask to internal native representation
    Int32 mask;
    afHelper->ConvertChannelOutMaskToNativeMask(channelMask, &mask);
    paramsConverter->PutInt32(mask);
    // convert Java device type to internal representation
    Int32 device;
    CAudioDevice::ConvertDeviceTypeToInternalDevice(deviceType, &device);
    paramsConverter->PutInt32(device);
    // allocate an array to store the results
    AutoPtr<ArrayOf<Byte> > paramArray = ArrayOf<Byte>::Alloc(nbChannels * 4/*int to byte*/ * 3/*for mask, azimuth, elevation*/);

    // call into the effect framework
    Int32 status;
    AutoPtr<ArrayOf<Byte> > array;
    paramsConverter->GetArray((ArrayOf<Byte>**)&array);
    GetParameter(array, paramArray, &status);
    if (DEBUG) {
        // Logg.v(TAG, "getAngles(0x" + Integer.toHexString(inputChannelMask) + ", 0x"
        //         + Integer.toHexString(deviceType) + ") returns " + status);
    }

    if (status >= 0) {
        if (angles != NULL) {
            // convert and copy the results
            AutoPtr<IByteBuffer> resultConverter;
            bbhelper->Wrap(paramArray, (IByteBuffer**)&resultConverter);
            resultConverter->SetOrder(order);
            for (Int32 i = 0 ; i < nbChannels ; i++) {
                // write the channel mask
                Int32 value;
                resultConverter->GetInt32(i * 4 * 3, &value);
                afHelper->ConvertNativeChannelMaskToOutMask(value, &((*angles)[3 * i]));
                // write the azimuth
                resultConverter->GetInt32(i * 4 * 3 + 4, &((*angles)[3 * i + 1]));
                // write the elevation
                resultConverter->GetInt32(i * 4 * 3 + 8, &((*angles)[3 * i + 2]));
                if (DEBUG) {
                    // Log.v(TAG, "channel 0x" + Integer.toHexString(angles[3*i]).toUpperCase()
                    //         + " at az=" + angles[3*i+1] + "deg"
                    //         + " elev="  + angles[3*i+2] + "deg");
                }
            }
        }
        *result = TRUE;
        return NOERROR;
    } else if (status == IAudioEffect::ERROR_BAD_VALUE) {
        // a BAD_VALUE return from getParameter indicates the configuration is not supported
        // don't throw an exception, just return false
        *result = FALSE;
        return NOERROR;
    } else {
        // something wrong may have happened
        CheckStatus(status);
    }
    // unexpected virtualizer behavior
    Logger::E(TAG, "unexpected status code %d"
            " after getParameter(PARAM_VIRTUAL_SPEAKER_ANGLES)", status);
    *result = FALSE;
    return NOERROR;
}

ECode CVirtualizer::GetDeviceForModeQuery(
    /* [in] */ Int32 virtualizationMode,
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    switch (virtualizationMode) {
        case IVirtualizer::VIRTUALIZATION_MODE_BINAURAL:
            *result = IAudioDevice::TYPE_WIRED_HEADPHONES;
            return NOERROR;
        case IVirtualizer::VIRTUALIZATION_MODE_TRANSAURAL:
            *result = IAudioDevice::TYPE_BUILTIN_SPEAKER;
            return NOERROR;
        default:
            // throw (new IllegalArgumentException(
            //         "Virtualizer: illegal virtualization mode " + virtualizationMode));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

Int32 CVirtualizer::GetDeviceForModeForce(
    /* [in] */ Int32 virtualizationMode)
{
    if (virtualizationMode == IVirtualizer::VIRTUALIZATION_MODE_AUTO) {
        return IAudioDevice::TYPE_UNKNOWN;
    } else {
        Int32 deviceType;
        GetDeviceForModeQuery(virtualizationMode, &deviceType);
        return deviceType;
    }
}

Int32 CVirtualizer::DeviceToMode(
    /* [in] */ Int32 deviceType)
{
    switch (deviceType) {
        case IAudioDevice::TYPE_WIRED_HEADSET:
        case IAudioDevice::TYPE_WIRED_HEADPHONES:
        case IAudioDevice::TYPE_BLUETOOTH_SCO:
        case IAudioDevice::TYPE_BUILTIN_EARPIECE:
            return IVirtualizer::VIRTUALIZATION_MODE_BINAURAL;
        case IAudioDevice::TYPE_BUILTIN_SPEAKER:
        case IAudioDevice::TYPE_LINE_ANALOG:
        case IAudioDevice::TYPE_LINE_DIGITAL:
        case IAudioDevice::TYPE_BLUETOOTH_A2DP:
        case IAudioDevice::TYPE_HDMI:
        case IAudioDevice::TYPE_HDMI_ARC:
        case IAudioDevice::TYPE_USB_DEVICE:
        case IAudioDevice::TYPE_USB_ACCESSORY:
        case IAudioDevice::TYPE_DOCK:
        case IAudioDevice::TYPE_FM:
        case IAudioDevice::TYPE_AUX_LINE:
            return IVirtualizer::VIRTUALIZATION_MODE_TRANSAURAL;
        case IAudioDevice::TYPE_UNKNOWN:
        default:
            return IVirtualizer::VIRTUALIZATION_MODE_OFF;
    }
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
