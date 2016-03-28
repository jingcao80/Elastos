
#include "elastos/droid/server/location/GeoFencerProxy.h"
#include "elastos/droid/server/location/CGeoFenceListener.h"
#include "Elastos.Droid.App.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Location::EIID_IIGeoFenceListener;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

CAR_INTERFACE_IMPL(GeoFencerProxy::ServiceConnection, Object, IServiceConnection)

GeoFencerProxy::ServiceConnection::ServiceConnection(
    /* [in] */ GeoFencerProxy* host)
    : mHost(host)
{}

ECode GeoFencerProxy::ServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    synchronized (this) {
        mHost->mGeoFencer = IIGeoFencer::Probe(service);
        NotifyAll();
    }
    Logger::V(TAG, "onServiceConnected: mGeoFencer - %s", TO_CSTR(mHost->mGeoFencer));
    return NOERROR;
}

ECode GeoFencerProxy::ServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    synchronized (this) {
        mHost->mGeoFencer = NULL;
    }
    Logger::V(TAG, "onServiceDisconnected");
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(GeoFencerProxy::GeoFenceListener, Object, IIGeoFenceListener, IBinder)

ECode GeoFencerProxy::GeoFenceListener::constructor(
    /* [in] */ Handle64 host)
{
    mHost = (GeoFencerProxy*)host;
    return NOERROR;
}

ECode GeoFencerProxy::GeoFenceListener::GeoFenceExpired(
    /* [in] */ IPendingIntent* intent)
{
    if (LOGV_ENABLED)
        Logger::V(TAG, "geoFenceExpired - %s", TO_CSTR(intent));
    mHost->Remove(intent, TRUE);
    return NOERROR;
}

ECode GeoFencerProxy::GeoFenceListener::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

const String GeoFencerProxy::TAG("GeoFencerProxy");
const Boolean GeoFencerProxy::LOGV_ENABLED = TRUE;
AutoPtr<GeoFencerProxy> GeoFencerProxy::mGeoFencerProxy;

AutoPtr<GeoFencerProxy> GeoFencerProxy::GetGeoFencerProxy(
    /* [in] */ IContext* context,
    /* [in] */ const String& serviceName)
{
    if (mGeoFencerProxy == NULL) {
        mGeoFencerProxy = new GeoFencerProxy(context, serviceName);
    }
    return mGeoFencerProxy;
}

GeoFencerProxy::GeoFencerProxy(
    /* [in] */ IContext* context,
    /* [in] */ const String& serviceName)
{
    mContext = context;
    CIntent::New((IIntent**)&mIntent);
    mIntent->SetPackage(serviceName);
    Boolean res;
    mContext->BindService(mIntent, mServiceConnection,
        IContext::BIND_AUTO_CREATE | IContext::BIND_NOT_FOREGROUND
        | IContext::BIND_ALLOW_OOM_MANAGEMENT, &res);

    mServiceConnection = new ServiceConnection(this);
    CGeoFenceListener::New((Handle64)this, (IBinder**)&mListener);
}

void GeoFencerProxy::RemoveCaller(
    /* [in] */ Int32 uid)
{
    GeoFencerBase::RemoveCaller(uid);
    if (mGeoFencer != NULL) {
        mGeoFencer->ClearGeoFenceUser(uid);
    }
    else
        Logger::E(TAG, "removeCaller - mGeoFencer is null");
}

Boolean GeoFencerProxy::EnsureGeoFencer()
{
    if (mGeoFencer == NULL) {
        synchronized(mServiceConnection) {
            Logv(String("waiting..."));
            if (FAILED(mServiceConnection->Wait(60000))) {
                Logger::W(TAG, "Interrupted while waiting for GeoFencer");
                return FALSE;
            }
            Logv(String("woke up!!!"));
        }

        if (mGeoFencer == NULL) {
            Logger::W(TAG, "Timed out. No GeoFencer connection");
            return FALSE;
        }
    }

    return TRUE;
}

Boolean GeoFencerProxy::Start(
    /* [in] */ IGeoFenceParams* geofence)
{
    if (EnsureGeoFencer()) {
        Boolean res;
        mGeoFencer->SetGeoFence(mListener, geofence, &res);
        return res;
    }
    return FALSE;
}

Boolean GeoFencerProxy::Stop(
    /* [in] */ IPendingIntent* intent)
{
    if (EnsureGeoFencer()) {
        mGeoFencer->ClearGeoFence(mListener, intent);
        return TRUE;
    }
    return FALSE;
}

void GeoFencerProxy::Logv(
    /* [in] */ const String& s)
{
    if (LOGV_ENABLED) Logger::V(TAG, s);
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
