
#ifndef __ELASTOS_DROID_CONTENT_CSYNCREQUEST_H__
#define __ELASTOS_DROID_CONTENT_CSYNCREQUEST_H__

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_Content_CSyncRequest.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Convenience class to construct sync requests. See {@link android.content.SyncRequest.Builder}
 * for an explanation of the various functions. The resulting object is passed through to the
 * framework via {@link android.content.ContentResolver#requestSync(SyncRequest)}.
 */
CarClass(CSyncRequest)
    , public Object
    , public ISyncRequest
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSyncRequest();

    CARAPI constructor();

    /** {@hide} Protected ctor to instantiate anonymous SyncRequest. */
    CARAPI constructor(
        /* [in] */ ISyncRequestBuilder* b);

    /**
     * {@hide}
     * @return whether this sync is periodic or one-time. A Sync Request must be
     *         either one of these or an InvalidStateException will be thrown in
     *         Builder.build().
     */
    CARAPI IsPeriodic(
        /* [out] */ Boolean* periodic);

    /**
     * {@hide}
     * @return whether this sync is expedited.
     */
    CARAPI IsExpedited(
        /* [out] */ Boolean* periodic);

    /**
     * {@hide}
     *
     * @return account object for this sync.
     * @throws IllegalArgumentException if this function is called for a request that targets a
     * sync service.
     */
    CARAPI GetAccount(
        /* [out] */ IAccount** account);

    /**
     * {@hide}
     *
     * @return provider for this sync.
     * @throws IllegalArgumentException if this function is called for a request that targets a
     * sync service.
     */
    CARAPI GetProvider(
        /* [out] */ String* provider);

    /**
     * {@hide}
     * Retrieve bundle for this SyncRequest. Will not be null.
     */
    CARAPI GetBundle(
        /* [out] */ IBundle** account);

    /**
     * {@hide}
     * @return the earliest point in time that this sync can be scheduled.
     */
    CARAPI GetSyncFlexTime(
        /* [out] */ Int64* time);

    /**
     * {@hide}
     * @return the last point in time at which this sync must scheduled.
     */
    CARAPI GetSyncRunTime(
        /* [out] */ Int64* time);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* parcel);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

private:
    static const String TAG;// = "SyncRequest";
    /** Account to pass to the sync adapter. Can be null. */
    AutoPtr<IAccount> mAccountToSync;
    /** Authority string that corresponds to a ContentProvider. */
    String mAuthority;
    /** Bundle containing user info as well as sync settings. */
    AutoPtr<IBundle> mExtras;
    /** Don't allow this sync request on metered networks. */
    Boolean mDisallowMetered;
    /**
     * Amount of time before {@link #mSyncRunTimeSecs} from which the sync may optionally be
     * started.
     */
    Int64 mSyncFlexTimeSecs;
    /**
     * Specifies a point in the future at which the sync must have been scheduled to run.
     */
    Int64 mSyncRunTimeSecs;
    /** Periodic versus one-off. */
    Boolean mIsPeriodic;
    /** Service versus provider. */
    Boolean mIsAuthority;
    /** Sync should be run in lieu of other syncs. */
    Boolean mIsExpedited;
};


}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCREQUEST_H__
