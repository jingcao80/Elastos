#include "AudioDeviceManagerObserver.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IAudioManagerHelper;
using Elastos::Droid::Media::CAudioManagerHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String AudioDeviceManagerObserver::TAG("AudioDeviceManagerObserver");
const Boolean AudioDeviceManagerObserver::LOG = TRUE;
const Int32 AudioDeviceManagerObserver::MAX_AUDIO_DEVICES;
const String AudioDeviceManagerObserver::AUDIO_TYPE = String("audioType");
const Int32 AudioDeviceManagerObserver::AUDIO_INPUT_TYPE;
const Int32 AudioDeviceManagerObserver::AUDIO_OUTPUT_TYPE;
const String AudioDeviceManagerObserver::AUDIO_STATE = String("audioState");
const Int32 AudioDeviceManagerObserver::PLUG_IN;
const Int32 AudioDeviceManagerObserver::PLUG_OUT;
const String AudioDeviceManagerObserver::AUDIO_NAME = String("audioName");
const String AudioDeviceManagerObserver::EXTRA_MNG = String("extral_mng");
const String AudioDeviceManagerObserver::uAudioDevicesPath   = String("/sys/class/sound/");
const String AudioDeviceManagerObserver::uEventSubsystem     = String("SUBSYSTEM=sound");
const String AudioDeviceManagerObserver::uEventDevPath       = String("DEVPATH");
const String AudioDeviceManagerObserver::uEventDevName       = String("DEVNAME");
const String AudioDeviceManagerObserver::uEventAction        = String("ACTION");
const String AudioDeviceManagerObserver::uPcmDev             = String("snd/pcm");
const String AudioDeviceManagerObserver::uAudioInType = String("c");
const String AudioDeviceManagerObserver::uAudioOutType = String("p");
const String AudioDeviceManagerObserver::sH2wDevPath = String("/devices/virtual/switch/h2w");
const String AudioDeviceManagerObserver::sState = String("state");
const String AudioDeviceManagerObserver::H2W_DEV = String("AUDIO_H2W");
const String AudioDeviceManagerObserver::sHdmiDevPath = String("/devices/virtual/switch/hdmi");
AutoPtr<AudioDeviceManagerObserver> AudioDeviceManagerObserver::mAudioObserver;
Mutex AudioDeviceManagerObserver::mAudioObserverLock;

ECode AudioDeviceManagerObserver::BootCompletedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    // At any given time accessories could be inserted
    // one on the board, one on the dock and one on HDMI:
    // observe three UEVENTs
    mHost->Init();  // set initial status

    //monitor usb
    mHost->StartObserving(uEventSubsystem);

    //monitor h2w
    mHost->StartObserving(String("DEVPATH=") + sH2wDevPath);

    //monitor hdmi and cvbs,
    mHost->StartObservingTv(context);

    return NOERROR;
}

ECode AudioDeviceManagerObserver::ObservingTvReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_HDMISTATUS_CHANGED)) {
        Int32 hdmiplug;
        intent->GetInt32Extra(IDisplayManagerAw::EXTRA_HDMISTATUS, 0, &hdmiplug);
        if (hdmiplug == 1) {
            Int32 outputType;
            mHost->mDisplayManager->GetDisplayOutputType(0, &outputType);
            if (outputType == IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI) {
                Logger::D(TAG, "ACTION_HDMISTATUS_CHANGED:notifyHdmiAvailable");
                mHost->NotifyHdmiAvailable();
            }
            mHost->UpdateState(IAudioManager::AUDIO_NAME_HDMI, AUDIO_OUTPUT_TYPE, PLUG_IN);
        }
        else {
            mHost->UpdateState(IAudioManager::AUDIO_NAME_HDMI, AUDIO_OUTPUT_TYPE, PLUG_OUT);
        }

    }
    else if (action.Equals(IIntent::ACTION_TVDACSTATUS_CHANGED)) {
        Int32 tvdacplug;
        intent->GetInt32Extra(IDisplayManagerAw::EXTRA_TVSTATUS, 0, &tvdacplug);
        if (tvdacplug == 1) {
            //YPBPR plug in
            mHost->UpdateState(IAudioManager::AUDIO_NAME_CODEC, AUDIO_OUTPUT_TYPE, PLUG_IN);
        }
        else if (tvdacplug == 2) {
            //CVBS plug in
            mHost->UpdateState(IAudioManager::AUDIO_NAME_CODEC, AUDIO_OUTPUT_TYPE, PLUG_IN);
        }
        else {
            mHost->UpdateState(IAudioManager::AUDIO_NAME_CODEC, AUDIO_OUTPUT_TYPE, PLUG_OUT);
        }

    }
    else if (action.Equals(IIntent::ACTION_DISPLAY_OUTPUT_CHANGED)) {
        Int32 outputType;
        intent->GetInt32Extra(IDisplayManagerAw::EXTRA_DISPLAY_TYPE,
            IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_NONE, &outputType);
        if (outputType == IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI && mHost->GetHdmiPlugState() == PLUG_IN) {
            mHost->NotifyHdmiAvailable();
        }

    }

    return NOERROR;
}

AudioDeviceManagerObserver::AudioDeviceManagerObserver(
    /* [in] */ IContext* context)
    : mUsbAudioCnt(0)
{
    // {{String("audiocodec"),  String("unknown"),  String("unknown"),  String("AUDIO_CODEC")},
    // {String("sndhdmi"),     String("unknown"),  String("unknown"),  String("AUDIO_HDMI")},
    // {String("sndspdif"),    String("unknown"),  String("unknown"),  String("AUDIO_SPDIF")},
    // {String("unknown"),     String("unknown"),  String("unknown"),  String("unknown")},
    // {String("unknown"),     String("unknown"),  String("unknown"),  String("unknown")},
    // {String("unknown"),     String("unknown"),  String("unknown"),  String("unknown")},
    // {String("unknown"),     String("unknown"),  String("unknown"),  String("unknown")},
    // {String("unknown"),     String("unknown"),  String("unknown"),  String("unknown")},
    // {String("unknown"),     String("unknown"),  String("unknown"),  String("unknown")},};
    for (Int32 i = 0; i < 9; i++) {
        for(Int32 j = 0; j < 4; j++)
            mAudioNameMap[i][j] = String("unknown");
    }
    mAudioNameMap[0][0] = String("audiocodec");
    mAudioNameMap[1][0] = String("sndhdmi");
    mAudioNameMap[2][0] = String("sndspdif");
    mAudioNameMap[0][3] = String("AUDIO_CODEC");
    mAudioNameMap[1][3] = String("AUDIO_HDMI");
    mAudioNameMap[2][3] = String("AUDIO_SPDIF");

    mContext = context;
    AutoPtr<IInterface> ipm;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&ipm);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(ipm);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("AudioDeviceManagerObserver"),
        (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(FALSE);

    Logger::D(TAG, "AudioDeviceManagerObserver construct");
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&intentFilter);
    AutoPtr<BootCompletedReceiver> receiver = new BootCompletedReceiver(this);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(receiver, intentFilter, String(NULL), NULL, (IIntent**)&intent);
}

AutoPtr<AudioDeviceManagerObserver> AudioDeviceManagerObserver::GetInstance(
    /* [in] */ IContext* context)
{
    AutoLock lock(mAudioObserverLock);
    if (mAudioObserver == NULL) {
        mAudioObserver = new AudioDeviceManagerObserver(context);
    }
    return mAudioObserver;
}

void AudioDeviceManagerObserver::Init()
{
    AutoLock lock(mLock);
    AutoPtr<ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc(1024);

    String name_linux = String("unknown");
    String name_android = String("unknown");

    Logger::V(TAG, "AudioDeviceManagerObserver init()");

    for (Int32 card = 0; card < MAX_AUDIO_DEVICES; card++) {
        String newCard;
        newCard.AppendFormat("/sys/class/sound/card%d/", card);
        String newCardId = newCard + "id";
        Logger::D(TAG, "AudioDeviceManagerObserver: newCardId: %s", newCardId.string());

        ECode ec = NOERROR;
        do {
            AutoPtr<IFileReader> file;
            ec = CFileReader::New(newCardId, (IFileReader**)&file);
            if (FAILED(ec))
                break;
            Int32 len;
            ec = file->ReadChars(buffer, 0, 1024, &len);
            if (FAILED(ec))
                break;
            ec = ICloseable::Probe(file)->Close();
            if (FAILED(ec))
                break;

            name_linux = String(*buffer, 0, len).Trim();

            if (len > 0) {
                // SecureFile cardDir = new SecureFile(newCard);
                // String[] list = cardDir.list();
                Logger::D(TAG, "TODO: AudioDeviceManagerObserver.cpp, line: %d, SecureFile is not implemented", __LINE__);
                AutoPtr<ArrayOf<String> > list;
                if (card == 0) {
                    list = ArrayOf<String>::Alloc(2);
                    (*list)[0] = String("pcmC0D0p");
                    (*list)[1] = String("pcmC0D0c");
                }
                else if (card == 1) {
                    list = ArrayOf<String>::Alloc(1);
                    (*list)[0] = String("pcmC1D0p");
                }
                else if (card == 2) {
                    list = ArrayOf<String>::Alloc(2);
                    (*list)[0] = String("pcmC2D0p");
                }

                for (Int32 i = 0; i < list->GetLength(); i++) {
                    String name = (*list)[i];
                    if (name.StartWith("pcm")) {
                        Int32 length = name.GetLength();
                        String ext = name.Substring(length - 1, length);
                        Logger::D(TAG,"AudioDeviceManagerObserver: devName: %s", name.string());
                        if (ext.EqualsIgnoreCase(uAudioInType)) {
                            name_android = FindNameMap(name_linux, String("snd/") + name, TRUE);
                        }
                        else if (ext.EqualsIgnoreCase(uAudioOutType)) {
                            name_android = FindNameMap(name_linux, String("snd/") + name, FALSE);
                        }
                    }
                }
                Logger::D(TAG, "AudioDeviceManagerObserver: name_linux: %s, name_android: %s",
                    name_linux.string(), name_android.string());
            }
        } while(0);

        if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
            Logger::V(TAG, "This kernel does not have sound card %d", card);
            break;
        }
        else if (FAILED(ec)) {
            Logger::E(TAG, "ec = 0x%08x", ec);
        }
    }
}

/** when the id is not NULL,it will find mapped androidName by id,
 *if the devName is not NULL,its device name will be set to devName,else not change the device name.*/
String AudioDeviceManagerObserver::FindNameMap(
    /* [in] */ const String& id,
    /* [in] */ const String& devName,
    /* [in] */ Boolean audioIn)
{
    Logger::D(TAG, "~~~~~~~~ AudioDeviceManagerObserver: findNameMap: id: %s", id.string());
    String out;
    if (id != NULL) {
        if (devName == NULL) {
            devName = String("unknown");
        }
        for (Int32 index = 0; index < MAX_AUDIO_DEVICES; index++) {
            if (mAudioNameMap[index][0].Equals("unknown")) {
                mAudioNameMap[index][0] = id;
                if (audioIn) {
                    mAudioNameMap[index][1] = devName;
                }
                else {
                    mAudioNameMap[index][2] = devName;
                }
                mAudioNameMap[index][3] = String("");
                mAudioNameMap[index][3].AppendFormat("AUDIO_USB%d", mUsbAudioCnt++);

                out = mAudioNameMap[index][3];

                // Logger::D(TAG, "xxxx index: " + index + " in: " + mAudioNameMap[index][0] + " out:" + mAudioNameMap[index][1]);
                break;
            }

            if (mAudioNameMap[index][0].Equals(id)) {
                if (audioIn) {
                    mAudioNameMap[index][1] = devName;
                }
                else {
                    mAudioNameMap[index][2] = devName;
                }

                out = mAudioNameMap[index][3];
                // Logger::D(TAG, "qqqq index: " + index + " in: " + mAudioNameMap[index][0] + " out:" + mAudioNameMap[index][1]);
                break;
            }
        }
    }

    return out;
}

/** find mapped androidName by devName,if the reset is TRUE,its devName will reset to 'unknown' when find it */
String AudioDeviceManagerObserver::FindNameMap(
    /* [in] */ const String& devName,
    /* [in] */ Boolean audioIn,
    /* [in] */ Boolean reset)
{
    Logger::D(TAG,"~~~~~~~~ AudioDeviceManagerObserver: findNameMap: devName: ", devName.string());
    String out;
    if (devName != NULL && !devName.Equals("unknown")) {
        for (Int32 index = 0; index < MAX_AUDIO_DEVICES; index++) {
            if (audioIn && mAudioNameMap[index][1].Equals(devName)) {
                out = mAudioNameMap[index][3];
                if (reset) {
                    mAudioNameMap[index][1] = "unknown";
                }
                break;
            }
            else if (!audioIn && mAudioNameMap[index][2].Equals(devName)) {
                out = mAudioNameMap[index][3];
                if (reset) {
                    mAudioNameMap[index][2] = "unknown";
                }
                break;
            }
        }
    }
    return out;
}

void AudioDeviceManagerObserver::StartObservingTv(
    /* [in] */ IContext* context)
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_HDMISTATUS_CHANGED);
    filter->AddAction(IIntent::ACTION_TVDACSTATUS_CHANGED);
    filter->AddAction(IIntent::ACTION_DISPLAY_OUTPUT_CHANGED);
    AutoPtr<ObservingTvReceiver> receiver = new ObservingTvReceiver(this);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(receiver.Get(), filter, (IIntent**)&intent);

    //check whether the hdmi is available
    AutoPtr<IInterface> idm;
    mContext->GetSystemService(IContext::DISPLAY_SERVICE_AW, (IInterface**)&idm);
    mDisplayManager = IDisplayManagerAw::Probe(idm);
    Int32 curType;
    mDisplayManager->GetDisplayOutputType(0, &curType);
    if (curType == IDisplayManagerAw::DISPLAY_OUTPUT_TYPE_HDMI && GetHdmiPlugState() == PLUG_IN) {
        NotifyHdmiAvailable();
    }
}

void AudioDeviceManagerObserver::OnUEvent(
    /* [in] */ IUEvent* event)
{
    Logger::D(TAG, "Audio device change: %s", event->ToString().string());

    // try {
        String devPath = event->Get(uEventDevPath);
        String devName = event->Get(uEventDevName);
        String action = event->Get(uEventAction);
        String devCardPath;
        String sndName;
        String sndNameMap("unknown");
        String audioType;

        //deal with headphone
        if ((devPath != NULL) && (devPath.Equals(sH2wDevPath))) {
            String switchName = event->Get(String("SWITCH_NAME"));
            Int32 state = StringUtils::ParseInt32(event->Get(String("SWITCH_STATE")));
            Logger::D(TAG, "device %s%s", switchName.string(), ((state != 0) ? " connected" : " disconnected"));
            UpdateState(IAudioManager::AUDIO_NAME_CODEC, AUDIO_OUTPUT_TYPE, (state != 0) ? PLUG_IN : PLUG_OUT, H2W_DEV);
        }

        //deal with usb audio
        if ((devName != NULL) && (devName.Substring(0, 7).Equals(uPcmDev.Substring(0, 7)))) {
            Logger::D(TAG, "action: %s devName: %s devPath: %s", action.string(), devName.string(), devPath.string());
            AutoPtr<ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc(64);
            Int32 index = devPath.LastIndexOf("/");
            devCardPath = devPath.Substring(0, index);
            devCardPath = devCardPath + "/id";
            Int32 length = devName.GetLength();
            audioType = devName.Substring(length - 1, length);
            if (action.Equals("add")) {
                Int32 cnt = 10;
                while((cnt-- != 0)) {
                    ECode ec = NOERROR;
                    Boolean isBreak = FALSE;
                    do {
                        AutoPtr<IFileReader> file;
                        ec = CFileReader::New(String("sys") + devCardPath, (IFileReader**)&file);
                        if (FAILED(ec))
                            break;
                        Int32 len = 0;
                        ec = file->ReadChars(buffer, 0, 64, &len);
                        if (FAILED(ec))
                            break;
                        ec = ICloseable::Probe(file)->Close();
                        if (FAILED(ec))
                            break;

                        if (len > 0) {
                            sndName = String(*buffer, 0, len).Trim();
                            if (audioType.EqualsIgnoreCase(uAudioInType)) {
                                sndNameMap = FindNameMap(sndName, devName, TRUE);
                                UpdateState(sndNameMap, AUDIO_INPUT_TYPE, PLUG_IN);
                            }
                            else if (audioType.EqualsIgnoreCase(uAudioOutType)) {
                                sndNameMap = FindNameMap(sndName,devName, FALSE);
                                UpdateState(sndNameMap, AUDIO_OUTPUT_TYPE, PLUG_IN);
                            }
                            isBreak = TRUE;
                        }
                    } while(0);

                    if (isBreak)
                        break;

                    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
                        if (cnt == 0) {
                            Slogger::E(TAG, "can not read card id");
                            return ;
                        }

                        Slogger::W(TAG, "read card id, wait for a moment ......");
                        Thread::Sleep(10);
                    }
                    else if (FAILED(ec)) {
                        Slogger::E(TAG, "ec = 0x%08x", ec);
                    }
                }
            }
            else if (action.Equals("remove")) {
                // try{
                    if (audioType.EqualsIgnoreCase(uAudioInType)) {
                        sndNameMap = FindNameMap(devName, TRUE, TRUE);
                        UpdateState(sndNameMap, AUDIO_INPUT_TYPE, PLUG_OUT);
                    }
                    else if (audioType.EqualsIgnoreCase(uAudioOutType)) {
                        sndNameMap = FindNameMap(devName, FALSE, TRUE);
                        UpdateState(sndNameMap, AUDIO_OUTPUT_TYPE, PLUG_OUT);
                    }
                // }catch(Exception e) {
                //     Slog.e(TAG,"",e);
                // }
            }

        }
    // } catch (NumberFormatException e) {
    //     Slog.e(TAG, "Could not parse switch state from event " + event);
    // }
}

void AudioDeviceManagerObserver::UpdateState(
    /* [in] */ const String& name,
    /* [in] */ Int32 type,
    /* [in] */ Int32 state)
{
    UpdateState(name, type, state, String(NULL));
}

void AudioDeviceManagerObserver::UpdateState(
    /* [in] */ const String& name,
    /* [in] */ Int32 type,
    /* [in] */ Int32 state,
    /* [in] */ const String& extraMng)
{
    AutoLock lock(mLock);
    Logger::D(TAG, "name: %s, state: %d, type: %d", name.string(), state, type);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_AUDIO_PLUG_IN_OUT, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutInt32(AUDIO_STATE, state);
    bundle->PutString(AUDIO_NAME, name);
    bundle->PutInt32(AUDIO_TYPE, type);
    if (extraMng != NULL) {
        bundle->PutString(EXTRA_MNG, extraMng);
    }
    intent->PutExtras(bundle);
    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
}

Int32 AudioDeviceManagerObserver::GetDevState(
    /* [in] */ const String& statePath)
{
    ECode ec;
    do {
        Int32 buffer = 32;
        AutoPtr<IFile> file;
        ec = CFile::New(statePath, (IFile**)&file);
        if (FAILED(ec))
            break;
        AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(buffer);
        String d;
        AutoPtr<IFileInputStream> fis;
        ec = CFileInputStream::New(file, (IFileInputStream**)&fis);
        if (FAILED(ec))
            break;
        AutoPtr<IBufferedInputStream> i_stream;
        ec = CBufferedInputStream::New(fis, (IBufferedInputStream**)&i_stream);
        if (FAILED(ec))
            break;
        Int32 i = 0;
        ec = i_stream->ReadBytes(data, 0, buffer, &i);
        if (FAILED(ec))
            break;
        if (i != -1) {
            d = String((char*)data->GetPayload(), 1);
        }
        Int32 state = StringUtils::ParseInt32(d);
        return state;
    } while(0);

    Slogger::E(TAG, "Cound not parse switch state for %s ,fail because 0x%08x", statePath.string(), ec);
    return 0;
}

Int32 AudioDeviceManagerObserver::GetHeadphoneAvailableState()
{
    String h2wState = String("/sys") + sH2wDevPath + "/" + sState;
    Int32 state = GetDevState(h2wState);
    if (state == 0) {
        return IAudioSystem::DEVICE_STATE_UNAVAILABLE;
    }
    else if (state == 2) {
        return IAudioSystem::DEVICE_STATE_AVAILABLE;
    }
    else {
        return IAudioSystem::DEVICE_STATE_UNAVAILABLE;
    }
}

//fix me: In A20, the hdmi audio only be available after setting hdmi display mode,
// it doesn't init itself in driver insmod.
void AudioDeviceManagerObserver::NotifyHdmiAvailable()
{
    Logger::D(TAG, "hdmi is available");
    AutoPtr<IAudioManagerHelper> audioManagerSystemHelper;
    CAudioManagerHelper::AcquireSingleton((IAudioManagerHelper**)&audioManagerSystemHelper);
    // audioManagerSystemHelper->SetHdmiAvailable(TRUE);

    // Thread::GetCurrentThread()->Sleep(100);
    Thread::Sleep(100);

    //if the hdmi output is expected, add it to current actived audio devices.
    AutoPtr<IInterface> iam;
    mContext->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&iam);
    AutoPtr<IAudioManager> mng = IAudioManager::Probe(iam);
    AutoPtr<ArrayOf<String> > list;
    mng->GetActiveAudioDevices(IAudioManager::AUDIO_OUTPUT_ACTIVE, (ArrayOf<String>**)&list);
    // Boolean res;
    // if (audioManagerSystemHelper->GetHdmiExpected(&res), res) {
    //     mng->SetAudioDeviceActive(*list, IAudioManager::AUDIO_OUTPUT_ACTIVE);
    // }
}

Int32 AudioDeviceManagerObserver::GetHdmiPlugState()
{
    // String hdmiState = String("/sys") + hdmiDevPath + "/" + sState;
    Int32 state;
    mDisplayManager->GetHdmiHotPlugStatus(&state);
    if (state == 1) {
        return PLUG_IN;
    }
    else {
        return PLUG_OUT;
    }
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
