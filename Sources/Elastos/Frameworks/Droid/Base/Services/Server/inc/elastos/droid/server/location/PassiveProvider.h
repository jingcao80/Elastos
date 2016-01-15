
#ifndef __ELASTOS_DROID_SERVER_LOCATION_PASSIVEPROVIDER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_PASSIVEPROVIDER_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#else
#include "Elastos.Droid.Core.h"
#endif
#include "elastos/droid/ext/frameworkext.h"
#include "location/LocationProviderInterface.h"

using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::IILocationManager;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::CProviderProperties;
using Elastos::Droid::Location::IProviderProperties;
using Elastos::Droid::Location::IProviderRequest;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IWorkSource;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * A passive location provider reports locations received from other providers
 * for clients that want to listen passively without actually triggering
 * location updates.
 *
 * {@hide}
 */
class PassiveProvider
        : public ILocationProviderInterface
        , public ElRefBase
{
private:
    static const String TAG;// = "PassiveProvider";

public:

    CAR_INTERFACE_DECL()

    PassiveProvider(
        /* [in] */ IILocationManager* locationManager);

    //@Override
    CARAPI GetName(
        /* [out] */ String* name);

    //@Override
    CARAPI GetProperties(
        /* [out] */ IProviderProperties** properties);

    //@Override
    CARAPI IsEnabled(
        /* [out] */ Boolean* enable);

    //@Override
    CARAPI Enable();

    //@Override
    CARAPI Disable();

    //@Override
    CARAPI GetStatus(
        /* [in] */ IBundle* extras,
        /* [out] */ Int32* status);

    //@Override
    CARAPI GetStatusUpdateTime(
        /* [out] */ Int64* time);

    //@Override
    CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source);
    //@Override
    CARAPI SwitchUser(
        /* [in] */ Int32 userId);

    CARAPI UpdateLocation(
        /* [in] */ ILocation* location);

    //@Override
    CARAPI SendExtraCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    static AutoPtr<IProviderProperties> PROPERTIES;// = new ProviderProperties(
            //false, false, false, false, false, false, false,
            //Criteria.POWER_LOW, Criteria.ACCURACY_COARSE);

    AutoPtr<IILocationManager> mLocationManager;
    Boolean mReportLocation;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_PASSIVEPROVIDER_H__
