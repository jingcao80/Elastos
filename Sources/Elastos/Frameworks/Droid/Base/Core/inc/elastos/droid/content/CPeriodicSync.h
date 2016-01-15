
#ifndef __ELASTOS_DROID_CONTENT_CPERIODICSYNC_H__
#define __ELASTOS_DROID_CONTENT_CPERIODICSYNC_H__

#include "_Elastos_Droid_Content_CPeriodicSync.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Value type that contains information about a periodic sync.
 */
CarClass(CPeriodicSync)
    , public Object
    , public IPeriodicSync
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CPeriodicSync();

    virtual ~CPeriodicSync();

    CARAPI constructor();

    /**
     * Creates a new PeriodicSync, copying the Bundle. This constructor is no longer used.
     */
    CARAPI constructor(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 periodInSeconds);

    /**
     * Create a copy of a periodic sync.
     * {@hide}
     */
    CARAPI constructor(
        /* [in] */ IPeriodicSync* other);

    /**
     * A PeriodicSync for a sync with a specified provider.
     * {@hide}
     */
    CARAPI constructor(
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 periodInSeconds,
        /* [in] */ Int64 flexTime);

    /** The account to be synced */
    CARAPI GetAccount(
        /* [out] */ IAccount** account);

    /** The authority of the sync */
    CARAPI GetAuthority(
        /* [out] */ String* authority);

    /** Any extras that parameters that are to be passed to the sync adapter. */
    CARAPI GetExtras(
        /* [out] */ IBundle** extras);

    CARAPI GetPeriod(
        /* [out] */ Int64* period);

    CARAPI GetFlexTime(
        /* [out] */ Int64* flexTime);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Periodic sync extra comparison function.
     * {@hide}
     */
    static Boolean SyncExtrasEquals(
        /* [in] */ IBundle* b1,
        /* [in] */ IBundle* b2);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    /** The account to be synced. Can be null. */
    AutoPtr<IAccount> mAccount;

    /** The authority of the sync. Can be null. */
    String mAuthority;

    /** Any extras that parameters that are to be passed to the sync adapter. */
    AutoPtr<IBundle> mExtras;

    /** How frequently the sync should be scheduled, in seconds. Kept around for API purposes. */
    Int64 mPeriod;

public:
    /**
     * How much flexibility can be taken in scheduling the sync, in seconds.
     * {@hide}
     */
    Int64 mFlexTime;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CPERIODICSYNC_H__
