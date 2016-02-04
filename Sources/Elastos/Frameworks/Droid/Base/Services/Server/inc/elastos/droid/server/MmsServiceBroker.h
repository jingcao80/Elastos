#ifndef __ELASTOS_DROID_SERVER_MMSSERVICEBROKER_H__
#define __ELASTOS_DROID_SERVER_MMSSERVICEBROKER_H__

#include "elastos/droid/server/SystemService.h"

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Internal::Telephony::IIMms;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * This class is a proxy for MmsService APIs. We need this because MmsService runs
 * in phone process and may crash anytime. This manages a connection to the actual
 * MmsService and bridges the public SMS/MMS APIs with MmsService implementation.
 */
class MmsServiceBroker
    : public SystemService
{
public:

    // Service API calls implementation, proxied to the real MmsService in "com.android.mms.service"
    class BinderService
        : public Object
        , public IIMms
        , public IBinder
    {
    public:
        CARAPI constructor(
            /* [in] */ ISystemService* mmsServiceBroker);

        //@Override
        CARAPI SendMessage(
            /* [in] */ Int64 subId,
            /* [in] */ const String& callingPkg,
            /* [in] */ IUri* contentUri,
            /* [in] */ const String& locationUrl,
            /* [in] */ IBundle* configOverrides,
            /* [in] */ IPendingIntent* sentIntent);

        //@Override
        CARAPI DownloadMessage(
            /* [in] */ Int64 subId,
            /* [in] */ const String& callingPkg,
            /* [in] */ const String& locationUrl,
            /* [in] */ IUri* contentUri,
            /* [in] */ IBundle* configOverrides,
            /* [in] */ IPendingIntent* downloadedIntent);

        //@Override
        CARAPI UpdateMmsSendStatus(
            /* [in] */ Int32 messageRef,
            /* [in] */ ArrayOf<Byte>* pdu,
            /* [in] */ Int32 status);

        //@Override
        CARAPI UpdateMmsDownloadStatus(
            /* [in] */ Int32 messageRef,
            /* [in] */ Int32 status);

        //@Override
        CARAPI GetCarrierConfigValues(
            /* [in] */ Int64 subId,
            /* [out] */ IBundle* bundle);

        //@Override
        CARAPI ImportTextMessage(
            /* [in] */ const String& callingPkg,
            /* [in] */ const String& address,
            /* [in] */ Int32 type,
            /* [in] */ const String& text,
            /* [in] */ Int64 timestampMillis,
            /* [in] */ Boolean seen,
            /* [in] */ Boolean read,
            /* [out] */ IUri** uri);

        //@Override
        CARAPI ImportMultimediaMessage(
            /* [in] */ const String& callingPkg,
            /* [in] */ Uri contentUri,
            /* [in] */ const String& messageId,
            /* [in] */ Int64 timestampSecs,
            /* [in] */ Boolean seen,
            /* [in] */ Boolean read,
            /* [out] */ IUri** uri);

        //@Override
        CARAPI DeleteStoredMessage(
            /* [in] */ const String& callingPkg,
            /* [in] */ IUri* messageUri,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI DeleteStoredConversation(
            /* [in] */ const String& callingPkg,
            /* [in] */ Int64 conversationId,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI UpdateStoredMessageStatus(
            /* [in] */ const String& callingPkg,
            /* [in] */ IUri* messageUri,
            /* [in] */ IContentValues* statusValues,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI ArchiveStoredConversation(
            /* [in] */ const String& callingPkg,
            /* [in] */ Int64 conversationId,
            /* [in] */ Boolean archived,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI AddTextMessageDraft(
            /* [in] */ const String& callingPkg,
            /* [in] */ const String& address,
            /* [in] */ const String& text,
            /* [out] */ IUri** uri);

        //@Override
        CARAPI AddMultimediaMessageDraft(
            /* [in] */ const String& callingPkg,
            /* [in] */ IUri* contentUri,
            /* [out] */ IUri** uri);

        //@Override
        CARAPI SendStoredMessage(
            /* [in] */ Int64 subId,
            /* [in] */ const String& callingPkg,
            /* [in] */ IUri* messageUri,
            /* [in] */ IBundle* configOverrides,
            /* [in] */ IPendingIntent* sentIntent);

        //@Override
        CARAPI SetAutoPersisting(
            /* [in] */ const String& callingPkg,
            /* [in] */ Boolean enabled);

        //@Override
        CARAPI GetAutoPersisting(
            /* [out] */ Boolean* result);

    private:
        MmsServiceBroker* mHost;
    };
private:

    class ConnectionHandler
        : public Handler
    {
    public:
        ConnectionHandler(
            /* [in] */ MmsServiceBroker* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        MmsServiceBroker* mHost;
    };

    class Connection
        : public Object
        , public IServiceConnection
    {
    public:
        Connection(
            /* [in] */ MmsServiceBroker* host);

        //@Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        //@Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);
    private:
        MmsServiceBroker* mHost;
    };

public:
    MmsServiceBroker();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI OnStart();

    CARAPI SystemRunning();

private:
    CARAPI TryConnecting();

    CARAPI EnsureService();

    /**
     * Making sure when we obtain the mService instance it is always valid.
     * Throws {@link RuntimeException} when it is empty.
     */
    AutoPtr<IIMms> GetServiceGuarded();

    AutoPtr<IAppOpsManager> GetAppOpsManager();

    AutoPtr<IPackageManager> GetPackageManager();

    AutoPtr<ITelephonyManager> GetTelephonyManager();

    /*
     * Throws a security exception unless the caller has carrier privilege.
     */
    CARAPI EnforceCarrierPrivilege();

private:
    static const String TAG;

    static const AutoPtr<IComponentName> MMS_SERVICE_COMPONENT;;

    static const Int32 MSG_TRY_CONNECTING;;

    static const AutoPtr<IUri> FAKE_SMS_SENT_URI;
    static const AutoPtr<IUri> FAKE_MMS_SENT_URI;
    static const AutoPtr<IUri> FAKE_SMS_DRAFT_URI;
    static const AutoPtr<IUri> FAKE_MMS_DRAFT_URI;

    static const Int64 SERVICE_CONNECTION_WAIT_TIME_MS; // 4 seconds
    static const Int64 RETRY_DELAY_ON_DISCONNECTION_MS; // 3 seconds

    AutoPtr<IContext> mContext;
    // The actual MMS service instance to invoke
    volatile AutoPtr<IIMms> mService;

    // Cached system service instances
    volatile AutoPtr<IAppOpsManager> mAppOpsManager;
    volatile AutoPtr<IPackageManager> mPackageManager;
    volatile AutoPtr<ITelephonyManager> mTelephonyManager;

    AutoPtr<ConnectionHandler> mConnectionHandler
    AutoPtr<IServiceConnection> mConnection;
};


} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_MMSSERVICEBROKER_H__

