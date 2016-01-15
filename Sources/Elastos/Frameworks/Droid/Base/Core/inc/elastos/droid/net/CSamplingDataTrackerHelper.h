
#ifndef __ELASTOS_DROID_NET_CSAMPLINGDATATRACKERHELPER_H__
#define __ELASTOS_DROID_NET_CSAMPLINGDATATRACKERHELPER_H__

#include "_Elastos_Droid_Net_CSamplingDataTrackerHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * @hide
 */
CarClass(CSamplingDataTrackerHelper)
    , public Singleton
    , public ISamplingDataTrackerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetSamplingSnapshots(
        /* [in] */ IMap* mapIfaceToSample);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CSAMPLINGDATATRACKERHELPER_H__
