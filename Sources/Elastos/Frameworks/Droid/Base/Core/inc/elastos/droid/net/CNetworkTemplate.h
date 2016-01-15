
#ifndef __ELASTOS_DROID_NET_CNETWORKTEMPLATE_H__
#define __ELASTOS_DROID_NET_CNETWORKTEMPLATE_H__

#include "_Elastos_Droid_Net_CNetworkTemplate.h"
#include "elastos/droid/net/NetworkTemplate.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Template definition used to generically match {@link NetworkIdentity},
 * usually when collecting statistics.
 *
 * @hide
 */
CarClass(CNetworkTemplate)
    , public NetworkTemplate
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKTEMPLATE_H__
