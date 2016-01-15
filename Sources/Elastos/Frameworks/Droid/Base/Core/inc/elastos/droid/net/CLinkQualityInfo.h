
#ifndef __ELASTOS_DROID_NET_CLINKQUALITYINFO_H__
#define __ELASTOS_DROID_NET_CLINKQUALITYINFO_H__

#include "_Elastos_Droid_Net_CLinkQualityInfo.h"
#include "elastos/droid/net/LinkQualityInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 *  Class that represents useful attributes of generic network links
 *  such as the upload/download throughput or packet error rate.
 *  Generally speaking, you should be dealing with instances of
 *  LinkQualityInfo subclasses, such as {@link android.net.#WifiLinkQualityInfo}
 *  or {@link android.net.#MobileLinkQualityInfo} which provide additional
 *  information.
 *  @hide
 */
CarClass(CLinkQualityInfo)
    , public LinkQualityInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CLINKQUALITYINFO_H__
