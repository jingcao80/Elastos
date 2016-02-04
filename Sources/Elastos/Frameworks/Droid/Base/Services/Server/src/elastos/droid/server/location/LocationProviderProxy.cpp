#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/server/location/LocationProviderProxy.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Location::ILocationProvider;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::IBinder;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::IAppendable;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::IO::IFlushable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//===============================
//LocationProviderProxy::NewServiceWorkRunnable
//===============================

LocationProviderProxy::NewServiceWorkRunnable::NewServiceWorkRunnable(
    /* [in] */ LocationProviderProxy* host)
    : mHost(host)
{}

ECode LocationProviderProxy::NewServiceWorkRunnable::Run()
{
   if (D) Logger::D(TAG, "applying state to connected service");

    Boolean enabled;
    AutoPtr<IProviderProperties> properties;
    AutoPtr<IProviderRequest> request;
    AutoPtr<IWorkSource> source;
    AutoPtr<IILocationProvider> service;
    synchronized(this) {
        enabled = mHost->mEnabled;
        request = mHost->mRequest;
        source = mHost->mWorksource;
        service = mHost->GetService();
    }

    if (service == NULL) return E_NULL_POINTER_EXCEPTION;

    // load properties from provider
    ECode ec = service->GetProperties((IProviderProperties**)&properties);
    if (FAILED(ec)) {
        Logger::W(TAG, "%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    if (properties == NULL) {
        Logger::E(TAG, "%s has invalid locatino provider properties",
            mHost->mServiceWatcher->GetBestPackageName().string());
    }

    // apply current state to new service
    if (enabled) {
        service->Enable();
        if (request != NULL) {
            service->SetRequest(request, source);
        }
    }

    synchronized(this) {
        mHost->mProperties = properties;
    }
    return NOERROR;
}

//=================================
// LocationProviderProxy
//=================================

const String LocationProviderProxy::TAG("LocationProviderProxy");
const Boolean LocationProviderProxy::D = FALSE;//ILocationManagerService::D;

CAR_INTERFACE_IMPL(LocationProviderProxy, Object, ILocationProviderInterface)

AutoPtr<LocationProviderProxy> LocationProviderProxy::CreateAndBind(
    /* [in] */ IContext* context,
    /* [in] */ const String& name,
    /* [in] */ const String& action,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNamesResId,
    /* [in] */ IHandler* handler)
{
    AutoPtr<LocationProviderProxy> proxy = new LocationProviderProxy(
        context, name, action, overlaySwitchResId,
        defaultServicePackageNameResId, initialPackageNamesResId, handler);
    if (proxy->Bind()) {
        return proxy;
    } else {
        return NULL;
    }
}

LocationProviderProxy::LocationProviderProxy(
    /* [in] */ IContext* context,
    /* [in] */ const String& name,
    /* [in] */ const String& action,
    /* [in] */ Int32 overlaySwitchResId,
    /* [in] */ Int32 defaultServicePackageNameResId,
    /* [in] */ Int32 initialPackageNamesResId,
    /* [in] */ IHandler* handler)
       : mContext(context)
       , mName(name)
       , mEnabled(FALSE)
{
    CWorkSource::New((IWorkSource**)&mWorksource);
    mNewServiceWork = new NewServiceWorkRunnable(this);
    mServiceWatcher = new ServiceWatcher(
        mContext, TAG + "-" + name, action, overlaySwitchResId,
        defaultServicePackageNameResId, initialPackageNamesResId,
        mNewServiceWork, handler);
}

Boolean LocationProviderProxy::Bind()
{
    return mServiceWatcher->Start();
}

AutoPtr<IILocationProvider> LocationProviderProxy::GetService()
{
    AutoPtr<IBinder> binder = mServiceWatcher->GetBinder();
    return (binder != NULL) ? IILocationProvider::Probe(binder) : NULL;
}

ECode LocationProviderProxy::GetConnectedPackageName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mServiceWatcher->GetBestPackageName();
    return NOERROR;
}

ECode LocationProviderProxy::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode LocationProviderProxy::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    VALIDATE_NOT_NULL(properties)
    synchronized(this) {
        *properties = mProperties;
        REFCOUNT_ADD(*properties);
    }
    return NOERROR;
}

ECode LocationProviderProxy::Enable()
{
    synchronized(this) {
        mEnabled = TRUE;
    }
    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) return E_NULL_POINTER_EXCEPTION;

    ECode ec = service->Enable();
    if (FAILED(ec)) {
        Logger::W(TAG, "%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    // if (ec == E_EXCEPRION) {
    //     // never let remote service crash system server
    //     Logger::E(TAG, "Exception from %s", mServiceWatcher->GetBestPackageName().string());
    //     return ec;
    // }
    return NOERROR;
}

ECode LocationProviderProxy::Disable()
{
    synchronized(this) {
        mEnabled = FALSE;
    }

    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) return E_NULL_POINTER_EXCEPTION;

    ECode ec = service->Disable();
    if (FAILED(ec)) {
        Logger::W(TAG, "%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    // if (ec == E_EXCEPRION) {
    //     // never let remote service crash system server
    //     Logger::E(TAG, "Exception from %s", mServiceWatcher->GetBestPackageName().string());
    //     return ec;
    // }
    return NOERROR;
}

ECode LocationProviderProxy::IsEnabled(
    /* [out] */ Boolean* enable)
{
    VALIDATE_NOT_NULL(enable);
    synchronized(this) {
        *enable = mEnabled;
    }
    return NOERROR;
}

ECode LocationProviderProxy::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    synchronized(this) {
        mRequest = request;
        mWorksource = source;
    }

    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) return E_NULL_POINTER_EXCEPTION;
    ECode ec = service->SetRequest(request, source);
    if (FAILED(ec)) {
        Logger::W(TAG, "%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    // if (ec == E_EXCEPRION) {
    //     // never let remote service crash system server
    //     Logger::E(TAG, "Exception from %s", mServiceWatcher->GetBestPackageName().string());
    //     return ec;
    // }
    return NOERROR;
}

ECode LocationProviderProxy::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    AutoPtr<IAppendable> appendable = IAppendable::Probe(pw);
    AutoPtr<ICharSequence> cs;
    CString::New(String("REMOTE SERVICE"), (ICharSequence**)&cs);
    appendable->Append(cs.Get());
    cs = NULL;
    CString::New(String(" name="), (ICharSequence**)&cs);
    appendable->Append(cs.Get());
    cs = NULL;
    CString::New(mName, (ICharSequence**)&cs);
    appendable->Append(cs.Get());
    cs = NULL;
    CString::New(String(" pkg="), (ICharSequence**)&cs);
    appendable->Append(cs.Get());
    cs = NULL;
    CString::New(mServiceWatcher->GetBestPackageName(), (ICharSequence**)&cs);
    appendable->Append(cs.Get());
    cs = NULL;
    CString::New(String(" version="), (ICharSequence**)&cs);
    appendable->Append(cs.Get());
    StringBuilder sb("");
    sb += mServiceWatcher->GetBestVersion();
    cs = NULL;
    CString::New(sb.ToString(), (ICharSequence**)&cs);
    appendable->Append(cs.Get());
    appendable->AppendChar('\n');

    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) {
        pw->Println(String("service down (null)"));
        return E_NULL_POINTER_EXCEPTION;
    }
    IFlushable::Probe(pw)->Flush();

    //TODO
    // try {
    //     service.asBinder().dump(fd, args);
    // } catch (RemoteException e) {
    //     pw.println("service down (RemoteException)");
    //     Log.w(TAG, e);
    // } catch (Exception e) {
    //     pw.println("service down (Exception)");
    //     // never let remote service crash system server
    //     Log.e(TAG, "Exception from " + mServiceWatcher.getBestPackageName(), e);
    // }
    return NOERROR;
}

ECode LocationProviderProxy::GetStatus(
    /* [in] */ IBundle* extras,
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);
    AutoPtr<IILocationProvider> service = GetService();
    if (service != NULL) {
        *status = ILocationProvider::TEMPORARILY_UNAVAILABLE;
        return NOERROR;
    }
    ECode ec = service->GetStatus((IBundle**)&extras, status);
    if (FAILED(ec)) {
        Logger::W(TAG, "%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    // if (ec == E_EXCEPRION) {
    //     // never let remote service crash system server
    //     Logger::E(TAG, "Exception from %s", mServiceWatcher->GetBestPackageName().string());
    //     return ec;
    // }
    return NOERROR;
}

//@Override
ECode LocationProviderProxy::GetStatusUpdateTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) {
        *time = 0;
        return NOERROR;
    }
    ECode ec = service->GetStatusUpdateTime(time);
    if (FAILED(ec)) {
        Logger::W(TAG, "%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    // if (ec == E_EXCEPRION) {
    //     // never let remote service crash system server
    //     Logger::E(TAG, "Exception from %s", mServiceWatcher->GetBestPackageName().string());
    //     return ec;
    // }
    return NOERROR;
}

ECode LocationProviderProxy::SendExtraCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IILocationProvider> service = GetService();
    if (service == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    ECode ec = service->SendExtraCommand(command, extras, (IBundle**)&extras, result);
    if (FAILED(ec)) {
        Logger::W(TAG, "%08x", ec);
        return E_REMOTE_EXCEPTION;
    }
    // if (ec == E_EXCEPRION) {
    //     // never let remote service crash system server
    //     Logger::E(TAG, "Exception from %s", mServiceWatcher->GetBestPackageName().string());
    //     return ec;
    // }
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
