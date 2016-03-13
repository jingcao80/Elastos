
#ifndef __ELASTOS_DROID_SERVER_MEDIA_RemoteDisplayProviderWatcher_H__
#define __ELASTOS_DROID_SERVER_MEDIA_RemoteDisplayProviderWatcher_H__

#include "elastos/droid/server/media/RemoteDisplayProviderProxy.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

extern "C" const InterfaceID EIID_IRemoteDisplayProviderWatcherCallback;

/**
 * Watches for remote display provider services to be installed.
 * Adds a provider to the media router for each registered service.
 *
 * @see RemoteDisplayProviderProxy
 */
class RemoteDisplayProviderWatcher : public Object
{
public:
    interface ICallback : public IInterface
    {
    public:
        virtual CARAPI AddProvider(
            /* [in] */ RemoteDisplayProviderProxy* provider) = 0;

        virtual CARAPI RemoveProvider(
            /* [in] */ RemoteDisplayProviderProxy* provider) = 0;
    };

private:
    class ScanPackagesReceiver : public BroadcastReceiver
    {
    public:
        ScanPackagesReceiver(
            /* [in] */ RemoteDisplayProviderWatcher* host)
            : mHost(host)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        RemoteDisplayProviderWatcher* mHost;
    };

    class ScanPackagesRunnable : public Runnable
    {
    public:
        ScanPackagesRunnable(
            /* [in] */ RemoteDisplayProviderWatcher* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        RemoteDisplayProviderWatcher* mHost;
    };

public:
    RemoteDisplayProviderWatcher(
        /* [in] */ IContext* context,
        /* [in] */ ICallback* callback,
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 userId);

    // CARAPI_(void) dump(PrintWriter pw, String prefix);

    CARAPI_(void) Start();

    CARAPI_(void) Stop();

private:
    CARAPI_(void) ScanPackages();

    CARAPI_(Boolean) VerifyServiceTrusted(
        /* [in] */ IServiceInfo* serviceInfo);

    CARAPI_(Boolean) HasCaptureVideoPermission(
        /* [in] */ const String& packageName);

    CARAPI_(Int32) FindProvider(
        /* [in] */ const String& packageName,
        /* [in] */ const String& className);

 private:
    static const String TAG;  // max. 23 chars
    static const Boolean DEBUG = FALSE;

    AutoPtr<IContext> mContext;
    AutoPtr<ICallback> mCallback;
    AutoPtr<IHandler> mHandler;
    Int32 mUserId;
    AutoPtr<IPackageManager> mPackageManager;

    AutoPtr<IArrayList> mProviders;
    Boolean mRunning;

    AutoPtr<IBroadcastReceiver> mScanPackagesReceiver;
    AutoPtr<IRunnable> mScanPackagesRunnable;
};

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_MEDIA_RemoteDisplayProviderWatcher_H__
