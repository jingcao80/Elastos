
#ifndef __ELASTOS_DROID_SERVER_CONTENT_CCONTENTSERVICE_H__
#define __ELASTOS_DROID_SERVER_CONTENT_CCONTENTSERVICE_H__

#include "_Elastos_Droid_Server_Content_CContentService.h"
#include "elastos/droid/server/content/SyncManager.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISyncAdapterType;
using Elastos::Droid::Content::ISyncStatusInfo;
using Elastos::Droid::Content::IISyncStatusObserver;
using Elastos::Droid::Content::IIContentService;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IPeriodicSync;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::ISyncInfo;
using Elastos::Droid::Content::ISyncRequest;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

CarClass(CContentService)
    , public Object
    , public IIContentService
    , public IBinder
{
public:
    class ObserverNode;
    /**
     * Hide this class since it is not part of api,
     * but current unittest framework requires it to be public
     * @hide
     *
     */
    class ObserverCall
        : public Object
    {
    public:
        ObserverCall(
            /* [in] */ ObserverNode* node,
            /* [in] */ IIContentObserver* observer,
            /* [in] */ Boolean selfChange);

    public:
        AutoPtr<ObserverNode> mNode;
        AutoPtr<IIContentObserver> mObserver;
        Boolean mSelfChange;
    };

    /**
     * Hide this class since it is not part of api,
     * but current unittest framework requires it to be public
     * @hide
     */
    class ObserverNode
        : public Object
    {
    public:
        class ObserverEntry
            : public Object
            , public IProxyDeathRecipient
        {
        public:
            CAR_INTERFACE_DECL()

            ObserverEntry(
                /* [in] */ ObserverNode* host,
                /* [in] */ IIContentObserver* observer,
                /* [in] */ Boolean notify,
                /* [in] */ Object* observersLock,
                /* [in] */ Int32 uid,
                /* [in] */ Int32 pid,
                /* [in] */ Int32 userHandle);

            CARAPI ProxyDied();

            CARAPI_(void) DumpLocked(
                /* [in] */ IFileDescriptor* fd,
                /* [in] */ IPrintWriter* pw,
                /* [in] */ ArrayOf<String>* args,
                /* [in] */ const String& name,
                /* [in] */ const String& prefix,
                /* [in] */ HashMap<Int32, Int32>* pidCounts);

        public:
            AutoPtr<IIContentObserver> mObserver;
            Int32 mUid;
            Int32 mPid;
            Boolean mNotifyForDescendants;
            Int32 mUserHandle;

        private:
            ObserverNode* mHost;
            Object* mObserversLock;
        };

    public:
        ObserverNode(
            /* [in] */ const String& name);

        CARAPI_(void) DumpLocked(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args,
            /* [in] */ const String& name,
            /* [in] */ const String& prefix,
            /* [in] */ ArrayOf<Int32>* counts,
            /* [in] */ HashMap<Int32, Int32>* pidCounts);

        // Invariant:  userHandle is either a hard user number or is USER_ALL
        CARAPI AddObserverLocked(
            /* [in] */ IUri* uri,
            /* [in] */ IIContentObserver* observer,
            /* [in] */ Boolean notifyForDescendants,
            /* [in] */ Object* observersLock,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 pid,
            /* [in] */ Int32 userHandle);

        CARAPI_(Boolean) RemoveObserverLocked(
            /* [in] */ IIContentObserver* observer);

        /**
         * targetUserHandle is either a hard user handle or is USER_ALL
         */
        CARAPI_(void) CollectObserversLocked(
            /* [in] */ IUri* uri,
            /* [in] */ Int32 index,
            /* [in] */ IIContentObserver* observer,
            /* [in] */ Boolean observerWantsSelfNotifications,
            /* [in] */ Int32 targetUserHandle,
            /* [in] */ List<AutoPtr<ObserverCall> >* calls);

    private:
        CARAPI_(String) GetUriSegment(
            /* [in] */ IUri* uri,
            /* [in] */ Int32 index);

        CARAPI_(Int32) CountUriSegments(
            /* [in] */ IUri* uri);

        CARAPI AddObserverLocked(
            /* [in] */ IUri* uri,
            /* [in] */ Int32 index,
            /* [in] */ IIContentObserver* observer,
            /* [in] */ Boolean notifyForDescendants,
            /* [in] */ Object* observersLock,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 pid,
            /* [in] */ Int32 userHandle);

        CARAPI_(void) CollectMyObserversLocked(
            /* [in] */ Boolean leaf,
            /* [in] */ IIContentObserver* observer,
            /* [in] */ Boolean observerWantsSelfNotifications,
            /* [in] */ Int32 targetUserHandle,
            /* [in] */ List<AutoPtr<ObserverCall> >* calls);

    public:
        static const Int32 INSERT_TYPE = 0;
        static const Int32 UPDATE_TYPE = 1;
        static const Int32 DELETE_TYPE = 2;

    public:
        String mName;
        List<AutoPtr<ObserverNode> > mChildren;
        List<AutoPtr<ObserverEntry> > mObservers;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL()

    CContentService();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean factoryTest);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI SystemReady();

    /**
     * Register a content observer tied to a specific user's view of the provider.
     * @param userHandle the user whose view of the provider is to be observed.  May be
     *     the calling user without requiring any permission, otherwise the caller needs to
     *     hold the INTERACT_ACROSS_USERS_FULL permission.  Pseudousers USER_ALL and
     *     USER_CURRENT are properly handled; all other pseudousers are forbidden.
     */
    CARAPI RegisterContentObserver(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean notifyForDescendants,
        /* [in] */ IIContentObserver* observer,
        /* [in] */ Int32 userHandle);

    CARAPI RegisterContentObserver(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean notifyForDescendants,
        /* [in] */ IIContentObserver* observer);

    CARAPI UnregisterContentObserver(
        /* [in] */ IIContentObserver* observer);

    /**
     * Notify observers of a particular user's view of the provider.
     * @param userHandle the user whose view of the provider is to be notified.  May be
     *     the calling user without requiring any permission, otherwise the caller needs to
     *     hold the INTERACT_ACROSS_USERS_FULL permission.  Pseudousers USER_ALL and
     *     USER_CURRENT are properly interpreted; no other pseudousers are allowed.
     */
    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IIContentObserver* observer,
        /* [in] */ Boolean observerWantsSelfNotifications,
        /* [in] */ Boolean syncToNetwork,
        /* [in] */ Int32 userHandle);

    CARAPI NotifyChange(
        /* [in] */ IUri* uri,
        /* [in] */ IIContentObserver* observer,
        /* [in] */ Boolean observerWantsSelfNotifications,
        /* [in] */ Boolean syncToNetwork);

    CARAPI RequestSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras);

    CARAPI Sync(
        /* [in] */ ISyncRequest* request);

    /**
     * If the user id supplied is different to the calling user, the caller must hold the
     * INTERACT_ACROSS_USERS_FULL permission.
     */
    CARAPI SyncAsUser(
        /* [in] */ ISyncRequest* request,
        /* [in] */ Int32 userId);

    /**
     * Clear all scheduled sync operations that match the uri and cancel the active sync
     * if they match the authority and account, if they are present.
     *
     * @param account filter the pending and active syncs to cancel using this account, or null.
     * @param authority filter the pending and active syncs to cancel using this authority, or
     * null.
     * @param cname cancel syncs running on this service, or null for provider/account.
     */
    // @Override
    CARAPI CancelSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* cn);

    /**
     * Clear all scheduled sync operations that match the uri and cancel the active sync
     * if they match the authority and account, if they are present.
     *
     * <p> If the user id supplied is different to the calling user, the caller must hold the
     * INTERACT_ACROSS_USERS_FULL permission.
     *
     * @param account filter the pending and active syncs to cancel using this account, or null.
     * @param authority filter the pending and active syncs to cancel using this authority, or
     * null.
     * @param userId the user id for which to cancel sync operations.
     * @param cname cancel syncs running on this service, or null for provider/account.
     */
    // @Override
    CARAPI CancelSyncAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* cn,
        /* [in] */ Int32 userId);

    CARAPI CancelRequest(
        /* [in ]*/ ISyncRequest* request);

    /**
     * Get information about the SyncAdapters that are known to the system.
     * @return an array of SyncAdapters that have registered with the system
     */
    CARAPI GetSyncAdapterTypes(
        /* [out, callee] */ ArrayOf<ISyncAdapterType*>** result);

    /**
     * Get information about the SyncAdapters that are known to the system for a particular user.
     *
     * <p> If the user id supplied is different to the calling user, the caller must hold the
     * INTERACT_ACROSS_USERS_FULL permission.
     *
     * @return an array of SyncAdapters that have registered with the system
     */
    //@Override
    CARAPI GetSyncAdapterTypesAsUser(
        /* [in] */ Int32 userId,
        /* [out, callee] */ ArrayOf<ISyncAdapterType*>** result);

    CARAPI GetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [out] */ Boolean* result);

    CARAPI GetSyncAutomaticallyAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI SetSyncAutomatically(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Boolean sync);

    CARAPI SetSyncAutomaticallyAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Boolean sync,
        /* [in] */ Int32 userId);

    CARAPI AddPeriodicSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 pollFrequency);

    CARAPI RemovePeriodicSync(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ IBundle* extras);

    CARAPI GetPeriodicSyncs(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [out] */ IList** periodicSyncList);

    CARAPI GetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [out] */ Int32* value);

    /**
     * If the user id supplied is different to the calling user, the caller must hold the
     * INTERACT_ACROSS_USERS_FULL permission.
     */
    CARAPI GetIsSyncableAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* value);

    CARAPI SetIsSyncable(
        /* [in] */ IAccount* account,
        /* [in] */ const String& providerName,
        /* [in] */ Int32 syncable);

    CARAPI GetMasterSyncAutomatically(
        /* [out] */ Boolean* result);

    CARAPI GetMasterSyncAutomaticallyAsUser(
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI SetMasterSyncAutomatically(
        /* [in] */ Boolean flag);

    CARAPI SetMasterSyncAutomaticallyAsUser(
        /* [in] */ Boolean flag,
        /* [in] */ Int32 userId);

    CARAPI IsSyncActive(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* cname,
        /* [out] */ Boolean* isActive);

    CARAPI GetCurrentSyncs(
        /* [out] */ IList** syncInfoList);

    CARAPI GetCurrentSyncsAsUser(
        /* [in] */ Int32 userId,
        /* [out] */ IList** syncInfoList);

    CARAPI GetSyncStatus(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* cname,
        /* [out] */ ISyncStatusInfo** result);

    CARAPI GetSyncStatusAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* cname,
        /* [in] */ Int32 userId,
        /* [out] */ ISyncStatusInfo** result);

    CARAPI IsSyncPending(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* cname,
        /* [out] */ Boolean* isPending);

    CARAPI IsSyncPendingAsUser(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IComponentName* cname,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* isPending);

    CARAPI AddStatusChangeListener(
        /* [in] */ Int32 mask,
        /* [in] */ IISyncStatusObserver* observer);

    CARAPI RemoveStatusChangeListener(
        /* [in] */ IISyncStatusObserver* observer);

    static AutoPtr<IIContentService> Main(
        /* [in] */ IContext* context,
        /* [in] */ Boolean factoryTest);

private:
    CARAPI_(AutoPtr<SyncManager>) GetSyncManager();

    /**
     * Checks if the request is from the system or an app that has INTERACT_ACROSS_USERS_FULL
     * permission, if the userHandle is not for the caller.
     *
     * @param userHandle the user handle of the user we want to act on behalf of.
     * @param message the message to log on security exception.
     */
    CARAPI EnforceCrossUserPermission(
        /* [in] */ Int32 userHandle,
        /* [in] */ const String& message);

    CARAPI ToString(
        /* [out] */ String* str)
    {
        return Object::ToString(str);
    }
private:
    static const String TAG;
    static const Boolean DEBUG;

private:
    AutoPtr<IContext> mContext;
    Boolean mFactoryTest;
    AutoPtr<ObserverNode> mRootNode;
    AutoPtr<SyncManager> mSyncManager;
    Object mSyncManagerLock;
};

}
}
}
}

#endif // __ELASTOS_DROID_SERVER_CONTENT_CCONTENTSERVICE_H__
