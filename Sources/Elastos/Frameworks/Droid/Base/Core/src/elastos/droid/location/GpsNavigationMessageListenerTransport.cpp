
#include "Elastos.Droid.Content.h"
#include "elastos/droid/location/GpsNavigationMessageListenerTransport.h"
#include "elastos/droid/location/CGpsNavigationMessageListenerTransportListenerTransport.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Location {

//-------------GpsNavigationMessageListenerTransport::ListenerTransport::Operation-----------------------------//

CAR_INTERFACE_IMPL(GpsNavigationMessageListenerTransport::ListenerTransport::Operation, Object, ILocalListenerHelperListenerOperation)

GpsNavigationMessageListenerTransport::ListenerTransport::Operation::Operation(
    /* [in] */ IGpsNavigationMessageEvent* event)
    : mEvent(event)
{
}

ECode GpsNavigationMessageListenerTransport::ListenerTransport::Operation::Execute(
    /* [in] */ IInterface* listener)
{
    assert(listener == NULL);
    AutoPtr<IGpsNavigationMessageEventListener> gnmel = IGpsNavigationMessageEventListener::Probe(listener);
    if (gnmel != NULL) {
        gnmel->OnGpsNavigationMessageReceived(mEvent);
    }
    return NOERROR;
}

//-------------GpsNavigationMessageListenerTransport::ListenerTransport---------------------------------------------//

CAR_INTERFACE_IMPL_2(GpsNavigationMessageListenerTransport::ListenerTransport, Object, IIGpsMeasurementsListener, IBinder)

GpsNavigationMessageListenerTransport::ListenerTransport::ListenerTransport()
{
}

ECode GpsNavigationMessageListenerTransport::ListenerTransport::constructor(
    /* [in] */ IGpsNavigationMessageListenerTransport* host)
{
    mHost = (GpsNavigationMessageListenerTransport*)host;
    return NOERROR;
}

ECode GpsNavigationMessageListenerTransport::ListenerTransport::OnGpsNavigationMessageReceived(
    /* [in] */ IGpsNavigationMessageEvent* event)
{
    AutoPtr<Operation> operation = new Operation(event);
    AutoPtr<ILocalListenerHelperListenerOperation> llhlo = ILocalListenerHelperListenerOperation::Probe(operation);
    mHost->Foreach(llhlo);
    return NOERROR;
}

ECode GpsNavigationMessageListenerTransport::ListenerTransport::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

//-------------GpsNavigationMessageListenerTransport-------------------------------------------------------------------------//

CAR_INTERFACE_IMPL(GpsNavigationMessageListenerTransport, LocalListenerHelper, IGpsNavigationMessageListenerTransport)

GpsNavigationMessageListenerTransport::GpsNavigationMessageListenerTransport()
    : LocalListenerHelper(String("GpsNavigationMessageListenerTransport"))
{
    AutoPtr<IIGpsNavigationMessageListener> lt;
    CGpsNavigationMessageListenerTransportListenerTransport::New((IGpsNavigationMessageListenerTransport*)this, (IIGpsNavigationMessageListener**)&lt);
    mListenerTransport = IIGpsNavigationMessageListener::Probe(lt);
}

ECode GpsNavigationMessageListenerTransport::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IILocationManager* locationManager)
{
    mContext = context;
    mLocationManager = locationManager;
    return NOERROR;
}


ECode GpsNavigationMessageListenerTransport::RegisterWithServer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String pkgName;
    mContext->GetPackageName(&pkgName);
    return mLocationManager->AddGpsNavigationMessageListener(
            mListenerTransport, pkgName, result);
}

ECode GpsNavigationMessageListenerTransport::UnregisterFromServer()
{
    Boolean result = FALSE;
    return mLocationManager->RemoveGpsNavigationMessageListener(mListenerTransport, &result);
}

}//namespace Location
}//namespace Droid
}//namespace Elastos