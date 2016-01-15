
#ifndef __ELASTOS_DROID_NET_CSCOREDNETWORK_H__
#define __ELASTOS_DROID_NET_CSCOREDNETWORK_H__

#include "_Elastos_Droid_Net_CScoredNetwork.h"
#include "elastos/droid/net/ScoredNetwork.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A network identifier along with a score for the quality of that network.
 *
 * @hide
 */
// @SystemApi
CarClass(CScoredNetwork)
    , public ScoredNetwork
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CSCOREDNETWORK_H__
