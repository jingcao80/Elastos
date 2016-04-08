
#include "elastos/droid/systemui/statusbar/policy/CastControllerImpl.h"
#include "elastos/droid/systemui/statusbar/policy/CastDevice.h"
#include "../../R.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Media::Projection::EIID_IMediaProjectionManagerCallback;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::IUUID;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

const String CastControllerImpl::TAG("CastController");
const Boolean CastControllerImpl::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);

CastControllerImpl::MediaCallback::MediaCallback(
    /* [in] */ CastControllerImpl* host)
    : mHost(host)
{}

ECode CastControllerImpl::MediaCallback::OnRouteAdded(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* route)
{
    if (DEBUG) Logger::D(TAG, "onRouteAdded: %s", RouteToString(route).string());
    mHost->UpdateRemoteDisplays();
    return NOERROR;
}

ECode CastControllerImpl::MediaCallback::OnRouteChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* route)
{
    if (DEBUG) Logger::D(TAG, "onRouteChanged: %s", RouteToString(route).string());
    mHost->UpdateRemoteDisplays();
    return NOERROR;
}

ECode CastControllerImpl::MediaCallback::OnRouteRemoved(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* route)
{
    if (DEBUG) Logger::D(TAG, "onRouteRemoved: %s", RouteToString(route).string());
    mHost->UpdateRemoteDisplays();
    return NOERROR;
}

ECode CastControllerImpl::MediaCallback::OnRouteSelected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* route)
{
    if (DEBUG) Logger::D(TAG, "onRouteSelected(%d): %s", type, RouteToString(route).string());
    mHost->UpdateRemoteDisplays();
    return NOERROR;
}

ECode CastControllerImpl::MediaCallback::OnRouteUnselected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* route)
{
    if (DEBUG) Logger::D(TAG, "onRouteUnselected(%d): %s", type, RouteToString(route).string());
    mHost->UpdateRemoteDisplays();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CastControllerImpl::MediaProjectionManagerCallback, Object, IMediaProjectionManagerCallback);
CastControllerImpl::MediaProjectionManagerCallback::MediaProjectionManagerCallback(
    /* [in] */ CastControllerImpl* host)
    : mHost(host)
{}

ECode CastControllerImpl::MediaProjectionManagerCallback::OnStart(
    /* [in] */ IMediaProjectionInfo* info)
{
    mHost->SetProjection(info, TRUE);
    return NOERROR;
}

ECode CastControllerImpl::MediaProjectionManagerCallback::OnStop(
    /* [in] */ IMediaProjectionInfo* info)
{
    mHost->SetProjection(info, FALSE);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CastControllerImpl, Object, ICastController);
CastControllerImpl::CastControllerImpl(
    /* [in] */ IContext* context)
    : mDiscovering(FALSE)
    , mCallbackRegistered(FALSE)
{
    CArrayList::New((IArrayList**)&mCallbacks);
    CArrayMap::New((IArrayMap**)&mRoutes);
    mMediaCallback = new MediaCallback(this);
    mProjectionCallback = new MediaProjectionManagerCallback(this);
    mContext = context;
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&obj);
    mMediaRouter = IMediaRouter::Probe(obj);

    obj = NULL;
    context->GetSystemService(IContext::MEDIA_PROJECTION_SERVICE ,(IInterface**)&obj);
    mProjectionManager = IMediaProjectionManager::Probe(obj);
    mProjectionManager->GetActiveProjectionInfo((IMediaProjectionInfo**)&mProjection);
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mProjectionManager->AddCallback(mProjectionCallback, handler);
    if (DEBUG) Logger::D(TAG, "new CastController()");
}

ECode CastControllerImpl::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("CastController state:"));
    pw->Print(String("  mDiscovering="));
    pw->Println(mDiscovering);
    pw->Print(String("  mCallbackRegistered="));
    pw->Println(mCallbackRegistered);
    pw->Print(String("  mCallbacks.size="));
    Int32 size = 0;
    mCallbacks->GetSize(&size);
    pw->Println(size);
    pw->Print(String("  mRoutes.size="));
    mRoutes->GetSize(&size);
    pw->Println(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mRoutes->GetValueAt(i, (IInterface**)&obj);
        AutoPtr<IMediaRouterRouteInfo> route = IMediaRouterRouteInfo::Probe(obj);
        pw->Print(String("    "));
        pw->Println(RouteToString(route));
    }
    pw->Print(String("  mProjection="));
    pw->Println(mProjection);
    return NOERROR;
}

ECode CastControllerImpl::AddCallback(
    /* [in] */ ICastControllerCallback* callback)
{
    mCallbacks->Add(callback);
    FireOnCastDevicesChanged(callback);
    synchronized (mDiscoveringLock) {
        HandleDiscoveryChangeLocked();
    }
    return NOERROR;
}

ECode CastControllerImpl::RemoveCallback(
    /* [in] */ ICastControllerCallback* callback)
{
    mCallbacks->Remove(callback);
    synchronized (mDiscoveringLock) {
        HandleDiscoveryChangeLocked();
    }
    return NOERROR;
}

ECode CastControllerImpl::SetDiscovering(
    /* [in] */ Boolean request)
{
    synchronized (mDiscoveringLock) {
        if (mDiscovering == request) return NOERROR;
        mDiscovering = request;
        if (DEBUG) Logger::D(TAG, "setDiscovering: %d", request);
        HandleDiscoveryChangeLocked();
    }
    return NOERROR;
}

void CastControllerImpl::HandleDiscoveryChangeLocked()
{
    if (mCallbackRegistered) {
        mMediaRouter->RemoveCallback(IMediaRouterCallback::Probe(mMediaCallback));
        mCallbackRegistered = FALSE;
    }

    Int32 size = 0;
    mCallbacks->GetSize(&size);
    if (mDiscovering) {
        mMediaRouter->AddCallback(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY,
                IMediaRouterCallback::Probe(mMediaCallback),
                IMediaRouter::CALLBACK_FLAG_REQUEST_DISCOVERY);
        mCallbackRegistered = TRUE;
    }
    else if (size != 0) {
        mMediaRouter->AddCallback(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY,
                IMediaRouterCallback::Probe(mMediaCallback),
                IMediaRouter::CALLBACK_FLAG_PASSIVE_DISCOVERY);
        mCallbackRegistered = TRUE;
    }
}

ECode CastControllerImpl::SetCurrentUserId(
    /* [in] */ Int32 currentUserId)
{
    mMediaRouter->RebindAsUser(currentUserId);
    return NOERROR;
}

ECode CastControllerImpl::GetCastDevices(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArraySet> devices;  /*<CastDevice*/
    CArraySet::New((IArraySet**)&devices);
    synchronized (mProjectionLock) {
        if (mProjection != NULL) {
            AutoPtr<CastDevice> device = new CastDevice();
            String value;
            mProjection->GetPackageName(&value);
            device->SetId(value);
            device->SetName(GetAppName(value));
            mContext->GetString(R::string::quick_settings_casting, &value);
            device->SetDescription(value);
            device->SetState(ICastControllerCastDevice::STATE_CONNECTED);
            device->SetTag(mProjection);
            ISet::Probe(devices)->Add((ICastControllerCastDevice*)device->Probe(EIID_ICastControllerCastDevice));
            *result = ISet::Probe(devices);
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    synchronized(IObject::Probe(mRoutes)) {
        AutoPtr<ICollection> collection;
        mRoutes->GetValues((ICollection**)&collection);
        AutoPtr<IIterator> ator;
        collection->GetIterator((IIterator**)&ator);
        Boolean has = FALSE;
        while (ator->HasNext(&has), has) {
            AutoPtr<IInterface> obj;
            ator->GetNext((IInterface**)&obj);
            AutoPtr<IMediaRouterRouteInfo> route = IMediaRouterRouteInfo::Probe(obj);

            AutoPtr<CastDevice> device = new CastDevice();
            AutoPtr<IInterface> tag;
            route->GetTag((IInterface**)&tag);
            String value;
            IObject::Probe(tag)->ToString(&value);
            device->SetId(value);
            AutoPtr<ICharSequence> name;
            route->GetName(mContext, (ICharSequence**)&name);
            if (name != NULL) {
                name->ToString(&value);
                device->SetName(value);
            }
            else {
                device->SetName(String(NULL));
            }
            AutoPtr<ICharSequence> description;
            route->GetDescription((ICharSequence**)&description);
            if (description != NULL) {
                description->ToString(&value);
                device->SetDescription(value);
            }
            else {
                device->SetDescription(String(NULL));
            }

            Boolean tmp = FALSE;
            route->IsConnecting(&tmp);
            if (tmp) {
                device->SetState(ICastControllerCastDevice::STATE_CONNECTING);
            }
            else {
                route->IsSelected(&tmp);
                if (tmp) {
                    device->SetState(ICastControllerCastDevice::STATE_CONNECTED);
                }
                else  {
                    device->SetState(ICastControllerCastDevice::STATE_DISCONNECTED);
                }
            }
            device->SetTag(route);
            ISet::Probe(devices)->Add((ICastControllerCastDevice*)device->Probe(EIID_ICastControllerCastDevice));
        }
    }
    *result = ISet::Probe(devices);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CastControllerImpl::StartCasting(
    /* [in] */ ICastControllerCastDevice* device)
{
    AutoPtr<IInterface> tag;
    if (device == NULL || (device->GetTag((IInterface**)&tag), tag) == NULL) return NOERROR;
    AutoPtr<IMediaRouterRouteInfo> route = IMediaRouterRouteInfo::Probe(tag);
    if (DEBUG) Logger::D(TAG, "startCasting: %s", RouteToString(route).string());
    mMediaRouter->SelectRoute(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY, route);
    return NOERROR;
}

ECode CastControllerImpl::StopCasting(
    /* [in] */ ICastControllerCastDevice* device)
{
    AutoPtr<IInterface> tag;
    device->GetTag((IInterface**)&tag);
    Boolean isProjection = IMediaProjectionInfo::Probe(tag) != NULL;
    if (DEBUG) Logger::D(TAG, "stopCasting isProjection=%d", isProjection);
    if (isProjection) {
        AutoPtr<IMediaProjectionInfo> projection = IMediaProjectionInfo::Probe(tag);
        AutoPtr<IMediaProjectionInfo> pv;
        mProjectionManager->GetActiveProjectionInfo((IMediaProjectionInfo**)&pv);
        Boolean tmp = FALSE;
        if (IObject::Probe(pv)->Equals(projection, &tmp), tmp) {
            mProjectionManager->StopActiveProjection();
        }
        else {
            Logger::W(TAG, "Projection is no longer active: %p", projection.Get());
        }
    }
    else {
        AutoPtr<IMediaRouterRouteInfo> info;
        mMediaRouter->GetDefaultRoute((IMediaRouterRouteInfo**)&info);
        info->Select();
    }
    return NOERROR;
}

void CastControllerImpl::SetProjection(
    /* [in] */ IMediaProjectionInfo* projection,
    /* [in] */ Boolean started)
{
    Boolean changed = FALSE;
    AutoPtr<IMediaProjectionInfo> oldProjection = mProjection;
    synchronized (mProjectionLock) {
        Boolean isCurrent = FALSE;
        IObject::Probe(projection)->Equals(mProjection, &isCurrent);
        if (started && !isCurrent) {
            mProjection = projection;
            changed = TRUE;
        }
        else if (!started && isCurrent) {
            mProjection = NULL;
            changed = TRUE;
        }
    }
    if (changed) {
        if (DEBUG) Logger::D(TAG, "setProjection: %p -> %p", oldProjection.Get(), mProjection.Get());
        FireOnCastDevicesChanged();
    }
}

String CastControllerImpl::GetAppName(
    /* [in] */ const String& packageName)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IApplicationInfo> appInfo;
    ECode ec = pm->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&appInfo);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Logger::W(TAG, "Error getting appName for package: %s", packageName.string());
    }
    else {
        if (appInfo != NULL) {
            AutoPtr<ICharSequence> label;
            IPackageItemInfo::Probe(appInfo)->LoadLabel(pm, (ICharSequence**)&label);
            if (!TextUtils::IsEmpty(label)) {
                String value;
                label->ToString(&value);
                return value;
            }
        }
        Logger::W(TAG, "No label found for package: %s", packageName.string());
    }

    return packageName;
}

void CastControllerImpl::UpdateRemoteDisplays()
{
    synchronized(mRoutes) {
        mRoutes->Clear();
        Int32 n = 0;
        mMediaRouter->GetRouteCount(&n);
        Boolean tmp = FALSE;
        for (Int32 i = 0; i < n; i++) {
            AutoPtr<IMediaRouterRouteInfo> route;
            mMediaRouter->GetRouteAt(i, (IMediaRouterRouteInfo**)&route);
            if (route->IsEnabled(&tmp), !tmp) continue;
            if (route->MatchesTypes(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY, &tmp), !tmp) continue;
            EnsureTagExists(route);
            AutoPtr<IInterface> tag;
            route->GetTag((IInterface**)&tag);
            String value;
            IObject::Probe(tag)->ToString(&value);
            AutoPtr<ICharSequence> key;
            CString::New(value, (ICharSequence**)&key);
            mRoutes->Put(key, route);
        }
        AutoPtr<IMediaRouterRouteInfo> selected;
        mMediaRouter->GetSelectedRoute(IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY, (IMediaRouterRouteInfo**)&selected);
        if (selected != NULL && (selected->IsDefault(&tmp), !tmp)) {
            EnsureTagExists(selected);

            AutoPtr<IInterface> tag;
            selected->GetTag((IInterface**)&tag);
            String value;
            IObject::Probe(tag)->ToString(&value);
            AutoPtr<ICharSequence> key;
            CString::New(value, (ICharSequence**)&key);
            mRoutes->Put(key, selected);
        }
    }
    FireOnCastDevicesChanged();
}

void CastControllerImpl::EnsureTagExists(
    /* [in] */ IMediaRouterRouteInfo* route)
{
    AutoPtr<IInterface> tag;
    route->GetTag((IInterface**)&tag);
    if (tag == NULL) {
        AutoPtr<IUUIDHelper> helper;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
        AutoPtr<IUUID> uuid;
        helper->RandomUUID((IUUID**)&uuid);
        String value;
        uuid->ToString(&value);
        AutoPtr<ICharSequence> t;
        CString::New(value, (ICharSequence**)&t);
        route->SetTag(t);
    }
}

void CastControllerImpl::FireOnCastDevicesChanged()
{
    AutoPtr<IIterator> ator;
    mCallbacks->GetIterator((IIterator**)&ator);
    Boolean has = FALSE;
    while (ator->HasNext(&has), has) {
        AutoPtr<IInterface> obj;
        ator->GetNext((IInterface**)&obj);
        AutoPtr<ICastControllerCallback> callback = ICastControllerCallback::Probe(obj);
        FireOnCastDevicesChanged(callback);
    }
}

void CastControllerImpl::FireOnCastDevicesChanged(
    /* [in] */ ICastControllerCallback* callback)
{
    callback->OnCastDevicesChanged();
}

String CastControllerImpl::RouteToString(
    /* [in] */ IMediaRouterRouteInfo* route)
{
    if (route == NULL) return String(NULL);
    StringBuilder sb;
    AutoPtr<ICharSequence> cs;
    route->GetName((ICharSequence**)&cs);
    String value;
    cs->ToString(&value);
    sb.Append(value);
    sb.AppendChar('/');

    cs = NULL;
    route->GetDescription((ICharSequence**)&cs);
    cs->ToString(&value);
    sb.Append(value);
    sb.AppendChar('@');

    route->GetDeviceAddress(&value);
    sb.Append(value);

    sb.Append(",status=");
    cs = NULL;
    route->GetStatus((ICharSequence**)&cs);
    cs->ToString(&value);
    sb.Append(value);
    Boolean tmp = FALSE;
    if (route->IsDefault(&tmp), tmp) {
        sb.Append(",default");
    }
    if (route->IsEnabled(&tmp), tmp) {
        sb.Append(",enabled");
    }
    if (route->IsConnecting(&tmp), tmp) {
        sb.Append(",connecting");
    }
    if (route->IsSelected(&tmp), tmp) {
        sb.Append(",selected");
    }
    sb.Append(",id=");
    AutoPtr<IInterface> tag;
    route->GetTag((IInterface**)&tag);
    sb.Append(tag);
    return sb.ToString();
}

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
