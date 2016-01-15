
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

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("UsbCameraDeviceManagerObserver::BootCompletedReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        UsbCameraDeviceManagerObserver* mHost;
    };

public:
    static AutoPtr<UsbCameraDeviceManagerObserver> GetInstance(
        /* [in] */ IContext* context);

    CARAPI_(void) OnUEvent(
        /* [in] */ UEventObserver::UEvent* event);

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
