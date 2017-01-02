//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Content.h"
#include "elastos/droid/location/GpsMeasurementListenerTransport.h"
#include "elastos/droid/location/CGpsMeasurementListenerTransportListenerTransport.h"

using Elastos::Droid::Os::EIID_IBinder;

namespace Elastos {
namespace Droid {
namespace Location {

//==============================================
//GpsMeasurementListenerTransport::ListenerTransport::Operation
//==============================================

CAR_INTERFACE_IMPL(GpsMeasurementListenerTransport::ListenerTransport::Operation, Object, ILocalListenerHelperListenerOperation)

GpsMeasurementListenerTransport::ListenerTransport::Operation::Operation(
    /* [in] */ IGpsMeasurementsEvent* event)
    : mEvent(event)
{
}

ECode GpsMeasurementListenerTransport::ListenerTransport::Operation::Execute(
    /* [in] */ IInterface* listener)
{
    assert(listener == NULL);
    AutoPtr<IGpsMeasurementsEventListener> gmel = IGpsMeasurementsEventListener::Probe(listener);
    if (gmel != NULL) {
        gmel->OnGpsMeasurementsReceived(mEvent);
    }
    return NOERROR;
}

//==============================================
//GpsMeasurementListenerTransport::ListenerTransport
//==============================================

CAR_INTERFACE_IMPL_2(GpsMeasurementListenerTransport::ListenerTransport, Object, IIGpsMeasurementsListener, IBinder)

GpsMeasurementListenerTransport::ListenerTransport::ListenerTransport()
{
}

ECode GpsMeasurementListenerTransport::ListenerTransport::constructor(
    /* [in] */ IGpsMeasurementListenerTransport* host)
{
    mHost = (GpsMeasurementListenerTransport*)host;
    return NOERROR;
}

ECode GpsMeasurementListenerTransport::ListenerTransport::OnGpsMeasurementsReceived(
    /* [in] */ IGpsMeasurementsEvent* event)
{
    AutoPtr<Operation> operation = new Operation(event);
    AutoPtr<ILocalListenerHelperListenerOperation> llhlo = ILocalListenerHelperListenerOperation::Probe(operation);
    mHost->Foreach(llhlo);
    return NOERROR;
}

ECode GpsMeasurementListenerTransport::ListenerTransport::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

//==============================================
//GpsMeasurementListenerTransport
//==============================================

CAR_INTERFACE_IMPL(GpsMeasurementListenerTransport, LocalListenerHelper, IGpsMeasurementListenerTransport)

GpsMeasurementListenerTransport::GpsMeasurementListenerTransport()
    : LocalListenerHelper(String("GpsMeasurementListenerTransport"))
    , mContext(NULL)
{
}

ECode GpsMeasurementListenerTransport::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IILocationManager* locationManager)
{
    AutoPtr<IIGpsMeasurementsListener> lt;
    CGpsMeasurementListenerTransportListenerTransport::New(this, (IIGpsMeasurementsListener**)&mListenerTransport);

    mContext = context;
    mLocationManager = locationManager;
    return NOERROR;
}


ECode GpsMeasurementListenerTransport::RegisterWithServer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    String pkgName;
    mContext->GetPackageName(&pkgName);
    return mLocationManager->AddGpsMeasurementsListener(
            mListenerTransport, pkgName, result);
}

ECode GpsMeasurementListenerTransport::UnregisterFromServer()
{
    Boolean result = FALSE;
    return mLocationManager->RemoveGpsMeasurementsListener(mListenerTransport, &result);
}

}//namespace Location
}//namespace Droid
}//namespace Elastos
