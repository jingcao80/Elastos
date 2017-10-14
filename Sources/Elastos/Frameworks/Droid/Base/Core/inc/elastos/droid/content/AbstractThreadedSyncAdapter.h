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

#ifndef __ELASTOS_DROID_CONTENT_ABSTRACTTHREADEDSYNCADAPTER_H__
#define __ELASTOS_DROID_CONTENT_ABSTRACTTHREADEDSYNCADAPTER_H__

#include "elastos/droid/ext/frameworkext.h"
#define HASH_FOR_ACCOUNTS
#include "elastos/droid/ext/frameworkhash.h"
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Content.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Utility::Etl::HashMap;
using Elastos::Core::Thread;
using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * An abstract implementation of a SyncAdapter that spawns a thread to invoke a sync operation.
 * If a sync operation is already in progress when a startSync() request is received then an error
 * will be returned to the new request and the existing request will be allowed to continue.
 * When a startSync() is received and there is no sync operation in progress then a thread
 * will be started to run the operation and {@link #onPerformSync} will be invoked on that thread.
 * If a cancelSync() is received that matches an existing sync operation then the thread
 * that is running that sync operation will be interrupted, which will indicate to the thread
 * that the sync has been canceled.
 * <p>
 * In order to be a sync adapter one must extend this class, provide implementations for the
 * abstract methods and write a service that returns the result of {@link #getSyncAdapterBinder()}
 * in the service's {@link android.app.Service#onBind(android.content.Intent)} when invoked
 * with an intent with action <code>android.content.SyncAdapter</code>. This service
 * must specify the following intent filter and metadata tags in its AndroidManifest.xml file
 * <pre>
 *   &lt;intent-filter&gt;
 *     &lt;action android:name="android.content.SyncAdapter" /&gt;
 *   &lt;/intent-filter&gt;
 *   &lt;meta-data android:name="android.content.SyncAdapter"
 *             android:resource="@xml/syncadapter" /&gt;
 * </pre>
 * The <code>android:resource</code> attribute must point to a resource that looks like:
 * <pre>
 * &lt;sync-adapter xmlns:android="http://schemas.android.com/apk/res/android"
 *    android:contentAuthority="authority"
 *    android:accountType="accountType"
 *    android:userVisible="true|false"
 *    android:supportsUploading="true|false"
 *    android:allowParallelSyncs="true|false"
 *    android:isAlwaysSyncable="true|false"
 *    android:syncAdapterSettingsAction="ACTION_OF_SETTINGS_ACTIVITY"
 * /&gt;
 * </pre>
 * <ul>
 * <li>The <code>android:contentAuthority</code> and <code>android:accountType</code> attributes
 * indicate which content authority and for which account types this sync adapter serves.
 * <li><code>android:userVisible</code> defaults to true and controls whether or not this sync
 * adapter shows up in the Sync Settings screen.
 * <li><code>android:supportsUploading</code> defaults
 * to true and if true an upload-only sync will be requested for all syncadapters associated
 * with an authority whenever that authority's content provider does a
 * {@link ContentResolver#notifyChange(android.net.Uri, android.database.ContentObserver, boolean)}
 * with syncToNetwork set to true.
 * <li><code>android:allowParallelSyncs</code> defaults to false and if true indicates that
 * the sync adapter can handle syncs for multiple accounts at the same time. Otherwise
 * the SyncManager will wait until the sync adapter is not in use before requesting that
 * it sync an account's data.
 * <li><code>android:isAlwaysSyncable</code> defaults to false and if true tells the SyncManager
 * to intialize the isSyncable state to 1 for that sync adapter for each account that is added.
 * <li><code>android:syncAdapterSettingsAction</code> defaults to null and if supplied it
 * specifies an Intent action of an activity that can be used to adjust the sync adapter's
 * sync settings. The activity must live in the same package as the sync adapter.
 * </ul>
 */
class AbstractThreadedSyncAdapter
    : public Object
    , public IAbstractThreadedSyncAdapter
{
    class ISyncAdapterImpl
        : public Object
        , public IISyncAdapter
    {
    public:
        ISyncAdapterImpl(
            /* [in] */ AbstractThreadedSyncAdapter* context);

        ~ISyncAdapterImpl();

        CAR_INTERFACE_DECL()

        CARAPI StartSync(
            /* [in] */ IISyncContext* syncContext,
            /* [in] */ const String& authority,
            /* [in] */ IAccount* account,
            /* [in] */ IBundle* extras);

        CARAPI CancelSync(
            /* [in] */ IISyncContext* syncContext);

        CARAPI Initialize(
            /* [in] */ IAccount* account,
            /* [in] */ const String& authority);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        AutoPtr<AbstractThreadedSyncAdapter> mAdapterContext;

    };

    /**
     * The thread that invokes {@link AbstractThreadedSyncAdapter#onPerformSync}. It also acquires
     * the provider for this sync before calling onPerformSync and releases it afterwards. Cancel
     * this thread in order to cancel the sync.
     */
    class SyncThread
        : public Thread
    {
    friend class ISyncAdapterImpl;

    public:
        SyncThread(
            /* [in] */ const String& name,
            /* [in] */ ISyncContext* syncContext,
            /* [in] */ const String& authority,
            /* [in] */ IAccount* account,
            /* [in] */ IBundle* extras,
            /* [in] */ AbstractThreadedSyncAdapter* context);

        ~SyncThread();

        virtual CARAPI Run();

    private:
        CARAPI IsCanceled(
            /* [out] */ Boolean* isCanceled);

    private:
        AutoPtr<ISyncContext> mSyncContext;
        String mAuthority;
        AutoPtr<IAccount> mAccount;
        AutoPtr<IBundle> mExtras;
        AutoPtr<IAccount> mThreadsKey;
        AutoPtr<AbstractThreadedSyncAdapter> mAdapterContext;
    };

friend class ISyncAdapterImpl;

public:
    CAR_INTERFACE_DECL()

    AbstractThreadedSyncAdapter();

    virtual ~AbstractThreadedSyncAdapter();

    CARAPI GetContext(
        /* [out] */ IContext** context);

    /**
     * @return a reference to the IBinder of the SyncAdapter service.
     */
    CARAPI GetSyncAdapterBinder(
        /* [out] */ IBinder** binder);

    /**
     * Perform a sync for this account. SyncAdapter-specific parameters may
     * be specified in extras, which is guaranteed to not be null. Invocations
     * of this method are guaranteed to be serialized.
     *
     * @param account the account that should be synced
     * @param extras SyncAdapter-specific parameters
     * @param authority the authority of this sync request
     * @param provider a ContentProviderClient that points to the ContentProvider for this
     *   authority
     * @param syncResult SyncAdapter-specific parameters
     */
    virtual CARAPI OnPerformSync(
        /* [in] */ IAccount* account,
        /* [in] */ IBundle* extras,
        /* [in] */ const String& authority,
        /* [in] */ IContentProviderClient* provider,
        /* [in] */ ISyncResult* syncResult) = 0;

    /**
     * Indicates that a sync operation has been canceled. This will be invoked on a separate
     * thread than the sync thread and so you must consider the multi-threaded implications
     * of the work that you do in this method.
     * <p>
     * This will only be invoked when the SyncAdapter indicates that it doesn't support
     * parallel syncs.
     */
    virtual CARAPI OnSyncCanceled();

    /**
     * Indicates that a sync operation has been canceled. This will be invoked on a separate
     * thread than the sync thread and so you must consider the multi-threaded implications
     * of the work that you do in this method.
     * <p>
     * This will only be invoked when the SyncAdapter indicates that it does support
     * parallel syncs.
     * @param thread the Thread of the sync that is to be canceled.
     */
    virtual CARAPI OnSyncCanceled(
        /* [in] */ IThread* thread);

protected:
    CARAPI_(AutoPtr<IAccount>) ToSyncKey(
        /* [in] */ IAccount* account);

    /**
     * Creates an {@link AbstractThreadedSyncAdapter}.
     * @param context the {@link android.content.Context} that this is running within.
     * @param autoInitialize if true then sync requests that have
     * {@link ContentResolver#SYNC_EXTRAS_INITIALIZE} set will be internally handled by
     * {@link AbstractThreadedSyncAdapter} by calling
     * {@link ContentResolver#setIsSyncable(android.accounts.Account, String, int)} with 1 if it
     * is currently set to <0.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean autoInitialize);

    /**
     * Creates an {@link AbstractThreadedSyncAdapter}.
     * @param context the {@link android.content.Context} that this is running within.
     * @param autoInitialize if true then sync requests that have
     * {@link ContentResolver#SYNC_EXTRAS_INITIALIZE} set will be internally handled by
     * {@link AbstractThreadedSyncAdapter} by calling
     * {@link ContentResolver#setIsSyncable(android.accounts.Account, String, int)} with 1 if it
     * is currently set to <0.
     * @param allowParallelSyncs if true then allow syncs for different accounts to run
     * at the same time, each in their own thread. This must be consistent with the setting
     * in the SyncAdapter's configuration file.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Boolean autoInitialize,
        /* [in] */ Boolean allowParallelSyncs);

private:
    AutoPtr<IContext> mContext;
    /* AtomicInteger */ Int32 mNumSyncStarts;
    AutoPtr<ISyncAdapterImpl> mISyncAdapterImpl;

    // all accesses to this member variable must be synchronized on mSyncThreadLock
    HashMap<AutoPtr<IAccount>, AutoPtr<SyncThread> > mSyncThreads;
    Boolean mAutoInitialize;
    Boolean mAllowParallelSyncs;
    Object mSyncThreadLock;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_ABSTRACTTHREADEDSYNCADAPTER_H__
