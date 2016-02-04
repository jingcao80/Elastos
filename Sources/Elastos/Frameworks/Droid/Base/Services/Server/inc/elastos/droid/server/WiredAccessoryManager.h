
#ifndef __ELASTOS_DROID_SERVER_WIREDACCESSORYMANAGER_H__
#define __ELASTOS_DROID_SERVER_WIREDACCESSORYMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/input/CInputManagerService.h"
#include "elastos/droid/os/UEventObserver.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Server::Input::CInputManagerService;

namespace Elastos {
namespace Droid {
namespace Server {

class WiredAccessoryManager
    : public CInputManagerService::WiredAccessoryCallbacks
{
friend class WiredAccessoryObserver;

public:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ WiredAccessoryManager* service)
            : Handler(looper, NULL, TRUE)
            , mHost(service)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        WiredAccessoryManager* mHost;
    };

    class WiredAccessoryObserver
        : public UEventObserver
    {
    private:
        class UEventInfo
            : public Object
        {
        public:
            UEventInfo(
                /* [in] */ const String& devName,
                /* [in] */ Int32 state1Bits,
                /* [in] */ Int32 state2Bits,
                /* [in] */ Int32 stateNbits);

            String GetDevName();

            String GetDevPath();

            String GetSwitchStatePath();

            Boolean CheckSwitchExists();

            Int32 ComputeNewHeadsetState(
                /* [in] */ Int32 headsetState,
                /* [in] */ Int32 switchState);

        private:
            String mDevName;
            Int32 mState1Bits;
            Int32 mState2Bits;
            Int32 mStateNbits;
        };

    public:
        WiredAccessoryObserver(
            /* [in] */ WiredAccessoryManager* host);

        ~WiredAccessoryObserver();

        CARAPI_(void) Init();

        CARAPI_(void) OnUEvent(
            /* [in] */ IUEvent* event);

    private:
        CARAPI_(AutoPtr< List<AutoPtr<UEventInfo> > >) MakeObservedUEventList();

        CARAPI_(void) UpdateStateLocked(
            /* [in] */ const String& devPath,
            /* [in] */ const String& name,
            /* [in] */ Int32 state);

    private:
        AutoPtr< List<AutoPtr<UEventInfo> > > mUEventInfo;

        WiredAccessoryManager* mHost;
    };

private:
    class WiredAccessoryManagerReceiver
        : public BroadcastReceiver
    {
    public:
        WiredAccessoryManagerReceiver(
            /* [in] */ WiredAccessoryManager* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* ctx,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("WiredAccessoryManager::WiredAccessoryManagerReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        WiredAccessoryManager* mHost;
    };

public:
    WiredAccessoryManager(
        /* [in] */ IContext* context,
        /* [in] */ CInputManagerService* inputManager);

    CARAPI NotifyWiredAccessoryChanged(
        /* [in] */ Int64 whenNanos,
        /* [in] */ Int32 switchValues,
        /* [in] */ Int32 switchMask);

    CARAPI SystemReady();

private:
    CARAPI_(void) OnSystemReady();

    CARAPI_(void) UpdateLocked(
        /* [in] */ const String& newName,
        /* [in] */ Int32 newState);

    CARAPI_(void) SetDevicesState(
        /* [in] */ Int32 headsetState,
        /* [in] */ Int32 prevHeadsetState,
        /* [in] */ const String& headsetName);

    CARAPI_(void) SetDeviceStateLocked(
        /* [in] */ Int32 headset,
        /* [in] */ Int32 headsetState,
        /* [in] */ Int32 prevHeadsetState,
        /* [in] */ const String& headsetName);

    CARAPI_(String) SwitchCodeToString(
        /* [in] */ Int32 switchValues,
        /* [in] */ Int32 switchMask);

    CARAPI_(void) HandleMsgNewDeviceState(
        /* [in] */ Int32 headsetState,
        /* [in] */ Int32 prevHeadsetState,
        /* [in] */ const String& headsetName);

private:
    static const String TAG;
    static const Boolean LOG = TRUE;

    static const Int32 BIT_HEADSET = (1 << 0);
    static const Int32 BIT_HEADSET_NO_MIC = (1 << 1);
    static const Int32 BIT_USB_HEADSET_ANLG = (1 << 2);
    static const Int32 BIT_USB_HEADSET_DGTL = (1 << 3);
    static const Int32 BIT_HDMI_AUDIO = (1 << 4);
    static const Int32 BIT_LINEOUT = (1 << 4);
    static const Int32 SUPPORTED_HEADSETS =
        (BIT_HEADSET|BIT_HEADSET_NO_MIC|
        BIT_USB_HEADSET_ANLG|BIT_USB_HEADSET_DGTL|
        BIT_HDMI_AUDIO|BIT_LINEOUT);

    static const String NAME_H2W;
    static const String NAME_USB_AUDIO;
    static const String NAME_HDMI_AUDIO;
    static const String NAME_HDMI;

    static const Int32 MSG_NEW_DEVICE_STATE = 1;
    static const Int32 MSG_SYSTEM_READY = 2;


    Object mLock;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;  // held while there is a pending route change
    AutoPtr<IAudioManager> mAudioManager;

    Int32 mHeadsetState;

    Int32 mSwitchValues;

    AutoPtr<WiredAccessoryObserver> mObserver;
    AutoPtr<CInputManagerService> mInputManager;

    Boolean mUseDevInputEventForAudioJack;

    AutoPtr<IHandler> mHandler;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_WIREDACCESSORYMANAGER_H__
