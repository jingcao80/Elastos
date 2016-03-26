#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MTPSERVICE_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MTPSERVICE_H__

#include "_Elastos.Providers.MediaProvider.Media.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/app/Service.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/os/Runnable.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IPendingResult;
using Elastos::Droid::Mtp::IMtpDatabase;
using Elastos::Droid::Mtp::IMtpServer;
using Elastos::Droid::Mtp::IMtpStorage;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Storage::IStorageEventListener;
using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Os::Storage::IStorageVolume;
using Elastos::Droid::Os::Runnable;
using Elastos::Utility::Etl::HashMap;
using Elastos::Providers::MediaProvider::Media::IMtpService;
using Elastos::Providers::MediaProvider::Media::IIMtpService;

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

class MtpService
    : public Service
    , public IMtpService
{
public:
    class MyIMtpService
        : public Object
        , public IIMtpService
        , public IBinder
    {
        friend class MtpService;

    public:
        MyIMtpService();

        virtual ~MyIMtpService();

        CARAPI constructor(
            /* [in] */ IMtpService* owner);

        CAR_INTERFACE_DECL()

        CARAPI SendObjectAdded(
            /* [in] */ Int32 objectHandle);

        CARAPI SendObjectRemoved(
            /* [in] */ Int32 objectHandle);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        MtpService* mOwner;
    };

private:
    class MyStorageEventListener
        : public Object
        , public IStorageEventListener
    {
        friend class MtpService;

    public:
        MyStorageEventListener(
            /* [in] */ MtpService* owner);

        virtual ~MyStorageEventListener();

        CAR_INTERFACE_DECL()

        CARAPI OnStorageStateChanged(
            /* [in] */ const String& path,
            /* [in] */ const String& oldState,
            /* [in] */ const String& newState);

        CARAPI OnUsbMassStorageConnectionChanged(
            /* [in] */ Boolean connected);

    private:
        MtpService* mOwner;
    };

    class MyRunnable
        : public Runnable
    {
        friend class MtpService;

    public:
        MyRunnable(
            /* [in] */ MtpService* owner);

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        MtpService* mOwner;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
        friend class MtpService;

    public:
        MyBroadcastReceiver(
            /* [in] */ MtpService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        MtpService* mOwner;
    };

public:
    MtpService();

    virtual ~MtpService();

    CAR_INTERFACE_DECL();

    CARAPI OnCreate();

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    CARAPI OnDestroy();

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** result);

private:
    CARAPI_(void) AddStorageDevicesLocked();

    CARAPI_(void) UpdateDisabledStateLocked();

    CARAPI_(void) ManageServiceLocked();

    CARAPI_(void) VolumeMountedLocked(
        /* [in] */ const String& path);

    CARAPI_(void) AddStorageLocked(
        /* [in] */ IStorageVolume* volume);

    CARAPI_(void) RemoveStorageLocked(
        /* [in] */ IStorageVolume* volume);

private:
    static const String TAG;
    static const Boolean LOGD;

    // We restrict PTP to these subdirectories
    static const AutoPtr<ArrayOf<String> > PTP_DIRECTORIES;

    AutoPtr<IBroadcastReceiver> mReceiver;

    AutoPtr<IStorageEventListener> mStorageEventListener;

    AutoPtr<IMtpDatabase> mDatabase;
    static AutoPtr<IMtpServer> mServer;
    AutoPtr<IStorageManager> mStorageManager;
    /** Flag indicating if MTP is disabled due to keyguard */
    Boolean mMtpDisabled;
    Boolean mPtpMode;
    static HashMap<String, AutoPtr<IStorageVolume> > mVolumeMap;
    static HashMap<String, AutoPtr<IMtpStorage> > mStorageMap;
    AutoPtr<ArrayOf<IStorageVolume*> > mVolumes;

    static AutoPtr<IIMtpService> mBinder;
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MTPSERVICE_H__