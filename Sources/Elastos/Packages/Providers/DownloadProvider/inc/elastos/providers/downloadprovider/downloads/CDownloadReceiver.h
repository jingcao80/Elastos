
#ifndef __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_CDOWNLOADRECEIVER_H__
#define __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_CDOWNLOADRECEIVER_H__

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Providers_DownloadProvider_Downloads_CDownloadReceiver.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IPendingResult;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

/**
 * Receives system broadcasts (boot, network connectivity)
 */
CarClass(CDownloadReceiver)
    , public BroadcastReceiver
    , public IDownloadReceiver
{
public:
    class Runnable_1
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        Runnable_1(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent,
            /* [in] */ IPendingResult* result,
            /* [in] */ CDownloadReceiver* host);

        CARAPI Run();

    public:
        AutoPtr<IContext> mContext;
        AutoPtr<IIntent> mIntent;
        AutoPtr<IPendingResult> mResult;
        CDownloadReceiver* mHost;
    };

    class Runnable_2
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        Runnable_2(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent,
            /* [in] */ IPendingResult* result,
            /* [in] */ CDownloadReceiver* host);

        CARAPI Run();

    public:
        AutoPtr<IContext> mContext;
        AutoPtr<IIntent> mIntent;
        AutoPtr<IPendingResult> mResult;
        CDownloadReceiver* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI OnReceive(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) HandleUidRemoved(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Handle any broadcast related to a system notification.
     */
    CARAPI_(void) HandleNotificationBroadcast(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Mark the given {@link DownloadManager#COLUMN_ID} as being acknowledged by
     * user so it's not renewed later.
     */
    CARAPI_(void) HideNotification(
        /* [in] */ IContext* context,
        /* [in] */ Int64 id);

    /**
     * Start activity to display the file represented by the given
     * {@link DownloadManager#COLUMN_ID}.
     */
    CARAPI_(void) OpenDownload(
        /* [in] */ IContext* context,
        /* [in] */ Int64 id);

    /**
     * Notify the owner of a running download that its notification was clicked.
     */
    CARAPI_(void) SendNotificationClickedIntent(
        /* [in] */ IContext* context,
        /* [in] */ ArrayOf<Int64>* ids);

    static CARAPI_(String) GetString(
        /* [in] */ ICursor* cursor,
        /* [in] */ String col);

    static CARAPI_(Int32) GetInt32(
        /* [in] */ ICursor* cursor,
        /* [in] */ String col);

    CARAPI_(void) StartService(
        /* [in] */ IContext* context);

private:
    static AutoPtr<IHandler> sAsyncHandler;

    AutoPtr<ISystemFacade> mSystemFacade;
};

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos

#endif //__ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_CDOWNLOADRECEIVER_H__
