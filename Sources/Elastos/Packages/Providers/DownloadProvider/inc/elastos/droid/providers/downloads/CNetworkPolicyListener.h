
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CNETWORKPOLICYLISTENER_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CNETWORKPOLICYLISTENER_H__

#include "_Elastos_Droid_Providers_Downloads_CNetworkPolicyListener.h"
#include "elastos/droid/providers/downloads/CDownloadThread.h"

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Update {@link NotificationManager} to reflect current {@link DownloadInfo}
 * states. Collapses similar downloads into a single notification, and builds
 * {@link PendingIntent} that launch towards {@link DownloadReceiver}.
 */
CarClass(CNetworkPolicyListener)
    , public CDownloadThread::NetworkPolicyListener
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CNETWORKPOLICYLISTENER_H__
