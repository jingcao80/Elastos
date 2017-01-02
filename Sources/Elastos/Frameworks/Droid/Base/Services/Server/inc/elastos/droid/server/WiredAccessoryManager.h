//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_WIREDACCESSORYMANAGER_H__
#define __ELASTOS_DROID_SERVER_WIREDACCESSORYMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/input/CInputManagerService.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/UEventObserver.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::Server::Input::CInputManagerService;
using Elastos::Droid::Server::Input::IWiredAccessoryCallbacks;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {

class WiredAccessoryManager
    : public IWiredAccessoryCallbacks
    , public Object
{

friend class WiredAccessoryObserver;

public:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("WiredAccessoryManager::MyHandler")

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

        CARAPI OnUEvent(
            /* [in] */ IUEvent* event);

        CARAPI_(Int32) ValidateSwitchState(
            /* [in] */ Int32 state);

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

        TO_STRING_IMPL("WiredAccessoryManager::WiredAccessoryManagerReceiver: ")

    private:
        WiredAccessoryManager* mHost;
    };

public:
    WiredAccessoryManager(
        /* [in] */ IContext* context,
        /* [in] */ CInputManagerService* inputManager);

    CAR_INTERFACE_DECL()

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

    static Boolean mUseDevInputEventForAudioJack;

    AutoPtr<IHandler> mHandler;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_WIREDACCESSORYMANAGER_H__
