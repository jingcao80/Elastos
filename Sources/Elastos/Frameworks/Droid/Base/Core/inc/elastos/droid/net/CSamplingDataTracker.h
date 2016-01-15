
#ifndef __ELASTOS_DROID_NET_CSAMPLINGDATATRACKER_H__
#define __ELASTOS_DROID_NET_CSAMPLINGDATATRACKER_H__

#include "_Elastos_Droid_Net_CSamplingDataTracker.h"
#include "elastos/droid/net/SamplingDataTracker.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * @hide
 */
CarClass(CSamplingDataTracker)
    , public SamplingDataTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CSAMPLINGDATATRACKER_H__
