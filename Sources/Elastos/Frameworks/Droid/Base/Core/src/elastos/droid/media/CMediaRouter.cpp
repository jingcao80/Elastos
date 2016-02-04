
#include "elastos/droid/media/CMediaRouter.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/R.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/media/CAudioRoutesInfo.h"
#include "elastos/droid/media/CMediaRouteInfo.h"
#include "elastos/droid/media/CRouteCategory.h"
#include "elastos/droid/media/CRouteGroup.h"
#include "elastos/droid/media/CUserRouteInfo.h"
#include "elastos/droid/media/VolumeProvider.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::CString;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaRouter::TAG("MediaRouter");
AutoPtr<CMediaRouter::Static> CMediaRouter::sStatic;
HashMap< AutoPtr<IContext>, AutoPtr<IMediaRouter> > CMediaRouter::sRouters;
Mutex CMediaRouter::mStaticClass;

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

CMediaRouter::Static::Static()
{}

CMediaRouter::Static::~Static()
{}

ECode CMediaRouter::Static::constructor(
    /* [in] */ IContext* appContext,
    /* [in] */ CMediaRouter* owner)
{
    mOwner = owner;
    mAppContext = appContext;
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
    AutoPtr<IBinder> b = IBinder::Probe(obj);
    mAudioService = IIAudioService::Probe(obj.Get());

    appContext->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&mDisplayService);

    obj = NULL;
    serviceManager->GetService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&obj);
    mMediaRouterService = IMediaRouterService::Probe(obj);

    CRouteCategory::New(R::string::default_audio_route_category_name,
        ROUTE_TYPE_LIVE_AUDIO | ROUTE_TYPE_LIVE_VIDEO, FALSE, (IMediaRouterRouteCategory**)&mSystemCategory);

    mSystemCategory->SetIsSystem(TRUE);

    // Only the system can configure wifi displays.  The display manager
    // enforces this with a permission check.  Set a flag here so that we
    // know whether this process is actually allowed to scan and connect.
    Int32 vol;
    appContext->CheckPermission(Manifest::permission::CONFIGURE_WIFI_DISPLAY,
        Process::MyPid(), Process::MyUid(), &vol);

    mCanConfigureWifiDisplays = (vol == IPackageManager::PERMISSION_GRANTED);
    return NOERROR;
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
    mDefaultAudioVideo->UpdatePresentationDisplay(display);
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

    // Bind to the media router service.
    RebindAsUser(UserHandle::GetMyUserId());

    // Select the default route if the above didn't sync us up
    // appropriately with relevant system state.
    if (mSelectedRoute == NULL) {
        SelectDefaultRouteStatic();
    }
    return NOERROR;
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

    AutoPtr<ICharSequence> charSequence1;
    newRoutes->GetBluetoothName((ICharSequence**)&charSequence1);
    AutoPtr<ICharSequence> charSequence2;
    mCurAudioRoutesInfo->GetBluetoothName((ICharSequence**)&charSequence2);
    if (!TextUtils::Equals(charSequence1, charSequence2)) {
        mCurAudioRoutesInfo->SetBluetoothName(charSequence1);
        if (charSequence1 != NULL) {
            if (sStatic->mBluetoothA2dpRoute == NULL) {
                AutoPtr<IMediaRouterUserRouteInfo> info;
                CUserRouteInfo::New(sStatic->mSystemCategory, (IMediaRouterUserRouteInfo**)&info);
                AutoPtr<ICharSequence> cs;
                sStatic->mResources->GetText(R::string::bluetooth_a2dp_audio_route_name, (ICharSequence**)&cs);
                info->SetDescription(cs);
                info->SetName(charSequence1);
                info->SetSupportedTypes(ROUTE_TYPE_LIVE_AUDIO);
                sStatic->mBluetoothA2dpRoute = info;
                AddRouteStatic(sStatic->mBluetoothA2dpRoute);
            } else {
                sStatic->mBluetoothA2dpRoute->SetName(charSequence2);
                DispatchRouteChanged(sStatic->mBluetoothA2dpRoute);
            }
        } else if (sStatic->mBluetoothA2dpRoute != NULL) {
            RemoveRouteStatic(sStatic->mBluetoothA2dpRoute);
            sStatic->mBluetoothA2dpRoute = NULL;
        }
    }

    if (mBluetoothA2dpRoute != NULL) {
        Boolean a2dpEnabled;
        IsBluetoothA2dpOn(&a2dpEnabled);
        if (mainType != CAudioRoutesInfo::MAIN_SPEAKER &&
                mSelectedRoute == mBluetoothA2dpRoute && !a2dpEnabled) {
            SelectRouteStatic(ROUTE_TYPE_LIVE_AUDIO, mDefaultAudioVideo, FALSE);
        } else if ((mSelectedRoute == mDefaultAudioVideo || mSelectedRoute == NULL) &&
                a2dpEnabled) {
            SelectRouteStatic(ROUTE_TYPE_LIVE_AUDIO, mBluetoothA2dpRoute, FALSE);
        }
    }
}

Boolean CMediaRouter::Static::IsBluetoothA2dpOn()
{
    // try {
    ECode ec = NOERROR;
    Boolean flag = FALSE;
    ec = mAudioService->IsBluetoothA2dpOn(&flag);
    // } catch (RemoteException e) {
    if (SUCCEEDED(ec)) {
        return flag;
    }

    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "Error querying Bluetooth A2DP state");
        return FALSE;
    }
    // }
}

void CMediaRouter::Static::UpdateDiscoveryRequest()
{
    // What are we looking for today?
    Int32 routeTypes = 0;
    Int32 passiveRouteTypes = 0;
    Boolean activeScan = FALSE;
    Boolean activeScanWifiDisplay = FALSE;

    List< AutoPtr<CallbackInfo> >::Iterator it;
    for (it = mCallbacks.Begin(); it != mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = *it;
        if ((cbi->mFlags & (CALLBACK_FLAG_PERFORM_ACTIVE_SCAN
            | CALLBACK_FLAG_REQUEST_DISCOVERY)) != 0) {
            // Discovery explicitly requested.
            routeTypes |= cbi->mType;
        } else if ((cbi->mFlags & CALLBACK_FLAG_PASSIVE_DISCOVERY) != 0) {
            // Discovery only passively requested.
            passiveRouteTypes |= cbi->mType;
        } else {
            // Legacy case since applications don't specify the discovery flag.
            // Unfortunately we just have to assume they always need discovery
            // whenever they have a callback registered.
            routeTypes |= cbi->mType;
        }
        if ((cbi->mFlags & CALLBACK_FLAG_PERFORM_ACTIVE_SCAN) != 0) {
            activeScan = TRUE;
        if ((cbi->mType & ROUTE_TYPE_REMOTE_DISPLAY) != 0) {
            activeScanWifiDisplay = TRUE;
        }
        }
    }
    if (routeTypes != 0 || activeScan) {
        // If someone else requests discovery then enable the passive listeners.
        // This is used by the MediaRouteButton and MediaRouteActionProvider since
        // they don't receive lifecycle callbacks from the Activity.
        routeTypes |= passiveRouteTypes;
    }

    // Update wifi display scanning.
    // TODO: All of this should be managed by the media router service.
    if (mCanConfigureWifiDisplays) {
        if (mSelectedRoute != NULL
                && mSelectedRoute.matchesTypes(ROUTE_TYPE_REMOTE_DISPLAY)) {
            // Don't scan while already connected to a remote display since
            // it may interfere with the ongoing transmission.
            activeScanWifiDisplay = FALSE;
        }
        if (activeScanWifiDisplay) {
            if (!mActivelyScanningWifiDisplays) {
                mActivelyScanningWifiDisplays = TRUE;
                mDisplayService->StartWifiDisplayScan();
            }
        } else {
            if (mActivelyScanningWifiDisplays) {
                mActivelyScanningWifiDisplays = FALSE;
                mDisplayService->StopWifiDisplayScan();
            }
        }
    }

    // Tell the media router service all about it.
    if (routeTypes != mDiscoveryRequestRouteTypes
            || activeScan != mDiscoverRequestActiveScan) {
        mDiscoveryRequestRouteTypes = routeTypes;
        mDiscoverRequestActiveScan = activeScan;
        PublishClientDiscoveryRequest();
    }
}

void CMediaRouter::Static::SetSelectedRoute(
    /* [in] */ IRouteInfo* info,
    /* [in] */ Boolean explicit)
{
    // Must be non-reentrant.
    mSelectedRoute = info;
    PublishClientSelectedRoute(explicit);
}

void CMediaRouter::Static::RebindAsUser(
    /* [in] */ Int32 userId)
{
    if (mCurrentUserId != userId || userId < 0 || mClient == NULL) {
        if (mClient != NULL) {
            // try {
            ECode ec = mMediaRouterService->UnregisterClient(mClient);
            if (ec == (ECode)E_REMOTE_EXCEPTION) {
                Slogger::E(TAG, "Unable to unregister media router client.");
            }
            // } catch (RemoteException ex) {
                // Log.e(TAG, "Unable to unregister media router client.", ex);
            // }
            mClient = NULL;
        }

        mCurrentUserId = userId;

        // try {
        Client client = new Client();
        String packageName;
        mAppContext->GetPackageName(&packageName);
        ECode ec = mMediaRouterService->RegisterClientAsUser(client, packageName, userId);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Unable to register media router client.");
        }
        mClient = client;
        // } catch (RemoteException ex) {
            // Log.e(TAG, "Unable to register media router client.", ex);
        // }

        PublishClientDiscoveryRequest();
        PublishClientSelectedRoute(FALSE);
        UpdateClientState();
    }
}

void CMediaRouter::Static::PublishClientDiscoveryRequest()
{
    if (mClient != NULL) {
    // try {
    ECode ec = mMediaRouterService->SetDiscoveryRequest(mClient,
                mDiscoveryRequestRouteTypes, mDiscoverRequestActiveScan);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "Unable to publish media router client discovery request.");
    }
    // } catch (RemoteException ex) {
        // Log.e(TAG, "Unable to publish media router client discovery request.", ex);
    // }
    }
}

void CMediaRouter::Static::PublishClientSelectedRoute(
    /* [in] */ Boolean explicit)
{
    if (mClient != NULL) {
    // try {
    ECode ec = mMediaRouterService->SetSelectedRoute(mClient,
        mSelectedRoute != NULL ? mSelectedRoute.mGlobalRouteId : NULL, explicit);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "Unable to publish media router client selected route.");
    }
    // } catch (RemoteException ex) {
        // Log.e(TAG, "Unable to publish media router client selected route.", ex);
    // }
    }
}

void CMediaRouter::Static::UpdateClientState()
{
    // Update the client state.
    mClientState = NULL;
    ECode ec = NOERROR;
    if (mClient != NULL) {
        // try {
        ec = mMediaRouterService->GetState(mClient, (IMediaRouterClientState**)&mClientState);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Unable to retrieve media router client state.");
        }
        // } catch (RemoteException ex) {
            // Log.e(TAG, "Unable to retrieve media router client state.", ex);
        // }
    }
    // ArrayList<MediaRouterClientState::RouteInfo> globalRoutes =
    AutoPtr<IArrayList> globalRoutes =
        mClientState != NULL ? (MediaRouterClientState*)mClientState->mRoutes : NULL;
    String globallySelectedRouteId = mClientState != NULL ?
            (MediaRouterClientState*)mClientState->mGloballySelectedRouteId : NULL;

    // Add or update routes.
    Int32 size;
    Int32 globalRouteCount = globalRoutes != NULL ? (globalRoutes->GetSize(&size), size) : 0;
    for (Int32 i = 0; i < globalRouteCount; i++) {
        AutoPtr<IInterface> obj;
        globalRoutes->Get(i, (IInterface**)&obj);
        AutoPtr<MediaRouterClientState::RouteInfo> globalRoute =
            (MediaRouterClientState::RouteInfo*)IMediaRouterClientStateRouteInfo::Probe(obj);
        RouteInfo route = FindGlobalRoute(globalRoute.id);
        if (route == NULL) {
            route = MakeGlobalRoute(globalRoute);
            AddRouteStatic(route);
        } else {
            UpdateGlobalRoute(route, globalRoute);
        }
    }

    // Synchronize state with the globally selected route.
    if (globallySelectedRouteId != NULL) {
        AutoPtr<RouteInfo> route;
        FindGlobalRoute(globallySelectedRouteId, (RouteInfo**)&route);
        if (route == NULL) {
            Slogger::W(TAG, "Could not find new globally selected route: %s"
                    , globallySelectedRouteId.string());
        } else if (route != mSelectedRoute) {
            if (DEBUG) {
                Slogger::D(TAG, "Selecting new globally selected route: %p" + route);
            }
            selectRouteStatic(route.mSupportedTypes, route, FALSE);
        }
    } else if (mSelectedRoute != NULL && mSelectedRoute.mGlobalRouteId != NULL) {
        if (DEBUG) {
            Log.d(TAG, "Unselecting previous globally selected route: " + mSelectedRoute);
        }
        selectDefaultRouteStatic();
    }

    // Remove defunct routes.
    outer: for (Int32 i = mRoutes.size(); i-- > 0; ) {
        final RouteInfo route = mRoutes.get(i);
        final String globalRouteId = route.mGlobalRouteId;
        if (globalRouteId != NULL) {
            for (int j = 0; j < globalRouteCount; j++) {
                MediaRouterClientState.RouteInfo globalRoute = globalRoutes.get(j);
                if (globalRouteId.equals(globalRoute.id)) {
                    continue outer; // found
                }
            }
            // not found
            removeRouteStatic(route);
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

ECode CMediaRouter::Static::FindGlobalRoute(
    /* [in] */ const String& globalRouteId,
    /* [out] */ RouteInfo** result)
{
    VALIDATE_NOT_NULL(result);

// List< AutoPtr<IRouteInfo> > mRoutes;
    List< AutoPtr<RouteInfo> >::Iterator it = mRoutes.Begin();
    for (; it != mRoutes.End(); ++it) {
        AutoPtr<RouteInfo> route = *it;
        if (globalRouteId.Equals(route->mGlobalRouteId)) {
            *result = route;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    *result = NULL;
    return NOERROR;
}

//================================================================================
//                      CMediaRouter::RouteInfo
//================================================================================
CAR_INTERFACE_IMPL(CMediaRouter::RouteInfo, Object, IMediaRouterRouteInfo)

CMediaRouter::RouteInfo::RouteInfo()
    : mPlaybackType()
    , mVolumeMax(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME)
    , mVolume(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME)
    , mVolumeHandling(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME_HANDLING)
    , mPlaybackStream(IAudioManager::STREAM_MUSIC)
    , mPresentationDisplayId(-1)
    , mEnabled(TRUE)
{}

CMediaRouter::RouteInfo::~RouteInfo()
{}

ECode CMediaRouter::RouteInfo::constructor(
    /* [in] */ IMediaRouterRouteCategory* category)
{
    mCategory = category;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetName(
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result);
    return GetName(sStatic->mResources.Get(), result);
}

ECode CMediaRouter::RouteInfo::GetName(
    /* [in] */ IContext * context,
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return GetName(res.Get(), result);
}

ECode CMediaRouter::RouteInfo::GetName(
    /* [in] */ IResources* res,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    if (mNameResId != 0) {
        res->GetText(mNameResId, (IContext**)&mName);
        *result = mName.Get();
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    *result = mName.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetDescription(
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDescription;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetStatus(
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStatus;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::SetRealStatusCode(
    /* [in] */ Int32 statusCode,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (mRealStatusCode != statusCode) {
        mRealStatusCode = statusCode;
        return ResolveStatusCode(result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::ResolveStatusCode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 statusCode = mRealStatusCode;
    Boolean flag = FALSE;
    IsSelected(&flag);
    if (flag) {
        switch (statusCode) {
            // If the route is selected and its status appears to be between states
            // then report it as connecting even though it has not yet had a chance
            // to officially move into the CONNECTING state.  Note that routes in
            // the NONE state are assumed to not require an explicit connection
            // lifecycle whereas those that are AVAILABLE are assumed to have
            // to eventually proceed to CONNECTED.
            case STATUS_AVAILABLE:
            case STATUS_SCANNING:
                statusCode = IMediaRouterRouteInfo::STATUS_CONNECTING;
                break;
        }
    }
    if (mResolvedStatusCode == statusCode) {
        *result = FALSE;
        return NOERROR;
    }

    mResolvedStatusCode = statusCode;
    Int32 resId;
    switch (statusCode) {
        case IMediaRouterRouteInfo::STATUS_SCANNING:
            resId = R::string::media_route_status_scanning;
            break;
        case IMediaRouterRouteInfo::STATUS_CONNECTING:
            resId = R::string::media_route_status_connecting;
            break;
        case IMediaRouterRouteInfo::STATUS_AVAILABLE:
            resId = R::string::media_route_status_available;
            break;
        case IMediaRouterRouteInfo::STATUS_NOT_AVAILABLE:
            resId = R::string::media_route_status_not_available;
            break;
        case IMediaRouterRouteInfo::STATUS_IN_USE:
            resId = R::string::media_route_status_in_use;
            break;
        case IMediaRouterRouteInfo::STATUS_CONNECTED:
        case IMediaRouterRouteInfo::STATUS_NONE:
        default:
            resId = 0;
            break;
    }
    AutoPtr<ICharSequence> cs;
    sStatic->mResources->GetText(resId, (ICharSequence**)&cs);

    mStatus = resId != 0 ? cs.Get() : NULL;
    *result = TRUE;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetStatusCode(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResolvedStatusCode;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetSupportedTypes(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSupportedTypes;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::MatchesTypes(
    /* [in] */ Int32 types,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mSupportedTypes & types) != 0;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetGroup(
    /* [out] */ IMediaRouterRouteGroup ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGroup;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetCategory(
    /* [out] */ IMediaRouterRouteCategory ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetIconDrawable(
    /* [out] */ IDrawable ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIcon;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::SetTag(
    /* [in] */ IInterface * tag)
{
    mTag = (Object*)tag;
    return RouteUpdated();
}

ECode CMediaRouter::RouteInfo::GetTag(
    /* [out] */ IInterface ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = (IInterface*)mTag;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetPlaybackType(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPlaybackType;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetPlaybackStream(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPlaybackStream;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetVolume(
    /* [out] */ Int32 * result)
{
    if (mPlaybackType == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL) {
        Int32 vol = 0;
        // try {
        ECode ec = sStatic->mAudioService->GetStreamVolume(mPlaybackStream, &vol);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Error getting local stream volume");
        }
        // } catch (RemoteException e) {
            // Log.e(TAG, "Error getting local stream volume", e);
        // }
        *result = vol;
        return NOERROR;
    } else {
        *result = mVolume;
        return NOERROR;
    }
}

ECode CMediaRouter::RouteInfo::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    if (mPlaybackType == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL) {
        // try {
        String currentPackageName = CActivityThread::GetCurrentPackageName();
        ECode ec = sStatic->mAudioService->SetStreamVolume(mPlaybackStream, volume, 0,
            AcurrentPackageName.string());
        // } catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Error setting local stream volume");
        }

        if (SUCCEEDED(ec)) {
            return ec;
        }
        // }
    } else {
        return sStatic->RequestSetVolume(this, volume);
    }
}

ECode CMediaRouter::RouteInfo::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    if (mPlaybackType == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL) {
        // try {
        Int32 volume;
        GetVolume(&volume);
        Int32 volumeMax;
        GetVolumeMax(&volumeMax);
        Int32 volume = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(volume + direction, volumeMax));
        return sStatic->mAudioService->SetStreamVolume(mPlaybackStream, volume, 0,
                    CActivityThread::GetCurrentPackageName());
        // } catch (RemoteException e) {
            // Log.e(TAG, "Error setting local stream volume", e);
        // }
    } else {
        sStatic->RequestUpdateVolume(this, direction);
        return NOERROR;
    }
}

ECode CMediaRouter::RouteInfo::GetVolumeMax(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);

    if (mPlaybackType == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL) {
        // try {
        ECode ec = sStatic->mAudioService->GetStreamMaxVolume(mPlaybackStream, result);
        if (SUCCEEDED(ec)) {
            return ec;
        }
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Error getting local stream volume");
        }
        // } catch (RemoteException e) {
            // Log.e(TAG, "Error getting local stream volume", e);
        // }
    } else {
        *result = mVolumeMax;
        return NOERROR;
    }
}

ECode CMediaRouter::RouteInfo::GetVolumeHandling(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVolumeHandling;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetPresentationDisplay(
    /* [out] */ IDisplay ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPresentationDisplay;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::UpdatePresentationDisplay(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IDisplay> display = ChoosePresentationDisplay();
    if (mPresentationDisplay != display) {
        mPresentationDisplay = display;
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::GetDeviceAddress(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDeviceAddress;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::IsEnabled(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEnabled;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::IsConnecting(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResolvedStatusCode == IMediaRouterRouteInfo::STATUS_CONNECTING;
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::IsSelected(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    return IMediaRouterRouteInfo::Probe(this)->Equals(sStatic->mSelectedRoute, result);
}

ECode CMediaRouter::RouteInfo::IsDefault(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    return IMediaRouterRouteInfo::Probe(this)->Equals(sStatic.mDefaultAudioVideo, result);
}

ECode CMediaRouter::RouteInfo::Select()
{
    SelectRouteStatic(mSupportedTypes, this, TRUE);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::SetStatusInt(
    /* [in] */ ICharSequence* status)
{
    Boolean flag = FALSE;
    status->Equals(mStatus, &flag);
    if (!flag) {
        mStatus = status;
        if (mGroup != NULL) {
            mGroup->MemberStatusChanged(this, status);
        }
        return RouteUpdated();
    }
}

ECode CMediaRouter::RouteInfo::RouteUpdated()
{
    UpdateRoute(this);
    return NOERROR;
}

ECode CMediaRouter::RouteInfo::ToString(
    /* [out] */ String* result)
{
    Int32 types;
    GetSupportedTypes(&types);
    String supportedTypes;
    TypesToString(types, &supportedTypes);
    String name;
    AutoPtr<ICharSequence> cs;
    GetName((ICharSequence**)&cs);
    cs->ToString(&name);
    cs = NULL;
    GetDescription((ICharSequence**)&cs);
    String description;
    cs->GetDescription(&description);
    cs = NULL;
    String status;
    cs->GetStatus(&status);
    AutoPtr<IMediaRouterRouteCategory> rc;
    GetCategory((IMediaRouterRouteCategory **)&rc);
    String category;
    IObject::Probe(rc)->ToString(&category);
    String presentationDisplay;
    IObject::Probe(mPresentationDisplay)->ToString(&presentationDisplay);
    //todo
    *result = /*getClass().getSimpleName() */+ String("{ name=") + name +
            String(", description=") + description +
            String(", status=") + status +
            String(", category=") + category +
            String(", supportedTypes=") + supportedTypes +
            String(", presentationDisplay=") + presentationDisplay + String(" }");
    return NOERROR;
}

AutoPtr<Display> CMediaRouter::RouteInfo::ChoosePresentationDisplay()
{
    if ((mSupportedTypes & IMediaRouter::ROUTE_TYPE_LIVE_VIDEO) != 0) {
        AutoPtr<ArrayOf<AutoPtr<IDisplay> > > displays = sStatic->GetAllPresentationDisplays();

        // Ensure that the specified display is valid for presentations.
        // This check will normally disallow the default display unless it was
        // configured as a presentation display for some reason.
        assert(displays != NULL);
        Int32 len = displays->GetLength();

        AutoPtr<Display> dp = NULL;

        if (mPresentationDisplayId >= 0) {
            for (Int32 i = 0; i < len; i++) {
                Int32 displayId;
                (*displays)[i]->GetDisplayId(&displayId);
                if (displayId == mPresentationDisplayId) {
                    dp = NULL;
                    dp = (Display*)(*displays)[i];
                    return dp;
                }
            }
            return NULL;
        }

        // Find the indicated Wifi display by its address.
        if (mDeviceAddress != NULL) {
            for (Int32 i = 0; i < len; i++) {
                Int32 vol;
                (*displays)[i]->GetType(&vol);

                if (vol == IDisplay::TYPE_WIFI) {
                     String address;
                    (*displays)[i]->GetAddress(&address);
                    if (mDeviceAddress.Equals(address)) {
                        dp = NULL;
                        dp = (Display*)(*displays)[i];
                        return dp;
                    }
                }
            }
            return NULL;
        }

        // For the default route, choose the first presentation display from the list.
        if (IMediaRouterRouteInfo::Probe(this).Equals(sStatic->mDefaultAudioVideo) && len > 0) {
            dp = NULL;
            dp = (Display*)(*displays)[0];
            return dp;
        }
    }
    return NULL;
}

//================================================================================
//                      CMediaRouter::UserRouteInfo::SessionVolumeProvider
//================================================================================
CMediaRouter::UserRouteInfo::SessionVolumeProvider::SessionVolumeProvider()
{}

CMediaRouter::UserRouteInfo::SessionVolumeProvider::~SessionVolumeProvider()
{}

CAR_INTERFACE_IMPL_2(CMediaRouter::UserRouteInfo::SessionVolumeProvider, Object, IVolumeProvider, IMediaRouterUserRouteInfoSessionVolumeProvider)

ECode CMediaRouter::UserRouteInfo::SessionVolumeProvider::constructor(
    /* [in] */ Int32 volumeControl,
    /* [in] */ Int32 maxVolume,
    /* [in] */ Int32 currentVolume)
{
    return VolumeProvider::constructor(volumeControl, maxVolume, currentVolume);
}

ECode CMediaRouter::UserRouteInfo::SessionVolumeProvider::OnSetVolumeTo(
    /* [in] */ Int32 volume)
{
    assert(0 && "TODO");
    // sStatic.mHandler.post(new Runnable() {
    //     @Override
    //     public void run() {
    //         if (mVcb != null) {
    //             mVcb.vcb.onVolumeSetRequest(mVcb.route, volume);
    //         }
    //     }
    // });
}

ECode CMediaRouter::UserRouteInfo::SessionVolumeProvider::OnAdjustVolume(
    /* [in] */ Int32 direction)
{
    // sStatic.mHandler.post(new Runnable() {
    //     @Override
    //     public void run() {
    //         if (mVcb != null) {
    //             mVcb.vcb.onVolumeUpdateRequest(mVcb.route, direction);
    //         }
    //     }
    // });
}

//================================================================================
//                      CMediaRouter::UserRouteInfo
//================================================================================
CMediaRouter::UserRouteInfo::UserRouteInfo()
{}

CMediaRouter::UserRouteInfo::~UserRouteInfo()
{}

CAR_INTERFACE_IMPL_2(CMediaRouter::UserRouteInfo, Object, IMediaRouterUserRouteInfo, IMediaRouterRouteInfo)

ECode CMediaRouter::UserRouteInfo::constructor(
    /* [in] */ IMediaRouterRouteCategory* category)
{
    RouteInfo::constructor(category);
    mSupportedTypes = IMediaRouter::ROUTE_TYPE_USER;
    mPlaybackType = IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE;
    mVolumeHandling = IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED;
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::SetName(
    /* [in] */ ICharSequence * name)
{
    mName = name;
    return RouteUpdated();
}

ECode CMediaRouter::UserRouteInfo::SetName(
    /* [in] */ Int32 resId)
{
    mNameResId = resId;
    mName = NULL;
    return RouteUpdated();
}

ECode CMediaRouter::UserRouteInfo::SetDescription(
    /* [in] */ ICharSequence * description);

ECode CMediaRouter::UserRouteInfo::SetStatus(
    /* [in] */ ICharSequence * status)
{
    mDescription = description;
    return RouteUpdated();
}

ECode CMediaRouter::UserRouteInfo::SetRemoteControlClient(
    /* [in] */ IRemoteControlClient * rcc)
{
    mRcc = rcc;
    UpdatePlaybackInfoOnRcc();
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::GetRemoteControlClient(
    /* [out] */ IRemoteControlClient ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRcc;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::SetIconDrawable(
    /* [in] */ IDrawable * icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::SetIconResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> dr;
    sStatic->mResources->GetDrawable(resId, (IDrawable**)&dr);
    return SetIconDrawable(dr.Get());
}

ECode CMediaRouter::UserRouteInfo::SetVolumeCallback(
    /* [in] */ IMediaRouterVolumeCallback * vcb)
{
    AutoPtr<VolumeCallbackInfo> v = new VolumeCallbackInfo();
    v->constructor(vcb, this);
    mVcb = v;
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::SetPlaybackType(
    /* [in] */ Int32 type)
{
    if (mPlaybackType != type) {
        mPlaybackType = type;
        ConfigureSessionVolume();
    }
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::SetVolumeHandling(
    /* [in] */ Int32 volumeHandling)
{
    if (mVolumeHandling != volumeHandling) {
        mVolumeHandling = volumeHandling;
        ConfigureSessionVolume();
    }
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::SetVolume(
    /* [in] */ Int32 volume)
{
    Int32 vol;
    GetVolumeMax(&vol);
    volume = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(volume, vol));
    if (mVolume != volume) {
        mVolume = volume;
        if (mSvp != NULL) {
            mSvp->SetCurrentVolume(mVolume);
        }
        DispatchRouteVolumeChanged(this);
        if (mGroup != NULL) {
            mGroup->MemberVolumeChanged(this);
        }
    }
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    if (mVolumeHandling == IMediaRouterRouteInfo::PLAYBACK_VOLUME_VARIABLE) {
        if (mVcb == NULL) {
            Slogger::E(TAG, "Cannot requestSetVolume on user route - no volume callback set");
            return NOERROR;
        }
        mVcb->vcb->OnVolumeSetRequest(this, volume);
    }
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    if (mVolumeHandling == IMediaRouterRouteInfo::PLAYBACK_VOLUME_VARIABLE) {
        if (mVcb == NULL) {
            Slogger::E(TAG, "Cannot requestChangeVolume on user route - no volumec callback set");
            return NOERROR;
        }
        mVcb->mVcb->OnVolumeUpdateRequest(this, direction);
    }
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::SetVolumeMax(
    /* [in] */ Int32 volumeMax)
{
    if (mVolumeMax != volumeMax) {
        mVolumeMax = volumeMax;
        ConfigureSessionVolume();
    }
    return NOERROR;
}

ECode CMediaRouter::UserRouteInfo::SetPlaybackStream(
    /* [in] */ Int32 stream)
{
    if (mPlaybackStream != stream) {
        mPlaybackStream = stream;
        ConfigureSessionVolume();
    }
    return NOERROR;
}

void CMediaRouter::UserRouteInfo::UpdatePlaybackInfoOnRcc()
{
    ConfigureSessionVolume();
}

void CMediaRouter::UserRouteInfo::ConfigureSessionVolume()
{
    if (mRcc == NULL) {
        if (DEBUG) {
            Slogger::D(TAG, "No Rcc to configure volume for route %s", mName.string());
        }
        return;
    }
    AutoPtr<IMediaSession> session;
    mRcc->GetMediaSession((IMediaSession**)&session);
    if (session == NULL) {
        if (DEBUG) {
            Slogger::D(TAG, "Rcc has no session to configure volume");
        }
        return;
    }
    if (mPlaybackType == IRemoteControlClient::PLAYBACK_TYPE_REMOTE) {
        Int32 volumeControl = IVolumeProvider::VOLUME_CONTROL_FIXED;
        switch (mVolumeHandling) {
            case IRemoteControlClient::PLAYBACK_VOLUME_VARIABLE:
                volumeControl = IVolumeProvider::VOLUME_CONTROL_ABSOLUTE;
                break;
            case IRemoteControlClient::PLAYBACK_VOLUME_FIXED:
            default:
                break;
        }
        // Only register a new listener if necessary
        Int32 vol;
        IVolumeProvider::Probe(mSvp)->GetVolumeControl(&vol);
        Int32 vm;
        IVolumeProvider::Probe(mSvp)-GetMaxVolume(&vm);
        if (mSvp == NULL || vol != volumeControl
                || vm != mVolumeMax) {
            AutoPtr<SessionVolumeProvider> s = new SessionVolumeProvider();
            s->constructor(volumeControl, mVolumeMax, mVolume);
            mSvp = s;
            session->SetPlaybackToRemote(IVolumeProvider::Probe(mSvp));
        }
    } else {
        // We only know how to handle local and remote, fall back to local if not remote.
        AutoPtr<IAudioAttributesBuilder> bob;
        CAudioAttributesBuilder::New((IAudioAttributesBuilder**)&bob);
        bob->SetLegacyStreamType(mPlaybackStream);
        AutoPtr<IAudioAttributes> attributes;
        bob->Build((IAudioAttributes**)&attributes);
        session->SetPlaybackToLocal(attributes.Get());
        mSvp = NULL;
    }
}

//================================================================================
//                      CMediaRouter::RouteGroup
//================================================================================
CMediaRouter::RouteGroup::RouteGroup()
    : mUpdateName(FALSE)
{}

CMediaRouter::RouteGroup::~RouteGroup()
{}

CAR_INTERFACE_IMPL_2(CMediaRouter::RouteGroup, Object, IMediaRouterRouteGroup, IMediaRouterRouteInfo)

ECode CMediaRouter::RouteGroup::constructor(
    /* [in] */ IMediaRouterRouteCategory* category)
{
    RouteInfo::constructor(category);
    mGroup = this;
    mVolumeHandling = IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED;
    return NOERROR;
}

ECode CMediaRouter::RouteGroup::GetName(
    /* [in] */ IResources* res,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);

    if (mUpdateName) UpdateName();
    return RouteInfo::GetName(res, result);
}

ECode CMediaRouter::RouteGroup::AddRoute(
    /* [in] */ IMediaRouterRouteInfo * route)
{
    VALIDATE_NOT_NULL(route);
    AutoPtr<IMediaRouterRouteGroup> mr;
    route->GetGroup((IMediaRouterRouteGroup**)&mr);
    if (mr != NULL) {
        Slogger::E("RouteGroup", "Route %p is already part of a group.", route);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IMediaRouterRouteCategory> rc;
    route->GetCategory((IMediaRouterRouteCategory**)&rc);

    if (!rc->Equals(mCategory)) {
        Slogger::E("RouteGroup", "Route cannot be added to a group with a different category. (Route category=%p group category=%p)", rc, mCategory);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;;
    }
    Int32 at;
    mRoutes->GetSize(&at);
    mRoutes->Add(route);
    route->mGroup = this;
    mUpdateName = TRUE;
    UpdateVolume();
    RouteUpdated();
    DispatchRouteGrouped(route, this, at);
    return NOERROR;
}

ECode CMediaRouter::RouteGroup::AddRoute(
    /* [in] */ IMediaRouterRouteInfo * route,
    /* [in] */ Int32 insertAt)
{
    VALIDATE_NOT_NULL(route);
    AutoPtr<IMediaRouterRouteGroup> mr;
    route->GetGroup((IMediaRouterRouteGroup**)&mr);
    if (mr != NULL) {
        Slogger::E("RouteGroup", "Route %p is already part of a group.", route);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IMediaRouterRouteCategory> rc;
    route->GetCategory((IMediaRouterRouteCategory**)&rc);

    if (!rc->Equals(mCategory)) {
        Slogger::E("RouteGroup", "Route cannot be added to a group with a different category. (Route category=%p group category=%p)", rc, mCategory);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;;
    }
    mRoutes->Add(insertAt, route);
    route->mGroup = this;
    mUpdateName = TRUE;
    UpdateVolume();
    RouteUpdated();
    DispatchRouteGrouped(route, this, insertAt);
    return NOERROR;
}

ECode CMediaRouter::RouteGroup::RemoveRoute(
    /* [in] */ IMediaRouterRouteInfo * route)
{
    VALIDATE_NOT_NULL(route);
    AutoPtr<IMediaRouterRouteGroup> mr;
    route->GetGroup((IMediaRouterRouteGroup**)&mr);
    if (!mr.Equals(IMediaRouterRouteGroup::Probe(this))) {
        Slogger::E("RouteGroup", "Route %p is not a member of this group.", route);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRoutes->Remove(route);
    route->mGroup = NULL;
    mUpdateName = TRUE;
    UpdateVolume();
    DispatchRouteUngrouped(route, this);
    return RouteUpdated();
}

ECode CMediaRouter::RouteGroup::RemoveRoute(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> obj;
    mRoutes->Remove(index, (IInterface**)&obj);
    AutoPtr<RouteInfo> route = (RouteInfo*)IMediaRouterRouteInfo::Probe(obj);
    route->mGroup = NULL;
    mUpdateName = TRUE;
    UpdateVolume();
    DispatchRouteUngrouped(route, this);
    return RouteUpdated();
}

ECode CMediaRouter::RouteGroup::GetRouteCount(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mRoutes->GetSize(&size);
    *result = siz;
    return NOERROR;
}

ECode CMediaRouter::RouteGroup::GetRouteAt(
    /* [in] */ Int32 index,
    /* [out] */ IMediaRouterRouteInfo ** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mRoutes->Get(index, (IInterface**)&obj);
    *result = IMediaRouterRouteInfo::Probe(obj);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteGroup::SetIconDrawable(
    /* [in] */ IDrawable * icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode CMediaRouter::RouteGroup::SetIconResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> drawable;
    sStatic->mResources->GetDrawable(resId, (IDrawable**)&drawable);
    return SetIconDrawable(drawable.Get());
}

ECode CMediaRouter::RouteGroup::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    Int32 maxVol;
    GetVolumeMax(&maxVol);
    if (maxVol == 0) {
        return NOERROR;
    }

    Float scaledVolume = (Float) volume / maxVol;
    Int32 routeCount;
    GetRouteCount(&routeCount);
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IMediaRouterRouteInfo> mr;
        GetRouteAt(i, (IMediaRouterRouteInfo**)&mr);
        AutoPtr<RouteInfo> route = (RouteInfo*)mr.Get();
        Int32 vol;
        route->GetVolumeMax(&vol);
        Int32 routeVol = (Int32) (scaledVolume * vol);
        route->RequestSetVolume(routeVol);
    }
    if (volume != mVolume) {
        mVolume = volume;
        DispatchRouteVolumeChanged(this);
    }
    return NOERROR;
}

ECode CMediaRouter::RouteGroup::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    Int32 maxVol;
    GetVolumeMax(&maxVol);
    if (maxVol == 0) {
        return NOERROR;
    }

    Int32 routeCount;
    GetRouteCount(&routeCount);
    Int32 volume = 0;
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IMediaRouterRouteInfo> r;
        GetRouteAt(i, (IMediaRouterRouteInfo**)&r);
        AutoPtr<RouteInfo> route = (RouteInfo*)r.Get();
        route->RequestUpdateVolume(direction);
        Int32 routeVol;
        route->GetVolume(&routeVol);
        if (routeVol > volume) {
            volume = routeVol;
        }
    }
    if (volume != mVolume) {
        mVolume = volume;
        DispatchRouteVolumeChanged(this);
    }

    return NOERROR;
}

void CMediaRouter::RouteGroup::MemberNameChanged(
    /* [in] */ RouteInfo* info,
    /* [in] */ ICharSequence* name)
{
    mUpdateName = TRUE;
    RouteUpdated();
}

void CMediaRouter::RouteGroup::MemberStatusChanged(
    /* [in] */ RouteInfo* info,
    /* [in] */ ICharSequence* status)
{
    SetStatusInt(status);
}

void CMediaRouter::RouteGroup::MemberVolumeChanged(
    /* [in] */ RouteInfo* info)
{
    UpdateVolume();
}

void CMediaRouter::RouteGroup::UpdateVolume()
{
    // A group always represents the highest component volume value.
    Int32 routeCount;
    GetRouteCount(&routeCount);
    Int32 volume = 0;
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IMediaRouterRouteInfo> mr;
        GetRouteAt(i, (IMediaRouterRouteInfo**)&mr);
        Int32 routeVol;
        mr->GetVolume(&routeVol);
        if (routeVol > volume) {
            volume = routeVol;
        }
    }
    if (volume != mVolume) {
        mVolume = volume;
        DispatchRouteVolumeChanged(this);
    }
}

void CMediaRouter::RouteGroup::RouteUpdated()
{
    Int32 types = 0;
    Int32 count;
    mRoutes->GetSize(&count);
    if (count == 0) {
        // Don't keep empty groups in the router.
        CMediaRouterHelper::RemoveRouteStatic(this);
        return;
    }

    Int32 maxVolume = 0;
    Boolean isLocal = TRUE;
    Boolean isFixedVolume = TRUE;
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IMediaRouterRouteInfo> obj;
        mRoutes->Get(i, (IMediaRouterRouteInfo**)&obj);
        AutoPtr<RouteInfo> route = (RouteInfo*)obj;
        types |= route->mSupportedTypes;
        Int32 routeMaxVolume;
        route->GetVolumeMax(&routeMaxVolume);
        if (routeMaxVolume > maxVolume) {
            maxVolume = routeMaxVolume;
        }
        Int32 vol;
        isLocal &= (route->GetPlaybackType(&vol), vol) == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL;
        Int32 handling;
        isFixedVolume &= (route->GetVolumeHandling(&handling), handling) == IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED;
    }
    mPlaybackType = isLocal ? IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL : IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE;
    mVolumeHandling = isFixedVolume ? IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED : IMediaRouterRouteInfo::PLAYBACK_VOLUME_VARIABLE;
    mSupportedTypes = types;
    mVolumeMax = maxVolume;
    AutoPtr<IInterface> obj;
    mRoutes->Get(0, (IInterface**)&obj);
    AutoPtr<IDrawable> dr;
    IMediaRouterRouteInfo::Probe(obj)->GetIconDrawable((IDrawable**)&dr);
    mIcon = count == 1 ? dr : NULL;
    RouteInfo::RouteUpdated();
}

void CMediaRouter::RouteGroup::UpdateName()
{
    StringBuilder sb;
    Int32 count;
    mRoutes->GetSize(&count);
    AutoPtr<IInterface> obj;
    AutoPtr<RouteInfo> info;
    for (Int32 i = 0; i < count; i++) {
        mRoutes->Get(i, (IInterface**)&obj);
        info = NULL;
        info = (RouteInfo*)IMediaRouterRouteInfo::Probe(obj);
        // TODO: There's probably a much more correct way to localize this.
        if (i > 0) sb.Append(String(", "));
        sb.Append(info.mName);
    }
    sb.ToString(&mName);
    mUpdateName = FALSE;
}

ECode CMediaRouter::RouteGroup::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String routeInfoStr;
    RouteInfo::ToString(&routeInfoStr);
    StringBuilder sb(routeInfoStr);
    sb.Append('[');
    Int32 count;
    mRoutes->GetSize(&count);
    AutoPtr<IInterface> obj;
    AutoPtr<IMediaRouterRouteInfo> rf;
    for (Int32 i = 0; i < count; i++) {
        if (i > 0) sb.Append(String(", "));
        rf = NULL;
        mRoutes->Get(i, (IInterface**)&obj);
        rf = NULL;
        rf = IMediaRouterRouteInfo::Probe(obj);
        sb.Append(rf.Get());
    }
    sb.Append(']');
    return sb.ToString(result);
}

//================================================================================
//                      CMediaRouter::RouteCategory
//================================================================================
CMediaRouter::RouteCategory::RouteCategory()
{}

CMediaRouter::RouteCategory::~RouteCategory()
{}

CAR_INTERFACE_IMPL(CMediaRouter::RouteCategory, Object, IMediaRouterRouteCategory)

ECode CMediaRouter::RouteCategory::constructor(
    /* [in] */ ICharSequence* name,
    /* [in] */ Int32 types,
    /* [in] */ Boolean groupable)
{
    mName = name;
    mTypes = types;
    mGroupable = groupable;
    return NOERROR;
}

ECode CMediaRouter::RouteCategory::constructor(
    /* [in] */ Int32 nameResId,
    /* [in] */ Int32 types,
    /* [in] */ Boolean groupable)
{
    mNameResId = nameResId;
    mTypes = types;
    mGroupable = groupable;
    return NOERROR;
}

ECode CMediaRouter::RouteCategory::GetName(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    return GetName(sStatic->mResources, result);
}

ECode CMediaRouter::RouteCategory::GetName(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return GetName(res.Get(), result);
}

ECode CMediaRouter::RouteCategory::GetName(
    /* [in] */ IResources* res,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    if (mNameResId != 0) {
        return res->GetText(mNameResId, result);
    }
    *result = mName.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteCategory::GetRoutes(
    /* [in] */ IList* outList,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    if (outList == NULL) {
        AutoPtr<IArrayList> al;
        CArrayList::New((IArrayList**)&al);
        outList = IList::Probe(al);
    } else {
        outList->Clear();
    }

    Int32 count = GetRouteCountStatic();
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IMediaRouterRouteInfo> route = GetRouteAtStatic(i);
        if (route->mCategory.Equals(IMediaRouterRouteCategory::Probe(this))) {
            outList->Add(route.Get());
        }
    }
    *result = outList;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::RouteCategory::GetSupportedTypes(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTypes;
    return NOERROR;
}

ECode CMediaRouter::RouteCategory::IsGroupable(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGroupable;
    return NOERROR;
}

ECode CMediaRouter::RouteCategory::IsSystem(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsSystem;
    return NOERROR;
}

ECode CMediaRouter::RouteCategory::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = String("RouteCategory{ name=") + mName + String(" types=") + TypesToString(mTypes) +
            String(" groupable=") + mGroupable + String(" }");
    return NOERROR;
}

//================================================================================
//                      CMediaRouter::CallbackInfo
//================================================================================
CMediaRouter::CallbackInfo::CallbackInfo();

CMediaRouter::CallbackInfo::~CallbackInfo();

CAR_INTERFACE_IMPL()

ECode CMediaRouter::CallbackInfo::constructor(
    /* [in] */ ICallback* cb,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouter* router)
{
    mCb = cb;
    mType = type;
    mRouter = router;
    return NOERROR;
}

ECode CMediaRouter::CallbackInfo::FilterRouteEvent(
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<RouteInfo> r = (RouteInfo*)route;
    return FilterRouteEvent(r->mSupportedTypes, result);
}

ECode CMediaRouter::CallbackInfo::FilterRouteEvent(
    /* [in] */ Int32 supportedTypes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mFlags & IMediaRouter::CALLBACK_FLAG_UNFILTERED_EVENTS) != 0
            || (type & supportedTypes) != 0;
    return NOERROR;
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
CAR_INTERFACE_IMPL(CMediaRouter::VolumeCallbackInfo, Object, IMediaRouterVolumeCallbackInfo)

CMediaRouter::VolumeCallbackInfo::VolumeCallbackInfo()
{}

CMediaRouter::VolumeCallbackInfo::~VolumeCallbackInfo()
{}

ECode CMediaRouter::VolumeCallbackInfo::constructor(
    /* [in] */ IVolumeCallback* vcb,
    /* [in] */ IRouteInfo* route)
{
    mVcb = vcb;
    mRoute = route;
    return NOERROR;
}

ECode CMediaRouter::VolumeCallbackInfo::SetVolumeCallback(
    /* [in] */ IMediaRouterVolumeCallback* vcb)
{
    mVcb = vcb;
    return NOERROR;
}

ECode CMediaRouter::VolumeCallbackInfo::GetVolumeCallback(
    /* [out] */ IMediaRouterVolumeCallback** vcb)
{
    VALIDATE_NOT_NULL(vcb);
    *vcb = mVcb;
    REFCOUNT_ADD(*vcb);
    return NOERROR;
}

ECode CMediaRouter::VolumeCallbackInfo::SetRouteInfo(
    /* [in] */ IRouteInfo* route)
{
    mRoute = route;
    return NOERROR;
}

ECode CMediaRouter::VolumeCallbackInfo::GetRouteInfo(
    /* [out] */ IRouteInfo** route)
{
    VALIDATE_NOT_NULL(route);
    *route = mRoute;
    REFCOUNT_ADD(*route);
    return NOERROR;
}

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
    /* [out] */ IMediaRouterRouteCategory** result)
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
    /* [out] */ IMediaRouterRouteCategory** result)
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
    /* [out] */ IMediaRouterRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);

    *result = sStatic->mRoutes[index];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::CreateUserRoute(
    /* [in] */ IMediaRouterRouteCategory* category,
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
    /* [out] */ IMediaRouterRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMediaRouterRouteCategory> routeCategory;
    CRouteCategory::New(name, ROUTE_TYPE_USER, isGroupable, (IMediaRouterRouteCategory**)&routeCategory);
    *result = routeCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::CreateRouteCategory(
    /* [in] */ Int32 nameResId,
    /* [in] */ Boolean isGroupable,
    /* [out] */ IMediaRouterRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMediaRouterRouteCategory> routeCategory;
    CRouteCategory::New(nameResId, ROUTE_TYPE_USER, isGroupable, (IMediaRouterRouteCategory**)&routeCategory);
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
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ Boolean explicit)
{
    AutoPtr<IMediaRouterRouteInfo> oldRoute = sStatic->mSelectedRoute;
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

    AutoPtr<IMediaRouterRouteInfo> btRoute = sStatic->mBluetoothA2dpRoute;
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
    /* [in] */ IMediaRouterRouteInfo* info)
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
    /* [in] */ IMediaRouterRouteInfo* info)
{
    if (info == NULL) return;

    AutoPtr<IMediaRouterRouteCategory> cat;
    info->GetCategory((IMediaRouterRouteCategory**)&cat);
    List<AutoPtr<IMediaRouterRouteCategory> >::Iterator it = Find(sStatic->mCategories.Begin(), sStatic->mCategories.End(), cat);
    if (it == sStatic->mCategories.End()) {
        sStatic->mCategories.PushBack(cat);
    }

    Boolean tempState;
    if ((cat->IsGroupable(&tempState), tempState) && !(IRouteGroup::Probe(info) != NULL)) {
        // Enforce that any added route in a groupable category must be in a group.
        AutoPtr<IRouteGroup> group;
        AutoPtr<IMediaRouterRouteCategory> routeCategory;
        info->GetCategory((IMediaRouterRouteCategory**)&routeCategory);
        CRouteGroup::New(routeCategory, (IRouteGroup**)&group);
        Int32 tempValue;
        info->GetSupportedTypes(&tempValue);
        ((IMediaRouterRouteInfo*)&group)->SetSupportedTypes(tempValue);
        sStatic->mRoutes.PushBack((IMediaRouterRouteInfo*)&group);
        DispatchRouteAdded((IMediaRouterRouteInfo*)&group);
        group->AddRoute(info);

        info = (IMediaRouterRouteInfo*)&group;
    } else {
        sStatic->mRoutes.PushBack(info);
        DispatchRouteAdded(info);
    }
}

void CMediaRouter::RemoveRoute(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    List< AutoPtr<IMediaRouterRouteInfo> >::Iterator it = Find(sStatic->mRoutes.Begin(), sStatic->mRoutes.End(), AutoPtr<IRouteInfo>(info));
    if (it != sStatic->mRoutes.End()) {
        sStatic->mRoutes.Erase(it);

        AutoPtr<IMediaRouterRouteCategory> removingCat;
        info->GetCategory((IMediaRouterRouteCategory**)&removingCat);

        Boolean found = FALSE;
        it = sStatic->mRoutes.Begin();
        for (; it != sStatic->mRoutes.End(); ++it) {
            AutoPtr<IMediaRouterRouteCategory> cat;
            (*it)->GetCategory((IMediaRouterRouteCategory**)&cat);
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
            List< AutoPtr<IMediaRouterRouteCategory> >::Iterator iter = Find(sStatic->mCategories.Begin(), sStatic->mCategories.End(), removingCat);
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
        List< AutoPtr<IMediaRouterRouteInfo> >::Iterator it = sStatic->mRoutes.Begin();
        for (Int32 i = 0; i <= routeIndex; ++i, ++it) {
        }

        AutoPtr<IMediaRouterRouteInfo> info = *it;
        sStatic->mRoutes.Erase(it);

        AutoPtr<IMediaRouterRouteCategory> removingCat;
        info->GetCategory((IMediaRouterRouteCategory**)&removingCat);
        Boolean found = FALSE;
        it = sStatic->mRoutes.Begin();
        for (; it != sStatic->mRoutes.End(); ++it) {
            AutoPtr<IMediaRouterRouteCategory> cat;
            (*it)->GetCategory((IMediaRouterRouteCategory**)&cat);
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
            List< AutoPtr<IMediaRouterRouteCategory> >::Iterator it = Find(sStatic->mCategories.Begin(), sStatic->mCategories.End(), removingCat);
            sStatic->mCategories.Erase(it);
        }
        DispatchRouteRemoved(info);
    }
}

Int32 CMediaRouter::GetRouteCountStatic()
{
    return sStatic->mRoutes.GetSize();
}

AutoPtr<IMediaRouterRouteInfo> CMediaRouter::GetRouteAtStatic(
    /* [in] */ Int32 index)
{
    return sStatic->mRoutes[index];
}

void CMediaRouter::UpdateRoute(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    DispatchRouteChanged(info);
}

void CMediaRouter::DispatchRouteSelected(
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
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
    /* [in] */ IMediaRouterRouteInfo* info)
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
    /* [in] */ IMediaRouterRouteInfo* info)
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
    /* [in] */ IMediaRouterRouteInfo* info)
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
    /* [in] */ IMediaRouterRouteInfo* info)
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
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IRouteGroup* group,
    /* [in] */ Int32 index)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        Int32 tempValue;
        if ((cbi->mType & (((IMediaRouterRouteInfo*)&group)->GetSupportedTypes(&tempValue), tempValue)) != 0) {
            cbi->mCb->OnRouteGrouped(cbi->mRouter, info, group, index);
        }
    }
}

void CMediaRouter::DispatchRouteUngrouped(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IRouteGroup* group)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = sStatic->mCallbacks.Begin();
    for (; it != sStatic->mCallbacks.End(); ++it) {
        AutoPtr<CallbackInfo> cbi = (*it);
        Int32 tempValue;
        if ((cbi->mType & (((IMediaRouterRouteInfo*)&group)->GetSupportedTypes(&tempValue), tempValue)) != 0) {
            cbi->mCb->OnRouteUngrouped(cbi->mRouter, info, group);
        }
    }
}

void CMediaRouter::DispatchRouteVolumeChanged(
    /* [in] */ IMediaRouterRouteInfo* info)
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
    /* [in] */ IMediaRouterRouteInfo* info)
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
    AutoPtr<IMediaRouterRouteInfo> selectedRoute = sStatic->mSelectedRoute;
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
            AutoPtr<IMediaRouterRouteInfo> route = FindWifiDisplayRoute(d);
            UpdateWifiDisplayRoute(route, d, available, newStatus);
        }
        Boolean tempState;
        d->Equals(activeDisplay, &tempState);
        if (tempState) {
            AutoPtr<IMediaRouterRouteInfo> activeRoute = FindWifiDisplayRoute(d);
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

AutoPtr<IMediaRouterRouteInfo> CMediaRouter::MakeWifiDisplayRoute(
    /* [in] */ IWifiDisplay* display,
    /* [in] */ Boolean available)
{
    AutoPtr<IMediaRouterRouteInfo> newRoute;
    CMediaRouteInfo::New(sStatic->mSystemCategory, (IMediaRouterRouteInfo**)&newRoute);

    String tempText;
    display->GetDeviceAddress(&tempText);
    newRoute->SetDeviceAddress(tempText);

    newRoute->SetSupportedTypes(ROUTE_TYPE_LIVE_AUDIO | ROUTE_TYPE_LIVE_VIDEO);
    newRoute->SetVolumeHandling(IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED);
    newRoute->SetPlaybackType(IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE);

    Boolean tempState;
    newRoute->SetStatusCode(available ?
            IMediaRouterRouteInfo::STATUS_AVAILABLE : IMediaRouterRouteInfo::STATUS_CONNECTING, &tempState);
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
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ IWifiDisplay* display,
    /* [in] */ Boolean available,
    /* [in] */ IWifiDisplayStatus* wifiDisplayStatus)
{
    Int32 tempValue;
    wifiDisplayStatus->GetScanState(&tempValue);
    Boolean isScanning = tempValue == IWifiDisplayStatus::SCAN_STATE_SCANNING;

    Boolean changed = FALSE;
    Int32 newStatus = IMediaRouterRouteInfo::STATUS_NONE;

    if (available) {
        newStatus = isScanning ? IMediaRouterRouteInfo::STATUS_SCANNING : IMediaRouterRouteInfo::STATUS_AVAILABLE;
    } else {
        newStatus = IMediaRouterRouteInfo::STATUS_NOT_AVAILABLE;
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
                newStatus = IMediaRouterRouteInfo::STATUS_NONE;
                break;
            }
            case IWifiDisplayStatus::DISPLAY_STATE_CONNECTING:
            {
                newStatus = IMediaRouterRouteInfo::STATUS_CONNECTING;
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
        AutoPtr<IMediaRouterRouteInfo> defaultRoute = sStatic->mDefaultAudioVideo;
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

AutoPtr<IMediaRouterRouteInfo> CMediaRouter::FindWifiDisplayRoute(
    /* [in] */ IWifiDisplay* d)
{
    Int32 count = sStatic->mRoutes.GetSize();
    String tempText1, tempText2;

    List< AutoPtr<IMediaRouterRouteInfo> >::Iterator it = sStatic->mRoutes.Begin();
    for (; it != sStatic->mRoutes.End(); ++it) {
        AutoPtr<IMediaRouterRouteInfo> info = *it;
        d->GetDeviceAddress(&tempText1);
        info->GetDeviceAddress(&tempText2);
        if (tempText1.Equals(tempText2)) {
            return info;
        }
    }
    return NULL;
}

AutoPtr<IDisplay> CMediaRouter::ChoosePresentationDisplayForRoute(
    /* [in] */ IMediaRouterRouteInfo* route,
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
