
#ifndef __ELASTOS_DROID_CONTENT_CSYNCADAPTERTYPE_H__
#define __ELASTOS_DROID_CONTENT_CSYNCADAPTERTYPE_H__

#include "_Elastos_Droid_Content_CSyncAdapterType.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncAdapterType)
    , public Object
    , public ISyncAdapterType
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSyncAdapterType();

    virtual ~CSyncAdapterType();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& authority,
        /* [in] */ const String& accountType,
        /* [in] */ Boolean userVisible,
        /* [in] */ Boolean supportsUploading);

    /** @hide */
    CARAPI constructor(
        /* [in] */ const String& authority,
        /* [in] */ const String& accountType,
        /* [in] */ Boolean userVisible,
        /* [in] */ Boolean supportsUploading,
        /* [in] */ Boolean isAlwaysSyncable,
        /* [in] */ Boolean allowParallelSyncs,
        /* [in] */ const String& settingsActivity);

    /** @private */
    CARAPI constructor(
        /* [in] */ const String& authority,
        /* [in] */ const String& accountType);

    CARAPI GetAuthority(
        /* [out] */ String* authority);

    CARAPI SetAuthority(
        /* [in] */ const String& authority);

    CARAPI GetAccountType(
        /* [out] */ String* accountType);

    CARAPI SetAccountType(
        /* [in] */ const String&  accountType);

    CARAPI SetKey(
        /* [in] */ Boolean isKey);

    CARAPI IsKey(
        /* [out] */ Boolean* isKey);

    CARAPI SupportsUploading(
        /* [out] */ Boolean* isSupport);

    CARAPI IsUserVisible(
        /* [out] */ Boolean* isUserVisible);

    /**
     * @return True if this SyncAdapter supports syncing multiple accounts simultaneously.
     * If false then the SyncManager will take care to only start one sync at a time
     * using this SyncAdapter.
     */
    CARAPI AllowParallelSyncs(
        /* [out] */ Boolean* isAllow);

    /**
     * If true then the SyncManager will never issue an initialization sync to the SyncAdapter
     * and will instead automatically call
     * {@link ContentResolver#setIsSyncable(android.accounts.Account, String, int)} with a
     * value of 1 for each account and provider that this sync adapter supports.
     * @return true if the SyncAdapter does not require initialization and if it is ok for the
     * SyncAdapter to treat it as syncable automatically.
     */
    CARAPI IsAlwaysSyncable(
        /* [out] */ Boolean* isAlways);

    /**
     * @return The activity to use to invoke this SyncAdapter's settings activity.
     * May be null.
     */
    CARAPI GetSettingsActivity(
        /* [out] */ String* settingsActivity);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    static AutoPtr<ISyncAdapterType> NewKey(
        /* [in] */ const String& authority,
        /* [in] */ const String& accountType);

private:
    String mAuthority;
    String mAccountType;
    Boolean mIsKey;
    Boolean mUserVisible;
    Boolean mSupportsUploading;
    Boolean mIsAlwaysSyncable;
    Boolean mAllowParallelSyncs;
    String mSettingsActivity;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCADAPTERTYPE_H__
