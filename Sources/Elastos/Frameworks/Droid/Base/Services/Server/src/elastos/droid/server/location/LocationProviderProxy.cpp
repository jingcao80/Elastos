
#include "location/LocationProviderProxy.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Location::EIID_IILocationProvider;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String LocationProviderProxy::TAG("LocationProviderProxy");
const Boolean LocationProviderProxy::D = FALSE;//ILocationManagerService::D;

CAR_INTERFACE_IMPL(LocationProviderProxy::NewServiceWorkRunnable, IRunnable)

LocationProviderProxy::NewServiceWorkRunnable::NewServiceWorkRunnable(
    /* [in] */ LocationProviderProxy* host) : mHost(host)
{}

ECode LocationProviderProxy::NewServiceWorkRunnable::Run()
{
//    if (D) Log.d(TAG, "applying state to connected service");

    Boolean enabled;
    AutoPtr<IProviderProperties> properties;
    AutoPtr<IProviderRequest> request;
    AutoPtr<IWorkSource> source;
    AutoPtr<IILocationProvider> service;
    {
        AutoLock lock(mHost->mLock);
        enabled = mHost->mEnabled;
        request = mHost->mRequest;
        source = mHost->mWorksource;
        service = mHost->GetService();
    }

    if (service == NULL) return NOERROR;

//    try {
        // load properties from provider
        service->GetProperties((IProviderProperties**)&properties);
        if (properties == NULL) {
           Slogger::E(TAG, " has invalid locatino provider properties"/*, mServiceWatcher.getBestPackageName().string()*/);
        }

        // apply current state to new service
        if (enabled) {
            service->Enable();
            if (request != NULL) {
                service->SetRequest(request, source);
            }
        }
//    } catch (RemoteException e) {
//        Log.w(TAG, e);
//    } catch (Exception e) {
        // never let remote service crash system server
//        Log.e(TAG, "Exception from " + mServiceWatcher.getBestPackageName(), e);
//    }

    {
        AutoLock lock(mHost->mLock);
        mHost->mProperties = properties;
    }
    return NOERROR;
}

LocationProviderProxy::LocationProviderProxy(
    /* [in] */ IContext* context,
    /* [in] */ const String& name,
    /* [in] */ const String& action,
    /* [in] */ List<String>* initialPackageNames,
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 userId)
       : mContext(context)
       , mName(name)
       , mEnabled(FALSE)
{
    CWorkSource::New((IWorkSource**)&mWorksource);
    mNewServiceWork = new NewServiceWorkRunnable(this);
    mServiceWatcher = new ServiceWatcher(mContext, TAG, action, initialPackageNames,
            mNewServiceWork, handler, userId);
}

AutoPtr<LocationProviderProxy> LocationProviderProxy::CreateAndBind(
    /* [in] */ IContext* context,
    /* [in] */ const String& name,
    /* [in] */ const String& action,
    /* [in] */ List<String>* initialPackageNames,
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 userId)
{
    AutoPtr<LocationProviderProxy> proxy = new LocationProviderProxy(context, name, action,
            initialPackageNames, handler, userId);
    if (proxy->Bind()) {
        return proxy;
    } else {
        return NULL;
    }
}

PInterface LocationProviderProxy::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(ILocationProviderInterface*)this;
    }
    else if (riid == EIID_ILocationProviderInterface) {
        return (ILocationProviderInterface*)this;
    }

    return NULL;
}

UInt32 LocationProviderProxy::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 LocationProviderProxy::Release()
{
    return ElRefBase::Release();
}

ECode LocationProviderProxy::GetInterfaceID(
    /* [in] */ IInterface *object,
        /* [out] */ InterfaceID *IID)
{
    return E_NOT_IMPLEMENTED;
}

String LocationProviderProxy::GetConnectedPackageName()
{
    return mServiceWatcher->GetBestPackageName();
}

//@Override
ECode LocationProviderProxy::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

//@Override
ECode LocationProviderProxy::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    VALIDATE_NOT_NULL(properties)
    AutoLock lock(mLock);
    *properties = mProperties;
    REFCOUNT_ADD(*properties);
    return NOERROR;
}

//@Override
ECode LocationProviderProxy::Enable()
{
    //synchronized(mLock)
    {
        AutoLock lock(mLock);
        mEnabled = TRUE;
    }

    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) return NOERROR;

    //try {
    return service->Enable();
    //} catch (RemoteException e) {
    //    Log.w(TAG, e);
    //} catch (Exception e) {
        // never let remote service crash system server
    //    Log.e(TAG, "Exception from " + mServiceWatcher.getBestPackageName(), e);
    //}

}

//@Override
ECode LocationProviderProxy::Disable()
{
    //synchronized(mLock)
    {
        AutoLock lock(mLock);
        mEnabled = FALSE;
    }

    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) return NOERROR;

    //try {
    return service->Disable();
    //} catch (RemoteException e) {
    //    Log.w(TAG, e);
    //} catch (Exception e) {
        // never let remote service crash system server
    //    Log.e(TAG, "Exception from " + mServiceWatcher.getBestPackageName(), e);
    //}

}

//@Override
ECode LocationProviderProxy::IsEnabled(
    /* [out] */ Boolean* enable)
{
    AutoLock lock(mLock);
    VALIDATE_NOT_NULL(enable);
    *enable = mEnabled;
    return NOERROR;
}

//@Override
ECode LocationProviderProxy::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    //synchronized(mLock)
    {
        AutoLock lock(mLock);
        mRequest = request;
        mWorksource = source;
    }

    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) return NOERROR;

    //try {
    return service->SetRequest(request, source);
    //} catch (RemoteException e) {
    //    Log.w(TAG, e);
    //} catch (Exception e) {
        // never let remote service crash system server
    //    Log.e(TAG, "Exception from " + mServiceWatcher.getBestPackageName(), e);
    //}
}

//@Override
ECode LocationProviderProxy::SwitchUser(
    /* [in] */ Int32 userId)
{
    mServiceWatcher->SwitchUser(userId);
    return NOERROR;
}

//@Override
ECode LocationProviderProxy::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{/*
    pw.append("REMOTE SERVICE");
    pw.append(" name=").append(mName);
    pw.append(" pkg=").append(mServiceWatcher.getBestPackageName());
    pw.append(" version=").append("" + mServiceWatcher.getBestVersion());
    pw.append('\n');

    ILocationProvider service = getService();
    if (service == null) {
        pw.println("service down (null)");
        return;
    }
    pw.flush();

    try {
        service.asBinder().dump(fd, args);
    } catch (RemoteException e) {
        pw.println("service down (RemoteException)");
        Log.w(TAG, e);
    } catch (Exception e) {
        pw.println("service down (Exception)");
        // never let remote service crash system server
        Log.e(TAG, "Exception from " + mServiceWatcher.getBestPackageName(), e);
    }*/

    return NOERROR;
}

//@Override
ECode LocationProviderProxy::GetStatus(
    /* [in] */ IBundle* extras,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);

    *status = ILocationProvider::TEMPORARILY_UNAVAILABLE;
    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) {
        return NOERROR;
    }

    //try {
        return service->GetStatus(&extras, status);
    //} catch (RemoteException e) {
    //    Log.w(TAG, e);
    //} catch (Exception e) {
        // never let remote service crash system server
    //    Log.e(TAG, "Exception from " + mServiceWatcher.getBestPackageName(), e);
    //}
}

//@Override
ECode LocationProviderProxy::GetStatusUpdateTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);

    *time = 0;
    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) {
        return NOERROR;
    }

    //try {
        return service->GetStatusUpdateTime(time);
    //} catch (RemoteException e) {
    //    Log.w(TAG, e);
    //} catch (Exception e) {
        // never let remote service crash system server
    //    Log.e(TAG, "Exception from " + mServiceWatcher.getBestPackageName(), e);
    //}
}

//@Override
ECode LocationProviderProxy::SendExtraCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) {
        return NOERROR;
    }

    //try {
        return service->SendExtraCommand(command, &extras, result);
    //} catch (RemoteException e) {
    //    Log.w(TAG, e);
    //} catch (Exception e) {
        // never let remote service crash system server
    //    Log.e(TAG, "Exception from " + mServiceWatcher.getBestPackageName(), e);
    //}
}

Boolean LocationProviderProxy::Bind()
{
    return mServiceWatcher->Start();
}

AutoPtr<IILocationProvider> LocationProviderProxy::GetService()
{
//  return ILocationProvider.Stub.asInterface(mServiceWatcher.getBinder());
    AutoPtr<IBinder> binder = mServiceWatcher->GetBinder();
    if (binder != NULL) {
        AutoPtr<IILocationProvider> temp = (IILocationProvider*)binder->Probe(EIID_IILocationProvider);
        return temp;
    } else {
        return NULL;
    }
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
