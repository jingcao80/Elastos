
#include "elastos/droid/server/WiredAccessoryManager.h"
#include "elastos/droid/R.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Media.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::R;
using Elastos::Droid::View::IInputDevice;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::IO::CFile;
using Elastos::IO::CFileReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileReader;
using Elastos::IO::IReader;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Server {

ECode WiredAccessoryManager::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case WiredAccessoryManager::MSG_NEW_DEVICE_STATE:
        {
            Int32 arg1, arg2;
            msg->GetArg1(&arg1);
            msg->GetArg2(&arg2);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            String info;
            ICharSequence::Probe(obj)->ToString(&info);
            mHost->HandleMsgNewDeviceState(arg1, arg2, info);
            break;
        }

        case WiredAccessoryManager::MSG_SYSTEM_READY:
        {
            mHost->OnSystemReady();
            mHost->mWakeLock->ReleaseLock();
            break;
        }
    }

    return NOERROR;
}

ECode WiredAccessoryManager::WiredAccessoryManagerReceiver::OnReceive(
    /* [in] */ IContext* ctx,
    /* [in] */ IIntent* intent)
{
    mHost->OnSystemReady();
    return NOERROR;
}

const String WiredAccessoryManager::TAG("WiredAccessoryManager");// = WiredAccessoryManager.class.getSimpleName();
const Boolean WiredAccessoryManager::LOG;

const Int32 WiredAccessoryManager::BIT_HEADSET;
const Int32 WiredAccessoryManager::BIT_HEADSET_NO_MIC;
const Int32 WiredAccessoryManager::BIT_USB_HEADSET_ANLG;
const Int32 WiredAccessoryManager::BIT_USB_HEADSET_DGTL;
const Int32 WiredAccessoryManager::BIT_HDMI_AUDIO;
const Int32 WiredAccessoryManager::BIT_LINEOUT;
const Int32 WiredAccessoryManager::SUPPORTED_HEADSETS;

const String WiredAccessoryManager::NAME_H2W("h2w");
const String WiredAccessoryManager::NAME_USB_AUDIO("usb_audio");
const String WiredAccessoryManager::NAME_HDMI_AUDIO("hdmi_audio");
const String WiredAccessoryManager::NAME_HDMI("hdmi");

const Int32 WiredAccessoryManager::MSG_NEW_DEVICE_STATE;
const Int32 WiredAccessoryManager::MSG_SYSTEM_READY;
Boolean WiredAccessoryManager::mUseDevInputEventForAudioJack = FALSE;

WiredAccessoryManager::WiredAccessoryObserver::UEventInfo::UEventInfo(
    /* [in] */ const String& devName,
    /* [in] */ Int32 state1Bits,
    /* [in] */ Int32 state2Bits,
    /* [in] */ Int32 stateNBits)
    : mDevName(devName)
    , mState1Bits(state1Bits)
    , mState2Bits(state2Bits)
    , mStateNbits(stateNBits)
{}

String WiredAccessoryManager::WiredAccessoryObserver::UEventInfo::GetDevName()
{
    return mDevName;
}

String WiredAccessoryManager::WiredAccessoryObserver::UEventInfo::GetDevPath()
{
    String s("/devices/virtual/switch/");
    s += mDevName;
    return s;
}

String WiredAccessoryManager::WiredAccessoryObserver::UEventInfo::GetSwitchStatePath()
{
    String s("/sys/class/switch/");
    s += mDevName;
    s += "/state";
    return s;
}

Boolean WiredAccessoryManager::WiredAccessoryObserver::UEventInfo::CheckSwitchExists()
{
    AutoPtr<IFile> f;
    CFile::New(GetSwitchStatePath(), (IFile**)&f);
    Boolean exist;
    f->Exists(&exist);
    return exist;
}

Int32 WiredAccessoryManager::WiredAccessoryObserver::UEventInfo::ComputeNewHeadsetState(
    /* [in] */ Int32 headsetState,
    /* [in] */ Int32 switchState)
{
    Int32 preserveMask = ~(mState1Bits | mState2Bits| mStateNBits);
    Int32 setBits = ((switchState == 1) ? mState1Bits :
                  ((switchState == 2) ? mState2Bits :
                    ((switchState == mStateNbits) ? mStateNbits : 0)));

    return ((headsetState & preserveMask) | setBits);
}

WiredAccessoryManager::WiredAccessoryObserver::WiredAccessoryObserver(
    /* [in] */ WiredAccessoryManager* host)
    : mHost(host)
{
    mUEventInfo = MakeObservedUEventList();
}

WiredAccessoryManager::WiredAccessoryObserver::~WiredAccessoryObserver()
{
    mUEventInfo = NULL;
}

void WiredAccessoryManager::WiredAccessoryObserver::Init()
{
    {
        AutoLock lock(mHost->mLock);
        if (mHost->LOG) Slogger::V(mHost->TAG, "init()");
        AutoPtr<ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc(1024);

        List<AutoPtr<WiredAccessoryManager::WiredAccessoryObserver::UEventInfo> >::Iterator it;
        for (it = mUEventInfo->Begin(); it != mUEventInfo->End(); ++it) {
            AutoPtr<WiredAccessoryManager::WiredAccessoryObserver::UEventInfo> uei = *it;
//             try {
                Int32 curState;
                AutoPtr<IFileReader> file;
                CFileReader::New(uei->GetSwitchStatePath(), (IFileReader**)&file);
                Int32 len;
                IReader::Probe(file)->Read(buffer, 0, 1024, &len);
                ICloseable::Probe(file)->Close();
                String bufferString(*buffer, 0, len);
                curState = ValidateSwitchState(StringUtils::ParseInt32(bufferString.Trim()));
                if (curState > 0) {
                    UpdateStateLocked(uei->GetDevPath(), uei->GetDevName(), curState);
                }
//             } catch (FileNotFoundException e) {
//                 Slog.w(TAG, uei.getSwitchStatePath() +
//                         " not found while attempting to determine initial switch state");
//             } catch (Exception e) {
//                 Slog.e(TAG, "" , e);
//             }
        }
    }

    // At any given time accessories could be inserted
    // one on the board, one on the dock and one on HDMI:
    // observe three UEVENTs
    List<AutoPtr<WiredAccessoryManager::WiredAccessoryObserver::UEventInfo> >::Iterator it;
    for (it = mUEventInfo->Begin(); it!= mUEventInfo->End(); ++it) {
        UEventInfo* uei = *it;
        StartObserving(String("DEVPATH=") + uei->GetDevPath());
    }
}

Int32 WiredAccessoryManager::WiredAccessoryObserver::ValidateSwitchState(
    /* [in] */ Int32 state)
{
    // Some drivers, namely HTC headset ones, add additional bits to
    // the switch state. As we only are able to deal with the states
    // 0, 1 and 2, mask out all the other bits
    return state & 0x3;
}

AutoPtr< List<AutoPtr<WiredAccessoryManager::WiredAccessoryObserver::UEventInfo> > >
WiredAccessoryManager::WiredAccessoryObserver::MakeObservedUEventList()
{
    AutoPtr< List<AutoPtr<WiredAccessoryManager::WiredAccessoryObserver::UEventInfo> > > retVal;
    retVal = new List<AutoPtr<WiredAccessoryManager::WiredAccessoryObserver::UEventInfo> >();
    AutoPtr<WiredAccessoryManager::WiredAccessoryObserver::UEventInfo> uei;

    // Monitor h2w
    if (!mUseDevInputEventForAudioJack) {
        uei = new UEventInfo(NAME_H2W, BIT_HEADSET, BIT_HEADSET_NO_MIC, BIT_LINEOUT);
        if (uei->CheckSwitchExists()) {
            retVal->PuashBack(uei);
        }
        else {
            Slogger::W(mHost->TAG, "This kernel does not have wired headset support");
        }
    }

    // Monitor USB
    uei = new UEventInfo(NAME_USB_AUDIO, BIT_USB_HEADSET_ANLG, BIT_USB_HEADSET_DGTL, 0);
    if (uei->CheckSwitchExists()) {
        retVal->PushBack(uei);
    }
    else {
        Slogger::W(mHost->TAG, "This kernel does not have usb audio support");
    }

    // Monitor HDMI
    //
    // If the kernel has support for the "hdmi_audio" switch, use that.  It will be
    // signalled only when the HDMI driver has a video mode configured, and the downstream
    // sink indicates support for audio in its EDID.
    //
    // If the kernel does not have an "hdmi_audio" switch, just fall back on the older
    // "hdmi" switch instead.
    uei = new UEventInfo(NAME_HDMI_AUDIO, BIT_HDMI_AUDIO, 0, 0);
    if (uei->CheckSwitchExists()) {
        retVal->PushBack(uei);
    }
    else {
        uei = new UEventInfo(NAME_HDMI, BIT_HDMI_AUDIO, 0, 0);
        if (uei->CheckSwitchExists()) {
            retVal->PushBack(uei);
        }
        else {
            Slogger::W(mHost->TAG, "This kernel does not have HDMI audio support");
        }
    }

    return retVal;
}

// @Override
ECode WiredAccessoryManager::WiredAccessoryObserver::OnUEvent(
    /* [in] */ IUEvent* event)
{
    if (mHost->LOG) Slogger::V(mHost->TAG, "Headset UEVENT: %s", Object::ToString(event).string());

//     try {
        String devPath;
        event->Get(String("DEVPATH"), &devPath);
        String name;
        event->Get(String("SWITCH_NAME"), &name);
        Int32 state = ValidateSwitchState(StringUtils::ParseInt32(name));//Integer.parseInt(event.get("SWITCH_STATE"));
        {
            AutoLock lock(mHost->mLock);
            UpdateStateLocked(devPath, name, state);
        }
//     } catch (NumberFormatException e) {
//         Slog.e(TAG, "Could not parse switch state from event " + event);
//     }
        return NOERROR;
}

void WiredAccessoryManager::WiredAccessoryObserver::UpdateStateLocked(
    /* [in] */ const String& devPath,
    /* [in] */ const String& name,
    /* [in] */ Int32 state)
{
    List<AutoPtr<WiredAccessoryManager::WiredAccessoryObserver::UEventInfo> >::Iterator it;
    for (it = mUEventInfo->Begin(); it != mUEventInfo->End(); ++it) {
        UEventInfo* uei = *it;
        if (devPath.Equals(uei->GetDevPath())) {
            mHost->UpdateLocked(name, uei->ComputeNewHeadsetState(mHost->mHeadsetState, state));
            return;
        }
    }
}

WiredAccessoryManager::WiredAccessoryManager(
    /* [in] */ IContext* context,
    /* [in] */ CInputManagerService* inputManager)
    : mHeadsetState(0)
    , mSwitchValues(0)
{
    mHandler = new MyHandler(Looper::GetMyLooper(), this);
    AutoPtr<IInterface> powerService;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&powerService);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(powerService);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("WiredAccessoryManager"), (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(FALSE);
    AutoPtr<IInterface> audioService;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&audioService);
    mAudioManager = IAudioManager::Probe(audioService);
    mInputManager = inputManager;

    AutoPtr<IResources> resouces;
    context->GetResources((IResources**)&resouces);
    resouces->GetBoolean(R::bool_::config_useDevInputEventForAudioJack, &mUseDevInputEventForAudioJack);

    mObserver = new WiredAccessoryObserver(this);
}

CAR_INTERFACE_IMPL(WiredAccessoryManager, Object, IWiredAccessoryCallbacks)

void WiredAccessoryManager::OnSystemReady()
{
    if (mUseDevInputEventForAudioJack) {
        Int32 switchValues = 0;
        if (mInputManager->GetSwitchState(-1, IInputDevice::SOURCE_ANY, CInputManagerService::SW_HEADPHONE_INSERT) == 1) {
            switchValues |= CInputManagerService::SW_HEADPHONE_INSERT_BIT;
        }
        if (mInputManager->GetSwitchState(-1, IInputDevice::SOURCE_ANY, CInputManagerService::SW_MICROPHONE_INSERT) == 1) {
            switchValues |= CInputManagerService::SW_MICROPHONE_INSERT_BIT;
        }
        if (mInputManager->GetSwitchState(-1, IInputDevice::SOURCE_ANY, CInputManagerService::SW_LINEOUT_INSERT) == 1) {
            switchValues |= CInputManagerService::SW_LINEOUT_INSERT_BIT;
        }

        NotifyWiredAccessoryChanged(0, switchValues,
            CInputManagerService::SW_HEADPHONE_INSERT_BIT
            | CInputManagerService::SW_MICROPHONE_INSERT_BIT
            | CInputManagerService::SW_LINEOUT_INSERT_BIT);
    }

    mObserver->Init();
}

// @Override
ECode WiredAccessoryManager::NotifyWiredAccessoryChanged(
    /* [in] */ Int64 whenNanos,
    /* [in] */ Int32 switchValues,
    /* [in] */ Int32 switchMask)
{
    if (LOG) Slogger::V(TAG, "notifyWiredAccessoryChanged: when=%d bits=%s mask=0x%x", whenNanos
            , SwitchCodeToString(switchValues, switchMask).string()
            , switchMask);

    {
        AutoLock lock(mLock);
        Int32 headset;
        mSwitchValues = (mSwitchValues & ~switchMask) | switchValues;
        switch (mSwitchValues & (CInputManagerService::SW_HEADPHONE_INSERT_BIT
            | CInputManagerService::SW_MICROPHONE_INSERT_BIT
            | CInputManagerService::SW_LINEOUT_INSERT_BIT)) {
            case 0:
                headset = 0;
                break;

            case CInputManagerService::SW_HEADPHONE_INSERT_BIT:
                headset = BIT_HEADSET_NO_MIC;
                break;

            case CInputManagerService::SW_LINEOUT_INSERT_BIT:
                headset = BIT_LINEOUT;
                break;

            case CInputManagerService::SW_HEADPHONE_INSERT_BIT | CInputManagerService::SW_MICROPHONE_INSERT_BIT:
                headset = BIT_HEADSET;
                break;

            case CInputManagerService::SW_MICROPHONE_INSERT_BIT:
                headset = BIT_HEADSET;
                break;

            default:
                headset = 0;
                break;
        }

        UpdateLocked(NAME_H2W, (mHeadsetState & ~(BIT_HEADSET | BIT_HEADSET_NO_MIC | BIT_LINEOUT)) | headset);
    }
    return NOERROR;
}

ECode WiredAccessoryManager::SystemReady()
{
    {    AutoLock syncLock(mLock);
        mWakeLock->AcquireLock();

        AutoPtr<IMessage> msg;
        mHandler->ObtainMessage(MSG_SYSTEM_READY, 0, 0, NULL, (IMessage**)&msg);
        Boolean bval;
        mHandler->SendMessage(msg, &bval);
    }
    return NOERROR;
}

void WiredAccessoryManager::UpdateLocked(
    /* [in] */ const String& newName,
    /* [in] */ Int32 newState)
{
    // Retain only relevant bits
    Int32 headsetState = newState & SUPPORTED_HEADSETS;
    Int32 usb_headset_anlg = headsetState & BIT_USB_HEADSET_ANLG;
    Int32 usb_headset_dgtl = headsetState & BIT_USB_HEADSET_DGTL;
    Int32 h2w_headset = headsetState & (BIT_HEADSET | BIT_HEADSET_NO_MIC | BIT_LINEOUT);
    Boolean h2wStateChange = TRUE;
    Boolean usbStateChange = TRUE;
    if (LOG) Slogger::V(TAG, "newName=%s newState=%d headsetState=%d prev headsetState=%d", newName.string()
            , newState
            , headsetState
            , mHeadsetState);

    if (mHeadsetState == headsetState) {
        Logger::E(TAG, "No state change.");
        return;
    }

    // reject all suspect transitions: only accept state changes from:
    // - a: 0 headset to 1 headset
    // - b: 1 headset to 0 headset
    if (h2w_headset == (BIT_HEADSET | BIT_HEADSET_NO_MIC | BIT_LINEOUT)) {
        Logger::E(TAG, "Invalid combination, unsetting h2w flag");
        h2wStateChange = FALSE;
    }
    // - c: 0 usb headset to 1 usb headset
    // - d: 1 usb headset to 0 usb headset
    if (usb_headset_anlg == BIT_USB_HEADSET_ANLG && usb_headset_dgtl == BIT_USB_HEADSET_DGTL) {
        Logger::E(TAG, "Invalid combination, unsetting usb flag");
        usbStateChange = FALSE;
    }
    if (!h2wStateChange && !usbStateChange) {
        Logger::E(TAG, "invalid transition, returning ...");
        return;
    }

    mWakeLock->AcquireLock();

    AutoPtr<ICharSequence> seq;
    CString::New(newName, (ICharSequence**)&seq);

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_NEW_DEVICE_STATE, (IMessage**)&msg);
    msg->SetArg1(headsetState);
    msg->SetArg2(mHeadsetState);
    msg->SetObj(seq);
    Boolean result;
    mHandler->SendMessage(msg, &result);

    mHeadsetState = headsetState;
}

void WiredAccessoryManager::SetDevicesState(
    /* [in] */ Int32 headsetState,
    /* [in] */ Int32 prevHeadsetState,
    /* [in] */ const String& headsetName)
{
    AutoLock lock(mLock);
    Int32 allHeadsets = SUPPORTED_HEADSETS;
    for (Int32 curHeadset = 1; allHeadsets != 0; curHeadset <<= 1) {
        if ((curHeadset & allHeadsets) != 0) {
            SetDeviceStateLocked(curHeadset, headsetState, prevHeadsetState, headsetName);
            allHeadsets &= ~curHeadset;
        }
    }
}

void WiredAccessoryManager::SetDeviceStateLocked(
    /* [in] */ Int32 headset,
    /* [in] */ Int32 headsetState,
    /* [in] */ Int32 prevHeadsetState,
    /* [in] */ const String& headsetName)
{
    if ((headsetState & headset) != (prevHeadsetState & headset)) {
        Int32 outDevice = 0;
        Int32 inDevice = 0;
        Int32 state;

        if ((headsetState & headset) != 0) {
            state = 1;
        }
        else {
            state = 0;
        }

        if (headset == BIT_HEADSET) {
            outDevice = IAudioManager::DEVICE_OUT_WIRED_HEADSET;
            inDevice = IAudioManager::DEVICE_IN_WIRED_HEADSET;
        }
        else if (headset == BIT_HEADSET_NO_MIC){
            outDevice = IAudioManager::DEVICE_OUT_WIRED_HEADPHONE;
        }
        else if (headset == BIT_LINEOUT){
            outDevice = IAudioManager::DEVICE_OUT_LINE;
        }
        else if (headset == BIT_USB_HEADSET_ANLG) {
            outDevice = IAudioManager::DEVICE_OUT_ANLG_DOCK_HEADSET;
        }
        else if (headset == BIT_USB_HEADSET_DGTL) {
            outDevice = IAudioManager::DEVICE_OUT_DGTL_DOCK_HEADSET;
        }
        else if (headset == BIT_HDMI_AUDIO) {
            outDevice = IAudioManager::DEVICE_OUT_HDMI;
        }
        else {
            Slogger::E(TAG, "setDeviceState() invalid headset type: %d", headset);
            return;
        }

        if (LOG) {
            if (state == 1) Slogger::V(TAG, "device %s connected", headsetName.string());
            else Slogger::V(TAG, "device %s disconnected", headsetName.string());
        }

        if (outDevice != 0) {
            mAudioManager->SetWiredDeviceConnectionState(outDevice, state, headsetName);
        }
        if (inDevice != 0) {
            mAudioManager->SetWiredDeviceConnectionState(inDevice, state, headsetName);
        }
    }
}

String WiredAccessoryManager::SwitchCodeToString(
    /* [in] */ Int32 switchValues,
    /* [in] */ Int32 switchMask)
{
    StringBuilder sb;
    if ((switchMask & CInputManagerService::SW_HEADPHONE_INSERT_BIT) != 0 &&
            (switchValues & CInputManagerService::SW_HEADPHONE_INSERT_BIT) != 0) {
        sb.Append(String("SW_HEADPHONE_INSERT "));
    }
    if ((switchMask & CInputManagerService::SW_MICROPHONE_INSERT_BIT) != 0 &&
            (switchValues & CInputManagerService::SW_MICROPHONE_INSERT_BIT) != 0) {
        sb.Append(String("SW_MICROPHONE_INSERT"));
    }
    return sb.ToString();
}

void WiredAccessoryManager::HandleMsgNewDeviceState(
    /* [in] */ Int32 headsetState,
    /* [in] */ Int32 prevHeadsetState,
    /* [in] */ const String& headsetName)
{
    SetDevicesState(headsetState, prevHeadsetState, headsetName);
    mWakeLock->ReleaseLock();
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
