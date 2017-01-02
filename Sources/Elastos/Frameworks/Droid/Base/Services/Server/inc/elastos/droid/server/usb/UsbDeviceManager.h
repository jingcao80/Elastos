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

#ifndef __ELASTOS_DROID_SERVER_USB_USBDEVICEMANAGER_H__
#define __ELASTOS_DROID_SERVER_USB_USBDEVICEMANAGER_H__

#include "elastos/droid/server/usb/UsbSettingsManager.h"
#include "elastos/droid/server/usb/UsbDebuggingManager.h"
#include "elastos/droid/os/UEventObserver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Etl::HashMap;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Hardware::Usb::IUsbAccessory;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;

//class UsbSettingsManager;
namespace Elastos {
namespace Droid {
namespace Server {
namespace Usb {

typedef List< Pair<String, String> > StringPairList;

/**
 * UsbDeviceManager manages USB state in device mode.
 */
class UsbDeviceManager : public Object
{
private:
    class AdbSettingsObserver : public ContentObserver
    {
    public:
        TO_STRING_IMPL("UsbDeviceManager::AdbSettingsObserver")

        CARAPI constructor(
            /* [in] */ UsbDeviceManager* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        UsbDeviceManager* mHost;
    };

    /*
     * Listens for uevent messages from the kernel to monitor the USB state
     */
    class MyUEventObserver : public UEventObserver
    {
    public:
        MyUEventObserver(
            /* [in] */ UsbDeviceManager* host);

        CARAPI OnUEvent(
            /* [in] */ IUEvent* event);

    private:
        UsbDeviceManager* mHost;
    };

    class UsbHandler
        : public Handler
    {
    private:
        class BootCompletedReceiver : public BroadcastReceiver
        {
        public:
            BootCompletedReceiver(
                /* [in] */ UsbHandler* host);

            CARAPI OnReceive(
                /* [in] */ IContext* context,
                /* [in] */ IIntent* intent);

            TO_STRING_IMPL("UsbHandler::BootCompletedReceiver: ")
        private:
            UsbHandler* mHost;
        };

        class UserSwitchedReceiver : public BroadcastReceiver
        {
        public:
            UserSwitchedReceiver(
                /* [in] */ UsbHandler* host);

            CARAPI OnReceive(
                /* [in] */ IContext* context,
                /* [in] */ IIntent* intent);

            TO_STRING_IMPL("UsbHandler::UserSwitchedReceiver: ")
        private:
            UsbHandler* mHost;
        };

        class AdbNotificationObserver : public ContentObserver
        {
        public:
            TO_STRING_IMPL("UsbDeviceManager::UsbHandler::AdbNotificationObserver")

            CARAPI constructor(
                /* [in] */ UsbHandler* host)
            {
                ContentObserver::constructor(NULL);
                mHost = host;
                return NOERROR;
            }

            CARAPI OnChange(
                /* [in] */ Boolean selfChange)
            {
                mHost->UpdateAdbNotification();
                return NOERROR;
            }

        private:
            UsbHandler* mHost;
        };

    public:
        TO_STRING_IMPL("UsbDeviceManager::UsbHandler")

        UsbHandler(
            /* [in] */ UsbDeviceManager* host,
            /* [in] */ ILooper* looper);

        CARAPI Init();

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI_(void) UpdateState(
            /* [in] */ const String& state);

        using Handler::SendMessage;

        void SendMessage(
            /* [in] */ Int32 what,
            /* [in] */ Boolean arg);

        void SendMessage(
            /* [in] */ Int32 what,
            /* [in] */ IInterface* arg);

        void SendMessage(
            /* [in] */ Int32 what,
            /* [in] */ IInterface* arg1,
            /* [in] */ Boolean arg2);

        CARAPI_(void) HandleMsgUpdateState(
            /* [in] */ Boolean connected,
            /* [in] */ Boolean configured);

        CARAPI_(void) HandleMsgEnableAdb(
            /* [in] */ Boolean enabled);

        CARAPI_(void) HandleMsgSetCurrentFunctions(
            /* [in] */ const String& str,
            /* [in] */ Boolean useDeafault);

        CARAPI_(void) HandleMsgSystemReady();

        CARAPI_(void) HandleMsgBootCompleted();

        CARAPI_(void) HandleMsgUserSwitched(
            /* [in] */ Int32 currentUser);

        CARAPI_(AutoPtr<IUsbAccessory>) GetCurrentAccessory();

    private:
        CARAPI_(Boolean) WaitForState(
            /* [in] */ const String& state);

        CARAPI_(Boolean) SetUsbConfig(
            /* [in] */ const String& config);

        CARAPI_(void) SetAdbEnabled(
            /* [in] */ Boolean enable);

        CARAPI_(void) SetEnabledFunctions(
            /* [in] */ const String& functions,
            /* [in] */ Boolean makeDefault);

        CARAPI_(void) UpdateCurrentAccessory();

        CARAPI_(void) UpdateUsbState();

        CARAPI_(void) UpdateAudioSourceFunction();

        CARAPI_(void) UpdateUsbNotification();

        CARAPI_(void) UpdateAdbNotification();

        // public void dump(FileDescriptor fd, PrintWriter pw)

    private:
        // current USB state
        UsbDeviceManager* mHost;
        Boolean mConnected;
        Boolean mConfigured;
        String mCurrentFunctions;
        String mDefaultFunctions;
        AutoPtr<IUsbAccessory> mCurrentAccessory;
        Int32 mUsbNotificationId;
        Int32 mAdbNotificationId;
        Int32 mCurrentUser;

        AutoPtr<IBroadcastReceiver> mBootCompletedReceiver;
        AutoPtr<IBroadcastReceiver> mUserSwitchedReceiver;
        AutoPtr<IBroadcastReceiver> mBootFastReceiver;
    };

public:
    UsbDeviceManager(
        /* [in] */ IContext* context);

    ~UsbDeviceManager();

    CARAPI_(void) SetCurrentSettings(
        /* [in] */ UsbSettingsManager* settings);

    CARAPI_(void) SystemReady();

    /* returns the currently attached USB accessory */
    CARAPI_(AutoPtr<IUsbAccessory>) GetCurrentAccessory();

    /* opens the currently attached USB accessory */
    CARAPI OpenAccessory(
        /* [in] */ IUsbAccessory* accessory,
        /* [out] */ IParcelFileDescriptor** pfd);

    CARAPI_(void) SetCurrentFunctions(
        /* [in] */ const String& functions,
        /* [in] */ Boolean makeDefault);

    CARAPI_(void) SetMassStorageBackingFile(
        /* [in] */ const String& path);

    CARAPI_(void) AllowUsbDebugging(
        /* [in] */ Boolean alwaysAllow,
        /* [in] */ const String& publicKey);

    CARAPI_(void) DenyUsbDebugging();

    CARAPI ClearUsbDebuggingKeys();

    // public void dump(FileDescriptor fd, PrintWriter pw);

private:
    CARAPI_(AutoPtr<UsbSettingsManager>) GetCurrentSettings();

    CARAPI_(void) StartAccessoryMode();

    static CARAPI_(void) InitRndisAddress();

    static CARAPI_(String) AddFunction(
        /* [in] */ const String& functions,
        /* [in] */ const String& function);

    static CARAPI_(String) RemoveFunction(
        /* [in] */ const String& functions,
        /* [in] */ const String& function);

    static CARAPI_(Boolean) ContainsFunction(
        /* [in] */ const String& functions,
        /* [in] */ const String& function);

    CARAPI_(void) ReadOemUsbOverrideConfig();

    CARAPI_(Boolean) NeedsOemUsbOverride();

    CARAPI_(String) ProcessOemUsbOverride(
        /* [in] */ const String& usbFunctions);

    CARAPI_(AutoPtr< ArrayOf<String> >) NativeGetAccessoryStrings();

    CARAPI_(AutoPtr<IParcelFileDescriptor>) NativeOpenAccessory();

    CARAPI_(Boolean) NativeIsStartRequested();

    CARAPI_(Int32) NativeGetAudioMode();

private:
    static const String TAG;
    static const Boolean DEBUG;

    static const String USB_STATE_MATCH;
    static const String ACCESSORY_START_MATCH;
    static const String FUNCTIONS_PATH;
    static const String STATE_PATH;
    static const String MASS_STORAGE_FILE_PATH;
    static const String RNDIS_ETH_ADDR_PATH;
    static const String AUDIO_SOURCE_PCM_PATH;

    static const Int32 AUDIO_MODE_NONE = 0;
    static const Int32 AUDIO_MODE_SOURCE = 1;

    static const Int32 MSG_UPDATE_STATE;
    static const Int32 MSG_ENABLE_ADB;
    static const Int32 MSG_SET_CURRENT_FUNCTIONS;
    static const Int32 MSG_SYSTEM_READY;
    static const Int32 MSG_BOOT_COMPLETED;
    static const Int32 MSG_USER_SWITCHED;

    // Delay for debouncing USB disconnects.
    // We often get rapid connect/disconnect events when enabling USB functions,
    // which need debouncing.
    static const Int32 UPDATE_DELAY;

    // Time we received a request to enter USB accessory mode
    Int64 mAccessoryModeRequestTime;

    // Timeout for entering USB request mode.
    // Request is cancelled if host does not configure device within 10 seconds.
    static const Int32 ACCESSORY_REQUEST_TIMEOUT;

    static const String BOOT_MODE_PROPERTY;

    static const String DRIVER_NAME;

    AutoPtr<UsbHandler> mHandler;

    Boolean mBootCompleted;

    Object mLock;

    AutoPtr<IContext> mContext;
    AutoPtr<IContentResolver> mContentResolver;
    // @GuardedBy("mLock")
    AutoPtr<UsbSettingsManager> mCurrentSettings;
    AutoPtr<INotificationManager> mNotificationManager;
    Boolean mHasUsbAccessory;
    Boolean mUseUsbNotification;
    Boolean mAdbEnabled;
    Boolean mAudioSourceEnabled;

    AutoPtr<HashMap<String, AutoPtr<StringPairList> > > mOemModeMap;

    AutoPtr<ArrayOf<String> > mAccessoryStrings;
    AutoPtr<UsbDebuggingManager> mDebuggingManager;
    AutoPtr<MyUEventObserver> mUEventObserver;
};

} // namespace Usb
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_USB_USBDEVICEMANAGER_H__
