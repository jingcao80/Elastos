
#include "elastos/droid/server/hdmi/HdmiControlService.h"
#include "elastos/droid/server/hdmi/Constants.h"
#include "elastos/droid/server/hdmi/HdmiCecLocalDevice.h"
#include "elastos/droid/server/hdmi/HdmiCecMessageBuilder.h"
#include "elastos/droid/server/hdmi/HdmiLogger.h"
#include "elastos/droid/server/hdmi/HdmiMhlControllerStub.h"
#include "elastos/droid/server/hdmi/HdmiUtils.h"
#include "elastos/droid/server/hdmi/UnmodifiableSparseArray.h"
#include "elastos/droid/server/hdmi/UnmodifiableSparseInt32Array.h"
#include "elastos/droid/server/hdmi/HdmiMhlLocalDeviceStub.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Build.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <libcore/utility/EmptyArray.h>
#include <elastos/droid/provider/Settings.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Internal.h>
#include <Elastos.Droid.Provider.h>
#include <elastos/droid/net/ReturnOutValue.h>
#include <elastos/core/AutoLock.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Media.h>

using Elastos::Droid::Provider::Settings;
using Elastos::Core::IThread;
// using Elastos::Droid::Os::Build;
// using Elastos::Droid::Os::ISystemClock;
// using Elastos::Droid::Provider::ISettings::Global;
using Elastos::Core::CInteger32;
using Elastos::Core::CObject;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::Hdmi::EIID_IIHdmiControlService;
using Elastos::Droid::Hardware::Hdmi::EIID_IIHdmiControlCallback;
using Elastos::Droid::Hardware::Hdmi::IHdmiControlManager;
// using Elastos::Droid::Hardware::Hdmi::CHdmiDeviceInfo;
using Elastos::Droid::Internal::Utility::CIndentingPrintWriter;
using Elastos::Droid::Internal::Utility::IIndentingPrintWriter;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Server::ISystemService;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::CSimpleStringSplitter;
using Elastos::Droid::Text::ISimpleStringSplitter;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CSparseInt32Array;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Libcore::Utility::EmptyArray;
using Elastos::IO::IWriter;
using Elastos::Core::EIID_IRunnable;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

//=============================================================================
// HdmiControlService::VendorCommandListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::VendorCommandListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::VendorCommandListenerRecord::VendorCommandListenerRecord(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IIHdmiVendorCommandListener* listener,
    /* [in] */ Int32 deviceType)
    : mHost(host)
    , mListener(listener)
    , mDeviceType(deviceType)
{}

ECode HdmiControlService::VendorCommandListenerRecord::ProxyDied()
{
    synchronized(mHost->mLock) {
        mHost->mVendorCommandListenerRecords->Remove(TO_IINTERFACE(this));
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::HdmiControlBroadcastReceiver
//=============================================================================
HdmiControlService::HdmiControlBroadcastReceiver::HdmiControlBroadcastReceiver(
    /* [in] */ HdmiControlService* host)
    : mHost(host)
{}

ECode HdmiControlService::HdmiControlBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    mHost->AssertRunOnServiceThread();
    String action;
    intent->GetAction(&action);
    if (action == IIntent::ACTION_SCREEN_OFF) {
        Boolean isPowerOnOrTransient;
        mHost->IsPowerOnOrTransient(&isPowerOnOrTransient);
        if (isPowerOnOrTransient) {
            mHost->OnStandby();
        }
    }
    else if (action == IIntent::ACTION_SCREEN_ON) {
        Boolean isPowerStandbyOrTransient;
        mHost->IsPowerStandbyOrTransient(&isPowerStandbyOrTransient);
        if (isPowerStandbyOrTransient) {
            mHost->OnWakeUp();
        }
    }
    else if (action == IIntent::ACTION_CONFIGURATION_CHANGED) {
        String language;
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        AutoPtr<ILocale> defaultLocale;
        helper->GetDefault((ILocale**)&defaultLocale);
        defaultLocale->GetISO3Language(&language);
        if (!mHost->mLanguage.Equals(language)) {
            mHost->OnLanguageChanged(language);
        }
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SettingsObserver
//=============================================================================
HdmiControlService::SettingsObserver::SettingsObserver(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IHandler* handler)
    : mHost(host)
{
    ContentObserver::constructor(handler);
}

ECode HdmiControlService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    String option;
    uri->GetLastPathSegment(&option);
    Boolean enabled;
    mHost->ReadBooleanSetting(option, TRUE, &enabled);
    if (option == ISettingsGlobal::HDMI_CONTROL_ENABLED) {
        mHost->SetControlEnabled(enabled);
    }
    else if (option == ISettingsGlobal::HDMI_CONTROL_AUTO_WAKEUP_ENABLED) {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
        ((HdmiCecLocalDeviceTv*) tv.Get())->SetAutoWakeup(enabled);
        Int32 iEnabled;
        ToInt32(enabled, &iEnabled);
        mHost->SetCecOption(Constants::OPTION_CEC_AUTO_WAKEUP, iEnabled);
    }
    else if (option == ISettingsGlobal::HDMI_CONTROL_AUTO_DEVICE_OFF_ENABLED) {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
        ((HdmiCecLocalDeviceTv*) tv.Get())->SetAutoDeviceOff(enabled);
        // No need to propagate to HAL.
    }
    else if (option == ISettingsGlobal::MHL_INPUT_SWITCHING_ENABLED) {
        mHost->SetMhlInputChangeEnabled(enabled);
    }
    else if (option == ISettingsGlobal::MHL_POWER_CHARGE_ENABLED) {
        Int32 iEnabled;
        ToInt32(enabled, &iEnabled);
        mHost->mMhlController->SetOption(Constants::OPTION_MHL_POWER_CHARGE, iEnabled);
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::HdmiMhlVendorCommandListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::HdmiMhlVendorCommandListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::HdmiMhlVendorCommandListenerRecord::HdmiMhlVendorCommandListenerRecord(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IIHdmiMhlVendorCommandListener* listener)
    : mHost(host)
    , mListener(listener)
{}

ECode HdmiControlService::HdmiMhlVendorCommandListenerRecord::ProxyDied()
{
    return mHost->mMhlVendorCommandListenerRecords->Remove(TO_IINTERFACE(this));
}

//=============================================================================
// HdmiControlService::HotplugEventListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::HotplugEventListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::HotplugEventListenerRecord::HotplugEventListenerRecord(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IIHdmiHotplugEventListener* listener)
    : mHost(host)
    , mListener(listener)
{}

ECode HdmiControlService::HotplugEventListenerRecord::ProxyDied()
{
    synchronized(mHost->mLock) {
        mHost->mHotplugEventListenerRecords->Remove(TO_IINTERFACE(this));
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::DeviceEventListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::DeviceEventListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::DeviceEventListenerRecord::DeviceEventListenerRecord(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IIHdmiDeviceEventListener* listener)
    : mHost(host)
    , mListener(listener)
{}

ECode HdmiControlService::DeviceEventListenerRecord::ProxyDied()
{
    synchronized(mHost->mLock) {
        mHost->mDeviceEventListenerRecords->Remove(TO_IINTERFACE(this));
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SystemAudioModeChangeListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SystemAudioModeChangeListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::SystemAudioModeChangeListenerRecord::SystemAudioModeChangeListenerRecord(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
    : mHost(host)
    , mListener(listener)
{}

ECode HdmiControlService::SystemAudioModeChangeListenerRecord::ProxyDied()
{
    synchronized(mHost->mLock) {
        mHost->mSystemAudioModeChangeListenerRecords->Remove(TO_IINTERFACE(this));
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::HdmiRecordListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::HdmiRecordListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::HdmiRecordListenerRecord::HdmiRecordListenerRecord(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IIHdmiRecordListener* listener)
    : mHost(host)
    , mListener(listener)
{}

ECode HdmiControlService::HdmiRecordListenerRecord::ProxyDied()
{
    synchronized(mHost->mLock) {
        mHost->mRecordListenerRecord = NULL;
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::BinderService
//=============================================================================
CAR_INTERFACE_IMPL_2(HdmiControlService::BinderService, Object, IIHdmiControlService, IBinder)

HdmiControlService::BinderService::BinderService()
{}

ECode HdmiControlService::BinderService::constructor(
    /* [in] */ IHdmiControlService* host)
{
    mHost = (HdmiControlService*)host;
    return NOERROR;
}

ECode HdmiControlService::BinderService::GetSupportedTypes(
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    mHost->EnforceAccessPermission();
    // mLocalDevices is an unmodifiable list - no lock necesary.
    Int32 size;
    mHost->mLocalDevices->GetSize(&size);
    AutoPtr<ArrayOf<Int32> > localDevices = ArrayOf<Int32>::Alloc(size);
    for (Int32 i = 0; i < localDevices->GetLength(); ++i) {
        AutoPtr<IInterface> obj;
        mHost->mLocalDevices->Get(i, (IInterface**)&obj);
        Int32 iDevice;
        IInteger32::Probe(obj)->GetValue(&iDevice);
        localDevices->Set(i, iDevice);
    }
    *result = localDevices;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiControlService::BinderService::GetActiveSource(
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    mHost->EnforceAccessPermission();
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        Slogger::W(TAG, "Local tv device not available");
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IHdmiCecLocalDeviceActiveSource> activeSource;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetActiveSource((IHdmiCecLocalDeviceActiveSource**)&activeSource);
    Boolean isValid;
    activeSource->IsValid(&isValid);
    if (isValid) {
        // TODO: Waiting for CHdmiDeviceInfo
        assert(0);
        // return CHdmiDeviceInfo::New(activeSource->mLogicalAddress,
        //         activeSource->mPhysicalAddress, IHdmiDeviceInfo::PORT_INVALID,
        //         IHdmiDeviceInfo::DEVICE_INACTIVE, 0, String(""), result);
    }
    Int32 activePath;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetActivePath(&activePath);
    if (activePath != IHdmiDeviceInfo::PATH_INVALID) {
        Int32 activePortId;
        ((HdmiCecLocalDeviceTv*) tv.Get())->GetActivePortId(&activePortId);
        // TODO: Waiting for CHdmiDeviceInfo
        assert(0);
        // return CHdmiDeviceInfo::New(activePath, activePortId, result);
    }
    return NOERROR;
}

ECode HdmiControlService::BinderService::DeviceSelect(
    /* [in] */ Int32 deviceId,
    /* [in] */ IIHdmiControlCallback* callback)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new DeviceSelectRunnable(this->mHost, deviceId, callback));
    return NOERROR;
}

ECode HdmiControlService::BinderService::PortSelect(
    /* [in] */ Int32 portId,
    /* [in] */ IIHdmiControlCallback* callback)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new PortSelectRunnable(this->mHost, portId, callback));
    return NOERROR;
}

ECode HdmiControlService::BinderService::SendKeyEvent(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isPressed)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new SendKeyEventRunnable(this->mHost, deviceType, keyCode, isPressed));
    return NOERROR;
}

ECode HdmiControlService::BinderService::OneTouchPlay(
    /* [in] */ IIHdmiControlCallback* callback)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new OneTouchPlayRunnable(this->mHost, callback));
    return NOERROR;
}

ECode HdmiControlService::BinderService::QueryDisplayStatus(
    /* [in] */ IIHdmiControlCallback* callback)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new QueryDisplayStatusRunnable(this->mHost, callback));
    return NOERROR;
}

ECode HdmiControlService::BinderService::AddHotplugEventListener(
    /* [in] */ IIHdmiHotplugEventListener* listener)
{
    mHost->EnforceAccessPermission();
    mHost->AddHotplugEventListener(listener);
    return NOERROR;
}

ECode HdmiControlService::BinderService::RemoveHotplugEventListener(
    /* [in] */ IIHdmiHotplugEventListener* listener)
{
    mHost->EnforceAccessPermission();
    mHost->RemoveHotplugEventListener(listener);
    return NOERROR;
}

ECode HdmiControlService::BinderService::AddDeviceEventListener(
    /* [in] */ IIHdmiDeviceEventListener* listener)
{
    mHost->EnforceAccessPermission();
    mHost->AddDeviceEventListener(listener);
    return NOERROR;
}

ECode HdmiControlService::BinderService::GetPortInfo(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    mHost->EnforceAccessPermission();
    return mHost->GetPortInfo(result);
}

ECode HdmiControlService::BinderService::CanChangeSystemAudioMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mHost->EnforceAccessPermission();
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    return ((HdmiCecLocalDeviceTv*) tv.Get())->HasSystemAudioDevice(result);
}

ECode HdmiControlService::BinderService::GetSystemAudioMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mHost->EnforceAccessPermission();
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Boolean isSystemAudioActivated;
    ((HdmiCecLocalDeviceTv*) tv.Get())->IsSystemAudioActivated(&isSystemAudioActivated);
    *result = isSystemAudioActivated;
    return NOERROR;
}

ECode HdmiControlService::BinderService::SetSystemAudioMode(
    /* [in] */ Boolean enabled,
    /* [in] */ IIHdmiControlCallback* callback)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new SetSystemAudioModeRunnable(this->mHost, enabled, callback));
    return NOERROR;
}

ECode HdmiControlService::BinderService::AddSystemAudioModeChangeListener(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
{
    mHost->EnforceAccessPermission();
    mHost->AddSystemAudioModeChangeListner(listener);
    return NOERROR;
}

ECode HdmiControlService::BinderService::RemoveSystemAudioModeChangeListener(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
{
    mHost->EnforceAccessPermission();
    mHost->RemoveSystemAudioModeChangeListener(listener);
    return NOERROR;
}

ECode HdmiControlService::BinderService::SetInputChangeListener(
    /* [in] */ IIHdmiInputChangeListener* listener)
{
    mHost->EnforceAccessPermission();
    mHost->SetInputChangeListener(listener);
    return NOERROR;
}

ECode HdmiControlService::BinderService::GetInputDevices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    mHost->EnforceAccessPermission();
    // No need to hold the lock for obtaining TV device as the local device instance
    // is preserved while the HDMI control is enabled.
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    synchronized(mHost->mLock) {
        AutoPtr<IList> cecDevices;
        if (tv == NULL) {
            AutoPtr<ICollections> helper;
            CCollections::AcquireSingleton((ICollections**)&helper);
            helper->GetEmptyList((IList**)&cecDevices);
        }
        else {
            ((HdmiCecLocalDeviceTv*) tv.Get())->GetSafeExternalInputsLocked((IList**)&cecDevices);
        }
        AutoPtr<IList> mhlDevicesLocked, temp;
        mHost->GetMhlDevicesLocked((IList**)&mhlDevicesLocked);
        temp = HdmiUtils::MergeToUnmodifiableList(cecDevices, mhlDevicesLocked);
        *result = temp;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode HdmiControlService::BinderService::SetSystemAudioVolume(
    /* [in] */ Int32 oldIndex,
    /* [in] */ Int32 newIndex,
    /* [in] */ Int32 maxIndex)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new SetSystemAudioVolumeRunnable(this->mHost, oldIndex, newIndex, maxIndex));
    return NOERROR;
}

ECode HdmiControlService::BinderService::SetSystemAudioMute(
    /* [in] */ Boolean mute)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new SetSystemAudioMuteRunnable(this->mHost, mute));
    return NOERROR;
}

ECode HdmiControlService::BinderService::SetArcMode(
    /* [in] */ Boolean enabled)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new SetArcModeRunnable(this->mHost));
    return NOERROR;
}

ECode HdmiControlService::BinderService::SetProhibitMode(
    /* [in] */ Boolean enabled)
{
    mHost->EnforceAccessPermission();
    Boolean isTvDevice;
    mHost->IsTvDevice(&isTvDevice);
    if (!isTvDevice) {
        return NOERROR;
    }
    mHost->SetProhibitMode(enabled);
    return NOERROR;
}

ECode HdmiControlService::BinderService::AddVendorCommandListener(
    /* [in] */ IIHdmiVendorCommandListener* listener,
    /* [in] */ Int32 deviceType)
{
    mHost->EnforceAccessPermission();
    mHost->AddVendorCommandListener(listener, deviceType);
    return NOERROR;
}

ECode HdmiControlService::BinderService::SendVendorCommand(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 targetAddress,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Boolean hasVendorId)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new SendVendorCommandRunnable(this->mHost, deviceType, targetAddress, params, hasVendorId));
    return NOERROR;
}

ECode HdmiControlService::BinderService::SendStandby(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 deviceId)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new SendStandbyRunnable(this->mHost, deviceType, deviceId));
    return NOERROR;
}

ECode HdmiControlService::BinderService::SetHdmiRecordListener(
    /* [in] */ IIHdmiRecordListener* listener)
{
    mHost->EnforceAccessPermission();
    mHost->SetHdmiRecordListener(listener);
    return NOERROR;
}

ECode HdmiControlService::BinderService::StartOneTouchRecord(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new StartOneTouchRecordRunnable(this->mHost, recorderAddress, recordSource));
    return NOERROR;
}

ECode HdmiControlService::BinderService::StopOneTouchRecord(
    /* [in] */ Int32 recorderAddress)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new StopOneTouchRecordRunnable(this->mHost, recorderAddress));
    return NOERROR;
}

ECode HdmiControlService::BinderService::StartTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new StartTimerRecordingRunnable(this->mHost, recorderAddress, sourceType, recordSource));
    return NOERROR;
}

ECode HdmiControlService::BinderService::ClearTimerRecording(
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new ClearTimerRecordingRunnable(this->mHost, recorderAddress, sourceType, recordSource));
    return NOERROR;
}

ECode HdmiControlService::BinderService::SendMhlVendorCommand(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data)
{
    mHost->EnforceAccessPermission();
    mHost->RunOnServiceThread(new SendMhlVendorCommandRunnable(this->mHost, portId, offset, length, data));
    return NOERROR;
}

ECode HdmiControlService::BinderService::AddHdmiMhlVendorCommandListener(
    /* [in] */ IIHdmiMhlVendorCommandListener* listener)
{
    mHost->EnforceAccessPermission();
    mHost->AddHdmiMhlVendorCommandListener(listener);
    return NOERROR;
}

ECode HdmiControlService::BinderService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    context->EnforceCallingOrSelfPermission(Droid::Manifest::permission::DUMP, TAG);
    AutoPtr<IIndentingPrintWriter> pw;
    CIndentingPrintWriter::New(IWriter::Probe(writer), String("  "), (IIndentingPrintWriter**)&pw);
    String s("mHdmiControlEnabled: ");
    s += mHost->mHdmiControlEnabled ? "true" : "false";
    IPrintWriter::Probe(pw)->Println(s);
    s = String("mProhibitMode: ");
    s += mHost->mProhibitMode ? "true" : "false";
    IPrintWriter::Probe(pw)->Println(s);
    if (mHost->mCecController != NULL) {
        IPrintWriter::Probe(pw)->Println(String("mCecController: "));
        pw->IncreaseIndent();
        mHost->mCecController->Dump(pw);
        pw->DecreaseIndent();
    }
    IPrintWriter::Probe(pw)->Println(String("mPortInfo: "));
    pw->IncreaseIndent();
    FOR_EACH(it, mHost->mPortInfo) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiPortInfo> hdmiPortInfo = IHdmiPortInfo::Probe(obj);
        s = String("- ");
        s.Append(TO_STR(hdmiPortInfo));
        IPrintWriter::Probe(pw)->Println(s);
    }
    pw->DecreaseIndent();
    s = String("mPowerStatus: ");
    s.AppendFormat("%d", mHost->mPowerStatus);
    IPrintWriter::Probe(pw)->Println(s);
    return NOERROR;
}

ECode HdmiControlService::BinderService::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// HdmiControlService::InputChangeListenerRecord
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::InputChangeListenerRecord, Object, IProxyDeathRecipient)

HdmiControlService::InputChangeListenerRecord::InputChangeListenerRecord(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IIHdmiInputChangeListener* listener)
    : mHost(host)
    , mListener(listener)
{}

ECode HdmiControlService::InputChangeListenerRecord::ProxyDied()
{
    synchronized(mHost->mLock) {
        mHost->mInputChangeListenerRecord = NULL;
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::DeviceSelectRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::DeviceSelectRunnable, Object, IRunnable)

HdmiControlService::DeviceSelectRunnable::DeviceSelectRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 deviceId,
    /* [in] */ IIHdmiControlCallback* callback)
    : mHost(host)
    , mDeviceId(deviceId)
    , mCallback(callback)
{}

ECode HdmiControlService::DeviceSelectRunnable::Run()
{
    if (mCallback == NULL) {
        Slogger::E(TAG, "Callback cannot be null");
        return NOERROR;
    }
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        Slogger::W(TAG, "Local tv device not available");
        mHost->InvokeCallback(mCallback, IHdmiControlManager::RESULT_SOURCE_NOT_AVAILABLE);
        return NOERROR;
    }
    AutoPtr<HdmiMhlLocalDeviceStub> device;
    mHost->mMhlController->GetLocalDeviceById(mDeviceId, (HdmiMhlLocalDeviceStub**)&device);
    if (device != NULL) {
        Int32 portId;
        device->GetPortId(&portId);
        Int32 activePortId;
        ((HdmiCecLocalDeviceTv*) tv.Get())->GetActivePortId(&activePortId);
        if (portId == activePortId) {
            mHost->InvokeCallback(mCallback, IHdmiControlManager::RESULT_SUCCESS);
            return NOERROR;
        }
        // Upon selecting MHL device, we send RAP[Content On] to wake up
        // the connected mobile device, start routing control to switch ports.
        // callback is handled by MHL action.
        device->TurnOn(mCallback);
        ((HdmiCecLocalDeviceTv*) tv.Get())->DoManualPortSwitching(portId, NULL);
        return NOERROR;
    }
    ((HdmiCecLocalDeviceTv*) tv.Get())->DeviceSelect(mDeviceId, mCallback);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::PortSelectRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::PortSelectRunnable, Object, IRunnable)

HdmiControlService::PortSelectRunnable::PortSelectRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 portId,
    /* [in] */ IIHdmiControlCallback* callback)
    : mHost(host)
    , mPortId(portId)
    , mCallback(callback)
{}

ECode HdmiControlService::PortSelectRunnable::Run()
{
    if (mCallback == NULL) {
        Slogger::E(TAG, "Callback cannot be null");
        return NOERROR;
    }
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        Slogger::W(TAG, "Local tv device not available");
        mHost->InvokeCallback(mCallback, IHdmiControlManager::RESULT_SOURCE_NOT_AVAILABLE);
        return NOERROR;
    }
    ((HdmiCecLocalDeviceTv*) tv.Get())->DoManualPortSwitching(mPortId, mCallback);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SendKeyEventRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SendKeyEventRunnable, Object, IRunnable)

HdmiControlService::SendKeyEventRunnable::SendKeyEventRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 keyCode,
    /* [in] */ Boolean isPressed)
    : mHost(host)
    , mDeviceType(deviceType)
    , mKeyCode(keyCode)
    , mIsPressed(isPressed)
{}

ECode HdmiControlService::SendKeyEventRunnable::Run()
{
    AutoPtr<HdmiMhlLocalDeviceStub> device;
    mHost->mMhlController->GetLocalDevice(mHost->mActivePortId, (HdmiMhlLocalDeviceStub**)&device);
    if (device != NULL) {
        device->SendKeyEvent(mKeyCode, mIsPressed);
        return NOERROR;
    }
    if (mHost->mCecController != NULL) {
        AutoPtr<IHdmiCecLocalDevice> localDevice;
        mHost->mCecController->GetLocalDevice(mDeviceType, (IHdmiCecLocalDevice**)&localDevice);
        if (localDevice == NULL) {
            Slogger::W(TAG, "Local device not available");
            return NOERROR;
        }
        ((HdmiCecLocalDevice*)localDevice.Get())->SendKeyEvent(mKeyCode, mIsPressed);
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::OneTouchPlayRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::OneTouchPlayRunnable, Object, IRunnable)

HdmiControlService::OneTouchPlayRunnable::OneTouchPlayRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IIHdmiControlCallback* callback)
    : mHost(host)
    , mCallback(callback)
{}

ECode HdmiControlService::OneTouchPlayRunnable::Run()
{
    return mHost->OneTouchPlay(mCallback);
}

//=============================================================================
// HdmiControlService::QueryDisplayStatusRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::QueryDisplayStatusRunnable, Object, IRunnable)

HdmiControlService::QueryDisplayStatusRunnable::QueryDisplayStatusRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IIHdmiControlCallback* callback)
    : mHost(host)
    , mCallback(callback)
{}

ECode HdmiControlService::QueryDisplayStatusRunnable::Run()
{
    return mHost->QueryDisplayStatus(mCallback);
}

//=============================================================================
// HdmiControlService::SetSystemAudioModeRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SetSystemAudioModeRunnable, Object, IRunnable)

HdmiControlService::SetSystemAudioModeRunnable::SetSystemAudioModeRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Boolean enabled,
    /* [in] */ IIHdmiControlCallback* callback)
    : mHost(host)
    , mEnabled(enabled)
    , mCallback(callback)
{}

ECode HdmiControlService::SetSystemAudioModeRunnable::Run()
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        Slogger::W(TAG, "Local tv device not available");
        mHost->InvokeCallback(mCallback, IHdmiControlManager::RESULT_SOURCE_NOT_AVAILABLE);
        return NOERROR;
    }
    ((HdmiCecLocalDeviceTv*) tv.Get())->ChangeSystemAudioMode(mEnabled, mCallback);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SetSystemAudioVolumeRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SetSystemAudioVolumeRunnable, Object, IRunnable)

HdmiControlService::SetSystemAudioVolumeRunnable::SetSystemAudioVolumeRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 oldIndex,
    /* [in] */ Int32 newIndex,
    /* [in] */ Int32 maxIndex)
    : mHost(host)
    , mOldIndex(oldIndex)
    , mNewIndex(newIndex)
    , mMaxIndex(maxIndex)
{}

ECode HdmiControlService::SetSystemAudioVolumeRunnable::Run()
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        Slogger::W(TAG, "Local tv device not available");
        return NOERROR;
    }
    ((HdmiCecLocalDeviceTv*) tv.Get())->ChangeVolume(mOldIndex, mNewIndex - mOldIndex, mMaxIndex);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SetSystemAudioMuteRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SetSystemAudioMuteRunnable, Object, IRunnable)

HdmiControlService::SetSystemAudioMuteRunnable::SetSystemAudioMuteRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Boolean mute)
    : mHost(host)
    , mMute(mute)
{}

ECode HdmiControlService::SetSystemAudioMuteRunnable::Run()
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        Slogger::W(TAG, "Local tv device not available");
        return NOERROR;
    }
    ((HdmiCecLocalDeviceTv*) tv.Get())->ChangeMute(mMute);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SetArcModeRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SetArcModeRunnable, Object, IRunnable)

HdmiControlService::SetArcModeRunnable::SetArcModeRunnable(
    /* [in] */ HdmiControlService* host)
    : mHost(host)
{}

ECode HdmiControlService::SetArcModeRunnable::Run()
{
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        Slogger::W(TAG, "Local tv device not available to change arc mode.");
        return NOERROR;
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SendVendorCommandRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SendVendorCommandRunnable, Object, IRunnable)

HdmiControlService::SendVendorCommandRunnable::SendVendorCommandRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 targetAddress,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Boolean hasVendorId)
    : mHost(host)
    , mDeviceType(deviceType)
    , mTargetAddress(targetAddress)
    , mParams(params)
    , mHasVendorId(hasVendorId)
{}

ECode HdmiControlService::SendVendorCommandRunnable::Run()
{
    AutoPtr<IHdmiCecLocalDevice> device;
    mHost->mCecController->GetLocalDevice(mDeviceType, (IHdmiCecLocalDevice**)&device);
    if (device == NULL) {
        Slogger::W(TAG, "Local device not available");
        return NOERROR;
    }
    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    ((HdmiCecLocalDevice*)device.Get())->GetDeviceInfo((IHdmiDeviceInfo**)&deviceInfo);
    if (mHasVendorId) {
        Int32 logicalAddr;
        deviceInfo->GetLogicalAddress(&logicalAddr);
        Int32 vendorId;
        mHost->GetVendorId(&vendorId);
        AutoPtr<IHdmiCecMessage> cmd;
        HdmiCecMessageBuilder::BuildVendorCommandWithId(
                logicalAddr, mTargetAddress,
                vendorId, mParams, (IHdmiCecMessage**)&cmd);
        mHost->SendCecCommand(cmd);
    } else {
        AutoPtr<IHdmiCecMessage> cmd;
        Int32 logicalAddress;
        deviceInfo->GetLogicalAddress(&logicalAddress);
        HdmiCecMessageBuilder::BuildVendorCommand(logicalAddress, mTargetAddress, mParams, (IHdmiCecMessage**)&cmd);
        mHost->SendCecCommand(cmd);
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SendStandbyRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SendStandbyRunnable, Object, IRunnable)

HdmiControlService::SendStandbyRunnable::SendStandbyRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 deviceId)
    : mHost(host)
    , mDeviceType(deviceType)
    , mDeviceId(deviceId)
{}

ECode HdmiControlService::SendStandbyRunnable::Run()
{
    AutoPtr<IHdmiCecLocalDevice> device;
    mHost->mCecController->GetLocalDevice(mDeviceType, (IHdmiCecLocalDevice**)&device);
    if (device == NULL) {
        Slogger::W(TAG, "Local device not available");
        return NOERROR;
    }
    ((HdmiCecLocalDevice*) device.Get())->SendStandby(mDeviceId);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::StartOneTouchRecordRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::StartOneTouchRecordRunnable, Object, IRunnable)

HdmiControlService::StartOneTouchRecordRunnable::StartOneTouchRecordRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 recorderAddress,
    /* [in] */ ArrayOf<Byte>* recordSource)
    : mHost(host)
    , mRecorderAddress(recorderAddress)
    , mRecordSource(recordSource)
{}

ECode HdmiControlService::StartOneTouchRecordRunnable::Run()
{
    Boolean isTvDevice;
    mHost->IsTvDevice(&isTvDevice);
    if (!isTvDevice) {
        Slogger::W(TAG, "No TV is available.");
        return NOERROR;
    }
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    Int32 iNoUse;
    ((HdmiCecLocalDeviceTv*) tv.Get())->StartOneTouchRecord(mRecorderAddress, mRecordSource, &iNoUse);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::StopOneTouchRecordRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::StopOneTouchRecordRunnable, Object, IRunnable)

HdmiControlService::StopOneTouchRecordRunnable::StopOneTouchRecordRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 recorderAddress)
    : mHost(host)
    , mRecorderAddress(recorderAddress)
{}

ECode HdmiControlService::StopOneTouchRecordRunnable::Run()
{
    Boolean isTvDevice;
    mHost->IsTvDevice(&isTvDevice);
    if (!isTvDevice) {
        Slogger::W(TAG, "No TV is available.");
        return NOERROR;
    }
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->StopOneTouchRecord(mRecorderAddress);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::StartTimerRecordingRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::StartTimerRecordingRunnable, Object, IRunnable)

HdmiControlService::StartTimerRecordingRunnable::StartTimerRecordingRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
    : mHost(host)
    , mRecorderAddress(recorderAddress)
    , mSourceType(sourceType)
    , mRecordSource(recordSource)
{}

ECode HdmiControlService::StartTimerRecordingRunnable::Run()
{
    Boolean isTvDevice;
    mHost->IsTvDevice(&isTvDevice);
    if (!isTvDevice) {
        Slogger::W(TAG, "No TV is available.");
        return NOERROR;
    }
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->StartTimerRecording(mRecorderAddress, mSourceType, mRecordSource);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::ClearTimerRecordingRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::ClearTimerRecordingRunnable, Object, IRunnable)

HdmiControlService::ClearTimerRecordingRunnable::ClearTimerRecordingRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 recorderAddress,
    /* [in] */ Int32 sourceType,
    /* [in] */ ArrayOf<Byte>* recordSource)
    : mHost(host)
    , mRecorderAddress(recorderAddress)
    , mSourceType(sourceType)
    , mRecordSource(recordSource)
{}

ECode HdmiControlService::ClearTimerRecordingRunnable::Run()
{
    Boolean isTvDevice;
    mHost->IsTvDevice(&isTvDevice);
    if (!isTvDevice) {
        Slogger::W(TAG, "No TV is available.");
        return NOERROR;
    }
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    mHost->Tv((IHdmiCecLocalDeviceTv**)&tv);
    ((HdmiCecLocalDeviceTv*) tv.Get())->ClearTimerRecording(mRecorderAddress, mSourceType, mRecordSource);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SendMhlVendorCommandRunnable
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SendMhlVendorCommandRunnable, Object, IRunnable)

HdmiControlService::SendMhlVendorCommandRunnable::SendMhlVendorCommandRunnable(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 portId,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data)
    : mHost(host)
    , mPortId(portId)
    , mOffset(offset)
    , mLength(length)
    , mData(data)
{}

ECode HdmiControlService::SendMhlVendorCommandRunnable::Run()
{
    Boolean isControlEnabled;
    mHost->IsControlEnabled(&isControlEnabled);
    if (!isControlEnabled) {
        Slogger::W(TAG, "Hdmi control is disabled.");
        return NOERROR;
    }
    AutoPtr<HdmiMhlLocalDeviceStub> device;
    mHost->mMhlController->GetLocalDevice(mPortId, (HdmiMhlLocalDeviceStub**)&device);
    if (device == NULL) {
        Slogger::W(TAG, "Invalid port id:%d", mPortId);
        return NOERROR;
    }
    mHost->mMhlController->SendVendorCommand(mPortId, mOffset, mLength, mData);
    return NOERROR;
}

//=============================================================================
// HdmiControlService::InnerSub_AllocateAddressCallback
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::InnerSub_AllocateAddressCallback, Object, IHdmiCecControllerAllocateAddressCallback)

HdmiControlService::InnerSub_AllocateAddressCallback::InnerSub_AllocateAddressCallback(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IHdmiCecLocalDevice* localDevice,
    /* [in] */ IArrayList* allocatingDevices,
    /* [in] */ IArrayList* allocatedDevices,
    /* [in] */ ArrayOf<Int32>* finished,
    /* [in] */ Int32 initiatedBy)
    : mHost(host)
    , mLocalDevice(localDevice)
    , mAllocatingDevices(allocatingDevices)
    , mAllocatedDevices(allocatedDevices)
    , mFinished(finished)
    , mInitiatedBy(initiatedBy)
{}

ECode HdmiControlService::InnerSub_AllocateAddressCallback::OnAllocated(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 logicalAddress)
{
    if (logicalAddress == Constants::ADDR_UNREGISTERED) {
        Slogger::E(TAG, "Failed to allocate address:[device_type:%d]", deviceType);
    } else {
        // Set POWER_STATUS_ON to all local devices because they share lifetime
        // with system.
        AutoPtr<IHdmiDeviceInfo> deviceInfo;
        mHost->CreateDeviceInfo(logicalAddress, deviceType,
                IHdmiControlManager::POWER_STATUS_ON, (IHdmiDeviceInfo**)&deviceInfo);
        ((HdmiCecLocalDevice*)mLocalDevice.Get())->SetDeviceInfo(deviceInfo);
        mHost->mCecController->AddLocalDevice(deviceType, mLocalDevice);
        Int32 iNoUse;
        mHost->mCecController->AddLogicalAddress(logicalAddress, &iNoUse);
        mAllocatedDevices->Add(mLocalDevice);
    }
    // Address allocation completed for all devices. Notify each device.
    Int32 size;
    mAllocatingDevices->GetSize(&size);
    if (size == ++(*mFinished)[0]) {
        if (mInitiatedBy != INITIATED_BY_HOTPLUG) {
            // In case of the hotplug we don't call onInitializeCecComplete()
            // since we reallocate the logical address only.
            mHost->OnInitializeCecComplete();
        }
        mHost->NotifyAddressAllocated(mAllocatedDevices, mInitiatedBy);
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::OnStandbyPendingActionClearedCallback
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::OnStandbyPendingActionClearedCallback, Object, IHdmiCecLocalDevicePendingActionClearedCallback)

HdmiControlService::OnStandbyPendingActionClearedCallback::OnStandbyPendingActionClearedCallback(
    /* [in] */ HdmiControlService* host,
    /* [in] */ IList* devices)
    : mHost(host)
    , mDevices(devices)
{}

ECode HdmiControlService::OnStandbyPendingActionClearedCallback::OnCleared(
    /* [in] */ IHdmiCecLocalDevice* device)
{
    Slogger::V(TAG, "On standby-action cleared:%d", ((HdmiCecLocalDevice*)device)->mDeviceType);
    mDevices->Remove(device);
    Boolean isEmpty;
    mDevices->IsEmpty(&isEmpty);
    if (isEmpty) {
        mHost->OnStandbyCompleted();
        // We will not clear local devices here, since some OEM/SOC will keep passing
        // the received packets until the application processor enters to the sleep
        // actually.
    }
    return NOERROR;
}

//=============================================================================
// HdmiControlService::SetControlEnabledPendingActionClearedCallback
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService::SetControlEnabledPendingActionClearedCallback, Object, IHdmiCecLocalDevicePendingActionClearedCallback)

HdmiControlService::SetControlEnabledPendingActionClearedCallback::SetControlEnabledPendingActionClearedCallback(
    /* [in] */ HdmiControlService* host)
    : mHost(host)
{}

ECode HdmiControlService::SetControlEnabledPendingActionClearedCallback::OnCleared(
    /* [in] */ IHdmiCecLocalDevice* device)
{
    mHost->AssertRunOnServiceThread();
    mHost->ClearLocalDevices();
    return NOERROR;
}

//=============================================================================
// HdmiControlService::InnerSub_IHdmiControlCallbackStub
//=============================================================================
CAR_INTERFACE_IMPL_2(HdmiControlService::InnerSub_IHdmiControlCallbackStub, Object, IBinder, IIHdmiControlCallback)

HdmiControlService::InnerSub_IHdmiControlCallbackStub::InnerSub_IHdmiControlCallbackStub(
    /* [in] */ HdmiControlService* host,
    /* [in] */ Int32 lastInput)
    : mHost(host)
    , mLastInput(lastInput)
{}

ECode HdmiControlService::InnerSub_IHdmiControlCallbackStub::OnComplete(
    /* [in] */ Int32 result)
{
    // Keep the last input to switch back later when RAP[ContentOff] is received.
    // This effectively sets the port to invalid one if the switching is for
    // RAP[ContentOff].
    return mHost->SetLastInputForMhl(mLastInput);
}

ECode HdmiControlService::InnerSub_IHdmiControlCallbackStub::ToString(
    /* [out] */ String* result)
{
    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// HdmiControlService
//=============================================================================
CAR_INTERFACE_IMPL(HdmiControlService, SystemService, IHdmiControlService)

const String HdmiControlService::TAG("HdmiControlService");
const String HdmiControlService::PERMISSION("android.permission.HDMI_CEC");
const Int32 HdmiControlService::INITIATED_BY_ENABLE_CEC = 0;
const Int32 HdmiControlService::INITIATED_BY_BOOT_UP = 1;
const Int32 HdmiControlService::INITIATED_BY_SCREEN_ON = 2;
const Int32 HdmiControlService::INITIATED_BY_WAKE_UP_MESSAGE = 3;
const Int32 HdmiControlService::INITIATED_BY_HOTPLUG = 4;

HdmiControlService::HdmiControlService()
    : mHdmiControlEnabled(FALSE)
    , mProhibitMode(FALSE)
    , mPowerStatus(IHdmiControlManager::POWER_STATUS_STANDBY)
    , mStandbyMessageReceived(FALSE)
    , mWakeUpMessageReceived(FALSE)
    , mActivePortId(Constants::INVALID_PORT_ID)
    , mMhlInputChangeEnabled(FALSE)
    , mLastInputMhl(Constants::INVALID_PORT_ID)
{
    CHandlerThread::New(String("Hdmi Control Io Thread"), (IHandlerThread**)&mIoThread);
    Elastos::Core::CObject::New((IObject**)&mLock);
    CArrayList::New((IArrayList**)&mDeviceEventListenerRecords);
    CArrayList::New((IArrayList**)&mVendorCommandListenerRecords);
    CArrayList::New((IArrayList**)&mSystemAudioModeChangeListenerRecords);
    CHandler::New((IHandler**)&mHandler);
    mHdmiControlBroadcastReceiver = new HdmiControlBroadcastReceiver(this);
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> defaultLocale;
    helper->GetDefault((ILocale**)&defaultLocale);
    defaultLocale->GetISO3Language(&mLanguage);
    CArrayList::New((IArrayList**)&mMhlVendorCommandListenerRecords);
    CArrayList::New((IArrayList**)&mHotplugEventListenerRecords);
    return;
}

ECode HdmiControlService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    AutoPtr<ISystemProperties> helper;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&helper);
    String type;
    helper->Get(Constants::PROPERTY_DEVICE_TYPE, &type);
    GetInt32List(type, (IList**)&mLocalDevices);
    mSettingsObserver = new SettingsObserver(this, mHandler);
    return NOERROR;
}

ECode HdmiControlService::GetInt32List(
    /* [in] */ const String& string,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IArrayList> list;
    CArrayList::New((IArrayList**)&list);
    AutoPtr<ISimpleStringSplitter> splitter;
    CSimpleStringSplitter::New(',', (ISimpleStringSplitter**)&splitter);
    splitter->SetString(string);
    AutoPtr<IIterator> it;
    splitter->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        String item;
        ICharSequence::Probe(obj)->ToString(&item);
        // try {
        ECode ec;
        do {
            Int32 i32;
            ec = StringUtils::Parse(item, &i32);
            if (FAILED(ec)) break;
            AutoPtr<IInteger32> integer32;
            CInteger32::New(i32, (IInteger32**)&integer32);
            ec = list->Add(integer32);
        } while(FALSE);
        // } catch (NumberFormatException e) {
        if (FAILED(ec)) {
            if ((ECode) E_NUMBER_FORMAT_EXCEPTION == ec) {
                Slogger::W(TAG, "Can't parseInt: %s", item.string());
            }
            else
                return ec;
        }
        // }
    }
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    return helper->UnmodifiableList(IList::Probe(list), result);
}

ECode HdmiControlService::OnStart()
{
    IThread::Probe(mIoThread)->Start();
    mPowerStatus = IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_ON;
    mProhibitMode = FALSE;
    ReadBooleanSetting(ISettingsGlobal::HDMI_CONTROL_ENABLED, TRUE, &mHdmiControlEnabled);
    ReadBooleanSetting(ISettingsGlobal::MHL_INPUT_SWITCHING_ENABLED, TRUE, &mMhlInputChangeEnabled);
    HdmiCecController::Create(this, (HdmiCecController**)&mCecController);
    if (mCecController != NULL) {
        // TODO: Remove this as soon as OEM's HAL implementation is corrected.
        mCecController->SetOption(Constants::OPTION_CEC_ENABLE, Constants::ENABLED);
        // TODO: load value for mHdmiControlEnabled from preference.
        if (mHdmiControlEnabled) {
            InitializeCec(INITIATED_BY_BOOT_UP);
        }
    } else {
        Slogger::I(TAG, "Device does not support HDMI-CEC.");
    }
    HdmiMhlControllerStub::Create(this, (HdmiMhlControllerStub**)&mMhlController);
    Boolean isReady;
    mMhlController->IsReady(&isReady);
    if (!isReady) {
        Slogger::I(TAG, "Device does not support MHL-control.");
    }
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->GetEmptyList((IList**)&mMhlDevices);
    InitPortInfo();
    mMessageValidator = new HdmiCecMessageValidator();
    mMessageValidator->constructor(this);
    AutoPtr<BinderService> binderService = new BinderService();
    binderService->constructor(this);
    PublishBinderService(IContext::HDMI_CONTROL_SERVICE, binderService);
    // Register broadcast receiver for power state change.
    if (mCecController != NULL) {
        AutoPtr<IIntentFilter> filter;
        CIntentFilter::New((IIntentFilter**)&filter);
        filter->AddAction(IIntent::ACTION_SCREEN_OFF);
        filter->AddAction(IIntent::ACTION_SCREEN_ON);
        filter->AddAction(IIntent::ACTION_CONFIGURATION_CHANGED);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IIntent> iNoUse;
        context->RegisterReceiver(mHdmiControlBroadcastReceiver, filter, (IIntent**)&iNoUse);
    }
    return NOERROR;
}

ECode HdmiControlService::OnInitializeCecComplete()
{
    if (mPowerStatus == IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_ON) {
        mPowerStatus = IHdmiControlManager::POWER_STATUS_ON;
    }
    mWakeUpMessageReceived = FALSE;
    Boolean isTvDevice;
    IsTvDevice(&isTvDevice);
    if (isTvDevice) {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        Boolean isAutoWakeup;
        ((HdmiCecLocalDeviceTv*) tv.Get())->GetAutoWakeup(&isAutoWakeup);
        Int32 iAutoWakeup;
        ToInt32(isAutoWakeup, &iAutoWakeup);
        mCecController->SetOption(Constants::OPTION_CEC_AUTO_WAKEUP, iAutoWakeup);
        RegisterContentObserver();
    }
    return NOERROR;
}

ECode HdmiControlService::RegisterContentObserver()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ArrayOf<String> > settings = ArrayOf<String>::Alloc(5);
    settings->Set(0, ISettingsGlobal::HDMI_CONTROL_ENABLED);
    settings->Set(1, ISettingsGlobal::HDMI_CONTROL_AUTO_WAKEUP_ENABLED);
    settings->Set(2, ISettingsGlobal::HDMI_CONTROL_AUTO_DEVICE_OFF_ENABLED);
    settings->Set(3, ISettingsGlobal::MHL_INPUT_SWITCHING_ENABLED);
    settings->Set(4, ISettingsGlobal::MHL_POWER_CHARGE_ENABLED);
    AutoPtr<ISettingsGlobal> helper;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&helper);
    for (Int32 i = 0; i < settings->GetLength(); ++i) {
        String s = (*settings)[i];
        AutoPtr<IUri> uri;
        helper->GetUriFor(s, (IUri**)&uri);
        resolver->RegisterContentObserver(uri, FALSE, mSettingsObserver,
                UserHandle::USER_ALL);
    }
    return NOERROR;
}

ECode HdmiControlService::ToInt32(
    /* [in] */ Boolean enabled,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (enabled)
        *result = Constants::ENABLED;
    else
        *result = Constants::DISABLED;
    return NOERROR;
}

ECode HdmiControlService::ReadBooleanSetting(
    /* [in] */ const String& key,
    /* [in] */ Boolean defVal,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 iDefVal;
    ToInt32(defVal, &iDefVal);
    Int32 settings;
    Settings::Global::GetInt32(cr, key, iDefVal, &settings);
    *result = settings == Constants::ENABLED;
    return NOERROR;
}

ECode HdmiControlService::WriteBooleanSetting(
    /* [in] */ const String& key,
    /* [in] */ Boolean value)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    Int32 iValue;
    ToInt32(value, &iValue);
    Boolean bNoUse;
    Settings::Global::PutInt32(cr, key, iValue, &bNoUse);
    return NOERROR;
}

ECode HdmiControlService::UnregisterSettingsObserver()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    cr->UnregisterContentObserver(mSettingsObserver);
    return NOERROR;
}

ECode HdmiControlService::InitializeCec(
    /* [in] */ Int32 initiatedBy)
{
    mCecController->SetOption(Constants::OPTION_CEC_SERVICE_CONTROL, Constants::ENABLED);
    InitializeLocalDevices(initiatedBy);
    return NOERROR;
}

ECode HdmiControlService::InitializeLocalDevices(
    /* [in] */ Int32 initiatedBy)
{
    AssertRunOnServiceThread();
    // A container for [Device type, Local device info].
    AutoPtr<IArrayList> localDevices;
    CArrayList::New((IArrayList**)&localDevices);
    FOR_EACH(it, mLocalDevices) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Int32 type;
        IInteger32::Probe(obj)->GetValue(&type);
        AutoPtr<IHdmiCecLocalDevice> localDevice;
        mCecController->GetLocalDevice(type, (IHdmiCecLocalDevice**)&localDevice);
        if (localDevice == NULL) {
            HdmiCecLocalDevice::Create(this, type, (IHdmiCecLocalDevice**)&localDevice);
        }
        ((HdmiCecLocalDevice*)localDevice.Get())->Init();
        localDevices->Add(localDevice);
    }
    // It's now safe to flush existing local devices from mCecController since they were
    // already moved to 'localDevices'.
    ClearLocalDevices();
    AllocateLogicalAddress(localDevices, initiatedBy);
    return NOERROR;
}

ECode HdmiControlService::AllocateLogicalAddress(
    /* [in] */ IArrayList* allocatingDevices,
    /* [in] */ Int32 initiatedBy)
{
    AssertRunOnServiceThread();
    mCecController->ClearLogicalAddress();
    AutoPtr<IArrayList> allocatedDevices;
    CArrayList::New((IArrayList**)&allocatedDevices);
    AutoPtr<ArrayOf<Int32> > finished = ArrayOf<Int32>::Alloc(1);
    FOR_EACH(it, allocatingDevices) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiCecLocalDevice> localDevice = IHdmiCecLocalDevice::Probe(obj);
        Int32 type;
        ((HdmiCecLocalDevice*)localDevice.Get())->GetType(&type);
        Int32 preferredAddress;
        ((HdmiCecLocalDevice*)localDevice.Get())->GetPreferredAddress(&preferredAddress);
        mCecController->AllocateLogicalAddress(type, preferredAddress, new InnerSub_AllocateAddressCallback(this,
                localDevice, allocatingDevices, allocatedDevices, finished, initiatedBy));
    }
    return NOERROR;
}

ECode HdmiControlService::NotifyAddressAllocated(
    /* [in] */ IArrayList* devices,
    /* [in] */ Int32 initiatedBy)
{
    AssertRunOnServiceThread();
    FOR_EACH(it, devices) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiCecLocalDevice> device = IHdmiCecLocalDevice::Probe(obj);
        AutoPtr<IHdmiDeviceInfo> deviceInfo;
        ((HdmiCecLocalDevice*)device.Get())->GetDeviceInfo((IHdmiDeviceInfo**)&deviceInfo);
        Int32 address;
        deviceInfo->GetLogicalAddress(&address);
        ((HdmiCecLocalDevice*)device.Get())->HandleAddressAllocated(address, initiatedBy);
    }
    return NOERROR;
}

ECode HdmiControlService::InitPortInfo()
{
    AssertRunOnServiceThread();
    AutoPtr<ArrayOf<IHdmiPortInfo*> > cecPortInfo;
    // CEC HAL provides majority of the info while MHL does only MHL support flag for
    // each port. Return empty array if CEC HAL didn't provide the info.
    if (mCecController != NULL) {
        mCecController->GetPortInfos((ArrayOf<IHdmiPortInfo*>**)&cecPortInfo);
    }
    if (cecPortInfo == NULL) {
        return NOERROR;
    }
    AutoPtr<ISparseArray> portInfoMap;
    CSparseArray::New((ISparseArray**)&portInfoMap);
    AutoPtr<ISparseInt32Array> portIdMap;
    CSparseInt32Array::New((ISparseInt32Array**)&portIdMap);
    AutoPtr<ISparseArray> portDeviceMap;
    CSparseArray::New((ISparseArray**)&portDeviceMap);
    for (Int32 i = 0; i < cecPortInfo->GetLength(); ++i) {
        AutoPtr<IHdmiPortInfo> info = (*cecPortInfo)[i];
        Int32 address;
        info->GetAddress(&address);
        Int32 id;
        info->GetId(&id);
        portIdMap->Put(address, id);
        portInfoMap->Put(id, info);
        AutoPtr<IHdmiDeviceInfo> newInfo;
        // TODO: Waiting for CHdmiDeviceInfo
        assert(0);
        // CHdmiDeviceInfo::New(address, id, (IHdmiDeviceInfo**)&newInfo);
        portDeviceMap->Put(id, newInfo);
    }
    mPortIdMap = new UnmodifiableSparseInt32Array();
    mPortIdMap->constructor(portIdMap);
    mPortInfoMap = new UnmodifiableSparseArray();
    mPortInfoMap->constructor(portInfoMap);
    mPortDeviceMap = new UnmodifiableSparseArray();
    mPortDeviceMap->constructor(portDeviceMap);
    AutoPtr<ArrayOf<IHdmiPortInfo*> > mhlPortInfo;
    mMhlController->GetPortInfos((ArrayOf<IHdmiPortInfo*>**)&mhlPortInfo);
    AutoPtr<IArraySet> mhlSupportedPorts;
    CArraySet::New(mhlPortInfo->GetLength(), (IArraySet**)&mhlSupportedPorts);
    for (Int32 i = 0; i < mhlPortInfo->GetLength(); ++i) {
        AutoPtr<IHdmiPortInfo> info = (*mhlPortInfo)[i];
        Boolean isMhlSupported;
        info->IsMhlSupported(&isMhlSupported);
        if (isMhlSupported) {
            Int32 id;
            info->GetId(&id);
            AutoPtr<IInteger32> i32Id;
            CInteger32::New(id, (IInteger32**)&i32Id);
            mhlSupportedPorts->Add(i32Id);
        }
    }
    // Build HDMI port info list with CEC port info plus MHL supported flag. We can just use
    // cec port info if we do not have have port that supports MHL.
    Boolean isEmpty;
    mhlSupportedPorts->IsEmpty(&isEmpty);
    if (isEmpty) {
        AutoPtr<IList> list;
        Arrays::AsList(cecPortInfo, (IList**)&list);
        AutoPtr<ICollections> helper;
        CCollections::AcquireSingleton((ICollections**)&helper);
        helper->UnmodifiableList(list, (IList**)&mPortInfo);
        return NOERROR;
    }
    AutoPtr<IArrayList> result;
    CArrayList::New(cecPortInfo->GetLength(), (IArrayList**)&result);
    for (Int32 i = 0; i < cecPortInfo->GetLength(); ++i) {
        AutoPtr<IHdmiPortInfo> info = (*cecPortInfo)[i];
        Int32 id;
        info->GetId(&id);
        AutoPtr<IInteger32> i32Id;
        CInteger32::New(id, (IInteger32**)&i32Id);
        Boolean isContains;
        mhlSupportedPorts->Contains(i32Id, &isContains);
        if (isContains) {
            Boolean isArcSupported;
            info->IsArcSupported(&isArcSupported);
            Boolean isCecSupported;
            info->IsCecSupported(&isCecSupported);
            Int32 type;
            info->GetType(&type);
            Int32 id;
            info->GetId(&id);
            Int32 address;
            info->GetAddress(&address);
            AutoPtr<IHdmiPortInfo> portInfo;
            // TODO: Waiting for CHdmiPortInfo
            assert(0);
            // CHdmiPortInfo::New(id, type, address, isCecSupported, TRUE, isArcSupported, (IHdmiPortInfo**)&portInfo);
            result->Add(portInfo);
        } else {
            result->Add(info);
        }
    }
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->UnmodifiableList(IList::Probe(result), (IList**)&mPortInfo);
    return NOERROR;
}

ECode HdmiControlService::GetPortInfo(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mPortInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiControlService::GetPortInfo(
    /* [in] */ Int32 portId,
    /* [out] */ IHdmiPortInfo** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IInterface> obj;
    mPortInfoMap->Get(portId, NULL, (IInterface**)&obj);
    *result = IHdmiPortInfo::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiControlService::PortIdToPath(
    /* [in] */ Int32 portId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IHdmiPortInfo> portInfo;
    GetPortInfo(portId, (IHdmiPortInfo**)&portInfo);
    if (portInfo == NULL) {
        Slogger::E(TAG, "Cannot find the port info: %d", portId);
        return Constants::INVALID_PHYSICAL_ADDRESS;
    }
    return portInfo->GetAddress(result);
}

ECode HdmiControlService::PathToPortId(
    /* [in] */ Int32 path,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 portAddress = path & Constants::ROUTING_PATH_TOP_MASK;
    return mPortIdMap->Get(portAddress, Constants::INVALID_PORT_ID, result);
}

ECode HdmiControlService::IsValidPortId(
    /* [in] */ Int32 portId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiPortInfo> portInfo;
    GetPortInfo(portId, (IHdmiPortInfo**)&portInfo);
    *result = portInfo != NULL;
    return NOERROR;
}

ECode HdmiControlService::GetIoLooper(
    /* [out] */ ILooper** result)
{
    return mIoThread->GetLooper(result);
}

ECode HdmiControlService::GetServiceLooper(
    /* [out] */ ILooper** result)
{
    return mHandler->GetLooper(result);
}

ECode HdmiControlService::GetPhysicalAddress(
    /* [out] */ Int32* result)
{
    return mCecController->GetPhysicalAddress(result);
}

ECode HdmiControlService::GetVendorId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 vendorId;
    mCecController->GetVendorId(&vendorId);
    *result = vendorId;
    return NOERROR;
}

ECode HdmiControlService::GetDeviceInfo(
    /* [in] */ Int32 logicalAddress,
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    if (tv == NULL) {
        *result = NULL;
        return NOERROR;
    }
    return ((HdmiCecLocalDeviceTv*) tv.Get())->GetCecDeviceInfo(logicalAddress, result);
}

ECode HdmiControlService::GetCecVersion(
    /* [out] */ Int32* result)
{
    return mCecController->GetVersion(result);
}

ECode HdmiControlService::IsConnectedToArcPort(
    /* [in] */ Int32 physicalAddress,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 portId;
    PathToPortId(physicalAddress, &portId);
    if (portId != Constants::INVALID_PORT_ID) {
        AutoPtr<IInterface> obj;
        mPortInfoMap->Get(portId, (IInterface**)&obj);
        return IHdmiPortInfo::Probe(obj)->IsArcSupported(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiControlService::RunOnServiceThread(
    /* [in] */ IRunnable* runnable)
{
    Boolean bNoUse;
    return mHandler->Post(runnable, &bNoUse);
}

ECode HdmiControlService::RunOnServiceThreadAtFrontOfQueue(
    /* [in] */ IRunnable* runnable)
{
    Boolean bNoUse;
    return mHandler->PostAtFrontOfQueue(runnable, &bNoUse);
}

ECode HdmiControlService::AssertRunOnServiceThread()
{
    AutoPtr<ILooper> looper;
    mHandler->GetLooper((ILooper**)&looper);
    if (Looper::GetMyLooper() != looper) {
        Logger::E(TAG, "Should run on service thread.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

ECode HdmiControlService::SendCecCommand(
    /* [in] */ IHdmiCecMessage* command,
    /* [in] */ IHdmiControlServiceSendMessageCallback* callback)
{
    AssertRunOnServiceThread();
    Int32 isValid;
    mMessageValidator->IsValid(command, &isValid);
    if (isValid == HdmiCecMessageValidator::OK) {
        mCecController->SendCommand(command, callback);
    } else {
        HdmiLogger::Error("Invalid message type:%s", TO_CSTR(command));
        if (callback != NULL) {
            callback->OnSendCompleted(Constants::SEND_RESULT_FAILURE);
        }
    }
    return NOERROR;
}

ECode HdmiControlService::SendCecCommand(
    /* [in] */ IHdmiCecMessage* command)
{
    AssertRunOnServiceThread();
    SendCecCommand(command, NULL);
    return NOERROR;
}

ECode HdmiControlService::MaySendFeatureAbortCommand(
    /* [in] */ IHdmiCecMessage* command,
    /* [in] */ Int32 reason)
{
    AssertRunOnServiceThread();
    mCecController->MaySendFeatureAbortCommand(command, reason);
    return NOERROR;
}

ECode HdmiControlService::HandleCecCommand(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    Int32 errorCode;
    mMessageValidator->IsValid(message, &errorCode);
    if (errorCode != HdmiCecMessageValidator::OK) {
        // We'll not response on the messages with the invalid source or destination.
        if (errorCode == HdmiCecMessageValidator::ERROR_PARAMETER) {
            MaySendFeatureAbortCommand(message, Constants::ABORT_INVALID_OPERAND);
        }
        *result = TRUE;
        return NOERROR;
    }
    return DispatchMessageToLocalDevice(message, result);
}

ECode HdmiControlService::SetAudioReturnChannel(
    /* [in] */ Boolean enabled)
{
    return mCecController->SetAudioReturnChannel(enabled);
}

ECode HdmiControlService::DispatchMessageToLocalDevice(
    /* [in] */ IHdmiCecMessage* message,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    AutoPtr<IList> localDeviceList;
    mCecController->GetLocalDeviceList((IList**)&localDeviceList);
    FOR_EACH(it, localDeviceList) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiCecLocalDevice> device = IHdmiCecLocalDevice::Probe(obj);
        Int32 dest;
        message->GetDestination(&dest);
        Boolean isDispatchMessageOk;
        ((HdmiCecLocalDevice*) device.Get())->DispatchMessage(message, &isDispatchMessageOk);
        if (isDispatchMessageOk && dest != Constants::ADDR_BROADCAST) {
            *result = TRUE;
            return NOERROR;
        }
    }
    Int32 dest;
    message->GetDestination(&dest);
    if (dest != Constants::ADDR_BROADCAST) {
        HdmiLogger::Warning("Unhandled cec command:%s", TO_CSTR(message));
    }
    *result = FALSE;
    return NOERROR;
}

ECode HdmiControlService::OnHotplug(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    AssertRunOnServiceThread();
    AutoPtr<IArrayList> localDevices;
    CArrayList::New((IArrayList**)&localDevices);
    FOR_EACH(it, mLocalDevices) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        Int32 type;
        IInteger32::Probe(obj)->GetValue(&type);
        if (type == IHdmiDeviceInfo::DEVICE_TV) {
            // Skip the reallocation of the logical address on TV.
            continue;
        }
        AutoPtr<IHdmiCecLocalDevice> localDevice;
        mCecController->GetLocalDevice(type, (IHdmiCecLocalDevice**)&localDevice);
        if (localDevice == NULL) {
            HdmiCecLocalDevice::Create(this, type, (IHdmiCecLocalDevice**)&localDevice);
            ((HdmiCecLocalDevice*) localDevice.Get())->Init();
        }
        localDevices->Add(localDevice);
    }
    AllocateLogicalAddress(localDevices, INITIATED_BY_HOTPLUG);
    AutoPtr<IList> localDeviceList;
    mCecController->GetLocalDeviceList((IList**)&localDeviceList);
    FOR_EACH(iter, localDeviceList) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiCecLocalDevice> device = IHdmiCecLocalDevice::Probe(obj);
        ((HdmiCecLocalDevice*) device.Get())->OnHotplug(portId, connected);
    }
    AnnounceHotplugEvent(portId, connected);
    return NOERROR;
}

ECode HdmiControlService::PollDevices(
    /* [in] */ IHdmiControlServiceDevicePollingCallback* callback,
    /* [in] */ Int32 sourceAddress,
    /* [in] */ Int32 pickStrategy,
    /* [in] */ Int32 retryCount)
{
    AssertRunOnServiceThread();
    Int32 checkPollStrategyResult;
    CheckPollStrategy(pickStrategy, &checkPollStrategyResult);
    mCecController->PollDevices(callback, sourceAddress, checkPollStrategyResult,
            retryCount);
    return NOERROR;
}

ECode HdmiControlService::CheckPollStrategy(
    /* [in] */ Int32 pickStrategy,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Int32 strategy = pickStrategy & Constants::POLL_STRATEGY_MASK;
    if (strategy == 0) {
        Logger::E(TAG, "Invalid poll strategy:%d", pickStrategy);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 iterationStrategy = pickStrategy & Constants::POLL_ITERATION_STRATEGY_MASK;
    if (iterationStrategy == 0) {
        Logger::E(TAG, "Invalid iteration strategy:%d", pickStrategy);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    *result = strategy | iterationStrategy;
    return NOERROR;
}

ECode HdmiControlService::GetAllLocalDevices(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AssertRunOnServiceThread();
    return mCecController->GetLocalDeviceList(result);
}

ECode HdmiControlService::GetServiceLock(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mLock;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiControlService::SetAudioStatus(
    /* [in] */ Boolean mute,
    /* [in] */ Int32 volume)
{
    AutoPtr<IAudioManager> audioManager;
    GetAudioManager((IAudioManager**)&audioManager);
    Boolean muted;
    audioManager->IsStreamMute(IAudioManager::STREAM_MUSIC, &muted);
    if (mute) {
        if (!muted) {
            audioManager->SetStreamMute(IAudioManager::STREAM_MUSIC, TRUE);
        }
    } else {
        if (muted) {
            audioManager->SetStreamMute(IAudioManager::STREAM_MUSIC, FALSE);
        }
        // FLAG_HDMI_SYSTEM_AUDIO_VOLUME prevents audio manager from announcing
        // volume change notification back to hdmi control service.
        audioManager->SetStreamVolume(IAudioManager::STREAM_MUSIC, volume,
                IAudioManager::FLAG_SHOW_UI | IAudioManager::FLAG_HDMI_SYSTEM_AUDIO_VOLUME);
    }
    return NOERROR;
}

ECode HdmiControlService::AnnounceSystemAudioModeChange(
    /* [in] */ Boolean enabled)
{
    synchronized(mLock) {
        FOR_EACH(it, mSystemAudioModeChangeListenerRecords) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<SystemAudioModeChangeListenerRecord> record = (SystemAudioModeChangeListenerRecord*) IObject::Probe(obj);
            InvokeSystemAudioModeChangeLocked(record->mListener, enabled);
        }
    }
    return NOERROR;
}

ECode HdmiControlService::CreateDeviceInfo(
    /* [in] */ Int32 logicalAddress,
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 powerStatus,
    /* [out] */ IHdmiDeviceInfo** result)
{
    VALIDATE_NOT_NULL(result)

    // TODO: find better name instead of model name.
    String displayName = Build::MODEL;
    Int32 physicalAddr;
    GetPhysicalAddress(&physicalAddr);
    Int32 portId;
    PathToPortId(physicalAddr, &portId);
    Int32 vendorId;
    GetVendorId(&vendorId);
    // TODO: Waiting for CHdmiDeviceInfo
    assert(0);
    // return CHdmiDeviceInfo::New(logicalAddress, physicalAddr, portId, deviceType,
            // vendorId, displayName, result);
    return NOERROR;
}

ECode HdmiControlService::HandleMhlHotplugEvent(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    AssertRunOnServiceThread();
    if (connected) {
        AutoPtr<HdmiMhlLocalDeviceStub> newDevice = new HdmiMhlLocalDeviceStub();
        newDevice->constructor(this, portId);
        AutoPtr<HdmiMhlLocalDeviceStub> oldDevice;
        mMhlController->AddLocalDevice(newDevice, (HdmiMhlLocalDeviceStub**)&oldDevice);
        if (oldDevice != NULL) {
            oldDevice->OnDeviceRemoved();
            Slogger::I(TAG, "Old device of port %d is removed", portId);
        }
    } else {
        AutoPtr<HdmiMhlLocalDeviceStub> device;
        mMhlController->RemoveLocalDevice(portId, (HdmiMhlLocalDeviceStub**)&device);
        if (device != NULL) {
            device->OnDeviceRemoved();
            // There is no explicit event for device removal.
            // Hence we remove the device on hotplug event.
            AutoPtr<IHdmiDeviceInfo> deviceInfo;
            device->GetInfo((IHdmiDeviceInfo**)&deviceInfo);
            if (deviceInfo != NULL) {
                InvokeDeviceEventListeners(deviceInfo, IHdmiControlManager::DEVICE_EVENT_REMOVE_DEVICE);
                UpdateSafeMhlInput();
            }
        } else {
            Slogger::W(TAG, "No device to remove:[portId=%d", portId);
        }
    }
    AnnounceHotplugEvent(portId, connected);
    return NOERROR;
}

ECode HdmiControlService::HandleMhlBusModeChanged(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 busmode)
{
    AssertRunOnServiceThread();
    AutoPtr<HdmiMhlLocalDeviceStub> device;
    mMhlController->GetLocalDevice(portId, (HdmiMhlLocalDeviceStub**)&device);
    if (device != NULL) {
        device->SetBusMode(busmode);
    } else {
        Slogger::W(TAG, "No mhl device exists for bus mode change[portId:%d, busmode:%d]", portId, busmode);
    }
    return NOERROR;
}

ECode HdmiControlService::HandleMhlBusOvercurrent(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean on)
{
    AssertRunOnServiceThread();
    AutoPtr<HdmiMhlLocalDeviceStub> device;
    mMhlController->GetLocalDevice(portId, (HdmiMhlLocalDeviceStub**)&device);
    if (device != NULL) {
        device->OnBusOvercurrentDetected(on);
    } else {
        Slogger::W(TAG, "No mhl device exists for bus overcurrent event[portId:%d]", portId);
    }
    return NOERROR;
}

ECode HdmiControlService::HandleMhlDeviceStatusChanged(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 adopterId,
    /* [in] */ Int32 deviceId)
{
    AssertRunOnServiceThread();
    AutoPtr<HdmiMhlLocalDeviceStub> device;
    mMhlController->GetLocalDevice(portId, (HdmiMhlLocalDeviceStub**)&device);
    // Hotplug event should already have been called before device status change event.
    if (device != NULL) {
        device->SetDeviceStatusChange(adopterId, deviceId);
        AutoPtr<IHdmiDeviceInfo> deviceInfo;
        device->GetInfo((IHdmiDeviceInfo**)&deviceInfo);
        InvokeDeviceEventListeners(deviceInfo, IHdmiControlManager::DEVICE_EVENT_ADD_DEVICE);
        UpdateSafeMhlInput();
    } else {
        Slogger::W(TAG, "No mhl device exists for device status event[portId:%d"
                ", adopterId:%d, deviceId:%d]", portId, adopterId, deviceId);
    }
    return NOERROR;
}

ECode HdmiControlService::UpdateSafeMhlInput()
{
    AssertRunOnServiceThread();
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    AutoPtr<IList> inputs;
    helper->GetEmptyList((IList**)&inputs);
    AutoPtr<ISparseArray> devices;
    mMhlController->GetAllLocalDevices((ISparseArray**)&devices);
    Int32 size;
    devices->GetSize(&size);
    for (Int32 i = 0; i < size; ++i) {
        AutoPtr<IInterface> obj;
        devices->ValueAt(i, (IInterface**)&obj);
        AutoPtr<HdmiMhlLocalDeviceStub> device = (HdmiMhlLocalDeviceStub*) IObject::Probe(obj);
        AutoPtr<IHdmiDeviceInfo> info;
        device->GetInfo((IHdmiDeviceInfo**)&info);
        if (info != NULL) {
            Boolean isEmpty;
            inputs->IsEmpty(&isEmpty);
            if (isEmpty) {
                inputs = NULL;
                CArrayList::New((IList**)&inputs);
            }
            AutoPtr<IHdmiDeviceInfo> deviceInfo;
            device->GetInfo((IHdmiDeviceInfo**)&deviceInfo);
            inputs->Add(deviceInfo);
        }
    }
    synchronized(mLock) {
        mMhlDevices = inputs;
    }
    return NOERROR;
}

ECode HdmiControlService::GetMhlDevicesLocked(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mMhlDevices;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiControlService::EnforceAccessPermission()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->EnforceCallingOrSelfPermission(PERMISSION, TAG);
    return NOERROR;
}

ECode HdmiControlService::OneTouchPlay(
    /* [in] */ IIHdmiControlCallback* callback)
{
    AssertRunOnServiceThread();
    AutoPtr<HdmiCecLocalDevicePlayback> source;
    Playback((HdmiCecLocalDevicePlayback**)&source);
    if (source == NULL) {
        Slogger::W(TAG, "Local playback device not available");
        InvokeCallback(callback, IHdmiControlManager::RESULT_SOURCE_NOT_AVAILABLE);
        return NOERROR;
    }
    source->OneTouchPlay(callback);
    return NOERROR;
}

ECode HdmiControlService::QueryDisplayStatus(
    /* [in] */ IIHdmiControlCallback* callback)
{
    AssertRunOnServiceThread();
    AutoPtr<HdmiCecLocalDevicePlayback> source;
    Playback((HdmiCecLocalDevicePlayback**)&source);
    if (source == NULL) {
        Slogger::W(TAG, "Local playback device not available");
        InvokeCallback(callback, IHdmiControlManager::RESULT_SOURCE_NOT_AVAILABLE);
        return NOERROR;
    }
    source->QueryDisplayStatus(callback);
    return NOERROR;
}

ECode HdmiControlService::AddHotplugEventListener(
    /* [in] */ IIHdmiHotplugEventListener* listener)
{
    AutoPtr<HotplugEventListenerRecord> record = new HotplugEventListenerRecord(this, listener);
    // try {
    AutoPtr<IProxy> proxy = (IProxy*) listener->Probe(EIID_IProxy);
    ECode ec = NOERROR;
    if (proxy != NULL)
        ec = proxy->LinkToDeath(record, 0);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::W(TAG, "Listener already died");
            return NOERROR;
        }
        else
            return ec;
    }
    // }
    synchronized(mLock) {
        mHotplugEventListenerRecords->Add(TO_IINTERFACE(record));
    }
    return NOERROR;
}

ECode HdmiControlService::RemoveHotplugEventListener(
    /* [in] */ IIHdmiHotplugEventListener* listener)
{
    synchronized(mLock) {
        FOR_EACH(it, mHotplugEventListenerRecords) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<HotplugEventListenerRecord> record = (HotplugEventListenerRecord*) IObject::Probe(obj);
            if (IBinder::Probe(record->mListener) == IBinder::Probe(listener)) {
                AutoPtr<IProxy> proxy = (IProxy*) listener->Probe(EIID_IProxy);
                if (proxy != NULL) {
                    Boolean bNoUse;
                    proxy->UnlinkToDeath(record, 0, &bNoUse);
                }
                mHotplugEventListenerRecords->Remove(TO_IINTERFACE(record));
                break;
            }
        }
    }
    return NOERROR;
}

ECode HdmiControlService::AddDeviceEventListener(
    /* [in] */ IIHdmiDeviceEventListener* listener)
{
    AutoPtr<DeviceEventListenerRecord> record = new DeviceEventListenerRecord(this, listener);
    // try {
    ECode ec = NOERROR;
    AutoPtr<IProxy> proxy = (IProxy*) listener->Probe(EIID_IProxy);
    if (proxy != NULL)
        ec = proxy->LinkToDeath(record, 0);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::W(TAG, "Listener already died");
            return NOERROR;
        }
        else
            return ec;
    }
    // }
    synchronized(mLock) {
        mDeviceEventListenerRecords->Add(TO_IINTERFACE(record));
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeDeviceEventListeners(
    /* [in] */ IHdmiDeviceInfo* device,
    /* [in] */ Int32 status)
{
    synchronized(mLock) {
        FOR_EACH(it, mDeviceEventListenerRecords) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<DeviceEventListenerRecord> record = (DeviceEventListenerRecord*) IObject::Probe(obj);
            // try {
            ECode ec = record->mListener->OnStatusChanged(device, status);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "Failed to report device event:%d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode HdmiControlService::AddSystemAudioModeChangeListner(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
{
    AutoPtr<SystemAudioModeChangeListenerRecord> record = new SystemAudioModeChangeListenerRecord(this, listener);
    // try {
    ECode ec = NOERROR;
    AutoPtr<IProxy> proxy = (IProxy*) listener->Probe(EIID_IProxy);
    if (proxy != NULL)
        ec = proxy->LinkToDeath(record, 0);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::W(TAG, "Listener already died");
            return NOERROR;
        }
        return ec;
    }
    // }
    synchronized(mLock) {
        mSystemAudioModeChangeListenerRecords->Add(TO_IINTERFACE(record));
    }
    return NOERROR;
}

ECode HdmiControlService::RemoveSystemAudioModeChangeListener(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener)
{
    synchronized(mLock) {
        FOR_EACH(it, mSystemAudioModeChangeListenerRecords) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<SystemAudioModeChangeListenerRecord> record = (SystemAudioModeChangeListenerRecord*) IObject::Probe(obj);
            if (IBinder::Probe(record->mListener) == IBinder::Probe(listener)) {
                AutoPtr<IProxy> proxy = (IProxy*) listener->Probe(EIID_IProxy);
                if (proxy != NULL) {
                    Boolean bNoUse;
                    proxy->UnlinkToDeath(record, 0, &bNoUse);
                }
                mSystemAudioModeChangeListenerRecords->Remove(TO_IINTERFACE(record));
                break;
            }
        }
    }
    return NOERROR;
}

ECode HdmiControlService::SetInputChangeListener(
    /* [in] */ IIHdmiInputChangeListener* listener)
{
    synchronized(mLock) {
        mInputChangeListenerRecord = new InputChangeListenerRecord(this, listener);
        // try {
        ECode ec = NOERROR;
        AutoPtr<IProxy> proxy = (IProxy*) listener->Probe(EIID_IProxy);
        if (proxy != NULL)
            ec = proxy->LinkToDeath(mInputChangeListenerRecord, 0);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Slogger::W(TAG, "Listener already died");
                return NOERROR;
            }
            return ec;
        }
        // }
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeInputChangeListener(
    /* [in] */ IHdmiDeviceInfo* info)
{
    synchronized(mLock) {
        if (mInputChangeListenerRecord != NULL) {
            // try {
            ECode ec = mInputChangeListenerRecord->mListener->OnChanged(info);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::W(TAG, "Exception thrown by IHdmiInputChangeListener: %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode HdmiControlService::SetHdmiRecordListener(
    /* [in] */ IIHdmiRecordListener* listener)
{
    synchronized(mLock) {
        mRecordListenerRecord = new HdmiRecordListenerRecord(this, listener);
        // try {
        ECode ec = NOERROR;
        AutoPtr<IProxy> proxy = (IProxy*) listener->Probe(EIID_IProxy);
        if (proxy != NULL)
            ec = proxy->LinkToDeath(mRecordListenerRecord, 0);
        // } catch (RemoteException e) {
        if (FAILED(ec)) {
            if ((ECode) E_REMOTE_EXCEPTION == ec) {
                Slogger::W(TAG, "Listener already died. %d", ec);
            }
            else
                return ec;
        }
        // }
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeRecordRequestListener(
    /* [in] */ Int32 recorderAddress,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    synchronized(mLock) {
        if (mRecordListenerRecord != NULL) {
            // try {
            AutoPtr<ArrayOf<Byte> > oneTouchRecordSource;
            ECode ec = mRecordListenerRecord->mListener->GetOneTouchRecordSource(recorderAddress, (ArrayOf<Byte>**)&oneTouchRecordSource);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::W(TAG, "Failed to start record. %d", ec);
                }
                else
                    return ec;
            }
            else {
                *result = oneTouchRecordSource;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }

            // }
        }
        *result = EmptyArray::BYTE;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeOneTouchRecordResult(
    /* [in] */ Int32 result)
{
    synchronized(mLock) {
        if (mRecordListenerRecord != NULL) {
            // try {
            ECode ec = mRecordListenerRecord->mListener->OnOneTouchRecordResult(result);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::W(TAG, "Failed to call onOneTouchRecordResult.%d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeTimerRecordingResult(
    /* [in] */ Int32 result)
{
    synchronized(mLock) {
        if (mRecordListenerRecord != NULL) {
            // try {
            ECode ec = mRecordListenerRecord->mListener->OnTimerRecordingResult(result);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::W(TAG, "Failed to call onTimerRecordingResult. %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeClearTimerRecordingResult(
    /* [in] */ Int32 result)
{
    synchronized(mLock) {
        if (mRecordListenerRecord != NULL) {
            // try {
            ECode ec = mRecordListenerRecord->mListener->OnClearTimerRecordingResult(result);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::W(TAG, "Failed to call onClearTimerRecordingResult. %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeCallback(
    /* [in] */ IIHdmiControlCallback* callback,
    /* [in] */ Int32 result)
{
    // try {
    ECode ec = callback->OnComplete(result);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "Invoking callback failed:%d", ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode HdmiControlService::InvokeSystemAudioModeChangeLocked(
    /* [in] */ IIHdmiSystemAudioModeChangeListener* listener,
    /* [in] */ Boolean enabled)
{
    // try {
    ECode ec = listener->OnStatusChanged(enabled);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "Invoking callback failed:%d", ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode HdmiControlService::AnnounceHotplugEvent(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean connected)
{
    AutoPtr<IHdmiHotplugEvent> event;
    // TODO: Waiting for CHdmiHotplugEvent
    assert(0);
    // CHdmiHotplugEvent::New(portId, connected, (IHdmiHotplugEvent**)&event);
    synchronized(mLock) {
        FOR_EACH(it, mHotplugEventListenerRecords) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<HotplugEventListenerRecord> record = (HotplugEventListenerRecord*) IObject::Probe(obj);
            InvokeHotplugEventListenerLocked(record->mListener, event);
        }
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeHotplugEventListenerLocked(
    /* [in] */ IIHdmiHotplugEventListener* listener,
    /* [in] */ IHdmiHotplugEvent* event)
{
    // try {
    ECode ec = listener->OnReceived(event);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::E(TAG, "Failed to report hotplug event:%s %d", TO_CSTR(event), ec);
        }
        else
            return ec;
    }
    // }
    return NOERROR;
}

ECode HdmiControlService::Tv(
    /* [out] */ IHdmiCecLocalDeviceTv** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiCecLocalDevice> device;
    mCecController->GetLocalDevice(IHdmiDeviceInfo::DEVICE_TV, (IHdmiCecLocalDevice**)&device);
    *result = IHdmiCecLocalDeviceTv::Probe(device);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiControlService::IsTvDevice(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    *result = tv != NULL;
    return NOERROR;
}

ECode HdmiControlService::Playback(
    /* [out] */ HdmiCecLocalDevicePlayback** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IHdmiCecLocalDevice> device;
    mCecController->GetLocalDevice(IHdmiDeviceInfo::DEVICE_PLAYBACK, (IHdmiCecLocalDevice**)&device);
    *result = (HdmiCecLocalDevicePlayback*) device.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiControlService::GetAudioManager(
    /* [out] */ IAudioManager** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    *result = IAudioManager::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode HdmiControlService::IsControlEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(mLock) {
        *result = mHdmiControlEnabled;
    }
    return NOERROR;
}

ECode HdmiControlService::GetPowerStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AssertRunOnServiceThread();
    *result = mPowerStatus;
    return NOERROR;
}

ECode HdmiControlService::IsPowerOnOrTransient(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    *result = mPowerStatus == IHdmiControlManager::POWER_STATUS_ON
            || mPowerStatus == IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_ON;
    return NOERROR;
}

ECode HdmiControlService::IsPowerStandbyOrTransient(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    *result = mPowerStatus == IHdmiControlManager::POWER_STATUS_STANDBY
            || mPowerStatus == IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_STANDBY;
    return NOERROR;
}

ECode HdmiControlService::IsPowerStandby(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    AssertRunOnServiceThread();
    *result = mPowerStatus == IHdmiControlManager::POWER_STATUS_STANDBY;
    return NOERROR;
}

ECode HdmiControlService::WakeUp()
{
    AssertRunOnServiceThread();
    mWakeUpMessageReceived = TRUE;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->WakeUp(SystemClock::GetUptimeMillis());
    // PowerManger will send the broadcast Intent.ACTION_SCREEN_ON and after this gets
    // the intent, the sequence will continue at onWakeUp().
    return NOERROR;
}

ECode HdmiControlService::Standby()
{
    AssertRunOnServiceThread();
    mStandbyMessageReceived = TRUE;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->GoToSleep(SystemClock::GetUptimeMillis(), IPowerManager::GO_TO_SLEEP_REASON_HDMI, 0);
    // PowerManger will send the broadcast Intent.ACTION_SCREEN_OFF and after this gets
    // the intent, the sequence will continue at onStandby().
    return NOERROR;
}

ECode HdmiControlService::OnWakeUp()
{
    AssertRunOnServiceThread();
    mPowerStatus = IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_ON;
    if (mCecController != NULL) {
        if (mHdmiControlEnabled) {
            Int32 startReason = INITIATED_BY_SCREEN_ON;
            if (mWakeUpMessageReceived) {
                startReason = INITIATED_BY_WAKE_UP_MESSAGE;
            }
            InitializeCec(startReason);
        }
    } else {
        Slogger::I(TAG, "Device does not support HDMI-CEC.");
    }
    // TODO: Initialize MHL local devices.
    return NOERROR;
}

ECode HdmiControlService::OnStandby()
{
    AssertRunOnServiceThread();
    mPowerStatus = IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_STANDBY;
    AutoPtr<IList> devices;
    GetAllLocalDevices((IList**)&devices);
    DisableDevices(new OnStandbyPendingActionClearedCallback(this, devices));
    return NOERROR;
}

ECode HdmiControlService::OnLanguageChanged(
    /* [in] */ const String& language)
{
    AssertRunOnServiceThread();
    mLanguage = language;
    Boolean isTvDevice;
    IsTvDevice(&isTvDevice);
    if (isTvDevice) {
        AutoPtr<IHdmiCecLocalDeviceTv> tv;
        Tv((IHdmiCecLocalDeviceTv**)&tv);
        Boolean isBroadcastMenuLanguage;
        ((HdmiCecLocalDeviceTv*) tv.Get())->BroadcastMenuLanguage(language, &isBroadcastMenuLanguage);
    }
    return NOERROR;
}

ECode HdmiControlService::GetLanguage(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    AssertRunOnServiceThread();
    *result = mLanguage;
    return NOERROR;
}

ECode HdmiControlService::DisableDevices(
    /* [in] */ IHdmiCecLocalDevicePendingActionClearedCallback* callback)
{
    if (mCecController != NULL) {
        AutoPtr<IList> localDeviceList;
        mCecController->GetLocalDeviceList((IList**)&localDeviceList);
        FOR_EACH(it, localDeviceList) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IHdmiCecLocalDevice> device = IHdmiCecLocalDevice::Probe(obj);
            ((HdmiCecLocalDevice*) device.Get())->DisableDevice(mStandbyMessageReceived, callback);
        }
        Boolean isTvDevice;
        IsTvDevice(&isTvDevice);
        if (isTvDevice) {
            UnregisterSettingsObserver();
        }
    }
    mMhlController->ClearAllLocalDevices();
    return NOERROR;
}

ECode HdmiControlService::ClearLocalDevices()
{
    AssertRunOnServiceThread();
    if (mCecController == NULL) {
        return NOERROR;
    }
    mCecController->ClearLogicalAddress();
    mCecController->ClearLocalDevices();
    return NOERROR;
}

ECode HdmiControlService::OnStandbyCompleted()
{
    AssertRunOnServiceThread();
    Slogger::V(TAG, "onStandbyCompleted");
    if (mPowerStatus != IHdmiControlManager::POWER_STATUS_TRANSIENT_TO_STANDBY) {
        return NOERROR;
    }
    mPowerStatus = IHdmiControlManager::POWER_STATUS_STANDBY;
    AutoPtr<IList> localDeviceList;
    mCecController->GetLocalDeviceList((IList**)&localDeviceList);
    FOR_EACH(it, localDeviceList) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IHdmiCecLocalDevice> device = IHdmiCecLocalDevice::Probe(obj);
        ((HdmiCecLocalDevice*)device.Get())->OnStandby(mStandbyMessageReceived);
    }
    mStandbyMessageReceived = FALSE;
    mCecController->SetOption(Constants::OPTION_CEC_SERVICE_CONTROL, Constants::DISABLED);
    return NOERROR;
}

ECode HdmiControlService::AddVendorCommandListener(
    /* [in] */ IIHdmiVendorCommandListener* listener,
    /* [in] */ Int32 deviceType)
{
    AutoPtr<VendorCommandListenerRecord> record = new VendorCommandListenerRecord(this, listener, deviceType);
    // try {
    ECode ec = NOERROR;
    AutoPtr<IProxy> proxy = (IProxy*) listener->Probe(EIID_IProxy);
    if (proxy != NULL)
        ec = proxy->LinkToDeath(record, 0);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::W(TAG, "Listener already died");
            return NOERROR;
        }
        return ec;
    }
    // }
    synchronized(mLock) {
        mVendorCommandListenerRecords->Add(TO_IINTERFACE(record));
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeVendorCommandListeners(
    /* [in] */ Int32 deviceType,
    /* [in] */ Int32 srcAddress,
    /* [in] */ ArrayOf<Byte>* params,
    /* [in] */ Boolean hasVendorId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(mLock) {
        Boolean isEmpty;
        mVendorCommandListenerRecords->IsEmpty(&isEmpty);
        if (isEmpty) {
            *result = FALSE;
            return NOERROR;
        }
        FOR_EACH(it, mVendorCommandListenerRecords) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<VendorCommandListenerRecord> record = (VendorCommandListenerRecord*) IObject::Probe(obj);
            if (record->mDeviceType != deviceType) {
                continue;
            }
            // try {
            ECode ec = record->mListener->OnReceived(srcAddress, params, hasVendorId);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "Failed to notify vendor command reception %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
        *result = TRUE;
    }
    return NOERROR;
}

ECode HdmiControlService::AddHdmiMhlVendorCommandListener(
    /* [in] */ IIHdmiMhlVendorCommandListener* listener)
{
    AutoPtr<HdmiMhlVendorCommandListenerRecord> record =
            new HdmiMhlVendorCommandListenerRecord(this, listener);
    // try {
    ECode ec = NOERROR;
    AutoPtr<IProxy> proxy = (IProxy*) listener->Probe(EIID_IProxy);
    if (proxy != NULL)
        ec = proxy->LinkToDeath(record, 0);
    // } catch (RemoteException e) {
    if (FAILED(ec)) {
        if ((ECode) E_REMOTE_EXCEPTION == ec) {
            Slogger::W(TAG, "Listener already died.");
            return NOERROR;
        }
        return ec;
    }
    // }
    synchronized(mLock) {
        mMhlVendorCommandListenerRecords->Add(TO_IINTERFACE(record));
    }
    return NOERROR;
}

ECode HdmiControlService::InvokeMhlVendorCommandListeners(
    /* [in] */ Int32 portId,
    /* [in] */ Int32 offest,
    /* [in] */ Int32 length,
    /* [in] */ ArrayOf<Byte>* data)
{
    synchronized(mLock) {
        FOR_EACH(it, mMhlVendorCommandListenerRecords) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<HdmiMhlVendorCommandListenerRecord> record = (HdmiMhlVendorCommandListenerRecord*) IObject::Probe(obj);
            // try {
            ECode ec = record->mListener->OnReceived(portId, offest, length, data);
            // } catch (RemoteException e) {
            if (FAILED(ec)) {
                if ((ECode) E_REMOTE_EXCEPTION == ec) {
                    Slogger::E(TAG, "Failed to notify MHL vendor command %d", ec);
                }
                else
                    return ec;
            }
            // }
        }
    }
    return NOERROR;
}

ECode HdmiControlService::IsProhibitMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(mLock) {
        *result = mProhibitMode;
    }
    return NOERROR;
}

ECode HdmiControlService::SetProhibitMode(
    /* [in] */ Boolean enabled)
{
    synchronized(mLock) {
        mProhibitMode = enabled;
    }
    return NOERROR;
}

ECode HdmiControlService::SetCecOption(
    /* [in] */ Int32 key,
    /* [in] */ Int32 value)
{
    AssertRunOnServiceThread();
    mCecController->SetOption(key, value);
    return NOERROR;
}

ECode HdmiControlService::SetControlEnabled(
    /* [in] */ Boolean enabled)
{
    AssertRunOnServiceThread();
    Int32 value;
    ToInt32(enabled, &value);
    mCecController->SetOption(Constants::OPTION_CEC_ENABLE, value);
    mMhlController->SetOption(Constants::OPTION_MHL_ENABLE, value);
    synchronized(mLock) {
        mHdmiControlEnabled = enabled;
    }
    if (enabled) {
        InitializeCec(INITIATED_BY_ENABLE_CEC);
    } else {
        DisableDevices(new SetControlEnabledPendingActionClearedCallback(this));
    }
    return NOERROR;
}

ECode HdmiControlService::SetActivePortId(
    /* [in] */ Int32 portId)
{
    AssertRunOnServiceThread();
    mActivePortId = portId;
    // Resets last input for MHL, which stays valid only after the MHL device was selected,
    // and no further switching is done.
    SetLastInputForMhl(Constants::INVALID_PORT_ID);
    return NOERROR;
}

ECode HdmiControlService::SetLastInputForMhl(
    /* [in] */ Int32 portId)
{
    AssertRunOnServiceThread();
    mLastInputMhl = portId;
    return NOERROR;
}

ECode HdmiControlService::GetLastInputForMhl(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AssertRunOnServiceThread();
    *result = mLastInputMhl;
    return NOERROR;
}

ECode HdmiControlService::ChangeInputForMhl(
    /* [in] */ Int32 portId,
    /* [in] */ Boolean contentOn)
{
    AssertRunOnServiceThread();
    AutoPtr<IHdmiCecLocalDeviceTv> tv;
    Tv((IHdmiCecLocalDeviceTv**)&tv);
    Int32 activePortId;
    ((HdmiCecLocalDeviceTv*) tv.Get())->GetActivePortId(&activePortId);
    const Int32 lastInput = contentOn ? activePortId : Constants::INVALID_PORT_ID;
    ((HdmiCecLocalDeviceTv*) tv.Get())->DoManualPortSwitching(portId, new InnerSub_IHdmiControlCallbackStub(this, lastInput));
    // MHL device is always directly connected to the port. Update the active port ID to avoid
    // unnecessary post-routing control task.
    ((HdmiCecLocalDeviceTv*) tv.Get())->SetActivePortId(portId);
    // The port is either the MHL-enabled port where the mobile device is connected, or
    // the last port to go back to when turnoff command is received. Note that the last port
    // may not be the MHL-enabled one. In this case the device info to be passed to
    // input change listener should be the one describing the corresponding HDMI port.
    AutoPtr<HdmiMhlLocalDeviceStub> device;
    mMhlController->GetLocalDevice(portId, (HdmiMhlLocalDeviceStub**)&device);
    AutoPtr<IInterface> obj;
    mPortDeviceMap->Get(portId, (IInterface**)&obj);
    AutoPtr<IHdmiDeviceInfo> deviceInfo;
    device->GetInfo((IHdmiDeviceInfo**)&deviceInfo);
    AutoPtr<IHdmiDeviceInfo> info;
    if (device != NULL && deviceInfo != NULL) {
        device->GetInfo((IHdmiDeviceInfo**)&info);
    }
    else {
        info = IHdmiDeviceInfo::Probe(obj);
    }
    InvokeInputChangeListener(info);
    return NOERROR;
}

ECode HdmiControlService::SetMhlInputChangeEnabled(
    /* [in] */ Boolean enabled)
{
    Int32 iEnabled;
    ToInt32(enabled, &iEnabled);
    mMhlController->SetOption(Constants::OPTION_MHL_INPUT_SWITCHING, iEnabled);
    synchronized(mLock) {
        mMhlInputChangeEnabled = enabled;
    }
    return NOERROR;
}

ECode HdmiControlService::IsMhlInputChangeEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    synchronized(mLock) {
        *result = mMhlInputChangeEnabled;
    }
    return NOERROR;
}

ECode HdmiControlService::DisplayOsd(
    /* [in] */ Int32 messageId)
{
    AssertRunOnServiceThread();
    AutoPtr<IIntent> intent;
    CIntent::New(IHdmiControlManager::ACTION_OSD_MESSAGE, (IIntent**)&intent);
    intent->PutExtra(IHdmiControlManager::EXTRA_MESSAGE_ID, messageId);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->SendBroadcastAsUser(intent, UserHandle::ALL,
            HdmiControlService::PERMISSION);
    return NOERROR;
}

ECode HdmiControlService::DisplayOsd(
    /* [in] */ Int32 messageId,
    /* [in] */ Int32 extra)
{
    AssertRunOnServiceThread();
    AutoPtr<IIntent> intent;
    CIntent::New(IHdmiControlManager::ACTION_OSD_MESSAGE, (IIntent**)&intent);
    intent->PutExtra(IHdmiControlManager::EXTRA_MESSAGE_ID, messageId);
    intent->PutExtra(IHdmiControlManager::EXTRA_MESSAGE_EXTRAM_PARAM1, extra);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->SendBroadcastAsUser(intent, UserHandle::ALL,
            HdmiControlService::PERMISSION);
    return NOERROR;
}

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos
