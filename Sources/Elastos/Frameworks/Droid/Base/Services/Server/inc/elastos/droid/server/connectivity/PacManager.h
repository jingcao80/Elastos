
#ifndef __ELASTOS_DROID_SERVER_CONNECTIVITY_PAC_MANAGER_H__
#define __ELASTOS_DROID_SERVER_CONNECTIVITY_PAC_MANAGER_H__

#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/os/Runnable.h>
#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.Net.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IServiceConnection;
// using Elastos::Droid::Net::IIProxyService;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Provider::ISettings;


using Elastos::Net::IURL;
using Elastos::Net::IURLConnection;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

class PacManager
    : public Object
{
private:

    class ServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        ServiceConnection(
            /* [in] */ PacManager* host);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* component);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* component,
            /* [in] */ IBinder* binder);

    private:
        PacManager* mHost;
    };

    class ProxyConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        ProxyConnection(
            /* [in] */ PacManager* host);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* component);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* component,
            /* [in] */ IBinder* binder);

    private:
        PacManager* mHost;
    };

    class PacDownloaderRunnable
        : public Runnable
    {
    public:
        PacDownloaderRunnable(
            /* [in] */ PacManager* host);

        //@Override
        CARAPI Run();

    private:
        PacManager* mHost;
    };

    class PacRefreshIntentReceiver
        : public BroadcastReceiver
    {
    public:
        PacRefreshIntentReceiver(
            /* [in] */ PacManager* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str)
        {
            VALIDATE_NOT_NULL(str)
            *str = String("PacManager::PacRefreshIntentReceiver");
            return NOERROR;
        }
    private:
        PacManager* mHost;
    };

public:

    PacManager(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ Int32 proxyMessage);

    AutoPtr<IAlarmManager> GetAlarmManager();

    /**
     * Updates the PAC Manager with current Proxy information. This is called by
     * the ConnectivityService directly before a broadcast takes place to allow
     * the PacManager to indicate that the broadcast should not be sent and the
     * PacManager will trigger a new broadcast when it is ready.
     *
     * @param proxy Proxy information that is about to be broadcast.
     * @return Returns true when the broadcast should not be sent
     */
    Boolean SetCurrentProxyScriptUrl(
        /* [in] */ IProxyInfo* proxy);

private:

    /**
     * Does a post and reports back the status code.
     *
     * @throws IOException
     */
    static CARAPI Get(
        /* [in] */ IUri* pacUri,
        /* [out] */ String* str);

    Int32 GetNextDelay(
        /* [in] */ Int32 currentDelay);

    void LongSchedule();

    void Reschedule();

    String GetPacChangeDelay();

    Int64 GetDownloadDelay(
        /* [in] */ Int32 delayIndex);

    void SetDownloadIn(
        /* [in] */ Int32 delayIndex);

    Boolean SetCurrentProxyScript(
        /* [in] */ const String& script);

    void Bind();

    void Unbind();

    void SendPacBroadcast(
        /* [in] */ IProxyInfo* proxy);

    void SendProxyIfNeeded();

public:
    static const String PAC_PACKAGE;
    static const String PAC_SERVICE;
    static const String PAC_SERVICE_NAME;

    static const String PROXY_PACKAGE;
    static const String PROXY_SERVICE;

private:
    static const String TAG;

    static const String ACTION_PAC_REFRESH;

    static const String DEFAULT_DELAYS;
    static const Int32 DELAY_1;
    static const Int32 DELAY_4;
    static const Int32 DELAY_LONG;

    /** Keep these values up-to-date with ProxyService.java */
    static const String KEY_PROXY;
    String mCurrentPac;
    // @GuardedBy("mProxyLock")
    AutoPtr<IUri> mPacUrl;

    AutoPtr<IAlarmManager> mAlarmManager;
    // @GuardedBy("mProxyLock")
    // AutoPtr<IIProxyService> mProxyService;
    AutoPtr<IPendingIntent> mPacRefreshIntent;
    AutoPtr<IServiceConnection> mConnection;
    AutoPtr<IServiceConnection> mProxyConnection;
    AutoPtr<IContext> mContext;

    Int32 mCurrentDelay;
    Int32 mLastPort;

    Boolean mHasSentBroadcast;
    Boolean mHasDownloaded;

    AutoPtr<IHandler> mConnectivityHandler;
    Int32 mProxyMessage;

    /**
     * Used for locking when setting mProxyService and all references to mPacUrl or mCurrentPac.
     */
    Object mProxyLock;

    AutoPtr<IRunnable> mPacDownloader;
};

} // Connectivity
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CONNECTIVITY_PAC_MANAGER_H__
