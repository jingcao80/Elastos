
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CSamplingDataTracker.h"
#include "elastos/droid/net/CSamplingDataTrackerHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CSamplingDataTrackerHelper, Singleton, ISamplingDataTrackerHelper)

CAR_SINGLETON_IMPL(CSamplingDataTrackerHelper)

CSamplingDataTrackerHelper::GetSamplingSnapshots(
    /* [in] */ IMap* mapIfaceToSample)
{
    return CSamplingDataTracker::GetSamplingSnapshots(mapIfaceToSample);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
