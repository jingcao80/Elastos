
#ifndef __ELASTOS_DROID_SERVER_LOCATION_MOCKPROVIDER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_MOCKPROVIDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Location::IILocationManager;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Internal::Location::IProviderProperties;
using Elastos::Droid::Internal::Location::IProviderRequest;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Server::Location::ILocationProviderInterface;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

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
    : public Object
    , public ILocationProviderInterface
{
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

    CARAPI SetLocation(
        /* [in] */ ILocation* l);

    CARAPI ClearLocation();

    CARAPI SetStatus(
        /* [in] */ Int32 status,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 updateTime);

    CARAPI ClearStatus();

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix);

    //@Override
    CARAPI SetRequest(
        /* [in] */ IProviderRequest* request,
        /* [in] */ IWorkSource* source);

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
    AutoPtr<IBundle> mExtras;

    Int32 mStatus;
    Int64 mStatusUpdateTime;
    Boolean mHasLocation;
    Boolean mHasStatus;
    Boolean mEnabled;

    const static String TAG;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_MOCKPROVIDER_H__
