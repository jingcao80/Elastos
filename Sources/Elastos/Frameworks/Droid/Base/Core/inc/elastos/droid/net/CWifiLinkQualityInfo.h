
#ifndef __ELASTOS_DROID_NET_CWIFILINKQUALITYINFO_H__
#define __ELASTOS_DROID_NET_CWIFILINKQUALITYINFO_H__

#include "_Elastos_Droid_Net_CWifiLinkQualityInfo.h"
#include "elastos/droid/net/WifiLinkQualityInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 *  Class that represents useful attributes of wifi network links
 *  such as the upload/download throughput or error rate etc.
 *  @hide
 */
CarClass(CWifiLinkQualityInfo)
    , public WifiLinkQualityInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CWIFILINKQUALITYINFO_H__
