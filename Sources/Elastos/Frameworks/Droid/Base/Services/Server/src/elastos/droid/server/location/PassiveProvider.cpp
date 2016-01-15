
#include "location/PassiveProvider.h"

using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::ILocationProvider;
using Elastos::Droid::Location::CProviderRequest;
using Elastos::Droid::Location::IProviderRequest;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {
CAR_INTERFACE_IMPL(PassiveProvider, ILocationProviderInterface)

static AutoPtr<IProviderProperties> PROPERTIES_Init()
{
    AutoPtr<IProviderProperties> temp;
    CProviderProperties::New(FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
        ICriteria::Criteria_POWER_LOW, ICriteria::Criteria_ACCURACY_COARSE,
        (IProviderProperties**)&temp);

    return temp;
}

const String PassiveProvider::TAG("PassiveProvider");

AutoPtr<IProviderProperties> PassiveProvider::PROPERTIES = PROPERTIES_Init();

PassiveProvider::PassiveProvider(
    /* [in] */ IILocationManager* locationManager)
    : mLocationManager(locationManager)
{
}

//@Override
ECode PassiveProvider::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = ILocationManager::PASSIVE_PROVIDER;
    return NOERROR;
}

//@Override
ECode PassiveProvider::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    VALIDATE_NOT_NULL(properties);
    *properties = PROPERTIES;
    REFCOUNT_ADD(*properties);
    return NOERROR;
}

//@Override
ECode PassiveProvider::IsEnabled(
    /* [out] */ Boolean* enable)
{
    VALIDATE_NOT_NULL(enable);
    *enable = TRUE;
    return NOERROR;
}

//@Override
ECode PassiveProvider::Enable()
{
    return NOERROR;
}

//@Override
ECode PassiveProvider::Disable()
{
    return NOERROR;
}

//@Override
ECode PassiveProvider::GetStatus(
    /* [in] */ IBundle* extras,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);

    if (mReportLocation) {
        *status = ILocationProvider::AVAILABLE;
    } else {
        *status = ILocationProvider::TEMPORARILY_UNAVAILABLE;
    }

    return NOERROR;
}

//@Override
ECode PassiveProvider::GetStatusUpdateTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = -1;
    return NOERROR;
}

//@Override
ECode PassiveProvider::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    VALIDATE_NOT_NULL(request);
//    CProviderRequest* _request = (CProviderRequest*)request;
//    mReportLocation = _request->reportLocation;
    return NOERROR;
}

//@Override
ECode PassiveProvider::SwitchUser(
    /* [in] */ Int32 userId)
{
    // nothing to do here
    return NOERROR;
}

ECode PassiveProvider::UpdateLocation(
    /* [in] */ ILocation* location)
{
    if (mReportLocation) {
        //try {
            // pass the location back to the location manager
            return mLocationManager->ReportLocation(location, TRUE);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "RemoteException calling reportLocation");
        //}
    }

    return NOERROR;
}

//@Override
ECode PassiveProvider::SendExtraCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

//@Override
ECode PassiveProvider::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
//  pw.println("mReportLocation=" + mReportLocation);
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
