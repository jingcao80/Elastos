
#include "location/MockProvider.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

// {ac01c312-65e8-445a-a973-2ea720a308a8}
extern "C" const InterfaceID EIID_ILocationProviderInterface =
        { 0xac01c312, 0x65e8, 0x445a, { 0xa9, 0x73, 0x2e, 0xa7, 0x20, 0xa3, 0x08, 0xa8 } };

const String MockProvider::TAG("MockProvider");

CAR_INTERFACE_IMPL(MockProvider, ILocationProviderInterface)

MockProvider::MockProvider(
    /* [in] */ const String& name,
    /* [in] */ IILocationManager* locationManager,
    /* [in] */ IProviderProperties* properties)
{
    //if (properties == null) throw new NullPointerException("properties is null");
    assert(properties != NULL);

    mName = name;
    mLocationManager = locationManager;
    mProperties = properties;
    CLocation::New(name, (ILocation**)&mLocation);
}

//@Override
ECode MockProvider::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

//@Override
ECode MockProvider::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    VALIDATE_NOT_NULL(properties);
    *properties = mProperties;
    REFCOUNT_ADD(*properties);

    return NOERROR;
}

//@Override
ECode MockProvider::Disable()
{
    mEnabled = FALSE;
    return NOERROR;
}

//@Override
ECode MockProvider::Enable()
{
    mEnabled = TRUE;
    return NOERROR;
}

//@Override
ECode MockProvider::IsEnabled(
    /* [out] */ Boolean* enable)
{
    VALIDATE_NOT_NULL(enable);
    *enable = mEnabled;
    return NOERROR;
}

//@Override
ECode MockProvider::GetStatus(
    /* [in] */ IBundle* extras,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(extras);
    VALIDATE_NOT_NULL(status);

    if (mHasStatus) {
        extras->Clear();
        extras->PutAll(mExtras);
        *status = mStatus;
    } else {
        *status = ILocationProvider::AVAILABLE;
    }

    return NOERROR;
}

//@Override
ECode MockProvider::GetStatusUpdateTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mStatusUpdateTime;
    return NOERROR;
}

void MockProvider::SetLocation(
    /* [in] */ ILocation* l)
{
    mLocation->Set(l);
    mHasLocation = TRUE;
    if (mEnabled) {
        //try {
            mLocationManager->ReportLocation(mLocation, FALSE);
        //} catch (RemoteException e) {
        //    Log.e(TAG, "RemoteException calling reportLocation");
        //}
    }
}

void MockProvider::ClearLocation()
{
    mHasLocation = FALSE;
}

void MockProvider::SetStatus(
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 updateTime)
{
    mStatus = status;
    mStatusUpdateTime = updateTime;
    mExtras->Clear();
    if (extras != NULL) {
        mExtras->PutAll(extras);
    }
    mHasStatus = TRUE;
}

void MockProvider::ClearStatus()
{
    mHasStatus = FALSE;
    mStatusUpdateTime = 0;
}

//@Override
ECode MockProvider::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Dump(pw, String(""));
    return NOERROR;
}

void MockProvider::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{/*
    pw.println(prefix + mName);
    pw.println(prefix + "mHasLocation=" + mHasLocation);
    pw.println(prefix + "mLocation:");
    mLocation.dump(new PrintWriterPrinter(pw), prefix + "  ");
    pw.println(prefix + "mHasStatus=" + mHasStatus);
    pw.println(prefix + "mStatus=" + mStatus);
    pw.println(prefix + "mStatusUpdateTime=" + mStatusUpdateTime);
    pw.println(prefix + "mExtras=" + mExtras);
*/
}

//@Override
ECode MockProvider::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    return NOERROR;
}

//@Override
ECode MockProvider::SwitchUser(
    /* [in] */ Int32 userId)
{
    // nothing to do here
    return NOERROR;
}

//@Override
ECode MockProvider::SendExtraCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
