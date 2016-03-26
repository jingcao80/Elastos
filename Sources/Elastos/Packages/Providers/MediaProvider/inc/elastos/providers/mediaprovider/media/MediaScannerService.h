#ifndef _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIASCANNERSERVICE_H__
#define _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIASCANNERSERVICE_H__

#include "_Elastos.Providers.MediaProvider.Media.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Media::IMediaScanner;
using Elastos::Droid::Media::EIID_IIMediaScannerService;
using Elastos::Droid::Media::IIMediaScannerListener;
using Elastos::Droid::Media::IIMediaScannerService;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Providers {
namespace MediaProvider {
namespace Media {

class MediaScannerService
    : public Service
    , public IMediaScannerService
    , public IRunnable
{
public:
    class MyMediaScannerService
        : public Object
        , public IBinder
        , public IIMediaScannerService
    {
        friend class MediaScannerService;

    public:
        MyMediaScannerService();

        virtual ~MyMediaScannerService();

        CARAPI constructor(
            /* [in] */ IMediaScannerService* owner);

        CAR_INTERFACE_DECL()

        CARAPI RequestScanFile(
            /* [in] */ const String& path,
            /* [in] */ const String& mimeType,
            /* [in] */ IIMediaScannerListener* listener);

        CARAPI ScanFile(
            /* [in] */ const String& path,
            /* [in] */ const String& mimeType);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        MediaScannerService* mOwner;
    };

    class ServiceHandler
        : public Handler
    {
        friend class MediaScannerService;

    public:
        ServiceHandler(
            /* [in] */ MediaScannerService* owner);

        CAR_INTERFACE_DECL()

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        MediaScannerService* mOwner;
    };

public:
    MediaScannerService();

    ~MediaScannerService();

    CAR_INTERFACE_DECL()

    CARAPI OnCreate();

    CARAPI OnStartCommand(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 startId,
        /* [out] */ Int32* result);

    CARAPI OnDestroy();

    CARAPI Run();

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** result);

private:
    CARAPI_(void) OpenDatabase(
        /* [in] */ const String& volumeName);

    CARAPI_(AutoPtr<IMediaScanner>) CreateMediaScanner();

    CARAPI_(void) Scan(
        /* [in] */ ArrayOf<String>* directories,
        /* [in] */ const String& volumeName);

    CARAPI ScanFile(
        /* [in] */ const String& path,
        /* [in] */ const String& mimeType,
        /* [out] */ IUri** result);

private:
    static const String TAG;
    AutoPtr<ILooper> mServiceLooper;
    AutoPtr<ServiceHandler> mServiceHandler;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    static AutoPtr<ArrayOf<String> > mExternalStoragePaths;
    AutoPtr<IIMediaScannerService> mBinder;
};

} // namespace Media
} // namespace MediaProvider
} // namespace Providers
} // namespace Elastos

#endif // _ELASTOS_PROVIDERS_MEDIAPROVIDER_MEDIASCANNERSERVICE_H__