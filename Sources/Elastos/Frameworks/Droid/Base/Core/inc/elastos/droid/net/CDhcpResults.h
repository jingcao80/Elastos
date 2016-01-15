
#ifndef __ELASTOS_DROID_NET_CDHCPRESULTS_H__
#define __ELASTOS_DROID_NET_CDHCPRESULTS_H__

#include "_Elastos_Droid_Net_CDhcpResults.h"
#include "elastos/droid/net/DhcpResults.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A simple object for retrieving the results of a DHCP request.
 * Optimized (attempted) for that jni interface
 * TODO - remove when DhcpInfo is deprecated.  Move the remaining api to LinkProperties.
 * @hide
 */
CarClass(CDhcpResults)
    , public DhcpResults
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CDHCPRESULTS_H__