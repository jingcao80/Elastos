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

#include "UsbCameraDeviceManagerObserver.h"
#include "elastos/droid/app/ActivityManagerNative.h"

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::EIID_IBroadcastReceiver;
using Elastos::Droid::Hardware::CHardwareCameraHelper;
using Elastos::Droid::Hardware::IHardwareCameraHelper;
using Elastos::Droid::Hardware::Usb::IUsbCameraManager;

namespace Elastos {
namespace Droid {
namespace Server {

const String UsbCameraDeviceManagerObserver::TAG("UsbCameraDeviceManagerObserver");// = UsbCameraDeviceManagerObserver.class.getSimpleName();

const String UsbCameraDeviceManagerObserver::uUsbCameraDevicesPath("/sys/class/video4linux");
const String UsbCameraDeviceManagerObserver::uEventSubsystem("SUBSYSTEM=video");
const String UsbCameraDeviceManagerObserver::uEventDevPath("DEVPATH");
const String UsbCameraDeviceManagerObserver::uEventDevName("DEVNAME");
const String UsbCameraDeviceManagerObserver::uEventAction("ACTION");
const String UsbCameraDeviceManagerObserver::uVideoDev("video");
AutoPtr<UsbCameraDeviceManagerObserver> UsbCameraDeviceManagerObserver::mUsbCameraObserver;
Mutex UsbCameraDeviceManagerObserver::mStaticLock;

CARAPI UsbCameraDeviceManagerObserver::BootCompletedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    //monitor usb camera
    mHost->StartObserving(uEventSubsystem);
    AutoPtr<IHardwareCameraHelper> helper;
    CHardwareCameraHelper::AcquireSingleton((IHardwareCameraHelper**)&helper);
    helper->GetNumberOfCameras(&mHost->mUsbCameraTotalNumber);
    Logger::D(TAG, "UsbCameraDeviceManagerObserver startObserving mUsbCameraTotalNumber = %d", mHost->mUsbCameraTotalNumber );
    return NOERROR;
}

UsbCameraDeviceManagerObserver::UsbCameraDeviceManagerObserver(
    /* [in] */ IContext* context)
    : mUsbCameraTotalNumber(0)
    , mContext(context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&service);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(service);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, String("UsbCameraDeviceManagerObserver")
        , (IPowerManagerWakeLock**)&mWakeLock);
    mWakeLock->SetReferenceCounted(FALSE);

    Logger::D(TAG, "UsbCameraDeviceManagerObserver construct");

    AutoPtr<BootCompletedReceiver> receiver = new BootCompletedReceiver(this);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver((IBroadcastReceiver*)receiver->Probe(EIID_IBroadcastReceiver) ,filter
            , String(NULL), NULL, (IIntent**)&intent);
}

AutoPtr<UsbCameraDeviceManagerObserver> UsbCameraDeviceManagerObserver::GetInstance(
    /* [in] */ IContext* context)
{
    AutoLock lock(mStaticLock);
    if(mUsbCameraObserver == NULL){
        mUsbCameraObserver = new UsbCameraDeviceManagerObserver(context);
    }
    return mUsbCameraObserver;
}

void UsbCameraDeviceManagerObserver::OnUEvent(
    /* [in] */ IUEvent* event)
{
    Logger::D(TAG, "UsbCamera device change: %s", event->ToString().string());
//     try{
        String devPath = event->Get(uEventDevPath);
        String devName = event->Get(uEventDevName);
        String action = event->Get(uEventAction);

        if(action.Equals("add")){
            mUsbCameraTotalNumber++;
            Logger::I(TAG,"action.equals(add) mUsbCameraTotalNumber = %d", mUsbCameraTotalNumber);
            UpdateState(devName, mUsbCameraTotalNumber, IUsbCameraManager::PLUG_IN);
        }else if(action.Equals("remove")){
            if(mUsbCameraTotalNumber>0){
                mUsbCameraTotalNumber--;
            }
            Logger::I(TAG,"action.equals(remove) mUsbCameraTotalNumber = %d", mUsbCameraTotalNumber);
            UpdateState(devName, mUsbCameraTotalNumber, IUsbCameraManager::PLUG_OUT);
        }
//     }catch(Exception e){
//         Slog.e(TAG,"",e);
//     }
}

void UsbCameraDeviceManagerObserver::UpdateState(
    /* [in] */ const String& name,
    /* [in] */ Int32 state ,
    /* [in] */ Int32 totalNum)
{
    AutoLock lock(mLock);
    UpdateState(name,totalNum,state,String(NULL));
}

void UsbCameraDeviceManagerObserver::UpdateState(
    /* [in] */ const String& name,
    /* [in] */ Int32 state,
    /* [in] */ Int32 totalNum,
    /* [in] */ const String& extraMng)
{
    AutoLock lock(mLock);
    Logger::D(TAG, "name: %s state: %d totalNum: %d", name.string(), state, totalNum);
    AutoPtr<IIntent> intent;

    CIntent::New(IUsbCameraManager::ACTION_USB_CAMERA_PLUG_IN_OUT, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutInt32(IUsbCameraManager::USB_CAMERA_STATE,state);
    bundle->PutString(IUsbCameraManager::USB_CAMERA_NAME,name);
    bundle->PutInt32(IUsbCameraManager::USB_CAMERA_TOTAL_NUMBER,totalNum);
    if(extraMng != NULL){
        bundle->PutString(IUsbCameraManager::EXTRA_MNG, extraMng);
    }
    intent->PutExtras(bundle);
    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
}

} // namespace Server
} // namespace Droid
} // namespace Elastos