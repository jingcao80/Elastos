#ifndef __ELASTOS_DROID_APP_CCONTENTPROVIDERHOLDER_H__
#define __ELASTOS_DROID_APP_CCONTENTPROVIDERHOLDER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CContentProviderHolder.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIContentProvider;
using Elastos::Droid::Content::Pm::IProviderInfo;


CarClass(CContentProviderHolder)
    , public Object
    , public IContentProviderHolder
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentProviderHolder();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IProviderInfo* info);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetProviderInfo(
        /* [out] */ IProviderInfo** info);

    CARAPI SetProviderInfo(
        /* [in] */ IProviderInfo* info);

    CARAPI GetContentProvider(
        /* [out] */ IIContentProvider** provider);

    CARAPI SetContentProvider(
        /* [in] */ IIContentProvider* provider);

    CARAPI GetConnection(
        /* [out] */ IBinder** connection);

    CARAPI SetConnection(
        /* [in] */ IBinder* connection);

    CARAPI GetNoReleaseNeeded(
        /* [out] */ Boolean* noReleaseNeeded);

    CARAPI SetNoReleaseNeeded(
        /* [in] */ Boolean noReleaseNeeded);

private:
    AutoPtr<IProviderInfo> mInfo;
    AutoPtr<IIContentProvider> mProvider;
    AutoPtr<IBinder> mConnection;
    Boolean mNoReleaseNeeded;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CCONTENTPROVIDERHOLDER_H__
