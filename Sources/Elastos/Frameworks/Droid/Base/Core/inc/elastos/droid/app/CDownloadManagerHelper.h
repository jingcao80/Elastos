
#ifndef __ELASTOS_DROID_APP_CDOWNLOADMANAGER_HELPER_H__
#define __ELASTOS_DROID_APP_CDOWNLOADMANAGER_HELPER_H__

#include "_Elastos_Droid_App_CDownloadManagerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CDownloadManagerHelper)
    , public Singleton
    , public IDownloadManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetUNDERLYINGCOLUMNS(
        /* [out] */ ArrayOf<String>** columns);
    /**
     * Returns maximum size, in bytes, of downloads that may go over a mobile connection; or null if
     * there's no limit
     *
     * @param context the {@link Context} to use for accessing the {@link ContentResolver}
     * @return maximum size, in bytes, of downloads that may go over a mobile connection; or null if
     * there's no limit
     */
    CARAPI GetMaxBytesOverMobile(
        /* [in] */ IContext* context,
        /* [out] */ Int64* size);

    /**
     * Returns recommended maximum size, in bytes, of downloads that may go over a mobile
     * connection; or null if there's no recommended limit.  The user will have the option to bypass
     * this limit.
     *
     * @param context the {@link Context} to use for accessing the {@link ContentResolver}
     * @return recommended maximum size, in bytes, of downloads that may go over a mobile
     * connection; or null if there's no recommended limit.
     */
    CARAPI GetRecommendedMaxBytesOverMobile(
        /* [in] */ IContext* context,
        /* [out] */ Int64* size);

    /** {@hide} */
    CARAPI IsActiveNetworkExpensive(
        /* [in] */ IContext* context,
        /* [out] */ Boolean* actived);

    /** {@hide} */
    CARAPI GetActiveNetworkWarningBytes(
        /* [in] */ IContext* context,
        /* [out] */ Int64* bytes);
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CDOWNLOADMANAGER_HELPER_H__
