
#ifndef __ELASTOS_DROID_NET_CMOBILEDATASTATETRACKERHELPER_H__
#define __ELASTOS_DROID_NET_CMOBILEDATASTATETRACKERHELPER_H__

#include "_Elastos_Droid_Net_CMobileDataStateTrackerHelper.h"
#include <elastos/core/Singleton.h>

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
CarClass(CMobileDataStateTrackerHelper)
    , public Singleton
    , public IMobileDataStateTrackerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NetworkTypeToApnType(
        /* [in] */ Int32 netType,
        /* [out] */ String* result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CMOBILEDATASTATETRACKERHELPER_H__
