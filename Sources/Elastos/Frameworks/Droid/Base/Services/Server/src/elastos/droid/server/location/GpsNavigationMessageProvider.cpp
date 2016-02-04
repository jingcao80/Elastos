
#include "elastos/droid/server/location/GpsNavigationMessageProvider.h"

using Elastos::Droid::Location::IIGpsNavigationMessageListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//====================================
//GpsMeasurementsProvider::MyOperation
//====================================

CAR_INTERFACE_IMPL(GpsNavigationMessageProvider::MyOperation, Object, IListenerOperation)

GpsNavigationMessageProvider::MyOperation::MyOperation(
    /* [in] */ IGpsNavigationMessageEvent* event)
    : mEvent(event)
{}

ECode GpsNavigationMessageProvider::MyOperation::Execute(
    /* [in] */ IInterface* listener)
{
    AutoPtr<IIGpsNavigationMessageListener> gnmListener = IIGpsNavigationMessageListener::Probe(listener);
    gnmListener->OnGpsNavigationMessageReceived(mEvent);
    return NOERROR;
}

//====================================
//GpsNavigationMessageProvider
//====================================

GpsNavigationMessageProvider::GpsNavigationMessageProvider()
    :RemoteListenerHelper(String("GpsNavigationMessageProvider"))
{}

ECode GpsNavigationMessageProvider::OnNavigationMessageAvailable(
    /* [in] */ IGpsNavigationMessageEvent* event)
{
    AutoPtr<MyOperation> mo = new MyOperation(event);
    AutoPtr<IListenerOperation> operation = (IListenerOperation*)mo.Get();

    RemoteListenerHelper::Foreach(operation.Get());
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
