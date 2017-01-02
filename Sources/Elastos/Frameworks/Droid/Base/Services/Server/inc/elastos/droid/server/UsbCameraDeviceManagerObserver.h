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

#ifndef __ELASTOS_DROID_SERVER_USBCAMERADEVICEMANAGEROBSERVER_H__
#define __ELASTOS_DROID_SERVER_USBCAMERADEVICEMANAGEROBSERVER_H__

#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/UEventObserver.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::UEventObserver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace Server {

class UsbCameraDeviceManagerObserver
    : public UEventObserver
{
private:
    class BootCompletedReceiver
        : public BroadcastReceiver
    {
    public:
        BootCompletedReceiver(
            /* [in] */ UsbCameraDeviceManagerObserver* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("UsbCameraDeviceManagerObserver::BootCompletedReceiver: ")
    private:
        UsbCameraDeviceManagerObserver* mHost;
    };

public:
    static AutoPtr<UsbCameraDeviceManagerObserver> GetInstance(
        /* [in] */ IContext* context);

    CARAPI_(void) OnUEvent(
        /* [in] */ IUEvent* event);

    CARAPI_(void) UpdateState(
        /* [in] */ const String& name,
        /* [in] */ Int32 state ,
        /* [in] */ Int32 totalNum);

    CARAPI_(void) UpdateState(
        /* [in] */ const String& name,
        /* [in] */ Int32 state,
        /* [in] */ Int32 totalNum,
        /* [in] */ const String& extraMng);

private:
    UsbCameraDeviceManagerObserver(
        /* [in] */ IContext* context);

private:
    static const String TAG;

    static const String uUsbCameraDevicesPath;
    static const String uEventSubsystem;
    static const String uEventDevPath;
    static const String uEventDevName;
    static const String uEventAction;
    static const String uVideoDev;

    Int32 mUsbCameraTotalNumber;

    AutoPtr<IContext> mContext;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;  // held while there is a pending route change
    Object mLock;

    static AutoPtr<UsbCameraDeviceManagerObserver> mUsbCameraObserver;
    static Object mStaticLock;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_USBCAMERADEVICEMANAGEROBSERVER_H__
