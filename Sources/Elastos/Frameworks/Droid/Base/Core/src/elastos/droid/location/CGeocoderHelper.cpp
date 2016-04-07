
#include "Elastos.Droid.Content.h"
#include "elastos/droid/location/CGeocoderHelper.h"
#include "elastos/droid/os/CServiceManager.h"

using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Location {

CAR_INTERFACE_IMPL(CGeocoderHelper, Singleton, IGeocoderHelper)

CAR_SINGLETON_IMPL(CGeocoderHelper)

ECode CGeocoderHelper::IsPresent(
    /* [out] */ Boolean* isPresent)
{
    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> obj;
    serviceManager->GetService(IContext::LOCATION_SERVICE, (IInterface**)&obj);
    IILocationManager* lm = IILocationManager::Probe(obj);
    return lm->GeocoderIsPresent(isPresent);
}

} // namespace Location
} // namespace Droid
} // namespace Elastos