
#ifndef __ELASTOS_DROID_CONTENT_CSYNCINFO_H__
#define __ELASTOS_DROID_CONTENT_CSYNCINFO_H__

#include "_Elastos_Droid_Content_CSyncInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Accounts::IAccount;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncInfo)
    , public Object
    , public ISyncInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSyncInfo();

    virtual ~CSyncInfo();

    CARAPI constructor();

    /** @hide */
    CARAPI constructor(
        /* [in] */ Int32 authorityId,
        /* [in] */ IAccount* account,
        /* [in] */ const String& authority,
        /* [in] */ Int64 startTime);

    /** @hide */
    CARAPI constructor(
        /* [in] */ ISyncInfo* other);

    /** @hide */
    CARAPI GetAuthorityId(
        /* [out] */ Int32* authorityId);

    CARAPI SetAuthorityId(
        /* [in] */ Int32 authorityId);

    /**
     * The {@link Account} that is currently being synced.
     */
    CARAPI GetAccount(
        /* [out] */ IAccount** account);

    CARAPI SetAccount(
        /* [in] */ IAccount* account);

    /**
     * The authority of the provider that is currently being synced.
     */
    CARAPI GetAuthority(
        /* [out] */ String* authority);

    CARAPI SetAuthority(
        /* [in] */ const String& authority);

    /**
     * The start time of the current sync operation in milliseconds since boot.
     * This is represented in elapsed real time.
     * See {@link android.os.SystemClock#elapsedRealtime()}.
     */
    CARAPI GetStartTime(
        /* [out] */ Int64* startTime);

    CARAPI SetStartTime(
        /* [in] */ Int64 startTime);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    /** @hide */
    Int32 mAuthorityId;

    /**
     * The {@link Account} that is currently being synced.
     */
    AutoPtr<IAccount> mAccount;

    /**
     * The authority of the provider that is currently being synced.
     */
    String mAuthority;

    /**
     * The start time of the current sync operation in milliseconds since boot.
     * This is represented in elapsed real time.
     * See {@link android.os.SystemClock#elapsedRealtime()}.
     */
    Int64 mStartTime;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCINFO_H__
