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
    , mContext(NULL)
{
}

ECode GpsNavigationMessageListenerTransport::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IILocationManager* locationManager)
{
    mContext = context;
    mLocationManager = locationManager;
    CGpsNavigationMessageListenerTransportListenerTransport::New(this, (IIGpsNavigationMessageListener**)&mListenerTransport);
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