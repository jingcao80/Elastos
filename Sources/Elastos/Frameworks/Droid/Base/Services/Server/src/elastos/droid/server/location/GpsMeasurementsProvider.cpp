
#include "elastos/droid/server/location/GpsMeasurementsProvider.h"

using Elastos::Droid::Location::IIGpsMeasurementsListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//====================================
//GpsMeasurementsProvider::MyOperation
//====================================

CAR_INTERFACE_IMPL(GpsMeasurementsProvider::MyOperation, Object, IListenerOperation)

GpsMeasurementsProvider::MyOperation::MyOperation(
    /* [in] */ IGpsMeasurementsEvent* event)
    : mEvent(event)
{}

ECode GpsMeasurementsProvider::MyOperation::Execute(
    /* [in] */ IInterface* listener)
{
    AutoPtr<IIGpsMeasurementsListener> gmListener = IIGpsMeasurementsListener::Probe(listener);
    gmListener->OnGpsMeasurementsReceived(mEvent);
    return NOERROR;
}

//====================================
//GpsMeasurementsProvider
//====================================

GpsMeasurementsProvider::GpsMeasurementsProvider()
    : RemoteListenerHelper(String("GpsMeasurementsProvider"))
{}

ECode GpsMeasurementsProvider::OnMeasurementsAvailable(
    /* [in] */ IGpsMeasurementsEvent* event)
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
