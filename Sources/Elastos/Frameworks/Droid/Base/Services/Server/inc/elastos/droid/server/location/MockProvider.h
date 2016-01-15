
#ifndef __ELASTOS_DROID_SERVER_LOCATION_MOCKPROVIDER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_MOCKPROVIDER_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif
#include "elastos/droid/ext/frameworkext.h"
#include "location/LocationProviderInterface.h"

using Elastos::IO::IPrintWriter;
using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Location::CLocation;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::IILocationManager;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::ILocationProvider;
using Elastos::Droid::Location::IProviderProperties;
using Elastos::Droid::Location::IProviderRequest;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IWorkSource;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {


/**
 * A mock location provider used by LocationManagerService to implement test providers.
 *
 * {@hide}
 */
class MockProvider
        : public ILocationProviderInterface
        , public ElRefBase
{
private:

    static const String TAG;// = "MockProvider";

public:

    CAR_INTERFACE_DECL()

    MockProvider(
        /* [in] */ const String& name,
        /* [in] */ IILocationManager* locationManager,
        /* [in] */ IProviderProperties* properties);

    //@Override
    CARAPI GetName(
        /* [out] */ String* name);

    //@Override
    CARAPI GetProperties(
        /* [out] */ IProviderProperties** properties);

    //@Override
    CARAPI Disable();

    //@Override
    CARAPI Enable();

    //@Override
    CARAPI IsEnabled(
        /* [out] */ Boolean* enable);

    //@Override
    CARAPI GetStatus(
        /* [in] */ IBundle* extras,
        /* [out] */ Int32* status);

    //@Override
    CARAPI GetStatusUpdateTime(
        /* [out] */ Int64* time);

    CARAPI_(void) SetLocation(
        /* [in] */ ILocation* l);

    CARAPI_(void) ClearLocation();

    CARAPI_(void) SetStatus(
        /* [in] */ Int32 status,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 updateTime);

    CARAPI_(void) ClearStatus();

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI_(void) Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    //@Override
    CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source);

    //@Override
    CARAPI SwitchUser(
        /* [in] */ Int32 userId);

    //@Override
    CARAPI SendExtraCommand(
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);

private:
    String mName;
    AutoPtr<IProviderProperties> mProperties;
    AutoPtr<IILocationManager> mLocationManager;

    AutoPtr<ILocation> mLocation;
    AutoPtr<IBundle> mExtras;// = new Bundle();

    Int32 mStatus;
    Int64 mStatusUpdateTime;
    Boolean mHasLocation;
    Boolean mHasStatus;
    Boolean mEnabled;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_MOCKPROVIDER_H__
