#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/hardware/display/WifiDisplay.h"
#include "elastos/droid/media/CAudioRoutesInfo.h"
#include "elastos/droid/media/CMediaRouter.h"
#include "elastos/droid/media/CRouteCategory.h"
#include "elastos/droid/media/CMediaRouterRouteGroup.h"
#include "elastos/droid/media/CMediaRouterRouteInfo.h"
#include "elastos/droid/media/VolumeProvider.h"
#include "elastos/droid/media/CMediaRouterUserRouteInfo.h"
#include "elastos/droid/media/MediaRouterClientState.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/etl/List.h>
#include <elastos/droid/os/Handler.h>
//#include <elastos/utility/CArrayList.h>

using Elastos::Droid::App::CActivityThread;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::CResourcesHelper;
using Elastos::Droid::Content::Res::IResourcesHelper;
using Elastos::Droid::Hardware::Display::EIID_IDisplayListener;
using Elastos::Droid::Hardware::Display::WifiDisplay;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::CArrayList;
using Elastos::Core::CString;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Media {

const String CMediaRouter::TAG("MediaRouter");
const Boolean CMediaRouter::DEBUG = FALSE; // TODO Log.isLoggable(TAG, Log.DEBUG);

AutoPtr<IMediaRouterStatic> CMediaRouter::sStatic;

const Int32 CMediaRouter::ROUTE_TYPE_ANY = IMediaRouter::ROUTE_TYPE_LIVE_AUDIO | IMediaRouter::ROUTE_TYPE_LIVE_VIDEO
            | IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY | IMediaRouter::ROUTE_TYPE_USER;

HashMap< AutoPtr<IContext>, AutoPtr<IMediaRouter> > CMediaRouter::sRouters;

Object CMediaRouter::mStaticClass;

//-----------------------------------------------
//    CMediaRouter::Static::MyAudioRoutesObserver
//-----------------------------------------------
AutoPtr<IHandler> CMediaRouter::Static::mHandler;
AutoPtr<IIMediaRouterClient> CMediaRouter::Static::mClient;
CMediaRouter* CMediaRouter::Static::mOwner;

CAR_INTERFACE_IMPL(CMediaRouter::Static::MyAudioRoutesObserver, Object, IIAudioRoutesObserver);

CMediaRouter::Static::MyAudioRoutesObserver::MyAudioRoutesObserver(
    /* [in] */ Static* owner)
    : mOwner(owner)
{}

ECode CMediaRouter::Static::MyAudioRoutesObserver::DispatchAudioRoutesChanged(
    /* [in] */ IAudioRoutesInfo* newRoutes)
{
    AutoPtr<MyRunnable> myRunnable = new MyRunnable(newRoutes, mOwner);
    Boolean tempState;
    return mOwner->mHandler->Post(myRunnable.Get(), &tempState);
}

//------------------------------------
//    CMediaRouter::Static::MyRunnable
//------------------------------------
CAR_INTERFACE_IMPL(CMediaRouter::Static::MyRunnable, Object, IRunnable)

CMediaRouter::Static::MyRunnable::MyRunnable(
    /* [in] */ IAudioRoutesInfo* newRoutes,
    /* [in] */ Static* owner)
    : mOwner(owner)
{}

ECode CMediaRouter::Static::MyRunnable::Run()
{
    if (IIMediaRouterClient::Probe(this) == mClient) {
        return mOwner->UpdateAudioRoutes(mNewRoutes);
    }
    return NOERROR;
}

//------------------------------------
//    CMediaRouter::Static::MyStaticClient
//------------------------------------

CAR_INTERFACE_IMPL(CMediaRouter::Static::MyStaticClient, Object, IIMediaRouterClient);

CMediaRouter::Static::MyStaticClient::MyStaticClient(
    /* [in] */ Static* owner)
    : mOwner(owner)
{}

ECode CMediaRouter::Static::MyStaticClient::OnStateChanged()
{
    AutoPtr<MyRunnable> mr = new MyRunnable(NULL, mOwner);
    Boolean flag = FALSE;
    return mHandler->Post(IRunnable::Probe(mr), &flag);
}

//------------------------
//    CMediaRouter::Static
//------------------------

CAR_INTERFACE_IMPL_2(CMediaRouter::Static, Object, IDisplayListener, IMediaRouterStatic);

CMediaRouter::Static::Static(
    /* [in] */ IContext* appContext,
    /* [in] */ IMediaRouter* owner)
    : mCanConfigureWifiDisplays(FALSE)
    , mActivelyScanningWifiDisplays(FALSE)
    , mDiscoveryRequestRouteTypes(0)
    , mCurrentUserId(-1)
{
    mOwner = (CMediaRouter*)owner;
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

    obj = NULL;
    appContext->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&obj);
    mDisplayService = IDisplayManager::Probe(obj);

    obj = NULL;
    serviceManager->GetService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&obj);
    mMediaRouterService = IIMediaRouterService::Probe(obj);

    CRouteCategory::New(R::string::default_audio_route_category_name,
       ROUTE_TYPE_LIVE_AUDIO | ROUTE_TYPE_LIVE_VIDEO, FALSE, IMediaRouter::Probe(this), (IMediaRouterRouteCategory**)&mSystemCategory);

    ((RouteCategory*)(mSystemCategory.Get()))->mIsSystem = TRUE;

    // Only the system can configure wifi displays.  The display manager
    // enforces this with a permission check.  Set a flag here so that we
    // know whether this process is actually allowed to scan and connect.
    Int32 vol;
    appContext->CheckPermission(Manifest::permission::CONFIGURE_WIFI_DISPLAY,
        Process::MyPid(), Process::MyUid(), &vol);

    mCanConfigureWifiDisplays = (vol == IPackageManager::PERMISSION_GRANTED);
}

CMediaRouter::Static::~Static()
{}

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

ECode CMediaRouter::Static::GetAllPresentationDisplays(
    /* [out, callee] */ ArrayOf<IDisplay*>** result)
{
    VALIDATE_NOT_NULL(result);
    return mDisplayService->GetDisplays(IDisplayManager::DISPLAY_CATEGORY_PRESENTATION, result);
}

ECode CMediaRouter::Static::StartMonitoringRoutes(
    /* [in] */ IContext* appContext)
{
    CMediaRouterRouteInfo::New(mSystemCategory.Get(), mOwner, (IMediaRouterRouteInfo**)&mDefaultAudioVideo);

    String str = StringUtils::ToString(R::string::default_audio_route_name);

    ((MediaRouterRouteInfo*)mDefaultAudioVideo.Get())->SetName((StringUtils::ParseCharSequence(str)).Get());
    mDefaultAudioVideo->SetSupportedTypes((IMediaRouter::ROUTE_TYPE_LIVE_AUDIO | IMediaRouter::ROUTE_TYPE_LIVE_VIDEO));
    Boolean flag = FALSE;
    mDefaultAudioVideo->UpdatePresentationDisplay(&flag);
    AddRouteStatic((MediaRouterRouteInfo*)(mDefaultAudioVideo.Get()));

    // This will select the active wifi display route if there is one.
    AutoPtr<IWifiDisplayStatus> wifiDisplayStatus;
    mDisplayService->GetWifiDisplayStatus((IWifiDisplayStatus**)&wifiDisplayStatus);
    UpdateWifiDisplayStatus(wifiDisplayStatus.Get());

   AutoPtr<WifiDisplayStatusChangedReceiver> wifiDisplayStatusChangedReceiver
       = new WifiDisplayStatusChangedReceiver();//

   AutoPtr<IIntentFilter> intentFilter1;
   CIntentFilter::New(IDisplayManager::ACTION_WIFI_DISPLAY_STATUS_CHANGED,
       (IIntentFilter**)&intentFilter1);
   AutoPtr<IIntent> intent;
   appContext->RegisterReceiver(IBroadcastReceiver::Probe(wifiDisplayStatusChangedReceiver), intentFilter1.Get(), (IIntent**)&intent);
   AutoPtr<VolumeChangeReceiver> volumeChangeReceiver = new VolumeChangeReceiver();//

   AutoPtr<IIntentFilter> intentFilter2;
   CIntentFilter::New(IAudioManager::VOLUME_CHANGED_ACTION, (IIntentFilter**)&intentFilter2);
   intent = NULL;
   appContext->RegisterReceiver(IBroadcastReceiver::Probe(volumeChangeReceiver), intentFilter2.Get(), (IIntent**)&intent);//

   mDisplayService->RegisterDisplayListener(this, mHandler);//

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
   }//

   // Bind to the media router service.
   RebindAsUser(UserHandle::GetMyUserId());//

   // Select the default route if the above didn't sync us up
   // appropriately with relevant system state.
   if (mSelectedRoute == NULL) {
      SelectDefaultRouteStatic();
   }
   return NOERROR;
}

ECode CMediaRouter::Static::UpdateAudioRoutes(
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
        ((MediaRouterRouteInfo*)(((Static*)(sStatic.Get()))->mDefaultAudioVideo).Get())->mNameResId = name;
        DispatchRouteChanged((((Static*)(sStatic.Get()))->mDefaultAudioVideo).Get());
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
            if (((Static*)(sStatic.Get()))->mBluetoothA2dpRoute == NULL) {
                AutoPtr<IMediaRouterUserRouteInfo> info;
                CMediaRouterUserRouteInfo::New((((Static*)(sStatic.Get()))->mSystemCategory).Get(), (IMediaRouterUserRouteInfo**)&info);
                AutoPtr<ICharSequence> cs;
                ((Static*)(sStatic.Get()))->mResources->GetText(R::string::bluetooth_a2dp_audio_route_name, (ICharSequence**)&cs);
                info->SetDescription(cs);
                info->SetName(charSequence1);
                IMediaRouterRouteInfo::Probe(info)->SetSupportedTypes(IMediaRouter::ROUTE_TYPE_LIVE_AUDIO);
                ((Static*)(sStatic.Get()))->mBluetoothA2dpRoute = IMediaRouterRouteInfo::Probe(info);
                AddRouteStatic((MediaRouterRouteInfo*)(((Static*)(sStatic.Get()))->mBluetoothA2dpRoute).Get());
            } else {
                ((Static*)(sStatic.Get()))->mBluetoothA2dpRoute->SetName(charSequence2);
                DispatchRouteChanged((MediaRouterRouteInfo*)(((Static*)(sStatic.Get()))->mBluetoothA2dpRoute).Get());
            }
        } else if (((Static*)(sStatic.Get()))->mBluetoothA2dpRoute != NULL) {
              RemoveRouteStatic((MediaRouterRouteInfo*)(((Static*)(sStatic.Get()))->mBluetoothA2dpRoute).Get());
              ((Static*)(sStatic.Get()))->mBluetoothA2dpRoute = NULL;
        }
    }

    if (mBluetoothA2dpRoute != NULL) {
          Boolean a2dpEnabled;
          IsBluetoothA2dpOn(&a2dpEnabled);
          if (mainType != CAudioRoutesInfo::MAIN_SPEAKER &&
                  mSelectedRoute == mBluetoothA2dpRoute && !a2dpEnabled) {
              SelectRouteStatic(ROUTE_TYPE_LIVE_AUDIO, (MediaRouterRouteInfo*)mDefaultAudioVideo.Get(), FALSE);
          } else if ((mSelectedRoute == mDefaultAudioVideo || mSelectedRoute == NULL) &&
                  a2dpEnabled) {
              SelectRouteStatic(ROUTE_TYPE_LIVE_AUDIO, (MediaRouterRouteInfo*)mBluetoothA2dpRoute.Get(), FALSE);
          }
    }
    return NOERROR;
}

ECode CMediaRouter::Static::IsBluetoothA2dpOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // try {
    ECode ec = NOERROR;
    Boolean flag = FALSE;
    ec = mAudioService->IsBluetoothA2dpOn(&flag);
    // } catch (RemoteException e) {
    if (SUCCEEDED(ec)) {
        *result = flag;
        return NOERROR;
    }

    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "Error querying Bluetooth A2DP state");
        *result = FALSE;
        return NOERROR;
    }
        return NOERROR;
    // }
}

ECode CMediaRouter::Static::UpdateDiscoveryRequest()
{
    // What are we looking for today?
    Int32 routeTypes = 0;
    Int32 passiveRouteTypes = 0;
    Boolean activeScan = FALSE;
    Boolean activeScanWifiDisplay = FALSE;

    List< AutoPtr<CallbackInfo> >::Iterator it;
    AutoPtr<CallbackInfo> cbi;
    for (it = mCallbacks.Begin(); it != mCallbacks.End(); ++it) {
       cbi = NULL;
       cbi = *it;
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
    }//

    // Update wifi display scanning.
    // TODO: All of this should be managed by the media router service.
    if (mCanConfigureWifiDisplays) {
       Boolean bmathes = FALSE;
       mSelectedRoute->MatchesTypes(ROUTE_TYPE_REMOTE_DISPLAY, &bmathes);
       if (mSelectedRoute != NULL && bmathes) {
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
    }//

    // Tell the media router service all about it.
    if (routeTypes != mDiscoveryRequestRouteTypes
           || activeScan != mDiscoverRequestActiveScan) {
       mDiscoveryRequestRouteTypes = routeTypes;
       mDiscoverRequestActiveScan = activeScan;
       PublishClientDiscoveryRequest();
    }
    return NOERROR;
}

ECode CMediaRouter::Static::SetSelectedRoute(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ Boolean bexplicit)
{
    // Must be non-reentrant.
    mSelectedRoute = info;
    return PublishClientSelectedRoute(bexplicit);
}

ECode CMediaRouter::Static::RebindAsUser(
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
        AutoPtr<MyStaticClient> client = new MyStaticClient(this);
        String packageName;
        mAppContext->GetPackageName(&packageName);
        ECode ec = mMediaRouterService->RegisterClientAsUser(IIMediaRouterClient::Probe(client), packageName, userId);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(TAG, "Unable to register media router client.");
        }
        mClient = IIMediaRouterClient::Probe(client);
        // } catch (RemoteException ex) {
            // Log.e(TAG, "Unable to register media router client.", ex);
        // }

        PublishClientDiscoveryRequest();
        PublishClientSelectedRoute(FALSE);
        UpdateClientState();
    }
    return NOERROR;
}

ECode CMediaRouter::Static::PublishClientDiscoveryRequest()
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
    return NOERROR;
}

ECode CMediaRouter::Static::PublishClientSelectedRoute(
      /* [in] */ Boolean bexplicit)
{
    if (mClient != NULL) {
    // try {
    ECode ec = mMediaRouterService->SetSelectedRoute(mClient,
        mSelectedRoute != NULL ? ((MediaRouterRouteInfo*)(mSelectedRoute.Get()))->mGlobalRouteId : String(NULL), bexplicit);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Slogger::E(TAG, "Unable to publish media router client selected route.");
    }
    // } catch (RemoteException ex) {
        // Log.e(TAG, "Unable to publish media router client selected route.", ex);
    // }
    }
    return NOERROR;
}

ECode CMediaRouter::Static::UpdateClientState()
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
        mClientState != NULL ? ((MediaRouterClientState*)mClientState.Get())->mRoutes : NULL;
    String globallySelectedRouteId = mClientState != NULL ?
            ((MediaRouterClientState*)mClientState.Get())->mGloballySelectedRouteId : String(NULL);

    // Add or update routes.
    Int32 size;
    Int32 globalRouteCount = globalRoutes != NULL ? (globalRoutes->GetSize(&size), size) : 0;
    AutoPtr<IInterface> obj;
    AutoPtr<MediaRouterClientState::RouteInfo> globalRoute;
    AutoPtr<IMediaRouterRouteInfo> route;
    for (Int32 i = 0; i < globalRouteCount; i++) {
        obj = NULL;
        globalRoutes->Get(i, (IInterface**)&obj);
        globalRoute = NULL;
        globalRoute = (MediaRouterClientState::RouteInfo*)IMediaRouterClientStateRouteInfo::Probe(obj);
        route = NULL;
        FindGlobalRoute(globalRoute->mId, (IMediaRouterRouteInfo**)&route);
        if (route == NULL) {
            MakeGlobalRoute(IMediaRouterClientStateRouteInfo::Probe(globalRoute), (IMediaRouterRouteInfo**)&route);
            AddRouteStatic(route.Get());
        } else {
            UpdateGlobalRoute(route, IMediaRouterClientStateRouteInfo::Probe(globalRoute));
        }
    }

    // Synchronize state with the globally selected route.
    if (!globallySelectedRouteId.IsNull()) {
        FindGlobalRoute(globallySelectedRouteId, (IMediaRouterRouteInfo**)&route);
        if (route == NULL) {
            Slogger::W(TAG, "Could not find new globally selected route: %s"
                    , globallySelectedRouteId.string());
        } else if (route != mSelectedRoute) {
            if (DEBUG) {
                Slogger::D(TAG, "Selecting new globally selected route: %p", route.Get());
            }
            SelectRouteStatic(((MediaRouterRouteInfo*)(route.Get()))->mSupportedTypes, route.Get(), FALSE);
        }
    } else if (mSelectedRoute != NULL && !(((MediaRouterRouteInfo*)mSelectedRoute.Get())->mGlobalRouteId).IsNull()) {
        if (DEBUG) {
            Logger::D(TAG, "Unselecting previous globally selected route: %p" , mSelectedRoute.Get());
        }
        SelectDefaultRouteStatic();
    }
    return NOERROR;

    // Remove defunct routes.
    OUTER:
    {
        List<AutoPtr<IMediaRouterRouteInfo> >::Iterator it = mRoutes.Begin();
        AutoPtr<IMediaRouterRouteInfo> _route_;
        AutoPtr<MediaRouterRouteInfo> route_;
        AutoPtr<IInterface> obj_;
        AutoPtr<IMediaRouterClientStateRouteInfo> mcs;
        AutoPtr<MediaRouterClientState::RouteInfo> globalRoute_;
        for (; it != mRoutes.End(); ++it) {
            route_ = NULL;
            _route_ = *it;
            route_ = (MediaRouterRouteInfo*)(_route_.Get());
            String globalRouteId = route_->mGlobalRouteId;
            if (!globalRouteId.IsNull()) {
                for (Int32 j = 0; j < globalRouteCount; ++j) {
                    obj_ = NULL;
                    globalRoutes->Get(j, (IInterface**)&obj_);
                    mcs = NULL;
                    mcs = IMediaRouterClientStateRouteInfo::Probe(obj_);
                    globalRoute_ = NULL;
                    globalRoute_ = (MediaRouterClientState::RouteInfo*)(mcs.Get());
                    if (globalRouteId.Equals(globalRoute_->mId)) {
                        goto OUTER; //found
                    }
                }
                // notfound
                RemoveRouteStatic(route_.Get());
            }
        }
    }
    return NOERROR;
}

void CMediaRouter::Static::UpdatePresentationDisplays(
    /* [in] */ Int32 changedDisplayId)
{
    List<AutoPtr<IMediaRouterRouteInfo> >::Iterator it;
    AutoPtr<IMediaRouterRouteInfo> route_;
    AutoPtr<MediaRouterRouteInfo> route;
    Boolean updated = FALSE;
    Int32 displayId;
    for (it = mRoutes.Begin(); it != mRoutes.End(); ++it) {
        route_ = NULL;
        route_ = *it;
        route_->UpdatePresentationDisplay(&updated);
        route = NULL;
        route = (MediaRouterRouteInfo*)(route_.Get());
        route->mPresentationDisplay->GetDisplayId(&displayId);
        if (updated || (route->mPresentationDisplay != NULL
                && displayId == changedDisplayId)) {
            DispatchRoutePresentationDisplayChanged(route_);
        }
    }
}

ECode CMediaRouter::Static::FindGlobalRoute(
    /* [in] */ const String& globalRouteId,
    /* [out] */ IMediaRouterRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);

    List< AutoPtr<IMediaRouterRouteInfo> >::Iterator it = mRoutes.Begin();
    AutoPtr<IMediaRouterRouteInfo> route_;
    AutoPtr<MediaRouterRouteInfo> route;
    for (; it != mRoutes.End(); ++it) {
        route_ = NULL;
        route_ = *it;
        route = (MediaRouterRouteInfo*)(route_.Get());
        if (globalRouteId.Equals(route->mGlobalRouteId)) {
            *result = route_;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode CMediaRouter::Static::RequestUpdateVolume(
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ Int32 direction)
{
    AutoPtr<MediaRouterRouteInfo> route_ = (MediaRouterRouteInfo*)route;
    if (!(route_->mGlobalRouteId).IsNull() && mClient != NULL) {
        // try {
        ECode ec = mMediaRouterService->RequestUpdateVolume(mClient.Get(),
                    route_->mGlobalRouteId, direction);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::W(TAG, "Unable to request volume change.");
            return E_REMOTE_EXCEPTION;
        }

        if (SUCCEEDED(ec)) {
          return ec;
        }
        // } catch (RemoteException ex) {
            // Log.w(TAG, "Unable to request volume change.", ex);
        // }
    }
    return NOERROR;
}

ECode CMediaRouter::Static::RequestSetVolume(
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ Int32 volume)
{
    AutoPtr<MediaRouterRouteInfo> route_ = (MediaRouterRouteInfo*)route;
    if (!(route_->mGlobalRouteId).IsNull() && mClient != NULL) {
        // try {
        ECode ec = mMediaRouterService->RequestUpdateVolume(mClient.Get(),
                    route_->mGlobalRouteId, volume);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::W(TAG, "Unable to request volume change.");
            return E_REMOTE_EXCEPTION;
        }

        if (SUCCEEDED(ec)) {
          return ec;
        }
        // } catch (RemoteException ex) {
            // Log.w(TAG, "Unable to request volume change.", ex);
        // }
    }
    return NOERROR;
}

ECode CMediaRouter::Static::MakeGlobalRoute(
    /* [in] */ IMediaRouterClientStateRouteInfo* globalRoute_,
    /* [out] */ IMediaRouterRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<CMediaRouterRouteInfo> route;
    CMediaRouterRouteInfo::NewByFriend((((Static*)(sStatic.Get()))->mSystemCategory).Get(),
           mOwner, (CMediaRouterRouteInfo**)&route);
    AutoPtr<MediaRouterClientState::RouteInfo> globalRoute = (MediaRouterClientState::RouteInfo*)globalRoute_;
    route->mGlobalRouteId = globalRoute->mId;
    AutoPtr<ICharSequence> cs =
       StringUtils::ParseCharSequence(globalRoute->mName);
    route->SetName(cs.Get());
    cs = NULL;
    cs = StringUtils::ParseCharSequence(globalRoute->mDescription);
    route->mDescription = cs.Get();
    route->mSupportedTypes = globalRoute->mSupportedTypes;
    route->mEnabled = globalRoute->mEnabled;
    Boolean flag = FALSE;
    route->SetRealStatusCode(globalRoute->mStatusCode, &flag);
    route->mPlaybackType = globalRoute->mPlaybackType;
    route->mPlaybackStream = globalRoute->mPlaybackStream;
    route->mVolume = globalRoute->mVolume;
    route->mVolumeMax = globalRoute->mVolumeMax;
    route->mVolumeHandling = globalRoute->mVolumeHandling;
    route->mPresentationDisplayId = globalRoute->mPresentationDisplayId;
    route->UpdatePresentationDisplay(&flag);
    *result = IMediaRouterRouteInfo::Probe(route);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::Static::UpdateGlobalRoute(
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ IMediaRouterClientStateRouteInfo* globalRoute)
{
    Boolean changed = FALSE;
    Boolean volumeChanged = FALSE;
    Boolean presentationDisplayChanged = FALSE;

    AutoPtr<MediaRouterRouteInfo> route_ = (MediaRouterRouteInfo*)route;
    AutoPtr<MediaRouterClientState::RouteInfo> globalRoute_ = (MediaRouterClientState::RouteInfo*)globalRoute;

    AutoPtr<ICharSequence> cs =
        StringUtils::ParseCharSequence(globalRoute_->mName);

    if (!Object::Equals((route_->mName).Get(), cs.Get())) {
        route_->mName = cs.Get();
        changed = TRUE;
    }
    cs = NULL;
    cs = StringUtils::ParseCharSequence(globalRoute_->mDescription);
    if (!Object::Equals((route_->mDescription).Get(), cs.Get())) {
        route_->mDescription = cs.Get();
        changed = TRUE;
    }
    Int32 oldSupportedTypes = route_->mSupportedTypes;
    if (oldSupportedTypes != globalRoute_->mSupportedTypes) {
        route_->mSupportedTypes = globalRoute_->mSupportedTypes;
        changed = TRUE;
    }
    if (route_->mEnabled != globalRoute_->mEnabled) {
        route_->mEnabled = globalRoute_->mEnabled;
        changed = TRUE;
    }
    if (route_->mRealStatusCode != globalRoute_->mStatusCode) {
        Boolean flag = FALSE;
        route_->SetRealStatusCode(globalRoute_->mStatusCode, &flag);
        changed = TRUE;
    }
    if (route_->mPlaybackType != globalRoute_->mPlaybackType) {
        route_->mPlaybackType = globalRoute_->mPlaybackType;
        changed = TRUE;
    }
    if (route_->mPlaybackStream != globalRoute_->mPlaybackStream) {
        route_->mPlaybackStream = globalRoute_->mPlaybackStream;
        changed = TRUE;
    }
    if (route_->mVolume != globalRoute_->mVolume) {
        route_->mVolume = globalRoute_->mVolume;
        changed = TRUE;
        volumeChanged = TRUE;
    }
    if (route_->mVolumeMax != globalRoute_->mVolumeMax) {
        route_->mVolumeMax = globalRoute_->mVolumeMax;
        changed = TRUE;
        volumeChanged = TRUE;
    }
    if (route_->mVolumeHandling != globalRoute_->mVolumeHandling) {
        route_->mVolumeHandling = globalRoute_->mVolumeHandling;
        changed = TRUE;
        volumeChanged = TRUE;
    }
    if (route_->mPresentationDisplayId != globalRoute_->mPresentationDisplayId) {
        route_->mPresentationDisplayId = globalRoute_->mPresentationDisplayId;
        Boolean flag = FALSE;
        route_->UpdatePresentationDisplay(&flag);
        changed = TRUE;
        presentationDisplayChanged = TRUE;
    }

    if (changed) {
        DispatchRouteChanged(route_.Get(), oldSupportedTypes);
    }
    if (volumeChanged) {
        DispatchRouteVolumeChanged(route_.Get());
    }
    if (presentationDisplayChanged) {
        DispatchRoutePresentationDisplayChanged(route_.Get());
    }
    return NOERROR;
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
    /* [in] */ Boolean groupable,
    /* [in] */ IMediaRouter* host)
{
    mName = name;
    mTypes = types;
    mGroupable = groupable;
    mHost = (CMediaRouter*)host;
    return NOERROR;
}

ECode CMediaRouter::RouteCategory::constructor(
    /* [in] */ Int32 nameResId,
    /* [in] */ Int32 types,
    /* [in] */ Boolean groupable,
    /* [in] */ IMediaRouter* host)
{
    mNameResId = nameResId;
    mTypes = types;
    mGroupable = groupable;
    mHost = (CMediaRouter*)host;
    return NOERROR;
}

ECode CMediaRouter::RouteCategory::GetName(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    return GetName(((Static*)(sStatic.Get()))->mResources, result);
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
    }
    else {
       outList->Clear();
    }

    Int32 count;
    mHost->GetRouteCountStatic(&count);
    AutoPtr<IMediaRouterRouteInfo> route;
    for (Int32 i = 0; i < count; i++) {
        route = NULL;
        mHost->GetRouteAtStatic(i, (IMediaRouterRouteInfo**)&route);

       if ((((MediaRouterRouteInfo*)(route.Get()))->mCategory.Get()) == IMediaRouterRouteCategory::Probe(this)) {
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
    StringBuilder sb("RouteCategory{ name=");
    sb += Object::ToString(mName);
    sb += " types=";
    sb += TypesToString(mTypes);
    sb += " groupable=";
    sb += mGroupable;
    sb += "}";
    *result = sb.ToString();
    return NOERROR;
}

//================================================================================
//                      CMediaRouter::CallbackInfo
//================================================================================
CMediaRouter::CallbackInfo::CallbackInfo(
    /* [in] */ IMediaRouterCallback* cb,
    /* [in] */ Int32 type,
    /* [in] */ Int32 flags,
    /* [in] */ IMediaRouter* router)
    : mType(type)
    , mFlags(flags)
    , mCb(cb)
{
    mRouter = (CMediaRouter*)router;
}

CMediaRouter::CallbackInfo::~CallbackInfo()
{}

CAR_INTERFACE_IMPL(CMediaRouter::CallbackInfo, Object, IMediaRouterRouteCallbackInfo)//

ECode CMediaRouter::CallbackInfo::FilterRouteEvent(
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<MediaRouterRouteInfo> r = (MediaRouterRouteInfo*)route;
    return FilterRouteEvent(r->mSupportedTypes, result);
}

ECode CMediaRouter::CallbackInfo::FilterRouteEvent(
    /* [in] */ Int32 supportedTypes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mFlags & IMediaRouter::CALLBACK_FLAG_UNFILTERED_EVENTS) != 0
           || (mType & supportedTypes) != 0;
    return NOERROR;
}

//================================================================================
//                      CMediaRouter::VolumeCallbackInfo
//================================================================================
CAR_INTERFACE_IMPL(CMediaRouter::VolumeCallbackInfo, Object, IMediaRouterVolumeCallbackInfo)

CMediaRouter::VolumeCallbackInfo::VolumeCallbackInfo()
{}

CMediaRouter::VolumeCallbackInfo::~VolumeCallbackInfo()
{}

ECode CMediaRouter::VolumeCallbackInfo::constructor(
    /* [in] */ IMediaRouterVolumeCallback* vcb,
    /* [in] */ IMediaRouterRouteInfo* route)
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
    /* [in] */ IMediaRouterRouteInfo* route)
{
    mRoute = route;
    return NOERROR;
}

ECode CMediaRouter::VolumeCallbackInfo::GetRouteInfo(
    /* [out] */ IMediaRouterRouteInfo** route)
{
    VALIDATE_NOT_NULL(route);
    *route = mRoute;
    REFCOUNT_ADD(*route);
    return NOERROR;
}

//================================================================================
//                      CMediaRouter::VolumeChangeReceiver
//================================================================================
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
       }//

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

//================================================================================
//                      CMediaRouter
//================================================================================

CMediaRouter::CMediaRouter()
{}

CMediaRouter::~CMediaRouter()
{}

CAR_INTERFACE_IMPL(CMediaRouter, Object, IMediaRouter)

ECode CMediaRouter::constructor(
    /* [in] */ IContext* context)
{
    {
      AutoLock lock(mStaticClass);
      if (sStatic == NULL) {
          AutoPtr<IContext> appContext;
          context->GetApplicationContext((IContext**)&appContext);
          sStatic = new Static(appContext, this);
          ((Static*)(sStatic.Get()))->StartMonitoringRoutes(appContext);
      }
    }
    return NOERROR;
}

ECode CMediaRouter::GetSystemAudioRoute(
    /* [out] */ IMediaRouterRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);

    *result = ((Static*)(sStatic.Get()))->mDefaultAudioVideo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::GetSystemAudioCategory(
    /* [out] */ IMediaRouterRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);

    *result = ((Static*)(sStatic.Get()))->mSystemCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::GetSelectedRoute(
    /* [in] */ Int32 type,
    /* [out] */ IMediaRouterRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);
    Static* s = (Static*)sStatic.Get();
    Int32 temp;
    if (s->mSelectedRoute != NULL && (s->mSelectedRoute->GetSupportedTypes(&temp) & type) != 0) {
        // If the selected route supports any of the types supplied, it's still considered
        // 'selected' for that type.
        *result = s->mSelectedRoute;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    else if (type == ROUTE_TYPE_USER) {
        // The caller specifically asked for a user route and the currently selected route
        // doesn't qualify.
        *result = NULL;
        return NOERROR;
    }

    // If the above didn't match and we're not specifically asking for a user route,
    // consider the default selected.
    *result = s->mDefaultAudioVideo;//
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::AddCallback(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterCallback* cb)
{
    return AddCallback(types, cb, 0);
}

ECode CMediaRouter::AddCallback(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterCallback* cb,
    /* [in] */ Int32 flags)
{
    AutoPtr<CallbackInfo> info;
    Int32 index = FindCallbackInfo(cb);
    if (index >= 0) {
        info = NULL;
        info = (((Static*)(sStatic.Get()))->mCallbacks)[index];
        info->mType |= types;
        info->mFlags |= flags;
    } else {
        info = NULL;
        info = new CallbackInfo(cb, types, flags, IMediaRouter::Probe(this));
        (((Static*)(sStatic.Get()))->mCallbacks).PushBack(info);
    }
    return sStatic->UpdateDiscoveryRequest();
}

Int32 CMediaRouter::FindCallbackInfo(
    /* [in] */ IMediaRouterCallback* cb)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mCallbacks.Begin();
    Int32 i = 0;
    for (; it != ((Static*)(sStatic.Get()))->mCallbacks.End(); ++it) {
       if (cb == ((*it)->mCb).Get()) {
           return i;
       }
       ++i;
    }
    return -1;
}

ECode CMediaRouter::RemoveCallback(
    /* [in] */ IMediaRouterCallback* cb)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = (((Static*)(sStatic.Get()))->mCallbacks).Begin();
    for (; it != ((Static*)(sStatic.Get()))->mCallbacks.End(); ++it) {
       if (cb == ((*it)->mCb).Get()) {
           ((Static*)(sStatic.Get()))->mCallbacks.Erase(it);
           return sStatic->UpdateDiscoveryRequest();
       }
    }

    if (it == (((Static*)(sStatic.Get()))->mCallbacks).End()) {
      Logger::W(TAG, "removeCallback(" /*+ cb +*/ "): callback not registered");
      return NOERROR;
    }
      return NOERROR;
}

ECode CMediaRouter::SelectRoute(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterRouteInfo* route)
{
    return SelectRouteStatic(types, route, TRUE);
}

ECode CMediaRouter::SelectRouteInt(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ Boolean isExplicit)
{
    return SelectRouteStatic(types, route, isExplicit);
}

ECode CMediaRouter::AddUserRoute(
    /* [in] */ IMediaRouterUserRouteInfo* info)
{
    return AddRouteStatic((IMediaRouterRouteInfo*)info);
}

ECode CMediaRouter::AddRouteInt(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return AddRouteStatic(info);
}

ECode CMediaRouter::RemoveUserRoute(
    /* [in] */ IMediaRouterUserRouteInfo* info)
{
    return RemoveRouteStatic(IMediaRouterRouteInfo::Probe(info));
}

ECode CMediaRouter::ClearUserRoutes()
{
    AutoPtr<IMediaRouterRouteInfo> info;
    for (Int32 i = 0; i < (((Static*)(sStatic.Get()))->mRoutes).GetSize(); i++) {
        info = NULL;
        info = ((Static*)(sStatic.Get()))->mRoutes[i];
        // TODO Right now, RouteGroups only ever contain user routes.
        // The code below will need to change if this assumption does.
        if ((IMediaRouterUserRouteInfo::Probe(info) != NULL) || (IMediaRouterRouteGroup::Probe(info) != NULL)) {
           RemoveRouteStatic(info.Get());// sStatic->mRoutes is changed in this function!!!
           i--;
        }
    }
    return NOERROR;
}

ECode CMediaRouter::RemoveRouteInt32(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return RemoveRouteStatic(info);
}

ECode CMediaRouter::GetCategoryCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((Static*)(sStatic.Get()))->mCategories.GetSize();
    return NOERROR;
}

ECode CMediaRouter::GetCategoryAt(
    /* [in] */ Int32 index,
    /* [out] */ IMediaRouterRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((Static*)(sStatic.Get()))->mCategories[index];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::GetRouteCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((Static*)(sStatic.Get()))->mRoutes.GetSize();
    return NOERROR;
}

ECode CMediaRouter::GetRouteAt(
    /* [in] */ Int32 index,
    /* [out] */ IMediaRouterRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);//

    *result = ((Static*)(sStatic.Get()))->mRoutes[index];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::CreateUserRoute(
    /* [in] */ IMediaRouterRouteCategory* category,
    /* [out] */ IMediaRouterUserRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IMediaRouterUserRouteInfo> userRouteInfo;
    CMediaRouterUserRouteInfo::New(category, (IMediaRouterUserRouteInfo**)&userRouteInfo);
    *result = userRouteInfo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::CreateRouteCategory(
    /* [in] */ ICharSequence* name,
    /* [in] */ Boolean isGroupable,
    /* [out] */ IMediaRouterRouteCategory** result)
{
    VALIDATE_NOT_NULL(result);//

    AutoPtr<IMediaRouterRouteCategory> routeCategory;
    CRouteCategory::New(name, ROUTE_TYPE_USER, isGroupable, IMediaRouter::Probe(this), (IMediaRouterRouteCategory**)&routeCategory);
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
    CRouteCategory::New(nameResId, ROUTE_TYPE_USER, isGroupable, IMediaRouter::Probe(this), (IMediaRouterRouteCategory**)&routeCategory);
    *result = routeCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::GetDefaultRoute(
    /* [out] */ IMediaRouterRouteInfo ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((Static*)(sStatic.Get()))->mDefaultAudioVideo;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::GetSystemCategory(
    /* [out] */ IMediaRouterRouteCategory ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = ((Static*)(sStatic.Get()))->mSystemCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::GetSelectedRoute(
    /* [out] */ IMediaRouterRouteInfo ** result)
{
    VALIDATE_NOT_NULL(result);
    return GetSelectedRoute(ROUTE_TYPE_ANY, result);
}

ECode CMediaRouter::IsRouteAvailable(
    /* [in] */ Int32 types,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count = (((Static*)(sStatic.Get()))->mRoutes).GetSize();
    AutoPtr<IMediaRouterRouteInfo> route;
    Boolean flag = FALSE;
    for (Int32 i = 0; i < count; i++) {
      route = NULL;
      route = (((Static*)(sStatic.Get()))->mRoutes)[i];
      route->MatchesTypes(types, &flag);
      if (flag) {
        if ((flags & AVAILABILITY_FLAG_IGNORE_DEFAULT_ROUTE) == 0
          || route != ((Static*)(sStatic.Get()))->mDefaultAudioVideo) {
          *result = TRUE;
          return NOERROR;
        }
      }
    }

    // It doesn't look like we can find a matching route right now.
    *result = FALSE;
    return NOERROR;
}

ECode CMediaRouter::RebindAsUser(
    /* [in] */ Int32 userId)
{
    return sStatic->RebindAsUser(userId);
}

String CMediaRouter::TypesToString(
    /* [in] */ Int32 types)
{
    StringBuilder result;

    if ((types & ROUTE_TYPE_LIVE_AUDIO) != 0) {
       result += String("ROUTE_TYPE_LIVE_AUDIO ");
    }
    if ((types & ROUTE_TYPE_LIVE_VIDEO) != 0) {
       result += String("ROUTE_TYPE_LIVE_VIDEO ");
    }
    if ((types & ROUTE_TYPE_USER) != 0) {
       result += String("ROUTE_TYPE_USER ");
    }
    String tempText;
    result.ToString(&tempText);
    return tempText;
}

ECode CMediaRouter::SelectRouteStatic(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ Boolean isexplicit)
{
    AutoPtr<IMediaRouterRouteInfo> oldRoute = ((Static*)(sStatic.Get()))->mSelectedRoute;
    if (route == oldRoute) {
       return NOERROR;
    }
    Int32 tempValue;
    route->GetSupportedTypes(&tempValue);
    if ((tempValue & types) == 0) {
       Logger::W(TAG, String("selectRoute ignored; cannot select route with supported types ") +
               TypesToString(tempValue) + String(" into route types ") +
               TypesToString(types));
       return NOERROR;
    }//

    AutoPtr<IMediaRouterRouteInfo> btRoute = ((Static*)(sStatic.Get()))->mBluetoothA2dpRoute;
    if (btRoute != NULL && (types & ROUTE_TYPE_LIVE_AUDIO) != 0 &&
           (route == btRoute || route == ((Static*)(sStatic.Get()))->mDefaultAudioVideo)) {
       ECode ec;
    //        try {
           ec = ((Static*)(sStatic.Get()))->mAudioService->SetBluetoothA2dpOn(route == btRoute);
    //        } catch (RemoteException e) {
           if (FAILED(ec)) {
               Logger::E(TAG, "Error changing Bluetooth A2DP state"/*, e*/);
           }
    //        }
    }//

    AutoPtr<IWifiDisplay> activeDisplay;
    AutoPtr<IWifiDisplayStatus> wifiDisplayStatus;
    ((Static*)(sStatic.Get()))->mDisplayService->GetWifiDisplayStatus((IWifiDisplayStatus**)&wifiDisplayStatus);
    wifiDisplayStatus->GetActiveDisplay((IWifiDisplay**)&activeDisplay);//

    String tempText1, tempText2;
    Boolean oldRouteHasAddress = oldRoute != NULL && (oldRoute->GetDeviceAddress(&tempText1), tempText1) != NULL;
    Boolean newRouteHasAddress = route != NULL && (route->GetDeviceAddress(&tempText2), tempText2) != NULL;//

    if (activeDisplay != NULL || oldRouteHasAddress || newRouteHasAddress) {
        Boolean flag = FALSE;
        MatchesDeviceAddress(activeDisplay.Get(), route, &flag);

       if (newRouteHasAddress && !flag) {
           String tempText;
           route->GetDeviceAddress(&tempText);
           ((Static*)(sStatic.Get()))->mDisplayService->ConnectWifiDisplay(tempText);
       }
       else if (activeDisplay != NULL && !newRouteHasAddress) {
           ((Static*)(sStatic.Get()))->mDisplayService->DisconnectWifiDisplay();
       }
    }//

    if (oldRoute != NULL) {
       // TODO filter types properly
       Int32 tempValue;
       oldRoute->GetSupportedTypes(&tempValue);
       DispatchRouteUnselected(types & tempValue, oldRoute);
    }
    ((Static*)(sStatic.Get()))->mSelectedRoute = route;
    if (route != NULL) {
       // TODO filter types properly
       Int32 tempValue;
       route->GetSupportedTypes(&tempValue);
       DispatchRouteSelected(types & tempValue, route);
    }
    return NOERROR;
}

ECode CMediaRouter::SelectDefaultRouteStatic()
{
    // TODO: Be smarter about the route types here; this selects for all valid.
    Boolean flag = FALSE;
    ((Static*)(sStatic.Get()))->IsBluetoothA2dpOn(&flag);
    if ((((Static*)(sStatic.Get()))->mSelectedRoute != ((Static*)(sStatic.Get()))->mBluetoothA2dpRoute)
           && ((Static*)(sStatic.Get()))->mBluetoothA2dpRoute != NULL && flag) {
       SelectRouteStatic(ROUTE_TYPE_ANY, ((Static*)(sStatic.Get()))->mBluetoothA2dpRoute, FALSE);
    } else {
       SelectRouteStatic(ROUTE_TYPE_ANY, ((Static*)(sStatic.Get()))->mDefaultAudioVideo, FALSE);
    }
    return NOERROR;
}

ECode CMediaRouter::MatchesDeviceAddress(
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [out] */ Boolean* result)
{
    String tempText1;
    Boolean routeHasAddress = info != NULL && (info->GetDeviceAddress(&tempText1), tempText1) != NULL;
    if (display == NULL && !routeHasAddress) {
       *result = TRUE;
       return NOERROR;
    }

    if (display != NULL && routeHasAddress) {
       String tempText2;
       display->GetDeviceAddress(&tempText1);
       info->GetDeviceAddress(&tempText2);
       *result = tempText1.Equals(tempText2);
       return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CMediaRouter::AddRouteStatic(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    if (info == NULL) return NOERROR;//

    AutoPtr<IMediaRouterRouteCategory> cat;
    info->GetCategory((IMediaRouterRouteCategory**)&cat);
    List<AutoPtr<IMediaRouterRouteCategory> >::Iterator it = Find(((Static*)(sStatic.Get()))->mCategories.Begin(), ((Static*)(sStatic.Get()))->mCategories.End(), cat);
    if (it == ((Static*)(sStatic.Get()))->mCategories.End()) {
       (((Static*)(sStatic.Get()))->mCategories).PushBack(cat);
    }//

    Boolean tempState;
    if ((cat->IsGroupable(&tempState), tempState) && !(IMediaRouterRouteGroup::Probe(info) != NULL)) {
       // Enforce that any added route in a groupable category must be in a group.
       AutoPtr<IMediaRouterRouteGroup> group;
       AutoPtr<IMediaRouterRouteCategory> routeCategory;
       info->GetCategory((IMediaRouterRouteCategory**)&routeCategory);
       CMediaRouterRouteGroup::New(routeCategory, (IMediaRouterRouteGroup**)&group);
       Int32 tempValue;
       info->GetSupportedTypes(&tempValue);
       ((IMediaRouterRouteInfo*)&group)->SetSupportedTypes(tempValue);
       (((Static*)(sStatic.Get()))->mRoutes).PushBack((MediaRouterRouteInfo*)(group.Get()));
       DispatchRouteAdded((IMediaRouterRouteInfo*)&group);
       group->AddRoute(info);

       info = (IMediaRouterRouteInfo*)&group;
    } else {
       (((Static*)(sStatic.Get()))->mRoutes).PushBack(info);
       DispatchRouteAdded(info);
    }
    return NOERROR;
}

ECode CMediaRouter::RemoveRouteStatic(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    List< AutoPtr<IMediaRouterRouteInfo> >::Iterator it_ = Find(((Static*)(sStatic.Get()))->mRoutes.Begin(), ((Static*)(sStatic.Get()))->mRoutes.End(), info);
    ((Static*)(sStatic.Get()))->mRoutes.Erase(it_);//

    AutoPtr<IMediaRouterRouteCategory> removingCat;
    info->GetCategory((IMediaRouterRouteCategory**)&removingCat);
    Boolean found = FALSE;
    List< AutoPtr<IMediaRouterRouteInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mRoutes.Begin();
    AutoPtr<IMediaRouterRouteCategory> cat;
    for (; it != (((Static*)(sStatic.Get()))->mRoutes).End(); ++it) {
       cat = NULL;
       (*it)->GetCategory((IMediaRouterRouteCategory**)&cat);
       if (removingCat == cat) {
           found = TRUE;
           break;
       }
    }//

    Boolean isSelected = FALSE;
    info->IsSelected(&isSelected);
    if (isSelected) {
       // Removing the currently selected route? Select the default before we remove it.
       SelectDefaultRouteStatic();
    }
    if (!found) {
       List< AutoPtr<IMediaRouterRouteCategory> >::Iterator it =
       Find((((Static*)(sStatic.Get()))->mCategories).Begin(), (((Static*)(sStatic.Get()))->mCategories).End(), removingCat);
       ((Static*)(sStatic.Get()))->mCategories.Erase(it);
    }

    DispatchRouteRemoved(info);
    return NOERROR;
}

ECode CMediaRouter::GetRouteCountStatic(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (((Static*)sStatic.Get())->mRoutes).GetSize();
    return NOERROR;
}

ECode CMediaRouter::GetRouteAtStatic(
    /* [in] */ Int32 index,
    /* [out] */ IMediaRouterRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IMediaRouterRouteInfo> r = (((Static*)(sStatic.Get()))->mRoutes)[index];
    *result = r.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CMediaRouter::UpdateRoute(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return DispatchRouteChanged(info);
}

ECode CMediaRouter::DispatchRouteSelected(
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    List<AutoPtr<CallbackInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mCallbacks.Begin();
    Boolean flag = FALSE;
    AutoPtr<CallbackInfo> cbi;
    for (; it != ((Static*)(sStatic.Get()))->mCallbacks.End(); ++it) {
        cbi = NULL;
        cbi = *it;
        cbi->FilterRouteEvent(info, &flag);
        if (flag) {
            cbi->mCb->OnRouteSelected(cbi->mRouter, type, info);
        }
    }
    return NOERROR;
}

ECode CMediaRouter::DispatchRouteUnselected(
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    List<AutoPtr<CallbackInfo> >::Iterator it =
        (((Static*)(sStatic.Get()))->mCallbacks).Begin();
    Boolean flag = FALSE;
    AutoPtr<CallbackInfo> cbi;
    for (; it != (((Static*)(sStatic.Get()))->mCallbacks).End(); ++it) {
        cbi = NULL;
        cbi = *it;
        cbi->FilterRouteEvent(info, &flag);
        if (flag) {
           cbi->mCb->OnRouteUnselected((IMediaRouter*)(cbi->mRouter), type, info);
       }
    }
    return NOERROR;
}

ECode CMediaRouter::DispatchRouteChanged(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ Int32 oldSupportedTypes)
{
    Int32 newSupportedTypes = ((MediaRouterRouteInfo*)info)->mSupportedTypes;//

    List<AutoPtr<CallbackInfo> >::Iterator it =
        (((Static*)(sStatic.Get()))->mCallbacks).Begin();
    Boolean oldVisibility = FALSE;
    Boolean newVisibility = FALSE;
    AutoPtr<CallbackInfo> cbi;
    Boolean flag = FALSE;
    for (; it != (((Static*)(sStatic.Get()))->mCallbacks).End(); ++it) {
       // Reconstruct some of the history for callbacks that may not have observed
       // all of the events needed to correctly interpret the current state.
       // FIXME: This is a strong signal that we should deprecate route type filtering
       // completely in the future because it can lead to inconsistencies in
       // applications.
       cbi = NULL;
       cbi = *it;
       cbi->FilterRouteEvent(oldSupportedTypes, &oldVisibility);
       cbi->FilterRouteEvent(newSupportedTypes, &newVisibility);
       info->IsSelected(&flag);
       if (!oldVisibility && newVisibility) {
           cbi->mCb->OnRouteAdded(cbi->mRouter, info);
           if (flag) {
               cbi->mCb->OnRouteSelected(cbi->mRouter, newSupportedTypes, info);
           }
       }
       if (oldVisibility || newVisibility) {
           cbi->mCb->OnRouteChanged(cbi->mRouter, info);
       }
       if (oldVisibility && !newVisibility) {
           if (flag) {
               cbi->mCb->OnRouteUnselected(cbi->mRouter, oldSupportedTypes, info);
           }
           cbi->mCb->OnRouteRemoved(cbi->mRouter, info);
       }
    }
    return NOERROR;
}

ECode CMediaRouter::DispatchRouteChanged(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return DispatchRouteChanged(info, ((MediaRouterRouteInfo*)info)->mSupportedTypes);
}

ECode CMediaRouter::DispatchRouteAdded(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mCallbacks.Begin();
    AutoPtr<CallbackInfo> cbi;
    Boolean flag = FALSE;
    for (; it != ((Static*)(sStatic.Get()))->mCallbacks.End(); ++it) {
       cbi = NULL;
       cbi = (*it);
       cbi->FilterRouteEvent(info, &flag);
       if (flag) {
           cbi->mCb->OnRouteAdded(cbi->mRouter, info);
       }
    }
    return NOERROR;
}

ECode CMediaRouter::DispatchRouteRemoved(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mCallbacks.Begin();
    AutoPtr<CallbackInfo> cbi;
    Boolean flag = FALSE;
    for (; it != ((Static*)(sStatic.Get()))->mCallbacks.End(); ++it) {
       cbi = NULL;
       cbi = (*it);
       cbi->FilterRouteEvent(info, &flag);
       if (flag) {
           cbi->mCb->OnRouteRemoved(cbi->mRouter, info);
       }
    }
    return NOERROR;
}

CMediaRouter::DispatchRouteGrouped(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group,
    /* [in] */ Int32 index)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mCallbacks.Begin();
    AutoPtr<CallbackInfo> cbi;
    Boolean flag = FALSE;
    for (; it != ((Static*)(sStatic.Get()))->mCallbacks.End(); ++it) {
       cbi = NULL;
       cbi = (*it);
       cbi->FilterRouteEvent(IMediaRouterRouteInfo::Probe(group), &flag);
       if (flag) {
           cbi->mCb->OnRouteGrouped(cbi->mRouter, info, group, index);
       }
    }
    return NOERROR;
}

ECode CMediaRouter::DispatchRouteUngrouped(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mCallbacks.Begin();
    AutoPtr<CallbackInfo> cbi;
    Boolean flag = FALSE;
    for (; it != ((Static*)(sStatic.Get()))->mCallbacks.End(); ++it) {
       cbi = NULL;
       cbi = (*it);
       cbi->FilterRouteEvent(IMediaRouterRouteInfo::Probe(group), &flag);
       if (flag) {
           cbi->mCb->OnRouteUngrouped(cbi->mRouter, info, group);
       }
    }
    return NOERROR;
}

ECode CMediaRouter::DispatchRouteVolumeChanged(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mCallbacks.Begin();
    AutoPtr<CallbackInfo> cbi;
    Boolean flag = FALSE;
    for (; it != ((Static*)(sStatic.Get()))->mCallbacks.End(); ++it) {
       cbi = NULL;
       cbi = (*it);
       cbi->FilterRouteEvent(info, &flag);
       if (flag) {
           cbi->mCb->OnRouteVolumeChanged(cbi->mRouter, info);
       }
    }
    return NOERROR;
}

ECode CMediaRouter::DispatchRoutePresentationDisplayChanged(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    List< AutoPtr<CallbackInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mCallbacks.Begin();
    AutoPtr<CallbackInfo> cbi;
    Boolean flag = FALSE;
    for (; it != ((Static*)(sStatic.Get()))->mCallbacks.End(); ++it) {
       cbi = NULL;
       cbi = (*it);
       cbi->FilterRouteEvent(info, &flag);
       if (flag) {
           cbi->mCb->OnRoutePresentationDisplayChanged(cbi->mRouter, info);
       }
    }
    return NOERROR;
}

ECode CMediaRouter::SystemVolumeChanged(
    /* [in] */ Int32 newValue)
{
    AutoPtr<IMediaRouterRouteInfo> selectedRoute = ((Static*)(sStatic.Get()))->mSelectedRoute;
    if (selectedRoute == NULL) {
       return NOERROR;
    }

    if (selectedRoute == ((Static*)(sStatic.Get()))->mBluetoothA2dpRoute ||
           selectedRoute == ((Static*)(sStatic.Get()))->mDefaultAudioVideo) {
       DispatchRouteVolumeChanged(selectedRoute);
    }
    else if (((Static*)(sStatic.Get()))->mBluetoothA2dpRoute != NULL) {
    //        try {
           Boolean tempState;
           DispatchRouteVolumeChanged((((Static*)(sStatic.Get()))->mAudioService->IsBluetoothA2dpOn(&tempState), tempState) ?
                   ((Static*)(sStatic.Get()))->mBluetoothA2dpRoute : ((Static*)(sStatic.Get()))->mDefaultAudioVideo);
    //        } catch (RemoteException e) {
           Logger::E(TAG, "Error checking Bluetooth A2DP state to report volume change"/*, e*/);
    //        }
    } else {
       DispatchRouteVolumeChanged(((Static*)(sStatic.Get()))->mDefaultAudioVideo);
    }
    return NOERROR;
}

ECode CMediaRouter::UpdateWifiDisplayStatus(
    /* [in] */ IWifiDisplayStatus* status)
{
    AutoPtr<ArrayOf<IWifiDisplay*> > displays;
    AutoPtr<IWifiDisplay> activeDisplay;
    Int32 fStatus;
    status->GetFeatureState(&fStatus);
    if (fStatus == IWifiDisplayStatus::FEATURE_STATE_ON) {
        status->GetDisplays((ArrayOf<IWifiDisplay*>**)&displays);
        status->GetActiveDisplay((IWifiDisplay**)&activeDisplay);

        // Only the system is able to connect to wifi display routes.
        // The display manager will enforce this with a permission check but it
        // still publishes information about all available displays.
        // Filter the list down to just the active display.
        if (!(((Static*)(sStatic.Get()))->mCanConfigureWifiDisplays)) {
            if (activeDisplay != NULL) {
              displays = NULL;
              displays = ArrayOf<IWifiDisplay*>::Alloc(1);
              (*displays)[0] = activeDisplay.Get();
            } else {
              displays = NULL;
              displays = WifiDisplay::EMPTY_ARRAY;
            }
        }
    } else {
        displays = WifiDisplay::EMPTY_ARRAY;
        activeDisplay = NULL;
    }
    String adAddress;
    activeDisplay->GetDeviceAddress(&adAddress);
    String activeDisplayAddress = activeDisplay != NULL ?
            adAddress : String(NULL);

    // Add or update routes.
    Int32 length = displays->GetLength();
    AutoPtr<IWifiDisplay> d;
    for (Int32 i = 0; i < length; i++) {
      d = NULL;
      d = (*displays)[i];
      if (ShouldShowWifiDisplay(d.Get(), activeDisplay.Get())) {
        AutoPtr<IMediaRouterRouteInfo> route = FindWifiDisplayRoute(d);
        if (route == NULL) {
          MakeWifiDisplayRoute(d, status, (IMediaRouterRouteInfo**)&route);
          AddRouteStatic(route);
        } else {
          String address;
          d->GetDeviceAddress(&address);
          Boolean disconnected = !address.Equals(activeDisplayAddress)
                  && address.Equals(((Static*)(sStatic.Get()))->mPreviousActiveWifiDisplayAddress);
          UpdateWifiDisplayRoute(route.Get(), d.Get(), status, disconnected);
        }
        if (d == activeDisplay) {
          Int32 types;
          route->GetSupportedTypes(&types);
          SelectRouteStatic(types, route.Get(), FALSE);
        }
      }
    }

    // Remove stale routes.
    AutoPtr<IMediaRouterRouteInfo> rt;
    AutoPtr<IWifiDisplay> d_;
    for (Int32 i = (((Static*)(sStatic.Get()))->mRoutes).GetSize(); i-- > 0; ) {
      rt = NULL;
      rt = ((Static*)(sStatic.Get()))->mRoutes[i];
      String address = ((MediaRouterRouteInfo*)rt.Get())->mDeviceAddress;
      if (!address.IsNull()) {
        d_ = NULL;
        d_ = FindWifiDisplay(displays.Get(), address);
        if (d_ == NULL || !ShouldShowWifiDisplay(d_.Get(), activeDisplay.Get())) {
          RemoveRouteStatic(rt.Get());
        }
      }
    }

    // Remember the current active wifi display address so that we can infer disconnections.
    // TODO: This hack will go away once all of this is moved into the media router service.
    ((Static*)(sStatic.Get()))->mPreviousActiveWifiDisplayAddress = activeDisplayAddress;
    return NOERROR;
}

Boolean CMediaRouter::ShouldShowWifiDisplay(
    /* [in] */ IWifiDisplay* d,
    /* [in] */ IWifiDisplay* activeDisplay)
{
    Boolean flag = FALSE;
    d->IsRemembered(&flag);
    return flag || (d == activeDisplay);
}

ECode CMediaRouter::GetWifiDisplayStatusCode(
    /* [in] */ IWifiDisplay* d,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 newStatus;
    wfdStatus->GetScanState(&newStatus);
    Boolean flag = FALSE;
    if (newStatus == IWifiDisplayStatus::SCAN_STATE_SCANNING) {
       newStatus = IMediaRouterRouteInfo::STATUS_SCANNING;
    } else if ((d->IsAvailable(&flag), flag)) {
       newStatus = (d->CanConnect(&flag), flag) ?
               IMediaRouterRouteInfo::STATUS_AVAILABLE : IMediaRouterRouteInfo::STATUS_IN_USE;
    } else {
       newStatus = IMediaRouterRouteInfo::STATUS_NOT_AVAILABLE;
    }

    AutoPtr<IWifiDisplay> display;
    wfdStatus->GetActiveDisplay((IWifiDisplay**)&display);
    d->Equals(display, &flag);
    if (flag) {
       Int32 activeState;
       wfdStatus->GetActiveDisplayState(&activeState);
       switch (activeState) {
           case 2:
               newStatus = IMediaRouterRouteInfo::STATUS_CONNECTED;
               break;
           case 1:
               newStatus = IMediaRouterRouteInfo::STATUS_CONNECTING;
               break;
           case 0:
               Logger::E(TAG, "Active display is not connected!");
               break;
       }
    }

    *result = newStatus;
    return NOERROR;
}

ECode CMediaRouter::IsWifiDisplayEnabled(
    /* [in] */ IWifiDisplay* d,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean isAvailable = FALSE;
    Boolean canConnect = FALSE;
    d->IsAvailable(&isAvailable);
    d->CanConnect(&canConnect);
    AutoPtr<IWifiDisplay> wd;
    wfdStatus->GetActiveDisplay((IWifiDisplay**)&wd);
    Boolean flag = FALSE;
    d->Equals(wd.Get(), &flag);
    *result = isAvailable && (canConnect || flag);
    return NOERROR;
}

ECode CMediaRouter::MakeWifiDisplayRoute(
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [out] */ IMediaRouterRouteInfo** result)
{
    AutoPtr<CMediaRouterRouteInfo> newRoute;
    // CMediaRouterRouteInfo::NewByFriend(((Static*)(sStatic.Get()))->mSystemCategory, IMediaRouter::Probe(this), (CMediaRouterRouteInfo**)&newRoute);//

    String tempText;
    display->GetDeviceAddress(&tempText);
    newRoute->mDeviceAddress = tempText;

    newRoute->mSupportedTypes = ROUTE_TYPE_LIVE_AUDIO | ROUTE_TYPE_LIVE_VIDEO;
    newRoute->mVolumeHandling = IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED;
    newRoute->mPlaybackType = IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE;

    Boolean flag = FALSE;
    Int32 rsCode;
    GetWifiDisplayStatusCode(display, wfdStatus, &rsCode);
    newRoute->SetRealStatusCode(rsCode, &flag);
    Boolean isEnnable = FALSE;
    IsWifiDisplayEnabled(display, wfdStatus, &isEnnable);
    newRoute->mEnabled = isEnnable;
    display->GetFriendlyDisplayName(&tempText);
    newRoute->SetName(StringUtils::ParseCharSequence(tempText).Get());
    AutoPtr<ICharSequence> cs;
    ((Static*)(sStatic.Get()))->mResources->GetText(R::string::wireless_display_route_description, (ICharSequence**)&cs);
    newRoute->mDescription = cs;
    newRoute->UpdatePresentationDisplay(&flag);
    *result = IMediaRouterRouteInfo::Probe(newRoute);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

void CMediaRouter::UpdateWifiDisplayRoute(
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [in] */ Boolean disconnected)
{
    Boolean changed = FALSE;
    String newName;
    display->GetFriendlyDisplayName(&newName);
    AutoPtr<ICharSequence> cs;
    route->GetName((ICharSequence**)&cs);
    String rName;
    cs->ToString(&rName);
    if (!rName.Equals(newName)) {
       ((MediaRouterRouteInfo*)route)->mName = StringUtils::ParseCharSequence(newName);
       changed = TRUE;
    }//

    Boolean enabled = FALSE;
    IsWifiDisplayEnabled(display, wfdStatus, &enabled);
    changed |= ((MediaRouterRouteInfo*)route)->mEnabled != enabled;
    ((MediaRouterRouteInfo*)route)->mEnabled = enabled;//

    Int32 ret;
    GetWifiDisplayStatusCode(display, wfdStatus, &ret);
    route->SetRealStatusCode(ret, &enabled);
    changed |= enabled;//

    if (changed) {
       DispatchRouteChanged(route);
    }//

    route->IsSelected(&enabled);
    if ((!enabled || disconnected) && enabled) {
       // Oops, no longer available. Reselect the default.
       SelectDefaultRouteStatic();
    }
}

AutoPtr<IWifiDisplay> CMediaRouter::FindWifiDisplay(
    /* [in] */ ArrayOf<IWifiDisplay*>* displays,
    /* [in] */ const String& deviceAddress)
{
    Int32 length = displays->GetLength();
    AutoPtr<IWifiDisplay> d;
    AutoPtr<IInterface> obj;
    String address;
    for (Int32 i = 0; i < length; i++) {
       d = NULL;
       d = (*displays)[i];
       d->GetDeviceAddress(&address);
           if (address.Equals(deviceAddress)) {
               return d;
           }
       }
       return NULL;
}

AutoPtr<IMediaRouterRouteInfo> CMediaRouter::FindWifiDisplayRoute(
    /* [in] */ IWifiDisplay* d)
{
    Int32 count = ((Static*)(sStatic.Get()))->mRoutes.GetSize();
    String tempText1, tempText2;//

    List< AutoPtr<IMediaRouterRouteInfo> >::Iterator it = ((Static*)(sStatic.Get()))->mRoutes.Begin();
    for (; it != ((Static*)(sStatic.Get()))->mRoutes.End(); ++it) {
       AutoPtr<IMediaRouterRouteInfo> info = *it;
       d->GetDeviceAddress(&tempText1);
       info->GetDeviceAddress(&tempText2);
       if (tempText1.Equals(tempText2)) {
           return info;
       }
    }
    return NULL;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
