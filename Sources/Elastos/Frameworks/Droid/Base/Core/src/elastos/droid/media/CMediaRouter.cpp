
#include "elastos/droid/media/CMediaRouter.h"

#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/R.h"
#include "elastos/droid/media/CRouteCategory.h"
#include "elastos/droid/media/CAudioRoutesInfo.h"
#include "elastos/droid/media/CMediaRouteInfo.h"
#include "elastos/droid/content/CIntentFilter.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/media/CUserRouteInfo.h"
#include <elastos/core/StringBuilder.h>
#include "elastos/droid/media/CRouteGroup.h"

using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaRouter::TAG("MediaRouter");
AutoPtr<CMediaRouter::Static> CMediaRouter::sStatic;
HashMap< AutoPtr<IContext>, AutoPtr<IMediaRouter> > CMediaRouter::sRouters;
Mutex CMediaRouter::mStaticClass;

CMediaRouter::CallbackInfo::CallbackInfo(
    /* [in] */ ICallback* cb,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouter* router)
    : mCb(cb)
    , mType(type)
    , mRouter(router)
{}

//-----------------------------------------------
//    CMediaRouter::Static::MyAudioRoutesObserver
//-----------------------------------------------

CAR_INTERFACE_IMPL(CMediaRouter::Static::MyAudioRoutesObserver, IIAudioRoutesObserver);

CMediaRouter::Static::MyAudioRoutesObserver::MyAudioRoutesObserver(
    /* [in] */ Static* owner)
    : mOwner(owner)
{}

ECode CMediaRouter::Static::MyAudioRoutesObserver::DispatchAudioRoutesChanged(
    /* [in] */ IAudioRoutesInfo* newRoutes)
{
    AutoPtr<MyRunnable> myRunnable = new MyRunnable(newRoutes, (Static*)this);
    Boolean tempState;
    return mOwner->mHandler->Post(myRunnable.Get(), &tempState);
}

//------------------------------------
//    CMediaRouter::Static::MyRunnable
//------------------------------------

CAR_INTERFACE_IMPL(CMediaRouter::Static::MyRunnable, IRunnable);

CMediaRouter::Static::MyRunnable::MyRunnable(
    /* [in] */ IAudioRoutesInfo* newRoutes,
    /* [in] */ Static* owner)
    : mNewRoutes(newRoutes)
    , mOwner(owner)
{}

ECode CMediaRouter::Static::MyRunnable::Run()
{
    mOwner->UpdateAudioRoutes(mNewRoutes);
    return NOERROR;
}

//------------------------
//    CMediaRouter::Static
//------------------------

CAR_INTERFACE_IMPL(CMediaRouter::Static, IDisplayListener);

CMediaRouter::Static::Static(
    /* [in] */ IContext* appContext,
    /* [in] */ CMediaRouter* owner)
    : mOwner(owner)
{
    CAudioRoutesInfo::New((IAudioRoutesInfo**)&mCurAudioRoutesInfo);
    mAudioRoutesObserver = new MyAudioRoutesObserver(this);

    AutoPtr<IResourcesHelper> resourcesHelper;
    CResourcesHelper::AcquireSingleton((IResourcesHelper**)&resourcesHelper);
    resourcesHelper->GetSystem((IResources**)&mResources);

    AutoPtr<ILooper> looper;
    appContext->GetMainLooper((ILooper**)&looper);
    CHandler::New(looper, (IHandler**)&mHandler);

    AutoPtr<IServiceManager> serviceManager;
    CServiceManager::AcquireSingleton((IServiceManager**)&serviceManager);
    AutoPtr<IInterface> obj;
    serviceManager->GetService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioService = IIAudioService::Probe(obj.Get());

    appContext->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&mDisplayService);

    CRouteCategory::New(R::string::default_audio_route_category_name, ROUTE_TYPE_LIVE_AUDIO | ROUTE_TYPE_LIVE_VIDEO, FALSE, (IRouteCategory**)&mSystemCategory);

    mSystemCategory->SetIsSystem(TRUE);
}

ECode CMediaRouter::Static::OnDisplayAdded(
    /* [in] */ Int32 displayId)
{
    UpdatePresentationDisplays(displayId);
    return NOERROR;
}

ECode CMediaRouter::Static::OnDisplayChanged(
    /* [in] */ Int32 displayId)
{
    UpdatePresentationDisplays(displayId);
    return NOERROR;
}

ECode CMediaRouter::Static::OnDisplayRemoved(
    /* [in] */ Int32 displayId)
{
    UpdatePresentationDisplays(displayId);
    return NOERROR;
}

AutoPtr< ArrayOf<IDisplay*> > CMediaRouter::Static::GetAllPresentationDisplays()
{
    AutoPtr< ArrayOf<IDisplay*> > display;
    mDisplayService->GetDisplays(IDisplayManager::DISPLAY_CATEGORY_PRESENTATION,
        (ArrayOf<IDisplay*>**)&display);
    return display;
}

void CMediaRouter::Static::StartMonitoringRoutes(
    /* [in] */ IContext* appContext)
{
    CMediaRouteInfo::New(mSystemCategory, (IRouteInfo**)&mDefaultAudioVideo);

    mDefaultAudioVideo->SetName(R::string::default_audio_route_name);
    mDefaultAudioVideo->SetSupportedTypes((ROUTE_TYPE_LIVE_AUDIO | ROUTE_TYPE_LIVE_VIDEO));
    AutoPtr<IDisplay> display;
    display = mOwner->ChoosePresentationDisplayForRoute(
                      mDefaultAudioVideo, GetAllPresentationDisplays());
    mDefaultAudioVideo->SetPresentationDisplay(display);
    AddRouteStatic(mDefaultAudioVideo);

    // This will select the active wifi display route if there is one.
    AutoPtr<IWifiDisplayStatus> wifiDisplayStatus;
    mDisplayService->GetWifiDisplayStatus((IWifiDisplayStatus**)&wifiDisplayStatus);
    UpdateWifiDisplayStatus(wifiDisplayStatus);

    AutoPtr<WifiDisplayStatusChangedReceiver> wifiDisplayStatusChangedReceiver
        = new WifiDisplayStatusChangedReceiver();

    AutoPtr<IIntentFilter> intentFilter1;
    CIntentFilter::New(IDisplayManager::ACTION_WIFI_DISPLAY_STATUS_CHANGED,
        (IIntentFilter**)&intentFilter1);
    AutoPtr<IIntent> intent;
    appContext->RegisterReceiver(wifiDisplayStatusChangedReceiver, intentFilter1, (IIntent**)&intent);

    AutoPtr<VolumeChangeReceiver> volumeChangeReceiver = new VolumeChangeReceiver();

    AutoPtr<IIntentFilter> intentFilter2;
    CIntentFilter::New(IAudioManager::VOLUME_CHANGED_ACTION, (IIntentFilter**)&intentFilter2);
    intent = NULL;
    appContext->RegisterReceiver(volumeChangeReceiver, intentFilter2, (IIntent**)&intent);

    mDisplayService->RegisterDisplayListener(this, mHandler);

    AutoPtr<IAudioRoutesInfo> newAudioRoutes = NULL;
//    try {
        mAudioService->StartWatchingRoutes(mAudioRoutesObserver, (IAudioRoutesInfo**)&newAudioRoutes);
//    } catch (RemoteException e) {
//    }
    if (newAudioRoutes != NULL) {
        // This will select the active BT route if there is one and the current
        // selected route is the default system route, or if there is no selected
        // route yet.
        UpdateAudioRoutes(newAudioRoutes);
    }

    // Select the default route if the above didn't sync us up
    // appropriately with relevant system state.
    if (mSelectedRoute == NULL) {
        Int32 tempValue;
        mDefaultAudioVideo->GetSupportedTypes(&tempValue);
        SelectRouteStatic(tempValue, mDefaultAudioVideo);
    }
}

void CMediaRouter::Static::UpdateAudioRoutes(
    /* [in] */ IAudioRoutesInfo* newRoutes)
{
    Int32 tempValue1;
    newRoutes->GetMainType(&tempValue1);
    Int32 tempValue2;
    mCurAudioRoutesInfo->GetMainType(&tempValue2);
    if (tempValue1 != tempValue2) {
        mCurAudioRoutesInfo->SetMainType(tempValue1);
        Int32 name;
        if ((tempValue1 & CAudioRoutesInfo::MAIN_HEADPHONES) != 0
                || (tempValue1 & CAudioRoutesInfo::MAIN_HEADSET) != 0) {
            name = R::string::default_audio_route_name_headphones;
        }
        else if ((tempValue1 & CAudioRoutesInfo::MAIN_DOCK_SPEAKERS) != 0) {
            name = R::string::default_audio_route_name_dock_speakers;
        }
        else if ((tempValue1 & CAudioRoutesInfo::MAIN_HDMI) != 0) {
            name = R::string::default_media_route_name_hdmi;
        }
        else {
            name = R::string::default_audio_route_name;
        }
        sStatic->mDefaultAudioVideo->SetName(name);
        DispatchRouteChanged(sStatic->mDefaultAudioVideo);
    }

    Int32 mainType;
    mCurAudioRoutesInfo->GetMainType(&mainType);

    Boolean a2dpEnabled;
    ECode ec;
//    try {
        ec = mAudioService->IsBluetoothA2dpOn(&a2dpEnabled);
//    } catch (RemoteException e) {
        if (FAILED(ec)) {
            Logger::E(TAG, "Error querying Bluetooth A2DP state"/*, e*/);
            a2dpEnabled = FALSE;
        }
//    }

    AutoPtr<ICharSequence> charSequence1;
    newRoutes->GetBluetoothName((ICharSequence**)&charSequence1);
    AutoPtr<ICharSequence> charSequence2;
    mCurAudioRoutesInfo->GetBluetoothName((ICharSequence**)&charSequence2);
    if (!TextUtils::Equals(charSequence1, charSequence2)) {
        mCurAudioRoutesInfo->SetBluetoothName(charSequence1);
        if (charSequence1 != NULL) {
            if (sStatic->mBluetoothA2dpRoute == NULL) {
                AutoPtr<IRouteInfo> info;
                CMediaRouteInfo::New(sStatic->mSystemCategory, (IRouteInfo**)&info);

                info->SetName(charSequence1);
                info->SetSupportedTypes(ROUTE_TYPE_LIVE_AUDIO);
                sStatic->mBluetoothA2dpRoute = info;
                AddRouteStatic(sStatic->mBluetoothA2dpRoute);
            } else {
                sStatic->mBluetoothA2dpRoute->SetName(charSequence2);
                DispatchRouteChanged(sStatic->mBluetoothA2dpRoute);
            }
        } else if (sStatic->mBluetoothA2dpRoute != NULL) {
            RemoveRoute(sStatic->mBluetoothA2dpRoute);
            sStatic->mBluetoothA2dpRoute = NULL;
        }
    }

    if (mBluetoothA2dpRoute != NULL) {
        if (mainType != CAudioRoutesInfo::MAIN_SPEAKER &&
                mSelectedRoute == mBluetoothA2dpRoute && !a2dpEnabled) {
            SelectRouteStatic(ROUTE_TYPE_LIVE_AUDIO, mDefaultAudioVideo);
        } else if ((mSelectedRoute == mDefaultAudioVideo || mSelectedRoute == NULL) &&
                a2dpEnabled) {
            SelectRouteStatic(ROUTE_TYPE_LIVE_AUDIO, mBluetoothA2dpRoute);
        }
    }
}

void CMediaRouter::Static::UpdatePresentationDisplays(
    /* [in] */ Int32 changedDisplayId)
{
    AutoPtr< ArrayOf<IDisplay*> > displays = GetAllPresentationDisplays();
    List< AutoPtr<IRouteInfo> >::Iterator it;
    for (it = mRoutes.Begin(); it != mRoutes.End(); ++it) {
        AutoPtr<IRouteInfo> info = *it;
        AutoPtr<IDisplay> display = ChoosePresentationDisplayForRoute(info, displays);
        AutoPtr<IDisplay> tempDisplay;
        Int32 tempValue;
        if (display != (info->GetPresentationDisplay((IDisplay**)&tempDisplay), tempDisplay)
                || (display != NULL
                    && (display->GetDisplayId(&tempValue), tempValue) == changedDisplayId)) {
            info->SetPresentationDisplay(display);
            DispatchRoutePresentationDisplayChanged(info);
        }
    }
}

//------------------------

ECode CMediaRouter::SimpleCallback::OnRouteSelected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IRouteInfo* info)
{
    return NOERROR;
}

ECode CMediaRouter::SimpleCallback::OnRouteUnselected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IRouteInfo* info)
{
    return NOERROR;
}

ECode CMediaRouter::SimpleCallback::OnRouteAdded(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IRouteInfo* info)
{
    return NOERROR;
}

ECode CMediaRouter::SimpleCallback::OnRouteRemoved(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IRouteInfo* info)
{
    return NOERROR;
}

ECode CMediaRouter::SimpleCallback::OnRouteChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IRouteInfo* info)
{
    return NOERROR;
}

ECode CMediaRouter::SimpleCallback::OnRouteGrouped(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IRouteInfo* info,
    /* [in] */ IRouteGroup* group,
    /* [in] */ Int32 index)
{
    return NOERROR;
}

ECode CMediaRouter::SimpleCallback::OnRouteUngrouped(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IRouteInfo* info,
    /* [in] */ IRouteGroup* group)
{
    return NOERROR;
}

ECode CMediaRouter::SimpleCallback::OnRouteVolumeChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IRouteInfo* info)
{
    return NOERROR;
}

//------------------------

CMediaRouter::VolumeCallbackInfo::VolumeCallbackInfo(
    /* [in] */ IVolumeCallback* vcb,
    /* [in] */ IRouteInfo* route)
    : mVcb(vcb)
    , mRoute(route)
{}

//------------------------

ECode CMediaRouter::VolumeChangeReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String tempText;
    intent->GetAction(&tempText);
    if (tempText.Equals(IAudioManager::VOLUME_CHANGED_ACTION))
    {
        Int32 streamType;
        intent->GetInt32Extra(IAudioManager::EXTRA_VOLUME_STREAM_TYPE, -1, &streamType);
        if (streamType != IAudioManager::STREAM_MUSIC) {
            return NOERROR;
        }

        Int32 newVolume;
        intent->GetInt32Extra(IAudioManager::EXTRA_VOLUME_STREAM_VALUE, 0, &newVolume);
        Int32 oldVolume;
        intent->GetInt32Extra(IAudioManager::EXTRA_PREV_VOLUME_STREAM_VALUE, 0, &oldVolume);
        if (newVolume != oldVolume) {
            SystemVolumeChanged(newVolume);
        }
    }
    return NOERROR;
}

//------------------------

ECode CMediaRouter::WifiDisplayStatusChangedReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String tempText;
    intent->GetAction(&tempText);
    if (tempText.Equals(IDisplayManager::ACTION_WIFI_DISPLAY_STATUS_CHANGED)) {
        AutoPtr<IParcelable> parcelable;
        intent->GetParcelableExtra(IDisplayManager::EXTRA_WIFI_DISPLAY_STATUS,
            (IParcelable**)&parcelable);
        UpdateWifiDisplayStatus((IWifiDisplayStatus*)&parcelable);
    }
    return NOERROR;
}

//------------------------

CMediaRouter::CMediaRouter()
{}

ECode CMediaRouter::constructor(
    /* [in] */ IContext* context)
{
    {
        AutoLock lock(mStaticClass);
        if (sStatic == NULL) {
            AutoPtr<IContext> appContext;
            context->GetApplicationContext((IContext**)&appContext);
            sStatic = new Static(appContext, this);
            sStatic->StartMonitoringRoutes(appContext);
        }
    }
    return NOERROR;
}

ECode CMediaRouter::GetSystemAudioRoute(
    /* [out] */ IRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);

    *result = sStatic->mDefaultAudioVideo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::GetSystemAudioCategory(
    /* [out] */ IRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);

    *result = sStatic->mSystemCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::GetSelectedRoute(
    /* [in] */ Int32 type,
    /* [out] */ IRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 temp;
    if (sStatic->mSelectedRoute != NULL &&
            (sStatic->mSelectedRoute->GetSupportedTypes(&temp) & type) != 0) {
        // If the selected route supports any of the types supplied, it's still considered
        // 'selected' for that type.
        *result = sStatic->mSelectedRoute;
    } else if (type == ROUTE_TYPE_USER) {
        // The caller specifically asked for a user route and the currently selected route
        // doesn't qualify.
        *result = NULL;
    }
    // If the above didn't match and we're not specifically asking for a user route,
    // consider the default selected.
    *result = sStatic->mDefaultAudioVideo;

    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::AddCallback(
    /* [in] */ Int32 types,
    /* [in] */ ICallback* cb)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> info = *it;
        if (cb == info->mCb) {
            info->mType |= types;
            return NOERROR;
        }
    }
    AutoPtr<CallbackInfo> tempCallbackInfo = new CallbackInfo(cb, types, this);
    sStatic->mCallbacks.PushBack(tempCallbackInfo);
    return NOERROR;
}

ECode CMediaRouter::RemoveCallback(
    /* [in] */ ICallback* cb)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        if (cb == (*it)->mCb) {
            sStatic->mCallbacks.Erase(it);
            return NOERROR;
        }
    }

    Logger::W(TAG, "removeCallback(" /*+ cb +*/ "): callback not registered");
    return NOERROR;
}

ECode CMediaRouter::SelectRoute(
    /* [in] */ Int32 types,
    /* [in] */ IRouteInfo* route)
{
    // Applications shouldn't programmatically change anything but user routes.
    types &= ROUTE_TYPE_USER;
    SelectRouteStatic(types, route);
    return NOERROR;
}

ECode CMediaRouter::SelectRouteInt(
    /* [in] */ Int32 types,
    /* [in] */ IRouteInfo* route)
{
    SelectRouteStatic(types, route);
    return NOERROR;
}

ECode CMediaRouter::AddUserRoute(
    /* [in] */ IUserRouteInfo* info)
{
    AddRouteStatic((IRouteInfo*)info);
    return NOERROR;
}

ECode CMediaRouter::AddRouteInt(
    /* [in] */ IRouteInfo* info)
{
    AddRouteStatic(info);
    return NOERROR;
}

ECode CMediaRouter::RemoveUserRoute(
    /* [in] */ IUserRouteInfo* info)
{
    RemoveRoute((IRouteInfo*)info);
    return NOERROR;
}

ECode CMediaRouter::ClearUserRoutes()
{
    assert(0 && "TODO: can not remove route in a loop.!!luo.zhaohui, please fix it.");
    for (UInt32 i = 0; i < sStatic->mRoutes.GetSize(); i++) {
        AutoPtr<IRouteInfo> info = sStatic->mRoutes[i];
        // TODO Right now, RouteGroups only ever contain user routes.
        // The code below will need to change if this assumption does.
        if ((IUserRouteInfo::Probe(info) != NULL) || (IRouteGroup::Probe(info) != NULL)) {
            RemoveRouteAt(i);// sStatic->mRoutes is changed in this function!!!
            i--;
        }
    }
    return NOERROR;
}

ECode CMediaRouter::RemoveRouteInt32(
    /* [in] */ IRouteInfo* info)
{
    RemoveRoute(info);
    return NOERROR;
}

ECode CMediaRouter::GetCategoryCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = sStatic->mCategories.GetSize();
    return NOERROR;
}

ECode CMediaRouter::GetCategoryAt(
    /* [in] */ Int32 index,
    /* [out] */ IRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);
    *result = sStatic->mCategories[index];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::GetRouteCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = sStatic->mRoutes.GetSize();
    return NOERROR;
}

ECode CMediaRouter::GetRouteAt(
    /* [in] */ Int32 index,
    /* [out] */ IRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);

    *result = sStatic->mRoutes[index];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::CreateUserRoute(
    /* [in] */ IRouteCategory* category,
    /* [out] */ IUserRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IUserRouteInfo> userRouteInfo;
    CUserRouteInfo::New(category, (IUserRouteInfo**)&userRouteInfo);
    *result = userRouteInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::CreateRouteCategory(
    /* [in] */ ICharSequence* name,
    /* [in] */ Boolean isGroupable,
    /* [out] */ IRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IRouteCategory> routeCategory;
    CRouteCategory::New(name, ROUTE_TYPE_USER, isGroupable, (IRouteCategory**)&routeCategory);
    *result = routeCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::CreateRouteCategory(
    /* [in] */ Int32 nameResId,
    /* [in] */ Boolean isGroupable,
    /* [out] */ IRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IRouteCategory> routeCategory;
    CRouteCategory::New(nameResId, ROUTE_TYPE_USER, isGroupable, (IRouteCategory**)&routeCategory);
    *result = routeCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

String CMediaRouter::TypesToString(
    /* [in] */ Int32 types)
{
    StringBuilder result;

    if ((types & ROUTE_TYPE_LIVE_AUDIO) != 0) {
        result += "ROUTE_TYPE_LIVE_AUDIO ";
    }
    if ((types & ROUTE_TYPE_LIVE_VIDEO) != 0) {
        result += "ROUTE_TYPE_LIVE_VIDEO ";
    }
    if ((types & ROUTE_TYPE_USER) != 0) {
        result += "ROUTE_TYPE_USER ";
    }
    String tempText;
    result.ToString(&tempText);
    return tempText;
}

void CMediaRouter::SelectRouteStatic(
    /* [in] */ Int32 types,
    /* [in] */ IRouteInfo* route)
{
    AutoPtr<IRouteInfo> oldRoute = sStatic->mSelectedRoute;
    if (route == oldRoute) {
        return;
    }
    Int32 tempValue;
    route->GetSupportedTypes(&tempValue);
    if ((tempValue & types) == 0) {
        Logger::W(TAG, String("selectRoute ignored; cannot select route with supported types ") +
                TypesToString(tempValue) + String(" into route types ") +
                TypesToString(types));
        return;
    }

    AutoPtr<IRouteInfo> btRoute = sStatic->mBluetoothA2dpRoute;
    if (btRoute != NULL && (types & ROUTE_TYPE_LIVE_AUDIO) != 0 &&
            (route == btRoute || route == sStatic->mDefaultAudioVideo)) {
        ECode ec;
//        try {
            ec = sStatic->mAudioService->SetBluetoothA2dpOn(route == btRoute);
//        } catch (RemoteException e) {
            if (FAILED(ec)) {
                Logger::E(TAG, "Error changing Bluetooth A2DP state"/*, e*/);
            }
//        }
    }

    AutoPtr<IWifiDisplay> activeDisplay;
    AutoPtr<IWifiDisplayStatus> wifiDisplayStatus;
    sStatic->mDisplayService->GetWifiDisplayStatus((IWifiDisplayStatus**)&wifiDisplayStatus);
    wifiDisplayStatus->GetActiveDisplay((IWifiDisplay**)&activeDisplay);

    String tempText1, tempText2;
    Boolean oldRouteHasAddress = oldRoute != NULL && (oldRoute->GetDeviceAddress(&tempText1), tempText1) != NULL;
    Boolean newRouteHasAddress = route != NULL && (route->GetDeviceAddress(&tempText2), tempText2) != NULL;

    if (activeDisplay != NULL || oldRouteHasAddress || newRouteHasAddress) {
        if (newRouteHasAddress && !MatchesDeviceAddress(activeDisplay, route)) {
            String tempText;
            route->GetDeviceAddress(&tempText);
            sStatic->mDisplayService->ConnectWifiDisplay(tempText);
        }
        else if (activeDisplay != NULL && !newRouteHasAddress) {
            sStatic->mDisplayService->DisconnectWifiDisplay();
        }
    }

    if (oldRoute != NULL) {
        // TODO filter types properly
        Int32 tempValue;
        oldRoute->GetSupportedTypes(&tempValue);
        DispatchRouteUnselected(types & tempValue, oldRoute);
    }
    sStatic->mSelectedRoute = route;
    if (route != NULL) {
        // TODO filter types properly
        Int32 tempValue;
        route->GetSupportedTypes(&tempValue);
        DispatchRouteSelected(types & tempValue, route);
    }
}

Boolean CMediaRouter::MatchesDeviceAddress(
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IRouteInfo* info)
{
    String tempText1;
    Boolean routeHasAddress = info != NULL && (info->GetDeviceAddress(&tempText1), tempText1) != NULL;
    if (display == NULL && !routeHasAddress) {
        return TRUE;
    }

    if (display != NULL && routeHasAddress) {
        String tempText2;
        display->GetDeviceAddress(&tempText1);
        return tempText1.Equals((info->GetDeviceAddress(&tempText2), tempText2));
    }
    return FALSE;
}

void CMediaRouter::AddRouteStatic(
    /* [in] */ IRouteInfo* info)
{
    if (info == NULL) return;

    AutoPtr<IRouteCategory> cat;
    info->GetCategory((IRouteCategory**)&cat);
    List<AutoPtr<IRouteCategory> >::Iterator it = Find(sStatic->mCategories.Begin(), sStatic->mCategories.End(), cat);
    if (it == sStatic->mCategories.End()) {
        sStatic->mCategories.PushBack(cat);
    }

    Boolean tempState;
    if ((cat->IsGroupable(&tempState), tempState) && !(IRouteGroup::Probe(info) != NULL)) {
        // Enforce that any added route in a groupable category must be in a group.
        AutoPtr<IRouteGroup> group;
        AutoPtr<IRouteCategory> routeCategory;
        info->GetCategory((IRouteCategory**)&routeCategory);
        CRouteGroup::New(routeCategory, (IRouteGroup**)&group);
        Int32 tempValue;
        info->GetSupportedTypes(&tempValue);
        ((IRouteInfo*)&group)->SetSupportedTypes(tempValue);
        sStatic->mRoutes.PushBack((IRouteInfo*)&group);
        DispatchRouteAdded((IRouteInfo*)&group);
        group->AddRoute(info);

        info = (IRouteInfo*)&group;
    } else {
        sStatic->mRoutes.PushBack(info);
        DispatchRouteAdded(info);
    }
}

void CMediaRouter::RemoveRoute(
    /* [in] */ IRouteInfo* info)
{
    List< AutoPtr<IRouteInfo> >::Iterator it = Find(sStatic->mRoutes.Begin(), sStatic->mRoutes.End(), AutoPtr<IRouteInfo>(info));
    if (it != sStatic->mRoutes.End()) {
        sStatic->mRoutes.Erase(it);

        AutoPtr<IRouteCategory> removingCat;
        info->GetCategory((IRouteCategory**)&removingCat);

        Boolean found = FALSE;
        it = sStatic->mRoutes.Begin();
        for (; it != sStatic->mRoutes.End(); ++it) {
            AutoPtr<IRouteCategory> cat;
            (*it)->GetCategory((IRouteCategory**)&cat);
            if (removingCat == cat) {
                found = TRUE;
                break;
            }
        }

        if (info == sStatic->mSelectedRoute) {
            // Removing the currently selected route? Select the default before we remove it.
            // TODO: Be smarter about the route types here; this selects for all valid.
            SelectRouteStatic(ROUTE_TYPE_LIVE_AUDIO | ROUTE_TYPE_USER, sStatic->mDefaultAudioVideo);
        }

        if (!found) {
            List< AutoPtr<IRouteCategory> >::Iterator iter = Find(sStatic->mCategories.Begin(), sStatic->mCategories.End(), removingCat);
            if (iter != sStatic->mCategories.End())
                sStatic->mCategories.Erase(iter);
        }
        DispatchRouteRemoved(info);
    }
}

void CMediaRouter::RemoveRouteAt(
    /* [in] */ Int32 routeIndex)
{
    if (routeIndex >= 0 && routeIndex < sStatic->mRoutes.GetSize()) {
        List< AutoPtr<IRouteInfo> >::Iterator it = sStatic->mRoutes.Begin();
        for (Int32 i = 0; i <= routeIndex; ++i, ++it) {
        }

        AutoPtr<IRouteInfo> info = *it;
        sStatic->mRoutes.Erase(it);

        AutoPtr<IRouteCategory> removingCat;
        info->GetCategory((IRouteCategory**)&removingCat);
        Boolean found = FALSE;
        it = sStatic->mRoutes.Begin();
        for (; it != sStatic->mRoutes.End(); ++it) {
            AutoPtr<IRouteCategory> cat;
            (*it)->GetCategory((IRouteCategory**)&cat);
            if (removingCat == cat) {
                found = TRUE;
                break;
            }
        }

        if (info == sStatic->mSelectedRoute) {
            // Removing the currently selected route? Select the default before we remove it.
            // TODO: Be smarter about the route types here; this selects for all valid.
            SelectRouteStatic(ROUTE_TYPE_LIVE_AUDIO | ROUTE_TYPE_LIVE_VIDEO | ROUTE_TYPE_USER,
                    sStatic->mDefaultAudioVideo);
        }
        if (!found) {
            List< AutoPtr<IRouteCategory> >::Iterator it = Find(sStatic->mCategories.Begin(), sStatic->mCategories.End(), removingCat);
            sStatic->mCategories.Erase(it);
        }
        DispatchRouteRemoved(info);
    }
}

Int32 CMediaRouter::GetRouteCountStatic()
{
    return sStatic->mRoutes.GetSize();
}

AutoPtr<IRouteInfo> CMediaRouter::GetRouteAtStatic(
    /* [in] */ Int32 index)
{
    return sStatic->mRoutes[index];
}

void CMediaRouter::UpdateRoute(
    /* [in] */ IRouteInfo* info)
{
    DispatchRouteChanged(info);
}

void CMediaRouter::DispatchRouteSelected(
    /* [in] */ Int32 type,
    /* [in] */ IRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        if ((cbi->mType & type) != 0) {
            cbi->mCb->OnRouteSelected(cbi->mRouter, type, info);
        }
    }
}

void CMediaRouter::DispatchRouteUnselected(
    /* [in] */ Int32 type,
    /* [in] */ IRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        if ((cbi->mType & type) != 0) {
            cbi->mCb->OnRouteUnselected(cbi->mRouter, type, info);
        }
    }
}

void CMediaRouter::DispatchRouteChanged(
    /* [in] */ IRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        Int32 tempValue;
        if ((cbi->mType & (info->GetSupportedTypes(&tempValue), tempValue)) != 0) {
            cbi->mCb->OnRouteChanged(cbi->mRouter, info);
        }
    }
}

void CMediaRouter::DispatchRouteAdded(
    /* [in] */ IRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        Int32 tempValue;
        if ((cbi->mType & (info->GetSupportedTypes(&tempValue), tempValue)) != 0) {
            cbi->mCb->OnRouteAdded(cbi->mRouter, info);
        }
    }
}

void CMediaRouter::DispatchRouteRemoved(
    /* [in] */ IRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        Int32 tempValue;
        if ((cbi->mType & (info->GetSupportedTypes(&tempValue), tempValue)) != 0) {
            cbi->mCb->OnRouteRemoved(cbi->mRouter, info);
        }
    }
}

void CMediaRouter::DispatchRouteGrouped(
    /* [in] */ IRouteInfo* info,
    /* [in] */ IRouteGroup* group,
    /* [in] */ Int32 index)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        Int32 tempValue;
        if ((cbi->mType & (((IRouteInfo*)&group)->GetSupportedTypes(&tempValue), tempValue)) != 0) {
            cbi->mCb->OnRouteGrouped(cbi->mRouter, info, group, index);
        }
    }
}

void CMediaRouter::DispatchRouteUngrouped(
    /* [in] */ IRouteInfo* info,
    /* [in] */ IRouteGroup* group)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        Int32 tempValue;
        if ((cbi->mType & (((IRouteInfo*)&group)->GetSupportedTypes(&tempValue), tempValue)) != 0) {
            cbi->mCb->OnRouteUngrouped(cbi->mRouter, info, group);
        }
    }
}

void CMediaRouter::DispatchRouteVolumeChanged(
    /* [in] */ IRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        Int32 tempValue;
        if ((cbi->mType & (info->GetSupportedTypes(&tempValue), tempValue)) != 0) {
            cbi->mCb->OnRouteVolumeChanged(cbi->mRouter, info);
        }
    }
}

void CMediaRouter::DispatchRoutePresentationDisplayChanged(
    /* [in] */ IRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        Int32 tempValue;
        if ((cbi->mType & (info->GetSupportedTypes(&tempValue), tempValue)) != 0) {
            cbi->mCb->OnRoutePresentationDisplayChanged(cbi->mRouter, info);
        }
    }
}

void CMediaRouter::SystemVolumeChanged(
    /* [in] */ Int32 newValue)
{
    AutoPtr<IRouteInfo> selectedRoute = sStatic->mSelectedRoute;
    if (selectedRoute == NULL) {
        return;
    }

    if (selectedRoute == sStatic->mBluetoothA2dpRoute ||
            selectedRoute == sStatic->mDefaultAudioVideo) {
        DispatchRouteVolumeChanged(selectedRoute);
    }
    else if (sStatic->mBluetoothA2dpRoute != NULL) {
//        try {
            Boolean tempState;
            DispatchRouteVolumeChanged((sStatic->mAudioService->IsBluetoothA2dpOn(&tempState), tempState) ?
                    sStatic->mBluetoothA2dpRoute : sStatic->mDefaultAudioVideo);
//        } catch (RemoteException e) {
            Logger::E(TAG, "Error checking Bluetooth A2DP state to report volume change"/*, e*/);
//        }
    } else {
        DispatchRouteVolumeChanged(sStatic->mDefaultAudioVideo);
    }
}

void CMediaRouter::UpdateWifiDisplayStatus(
    /* [in] */ IWifiDisplayStatus* newStatus)
{
    AutoPtr<IWifiDisplayStatus> oldStatus = sStatic->mLastKnownWifiDisplayStatus;

    // TODO Naive implementation. Make this smarter later.
    Boolean wantScan = FALSE;
    Boolean blockScan = FALSE;
    AutoPtr< ArrayOf<IWifiDisplay*> > oldDisplays;
    if (oldStatus != NULL) {
        oldStatus->GetRememberedDisplays((ArrayOf<IWifiDisplay*>**)&oldDisplays);
    } else {
        oldDisplays = ArrayOf<IWifiDisplay*>::Alloc(0);
    }

    AutoPtr< ArrayOf<IWifiDisplay*> > newDisplays;
    newStatus->GetRememberedDisplays((ArrayOf<IWifiDisplay*>**)&newDisplays);

    AutoPtr< ArrayOf<IWifiDisplay*> > availableDisplays;
    newStatus->GetAvailableDisplays((ArrayOf<IWifiDisplay*>**)&availableDisplays);

    AutoPtr<IWifiDisplay> activeDisplay;
    newStatus->GetActiveDisplay((IWifiDisplay**)&activeDisplay);

    for (Int32 i = 0; i < newDisplays->GetLength(); i++) {
        AutoPtr<IWifiDisplay> d = (*newDisplays)[i];
        AutoPtr<IWifiDisplay> oldRemembered = FindMatchingDisplay(d, (ArrayOf<IWifiDisplay*>*)&oldDisplays);
        if (oldRemembered == NULL) {
            AddRouteStatic(MakeWifiDisplayRoute(d,
                    FindMatchingDisplay(d, (ArrayOf<IWifiDisplay*>*)&availableDisplays) != NULL));
            wantScan = TRUE;
        } else {
            Boolean available = FindMatchingDisplay(d, (ArrayOf<IWifiDisplay*>*)&availableDisplays) != NULL;
            AutoPtr<IRouteInfo> route = FindWifiDisplayRoute(d);
            UpdateWifiDisplayRoute(route, d, available, newStatus);
        }
        Boolean tempState;
        d->Equals(activeDisplay, &tempState);
        if (tempState) {
            AutoPtr<IRouteInfo> activeRoute = FindWifiDisplayRoute(d);
            if (activeRoute != NULL) {
                Int32 tempValue;
                activeRoute->GetSupportedTypes(&tempValue);
                SelectRouteStatic(tempValue, activeRoute);

                // Don't scan if we're already connected to a wifi display,
                // the scanning process can cause a hiccup with some configurations.
                blockScan = TRUE;
            }
        }
    }
    for (Int32 i = 0; i < oldDisplays->GetLength(); i++) {
        AutoPtr<IWifiDisplay> d = (*oldDisplays)[i];
        AutoPtr<IWifiDisplay> newDisplay = FindMatchingDisplay(d, (ArrayOf<IWifiDisplay*>*)&newDisplays);
        if (newDisplay == NULL) {
            RemoveRoute(FindWifiDisplayRoute(d));
        }
    }

    if (wantScan && !blockScan) {
        sStatic->mDisplayService->ScanWifiDisplays();
    }

    sStatic->mLastKnownWifiDisplayStatus = newStatus;
}

AutoPtr<IRouteInfo> CMediaRouter::MakeWifiDisplayRoute(
    /* [in] */ IWifiDisplay* display,
    /* [in] */ Boolean available)
{
    AutoPtr<IRouteInfo> newRoute;
    CMediaRouteInfo::New(sStatic->mSystemCategory, (IRouteInfo**)&newRoute);

    String tempText;
    display->GetDeviceAddress(&tempText);
    newRoute->SetDeviceAddress(tempText);

    newRoute->SetSupportedTypes(ROUTE_TYPE_LIVE_AUDIO | ROUTE_TYPE_LIVE_VIDEO);
    newRoute->SetVolumeHandling(IRouteInfo::PLAYBACK_VOLUME_FIXED);
    newRoute->SetPlaybackType(IRouteInfo::PLAYBACK_TYPE_REMOTE);

    Boolean tempState;
    newRoute->SetStatusCode(available ?
            IRouteInfo::STATUS_AVAILABLE : IRouteInfo::STATUS_CONNECTING, &tempState);
    newRoute->SetEnabled(available);

    display->GetFriendlyDisplayName(&tempText);
    AutoPtr<ICharSequence> charSequence;
    CString::New(tempText, (ICharSequence**)&charSequence);
    newRoute->SetName(charSequence);

    newRoute->SetPresentationDisplay(
        ChoosePresentationDisplayForRoute(newRoute, (ArrayOf<IDisplay*>*)sStatic->GetAllPresentationDisplays())
    );
    return newRoute;
}

void CMediaRouter::UpdateWifiDisplayRoute(
    /* [in] */ IRouteInfo* route,
    /* [in] */ IWifiDisplay* display,
    /* [in] */ Boolean available,
    /* [in] */ IWifiDisplayStatus* wifiDisplayStatus)
{
    Int32 tempValue;
    wifiDisplayStatus->GetScanState(&tempValue);
    Boolean isScanning = tempValue == IWifiDisplayStatus::SCAN_STATE_SCANNING;

    Boolean changed = FALSE;
    Int32 newStatus = IRouteInfo::STATUS_NONE;

    if (available) {
        newStatus = isScanning ? IRouteInfo::STATUS_SCANNING : IRouteInfo::STATUS_AVAILABLE;
    } else {
        newStatus = IRouteInfo::STATUS_NOT_AVAILABLE;
    }

    Boolean tempState;
    AutoPtr<IWifiDisplay> wifiDisplay;
    wifiDisplayStatus->GetActiveDisplay((IWifiDisplay**)&wifiDisplay);
    display->Equals(wifiDisplay, &tempState);
    if (tempState) {
        Int32 activeState;
        wifiDisplayStatus->GetActiveDisplayState(&activeState);
        switch (activeState) {
            case IWifiDisplayStatus::DISPLAY_STATE_CONNECTED:
            {
                newStatus = IRouteInfo::STATUS_NONE;
                break;
            }
            case IWifiDisplayStatus::DISPLAY_STATE_CONNECTING:
            {
                newStatus = IRouteInfo::STATUS_CONNECTING;
                break;
            }
            case IWifiDisplayStatus::DISPLAY_STATE_NOT_CONNECTED:
            {
                Logger::E(TAG, "Active display is not connected!");
                break;
            }
        }
    }

    String newName;
    display->GetFriendlyDisplayName(&newName);
    AutoPtr<ICharSequence> charSequence;
    route->GetName((ICharSequence**)&charSequence);
    String tempText;
    charSequence->ToString(&tempText);
    if (!tempText.Equals(newName)) {
        route->SetName(charSequence);
        changed = TRUE;
    }

    route->GetEnabled(&tempState);
    changed |=  tempState != available;
    route->SetEnabled(available);

    route->SetStatusCode(newStatus, &tempState);
    changed |= tempState;

    if (changed) {
        DispatchRouteChanged(route);
    }

    if (!available && route == sStatic->mSelectedRoute) {
        // Oops, no longer available. Reselect the default.
        AutoPtr<IRouteInfo> defaultRoute = sStatic->mDefaultAudioVideo;
        defaultRoute->GetSupportedTypes(&tempValue);
        SelectRouteStatic(tempValue, defaultRoute);
    }
}

AutoPtr<IWifiDisplay> CMediaRouter::FindMatchingDisplay(
    /* [in] */ IWifiDisplay* d,
    /* [in] */ ArrayOf<IWifiDisplay*>* displays)
{
    String tempText1, tempText2;
    for (Int32 i = 0; i < displays->GetLength(); i++) {
        AutoPtr<IWifiDisplay> other = (*displays)[i];
        d->GetDeviceAddress(&tempText1);
        other->GetDeviceAddress(&tempText2);
        //4.2.2 modify to  if (d.hasSameAddress(other))
        if (tempText1.Equals(tempText2)) {
            return other;
        }
    }
    return NULL;
}

AutoPtr<IRouteInfo> CMediaRouter::FindWifiDisplayRoute(
    /* [in] */ IWifiDisplay* d)
{
    Int32 count = sStatic->mRoutes.GetSize();
    String tempText1, tempText2;

    List< AutoPtr<IRouteInfo> >::Iterator it = sStatic->mRoutes.Begin();
    for (; it != sStatic->mRoutes.End(); ++it) {
        AutoPtr<IRouteInfo> info = *it;
        d->GetDeviceAddress(&tempText1);
        info->GetDeviceAddress(&tempText2);
        if (tempText1.Equals(tempText2)) {
            return info;
        }
    }
    return NULL;
}

AutoPtr<IDisplay> CMediaRouter::ChoosePresentationDisplayForRoute(
    /* [in] */ IRouteInfo* route,
    /* [in] */ ArrayOf<IDisplay*>* displays)
{
    Int32 tempValue;
    String tempText1, tempText2;
    if (((route->GetSupportedTypes(&tempValue), tempValue) & ROUTE_TYPE_LIVE_VIDEO) != 0) {
        if ((route->GetDeviceAddress(&tempText1), tempText1) != NULL) {
            // Find the indicated Wifi display by its address.
            for (Int32 i = 0; i < displays->GetLength(); ++i) {
                AutoPtr<IDisplay> display = (*displays)[i];
                if ((display->GetType(&tempValue), tempValue) == IDisplay::TYPE_WIFI
                        && tempText1.Equals((display->GetAddress(&tempText2), tempText2))) {
                    return display;
                }
            }
            return NULL;
        }

        if (route == sStatic->mDefaultAudioVideo && displays->GetLength() > 0) {
            // Choose the first presentation display from the list.
            return (*displays)[0];
        }
    }
    return NULL;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
