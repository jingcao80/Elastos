
#ifndef __ELASTOS_DROID_SERVER_AUDIODEVICEMANAGEROBSERVER_H__
#define __ELASTOS_DROID_SERVER_AUDIODEVICEMANAGEROBSERVER_H__

#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/UEventObserver.h"

using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::View::IDisplayManagerAw;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * AudioDeviceManagerObserver monitors for audio devices on the main board.
 */
class AudioDeviceManagerObserver : public UEventObserver
{
private:
    class BootCompletedReceiver
        : public BroadcastReceiver
    {
    public:
        BootCompletedReceiver(
            /* [in] */ AudioDeviceManagerObserver* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("AudioDeviceManagerObserver::BootCompletedReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        AudioDeviceManagerObserver* mHost;
    };

    class ObservingTvReceiver
        : public BroadcastReceiver
    {
    public:
        ObservingTvReceiver(
            /* [in] */ AudioDeviceManagerObserver* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("AudioDeviceManagerObserver::ObservingTvReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        AudioDeviceManagerObserver* mHost;
    };

public:
    static CARAPI_(AutoPtr<AudioDeviceManagerObserver>) GetInstance(
        /* [in] */ IContext* context);

    CARAPI_(void) OnUEvent(
        /* [in] */ UEventObserver::UEvent* event);

    CARAPI_(void) UpdateState(
        /* [in] */ const String& name,
        /* [in] */ Int32 type,
        /* [in] */ Int32 state);

    CARAPI_(void) UpdateState(
        /* [in] */ const String& name,
        /* [in] */ Int32 type,
        /* [in] */ Int32 state,
        /* [in] */ const String& extraMng);

    CARAPI_(Int32) GetHeadphoneAvailableState();

    CARAPI_(Int32) GetHdmiPlugState();

private:
    AudioDeviceManagerObserver(
        /* [in] */ IContext* context);

    CARAPI_(void) Init();

    /** when the id is not NULL,it will find mapped androidName by id,
     *if the devName is not NULL,its device name will be set to devName,else not change the device name.*/
    CARAPI_(String) FindNameMap(
        /* [in] */ const String& id,
        /* [in] */ const String& devName,
        /* [in] */ Boolean audioIn);

    /** find mapped androidName by devName,if the reset is TRUE,its devName will reset to 'unknown' when find it */
    CARAPI_(String) FindNameMap(
        /* [in] */ const String& devName,
        /* [in] */ Boolean audioIn,
        /* [in] */ Boolean reset);

    CARAPI_(void) StartObservingTv(
        /* [in] */ IContext* context);

    CARAPI_(Int32) GetDevState(
        /* [in] */ const String& statePath);

    //fix me: In A20, the hdmi audio only be available after setting hdmi display mode,
    // it doesn't init itself in driver insmod.
    CARAPI_(void) NotifyHdmiAvailable();

public:
    static const String AUDIO_TYPE;
    static const Int32 AUDIO_INPUT_TYPE = 0;
    static const Int32 AUDIO_OUTPUT_TYPE = 1;
    static const String AUDIO_STATE;
    static const Int32 PLUG_IN = 1;
    static const Int32 PLUG_OUT = 0;
    static const String AUDIO_NAME;
    static const String EXTRA_MNG;
    static const String H2W_DEV;

private:
    static const String TAG;
    static const Boolean LOG;
    static const Int32 MAX_AUDIO_DEVICES = 16;
    static const String uAudioDevicesPath;
    static const String uEventSubsystem;
    static const String uEventDevPath;
    static const String uEventDevName;
    static const String uEventAction;
    static const String uPcmDev;
    static const String uAudioInType;
    static const String uAudioOutType;
    static const String sH2wDevPath;
    static const String sState;
    static const String sHdmiDevPath;

private:
    Int32 mUsbAudioCnt;

    AutoPtr<IContext> mContext;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;  // held while there is a pending route change
    AutoPtr<IDisplayManagerAw> mDisplayManager;

    /* use id,devName,androidName to identify a audio dev, the id is the key(one id to one dev)*/
    /*a device = {id,devNameForIn,devNameForOut,androidName}*/
    String mAudioNameMap[9][4];
    Object mLock;

    static AutoPtr<AudioDeviceManagerObserver> mAudioObserver;
    static Object mAudioObserverLock;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AUDIODEVICEMANAGEROBSERVER_H__
