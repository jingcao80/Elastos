
#include "elastos/droid/hardware/hdmi/HdmiTvClient.h"
#include "elastos/droid/hardware/hdmi/HdmiRecordListener.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Hdmi {

CAR_INTERFACE_IMPL_2(HdmiTvClient::MyCallback, Object, IIHdmiControlCallback, IBinder)

HdmiTvClient::MyCallback::MyCallback(
    /* [in] */ IHdmiTvClientSelectCallback* _callback)
    : mCallback(_callback)
{
}

ECode HdmiTvClient::MyCallback::OnComplete(
    /* [in] */ Int32 result)
{
    return mCallback->OnComplete(result);
}

ECode HdmiTvClient::MyCallback::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

CAR_INTERFACE_IMPL(HdmiTvClient::MyListener, Object, IIHdmiInputChangeListener)

HdmiTvClient::MyListener::MyListener(
    /* [in] */ IHdmiTvClientInputChangeListener* listener)
    : mListener(listener)
{
}

ECode HdmiTvClient::MyListener::OnChanged(
    /* [in] */ IHdmiDeviceInfo* info)
{
    return mListener->OnChanged(info);
}

ECode HdmiTvClient::MyListener::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

CAR_INTERFACE_IMPL(HdmiTvClient::MyListener2, Object, IIHdmiRecordListener)

HdmiTvClient::MyListener2::MyListener2(
    /* [in] */ IHdmiRecordListener* listener)
    : mListener(listener)
{
}

ECode HdmiTvClient::MyListener2::GetOneTouchRecordSource(
    /* [in] */ Int32 recorderAddress,
    /* [out, callee] */ ArrayOf<Byte>** source)
{
    VALIDATE_NOT_NULL(source);
    *source = NULL;

    AutoPtr<IRecordSource> _source;
    mListener->GetOneTouchRecordSource(recorderAddress, (IRecordSource**)&_source);
    if (_source == NULL) {
        *source = NULL;
        return NOERROR;
    }

    Int32 size;
    _source->GetDataSize(TRUE, &size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    Int32 res;
    _source->ToByteArray(TRUE, data, 0, &res);
    *source = data;
    REFCOUNT_ADD(*source);
    return NOERROR;
}

ECode HdmiTvClient::MyListener2::OnOneTouchRecordResult(
    /* [in] */ Int32 result)
{
    return mListener->OnOneTouchRecordResult(result);
}

ECode HdmiTvClient::MyListener2::OnTimerRecordingResult(
    /* [in] */ Int32 result)
{
    AutoPtr<IHdmiRecordListenerTimerStatusData> tmp;
    HdmiRecordListener::TimerStatusData::ParseFrom(result, (IHdmiRecordListenerTimerStatusData**)&tmp);
    return mListener->OnTimerRecordingResult(tmp);
}

ECode HdmiTvClient::MyListener2::OnClearTimerRecordingResult(
    /* [in] */ Int32 result)
{
    return mListener->OnClearTimerRecordingResult(result);
}

ECode HdmiTvClient::MyListener2::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

CAR_INTERFACE_IMPL(HdmiTvClient::MyListener3, Object, IIHdmiMhlVendorCommandListener)

HdmiTvClient::MyListener3::MyListener3(
    /* [in] */ IHdmiTvClientHdmiMhlVendorCommandListener* listener)
    : mListener(listener)
{
}

ECode HdmiTvClient::MyListener3::OnReceived(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data)
{
    return mListener->OnReceived(portId, offset, length, data);
}

ECode HdmiTvClient::MyListener3::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = NULL;

    return NOERROR;
}

const String HdmiTvClient::TAG("HdmiTvClient");

CAR_INTERFACE_IMPL(HdmiTvClient, HdmiClient, IHdmiTvClient)

HdmiTvClient::HdmiTvClient()
{
}

ECode HdmiTvClient::constructor()
{
    return NOERROR;
}

ECode HdmiTvClient::constructor(
    /* [in] */ IIHdmiControlService* service)
{
    return HdmiClient::constructor(service);
}

ECode HdmiTvClient::Create(
    /* [in] */ IIHdmiControlService* service,
    /* [out] */ IHdmiTvClient** client)
{
    VALIDATE_NOT_NULL(client);
    *client = NULL;

    AutoPtr<HdmiTvClient> tmp = new HdmiTvClient();
    tmp->constructor(service);
    *client = (IHdmiTvClient*)tmp.Get();
    REFCOUNT_ADD(*client);
    return NOERROR;
}

ECode HdmiTvClient::GetDeviceType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    *type = IHdmiDeviceInfo::DEVICE_TV;
    return NOERROR;
}

ECode HdmiTvClient::DeviceSelect(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ IHdmiTvClientSelectCallback* _callback)
{
    if (_callback == NULL) {
        // throw new IllegalArgumentException("callback must not be null.");
        Slogger::E(TAG, "callback must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //try {
    AutoPtr<IIHdmiControlCallback> tmp = GetCallbackWrapper(_callback);
    ECode ec = mService->DeviceSelect(logicalAddress, tmp);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to select device: %d", ec);
    }
    //}
    return NOERROR;
}

AutoPtr<IIHdmiControlCallback> HdmiTvClient::GetCallbackWrapper(
    /* [in] */ IHdmiTvClientSelectCallback* _callback)
{
    AutoPtr<IIHdmiControlCallback> tmp = new MyCallback(_callback);
    return tmp;
}

ECode HdmiTvClient::PortSelect(
    /* [in] */ Int32 portId,
    /* [in] */ IHdmiTvClientSelectCallback* _callback)
{
    if (_callback == NULL) {
        //throw new IllegalArgumentException("Callback must not be null");
        Slogger::E(TAG, "Callback must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //try {
    AutoPtr<IIHdmiControlCallback> tmp = GetCallbackWrapper(_callback);
    ECode ec = mService->PortSelect(portId, tmp);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to select port: %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiTvClient::SetInputChangeListener(
    /* [in] */ IHdmiTvClientInputChangeListener* listener)
{
    if (listener == NULL) {
        //throw new IllegalArgumentException("listener must not be null.");
        Slogger::E(TAG, "listener must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //try {
    AutoPtr<IIHdmiInputChangeListener> tmp = GetListenerWrapper(listener);
    ECode ec = mService->SetInputChangeListener(tmp);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E("TAG", "Failed to set InputChangeListener:%d", ec);
    }
    //}
    return NOERROR;
}

AutoPtr<IIHdmiInputChangeListener> HdmiTvClient::GetListenerWrapper(
    /* [in] */ IHdmiTvClientInputChangeListener* listener)
{
    AutoPtr<IIHdmiInputChangeListener> tmp = new MyListener(listener);
    return tmp;
}

ECode HdmiTvClient::SetSystemAudioVolume(
    /* [in] */ Int32 oldIndex,
    /* [in] */ Int32 newIndex,
    /* [in] */ Int32 maxIndex)
{
    //try {
    ECode ec = mService->SetSystemAudioVolume(oldIndex, newIndex, maxIndex);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to set volume: %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiTvClient::SetSystemAudioMute(
    /* [in] */ Boolean mute)
{
    //try {
    ECode ec = mService->SetSystemAudioMute(mute);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to set mute: %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiTvClient::SetRecordListener(
    /* [in] */ IHdmiRecordListener* listener)
{
    if (listener == NULL) {
        //throw new IllegalArgumentException("listener must not be null.");
        Slogger::E(TAG, "listener must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //try {
    AutoPtr<IIHdmiRecordListener> tmp = GetListenerWrapper(listener);
    ECode ec = mService->SetHdmiRecordListener(tmp);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to set record listener.%d", ec);
    }
    //}
    return NOERROR;
}

AutoPtr<IIHdmiRecordListener> HdmiTvClient::GetListenerWrapper(
    /* [in] */ IHdmiRecordListener* listener)
{
    AutoPtr<IIHdmiRecordListener> tmp = new MyListener2(listener);
    return tmp;
}

ECode HdmiTvClient::StartOneTouchRecord(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ IRecordSource* source)
{
    if (source == NULL) {
        //throw new IllegalArgumentException("source must not be null.");
        Slogger::E(TAG, "source must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //try {
    Int32 size;
    source->GetDataSize(TRUE, &size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    Int32 res;
    source->ToByteArray(TRUE, data, 0, &res);
    ECode ec = mService->StartOneTouchRecord(recorderAddress, data);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to start record: %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiTvClient::StopOneTouchRecord(
    /* [in] */ Int32 recorderAddress)
{
    //try {
    ECode ec = mService->StopOneTouchRecord(recorderAddress);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to stop record: %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiTvClient::StartTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ITimerRecordSource* source)
{
    if (source == NULL) {
        //throw new IllegalArgumentException("source must not be null.");
        Slogger::E(TAG, "source must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CheckTimerRecordingSourceType(sourceType))

    //try {
    Int32 size;
    source->GetDataSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    Int32 res;
    source->ToByteArray(data, 0, &res);
    ECode ec = mService->StartTimerRecording(recorderAddress, sourceType, data);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to start record: %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiTvClient::CheckTimerRecordingSourceType(
    /* [in] */ Int32 sourceType)
{
    switch (sourceType) {
        case IHdmiControlManager::TIMER_RECORDING_TYPE_DIGITAL:
        case IHdmiControlManager::TIMER_RECORDING_TYPE_ANALOGUE:
        case IHdmiControlManager::TIMER_RECORDING_TYPE_EXTERNAL:
            break;
        default:
            //throw new IllegalArgumentException("Invalid source type:" + sourceType);
            Slogger::E(TAG, "Invalid source type:%d", sourceType);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode HdmiTvClient::ClearTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ITimerRecordSource* source)
{
    if (source == NULL) {
        //throw new IllegalArgumentException("source must not be null.");
        Slogger::E(TAG, "source must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(CheckTimerRecordingSourceType(sourceType))
    //try {
    Int32 size;
    source->GetDataSize(&size);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
    Int32 res;
    source->ToByteArray(data, 0, &res);
    ECode ec = mService->ClearTimerRecording(recorderAddress, sourceType, data);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to start record: %d", ec);
    }
    //}
    return NOERROR;
}

ECode HdmiTvClient::SetHdmiMhlVendorCommandListener(
    /* [in] */ IHdmiTvClientHdmiMhlVendorCommandListener* listener)
{
    if (listener == NULL) {
        //throw new IllegalArgumentException("listener must not be null.");
        Slogger::E(TAG, "listener must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //try {
    AutoPtr<IIHdmiMhlVendorCommandListener> tmp = GetListenerWrapper(listener);
    ECode ec = mService->AddHdmiMhlVendorCommandListener(tmp);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to set hdmi mhl vendor command listener: %d", ec);
    }
    //}
    return NOERROR;
}

AutoPtr<IIHdmiMhlVendorCommandListener> HdmiTvClient::GetListenerWrapper(
    /* [in] */ IHdmiTvClientHdmiMhlVendorCommandListener* listener)
{
    AutoPtr<IIHdmiMhlVendorCommandListener> tmp = new MyListener3(listener);
    return NOERROR;
}

ECode HdmiTvClient::SendMhlVendorCommand(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data)
{
    if (data == NULL || data->GetLength() != VENDOR_DATA_SIZE) {
        //throw new IllegalArgumentException("Invalid vendor command data.");
        Slogger::E(TAG, "Invalid vendor command data.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (offset < 0 || offset >= VENDOR_DATA_SIZE) {
        //throw new IllegalArgumentException("Invalid offset:" + offset);
        Slogger::E(TAG, "Invalid offset:%d" , offset);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (length < 0 || offset + length > VENDOR_DATA_SIZE) {
        //throw new IllegalArgumentException("Invalid length:" + length);
        Slogger::E(TAG, "Invalid length:%d" , length);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //try {
    ECode ec = mService->SendMhlVendorCommand(portId, offset, length, data);
    //} catch (RemoteException e) {
    if (FAILED(ec)) {
        Slogger::E(TAG, "failed to send vendor command: %d", ec);
    }
    //}
    return NOERROR;
}

} // namespace Hdmi
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
