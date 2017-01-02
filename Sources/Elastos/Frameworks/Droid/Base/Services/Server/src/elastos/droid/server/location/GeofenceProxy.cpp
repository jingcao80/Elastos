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

#include "elastos/droid/server/location/GeofenceProxy.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
// using Elastos::Droid::Hardware::Location::EIID_IGeofenceHardwareService;
using Elastos::Droid::Location::IIGeofenceProvider;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//=======================================
// GeofenceProxy::MyRunnable
//=======================================

GeofenceProxy::MyRunnable::MyRunnable(
    /* [in] */ GeofenceProxy* host)
    : mHost(host)
{}

ECode GeofenceProxy::MyRunnable::Run()
{
    Boolean result;
    return mHost->mHandler->SendEmptyMessage(GEOFENCE_PROVIDER_CONNECTED, &result);
}

//=======================================
// GeofenceProxy::MyServiceConnection
//=======================================

CAR_INTERFACE_IMPL(GeofenceProxy::MyServiceConnection, Object, IServiceConnection)

GeofenceProxy::MyServiceConnection::MyServiceConnection(
    /* [in] */ GeofenceProxy* host)
    : mHost(host)
{}

ECode GeofenceProxy::MyServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* service)
{
    {    AutoLock syncLock(this);
        mHost->mGeofenceHardware = IIGeofenceHardware::Probe(service);
        Boolean result;
        mHost->mHandler->SendEmptyMessage(GEOFENCE_HARDWARE_CONNECTED, &result);
    }
    return NOERROR;
}

ECode GeofenceProxy::MyServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    {    AutoLock syncLock(this);
        mHost->mGeofenceHardware = NULL;
        Boolean result;
        mHost->mHandler->SendEmptyMessage(GEOFENCE_HARDWARE_DISCONNECTED, &result);
    }
    return NOERROR;
}

//=======================================
// GeofenceProxy::MyHandler
//=======================================

GeofenceProxy::MyHandler::MyHandler(
    /* [in] */ GeofenceProxy* host)
    : mHost(host)
{}

ECode GeofenceProxy::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case GEOFENCE_PROVIDER_CONNECTED:
            {    AutoLock syncLock(this);
                if (mHost->mGeofenceHardware != NULL) {
                    mHost->SetGeofenceHardwareInProviderLocked();
                }
                // else: the geofence provider will be notified when the connection to
                // GeofenceHardwareService is established.
            }
            break;
        case GEOFENCE_HARDWARE_CONNECTED:
            {    AutoLock syncLock(this);
                // Theoretically this won't happen because once the GeofenceHardwareService
                // is connected to, we won't lose connection to it because it's a system
                // service. But this check does make the code more robust.
                if (mHost->mGeofenceHardware != NULL) {
                    mHost->SetGpsGeofenceLocked();
                    mHost->SetFusedGeofenceLocked();
                    mHost->SetGeofenceHardwareInProviderLocked();
                }
            }
            break;
        case GEOFENCE_HARDWARE_DISCONNECTED:
            {    AutoLock syncLock(this);
                if (mHost->mGeofenceHardware == NULL) {
                    mHost->SetGeofenceHardwareInProviderLocked();
                }
            }
            break;
    }
    return NOERROR;
}

//=======================================
// GeofenceProxy
//=======================================

const String GeofenceProxy::TAG("GeofenceProxy");
const String GeofenceProxy::SERVICE_ACTION("com.android.location.service.GeofenceProvider");
const Int32 GeofenceProxy::GEOFENCE_PROVIDER_CONNECTED;
const Int32 GeofenceProxy::GEOFENCE_HARDWARE_CONNECTED;
const Int32 GeofenceProxy::GEOFENCE_HARDWARE_DISCONNECTED;
const Int32 GeofenceProxy::GEOFENCE_GPS_HARDWARE_CONNECTED;
const Int32 GeofenceProxy::GEOFENCE_GPS_HARDWARE_DISCONNECTED;

AutoPtr<GeofenceProxy> GeofenceProxy::CreateAndBind(
    /* [in] */ IContext* context,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNameResId,
    /* [in] */ IHandler* handler,
    /* [in] */ IIGpsGeofenceHardware* gpsGeofence,
    /* [in] */ IIFusedGeofenceHardware* fusedGeofenceHardware)
{
    AutoPtr<GeofenceProxy> proxy = new GeofenceProxy(context, overlaySwitchResId,
        defaultServicePackageNameResId, initialPackageNameResId, handler, gpsGeofence,
        fusedGeofenceHardware);
    return (proxy->BindGeofenceProvider()) ? proxy : NULL;
}

GeofenceProxy::GeofenceProxy(
    /* [in] */ IContext* context,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNameResId,
    /* [in] */ IHandler* handler,
    /* [in] */ IIGpsGeofenceHardware* gpsGeofence,
    /* [in] */ IIFusedGeofenceHardware* fusedGeofenceHardware)
    : mContext(context)
    , mGpsGeofenceHardware(gpsGeofence)
    , mFusedGeofenceHardware(fusedGeofenceHardware)
{
    mServiceWatcher = new ServiceWatcher(context, TAG, SERVICE_ACTION, overlaySwitchResId,
        defaultServicePackageNameResId, initialPackageNameResId, mRunnable, handler);
    BindHardwareGeofence();
    mRunnable = new MyRunnable(this);
    mServiceConnection = new MyServiceConnection(this);
    mHandler = new MyHandler(this);
}

Boolean GeofenceProxy::BindGeofenceProvider()
{
    return mServiceWatcher->Start();
}

void GeofenceProxy::BindHardwareGeofence()
{
#if 0
    AutoPtr<IIntent> intent;
    CIntent::New(mContext, EIID_IGeofenceHardwareService, (IIntent**)&intent);
    Boolean result;
    mContext->BindServiceAsUser(intent,
        mServiceConnection, IContext::BIND_AUTO_CREATE, UserHandle::OWNER, &result);
#endif
}

void GeofenceProxy::SetGeofenceHardwareInProviderLocked()
{
    AutoPtr<IBinder> b = mServiceWatcher->GetBinder();
    AutoPtr<IIGeofenceProvider> provider = IIGeofenceProvider::Probe(b);
    if (provider != NULL) {
        ECode ec = provider->SetGeofenceHardware(mGeofenceHardware);
        if (FAILED(ec)) {
            Logger::E(TAG, "Remote Exception: setGeofenceHardwareInProviderLocked: %08x", ec);
            // return E_REMOTE_EXCEPTION;
        }
    }
}

void GeofenceProxy::SetGpsGeofenceLocked()
 {
    ECode ec = mGeofenceHardware->SetGpsGeofenceHardware(mGpsGeofenceHardware);
    if (FAILED(ec)) {
        Logger::E(TAG, "Error while connecting to GeofenceHardwareService");
        // return E_REMOTE_EXCEPTION;
    }
}

void GeofenceProxy::SetFusedGeofenceLocked()
{
    ECode ec = mGeofenceHardware->SetFusedGeofenceHardware(mFusedGeofenceHardware);
    if (FAILED(ec)) {
        Logger::E(TAG, "Error while connecting to GeofenceHardwareService");
        // return E_REMOTE_EXCEPTION;
    }
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos