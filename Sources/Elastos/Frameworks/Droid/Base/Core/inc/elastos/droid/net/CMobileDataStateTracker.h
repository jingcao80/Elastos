
#ifndef __ELASTOS_DROID_NET_CMOBILEDATASTATETRACKER_H__
#define __ELASTOS_DROID_NET_CMOBILEDATASTATETRACKER_H__

#include "_Elastos_Droid_Net_CMobileDataStateTracker.h"
#include "elastos/droid/net/MobileDataStateTracker.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Track the state of mobile data connectivity. This is done by
 * receiving broadcast intents from the Phone process whenever
 * the state of data connectivity changes.
 *
 * {@hide}
 */
CarClass(CMobileDataStateTracker)
    , public MobileDataStateTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CMOBILEDATASTATETRACKER_H__
