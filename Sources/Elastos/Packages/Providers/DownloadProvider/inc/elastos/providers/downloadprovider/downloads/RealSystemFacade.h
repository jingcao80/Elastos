
#ifndef __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_REALSYSTEMFACADE_H__
#define __ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_REALSYSTEMFACADE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Providers.DownloadProvider.Downloads.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Providers::DownloadProvider::Downloads::ISystemFacade;

namespace Elastos {
namespace Providers {
namespace DownloadProvider {
namespace Downloads {

class RealSystemFacade
    : public Object
    , public ISystemFacade
{
public:
    CAR_INTERFACE_DECL()

    RealSystemFacade(
        /* [in] */ IContext* context);

    CARAPI GetCurrentTimeMillis(
        /* [out] */ Int64* result);

    CARAPI GetActiveNetworkInfo(
        /* [in] */ Int32 uid,
        /* [out] */ INetworkInfo** result);

    CARAPI IsActiveNetworkMetered(
        /* [out] */ Boolean* result);

    CARAPI IsNetworkRoaming(
        /* [out] */ Boolean* result);

    CARAPI GetMaxBytesOverMobile(
        /* [out] */ Int64* result);

    CARAPI GetRecommendedMaxBytesOverMobile(
        /* [out] */ Int64* result);

    CARAPI SendBroadcast(
        /* [in] */ IIntent* intent);

    CARAPI UserOwnsPackage(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [out] */ Boolean* result);

private:
    AutoPtr<IContext> mContext;
};

} // namespace Downloads
} // namespace DownloadProvider
} // namespace Providers
} // namespace Elastos

#endif //__ELASTOS_PROVIDERS_DOWNLOADPROVIDER_DOWNLOADS_REALSYSTEMFACADE_H__
