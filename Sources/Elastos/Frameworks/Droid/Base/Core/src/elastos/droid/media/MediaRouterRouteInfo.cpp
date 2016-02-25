#include "elastos/droid/app/CActivityThread.h"
#include "elastos/droid/media/CMediaRouter.h"
#include "elastos/droid/media/MediaRouterRouteInfo.h"
#include "elastos/droid/media/MediaRouterRouteGroup.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>

using Elastos::Droid::App::CActivityThread;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Media {
//------------------------------------------------
//  MediaRouterRouteInfo::MyRemoteVolumeObserver
//------------------------------------------------
CAR_INTERFACE_IMPL(MediaRouterRouteInfo::MyRemoteVolumeObserver, Object, IIRemoteVolumeObserver);

MediaRouterRouteInfo::MyRemoteVolumeObserver::MyRemoteVolumeObserver(
    /* [in] */ MediaRouterRouteInfo* owner)
    : mOwner(owner)
{}

MediaRouterRouteInfo::MyRemoteVolumeObserver::~MyRemoteVolumeObserver()
{}

ECode MediaRouterRouteInfo::MyRemoteVolumeObserver::DispatchRemoteVolumeUpdate(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 value)
{
    AutoPtr<MyRunnable> myRunnable = new MyRunnable(direction, value, mOwner);
    Boolean tempState;
    CMediaRouter::Static* st = (CMediaRouter::Static*)((mHost->sStatic).Get());
    st->mHandler->Post(myRunnable, &tempState);
    return NOERROR;
}

//-------------------------------
// MediaRouterRouteInfo::MyRunnable
//-------------------------------
MediaRouterRouteInfo::MyRunnable::MyRunnable(
    /* [in] */ Int32 direction,
    /* [in] */ Int32 value,
    /* [in] */ MediaRouterRouteInfo* owner)
    : mDirection(direction)
    , mValue(value)
    , mOwner(owner)
{}

MediaRouterRouteInfo::MyRunnable::~MyRunnable()
{}

CAR_INTERFACE_IMPL(MediaRouterRouteInfo::MyRunnable, Object, IRunnable);

ECode MediaRouterRouteInfo::MyRunnable::Run()
{
    AutoPtr<CMediaRouter::VolumeCallbackInfo> vci = (CMediaRouter::VolumeCallbackInfo*)((mOwner->mVcb).Get());
    if (mOwner->mVcb != NULL) {
        if (mDirection != 0) {
            vci->mVcb->OnVolumeUpdateRequest(vci->mRoute, mDirection);
        } else {
            vci->mVcb->OnVolumeSetRequest(vci->mRoute, mValue);
        }
    }
    return NOERROR;
}

//-------------------------------
// MediaRouterRouteInfo
//-------------------------------
CMediaRouter* MediaRouterRouteInfo::mHost;

CAR_INTERFACE_IMPL(MediaRouterRouteInfo, Object, IMediaRouterRouteInfo)

MediaRouterRouteInfo::MediaRouterRouteInfo()
    : mNameResId(0)
    , mSupportedTypes(0)
    , mPlaybackType(IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL)
    , mVolumeMax(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME)
    , mVolume(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME)
    , mVolumeHandling(IRemoteControlClient::DEFAULT_PLAYBACK_VOLUME_HANDLING)
    , mPlaybackStream(IAudioManager::STREAM_MUSIC)
    , mPresentationDisplayId(-1)
    , mEnabled(TRUE)
    , mRealStatusCode(0)
    , mResolvedStatusCode(0)
{}

MediaRouterRouteInfo::~MediaRouterRouteInfo()
{}

ECode MediaRouterRouteInfo::constructor(
    /* [in] */ IMediaRouterRouteCategory* category,
    /* [in] */ IMediaRouter* host)
{
    mCategory = category;
    mRemoteVolObserver = new MyRemoteVolumeObserver(this);
    mHost = (CMediaRouter*)host;
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetName(
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result);
    return GetName((((CMediaRouter::Static*)((mHost->sStatic).Get()))->mResources).Get(), result);
}

ECode MediaRouterRouteInfo::SetName(
    /* [in] */ ICharSequence* name)
{
    mName = name;
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetName(
    /* [in] */ IContext * context,
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    return GetName(res.Get(), result);
}

ECode MediaRouterRouteInfo::GetName(
    /* [in] */ IResources* res,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);
    if (mNameResId != 0) {
        res->GetText(mNameResId, (ICharSequence**)&mName);
        *result = mName.Get();
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    *result = mName.Get();
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetDescription(
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDescription;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetStatus(
    /* [out] */ ICharSequence ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStatus;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterRouteInfo::SetRealStatusCode(
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

ECode MediaRouterRouteInfo::ResolveStatusCode(
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
    ((CMediaRouter::Static*)((mHost->sStatic).Get()))->mResources->GetText(resId, (ICharSequence**)&cs);

    mStatus = resId != 0 ? cs.Get() : NULL;
    *result = TRUE;
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetStatusCode(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResolvedStatusCode;
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetSupportedTypes(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSupportedTypes;
    return NOERROR;
}

ECode MediaRouterRouteInfo::SetSupportedTypes(
    /* [in] */ Int32 type)
{
    mSupportedTypes = type;
    return NOERROR;
}

ECode MediaRouterRouteInfo::MatchesTypes(
    /* [in] */ Int32 types,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = (mSupportedTypes & types) != 0;
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetGroup(
    /* [out] */ IMediaRouterRouteGroup ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mGroup;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetCategory(
    /* [out] */ IMediaRouterRouteCategory ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCategory;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetIconDrawable(
    /* [out] */ IDrawable ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIcon;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterRouteInfo::SetTag(
    /* [in] */ IInterface * tag)
{
    // mTag = (Object)(*(IObject::Probe(tag)));
    return RouteUpdated();
}

ECode MediaRouterRouteInfo::GetTag(
    /* [out] */ IInterface ** result)
{
    VALIDATE_NOT_NULL(result);
    // *result = (IInterface*)mTag;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetPlaybackType(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPlaybackType;
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetPlaybackStream(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPlaybackStream;
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetVolume(
    /* [out] */ Int32 * result)
{
    if (mPlaybackType == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL) {
        Int32 vol = 0;
        // try {
        ECode ec = ((CMediaRouter::Static*)((mHost->sStatic).Get()))->mAudioService->GetStreamVolume(mPlaybackStream, &vol);
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(mHost->TAG, "Error getting local stream volume");
        }
        // } catch (RemoteException e) {
            // Log.e(mHost->TAG, "Error getting local stream volume", e);
        // }
        *result = vol;
        return NOERROR;
    } else {
        *result = mVolume;
        return NOERROR;
    }
}

ECode MediaRouterRouteInfo::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    if (mPlaybackType == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL) {
        // try {
        String currentPackageName = CActivityThread::GetCurrentPackageName();
        ECode ec = ((CMediaRouter::Static*)((mHost->sStatic).Get()))->mAudioService->SetStreamVolume(mPlaybackStream, volume, 0,
            currentPackageName);
        // } catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(mHost->TAG, "Error setting local stream volume");
        }

        if (SUCCEEDED(ec)) {
            return ec;
        }
        // }
    } else {
        return ((CMediaRouter::Static*)((mHost->sStatic).Get()))->RequestSetVolume(this, volume);
    }
}

ECode MediaRouterRouteInfo::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    if (mPlaybackType == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL) {
        // try {
        Int32 volume_;
        GetVolume(&volume_);
        Int32 volumeMax;
        GetVolumeMax(&volumeMax);
        Int32 volume = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(volume_ + direction, volumeMax));
        ECode ec = ((CMediaRouter::Static*)(mHost->sStatic.Get()))->mAudioService->SetStreamVolume(mPlaybackStream, volume, 0,
                    CActivityThread::GetCurrentPackageName());
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(mHost->TAG, "Error setting local stream volume");
        }
        if (SUCCEEDED(ec)) {
            return ec;
        }
        // } catch (RemoteException e) {
            // Log.e(mHost->TAG, "Error setting local stream volume", e);
        // }
    } else {
        return ((CMediaRouter::Static*)((mHost->sStatic).Get()))->RequestUpdateVolume(this, direction);
    }
}

ECode MediaRouterRouteInfo::GetVolumeMax(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);

    if (mPlaybackType == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL) {
        // try {
        ECode ec = ((CMediaRouter::Static*)(mHost->sStatic.Get()))->mAudioService->GetStreamMaxVolume(mPlaybackStream, result);
        if (SUCCEEDED(ec)) {
            return ec;
        }
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            Slogger::E(mHost->TAG, "Error getting local stream volume");
        }
        // } catch (RemoteException e) {
            // Log.e(mHost->TAG, "Error getting local stream volume", e);
        // }
    } else {
        *result = mVolumeMax;
        return NOERROR;
    }
}

ECode MediaRouterRouteInfo::GetVolumeHandling(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVolumeHandling;
    return NOERROR;
}

ECode MediaRouterRouteInfo::GetPresentationDisplay(
    /* [out] */ IDisplay ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPresentationDisplay;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterRouteInfo::UpdatePresentationDisplay(
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

ECode MediaRouterRouteInfo::GetDeviceAddress(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDeviceAddress;
    return NOERROR;
}

ECode MediaRouterRouteInfo::IsEnabled(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mEnabled;
    return NOERROR;
}

ECode MediaRouterRouteInfo::IsConnecting(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResolvedStatusCode == IMediaRouterRouteInfo::STATUS_CONNECTING;
    return NOERROR;
}

ECode MediaRouterRouteInfo::IsSelected(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = IMediaRouterRouteInfo::Probe(this) == ((CMediaRouter::Static*)((mHost->sStatic).Get()))->mSelectedRoute;
    return NOERROR;
}

ECode MediaRouterRouteInfo::IsDefault(
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result);
    *result = IMediaRouterRouteInfo::Probe(this) == ((CMediaRouter::Static*)((mHost->sStatic).Get()))->mDefaultAudioVideo;
    return NOERROR;
}

ECode MediaRouterRouteInfo::Select()
{
    return CMediaRouter::SelectRouteStatic(mSupportedTypes, this, TRUE);
}

ECode MediaRouterRouteInfo::SetStatusInt(
    /* [in] */ ICharSequence* status)
{
    Boolean flag = status == mStatus.Get();
    if (!flag) {
        mStatus = status;
        if (mGroup != NULL) {
            ((MediaRouterRouteGroup*)mGroup.Get())->MemberStatusChanged(this, status);
        }
        return RouteUpdated();
    }
}

ECode MediaRouterRouteInfo::RouteUpdated()
{
    return CMediaRouter::UpdateRoute(this);
}

ECode MediaRouterRouteInfo::ToString(
    /* [out] */ String* result)
{
    Int32 types;
    GetSupportedTypes(&types);
    String supportedTypes = mHost->TypesToString(types);
    String name;
    AutoPtr<ICharSequence> cs;
    GetName((ICharSequence**)&cs);
    cs->ToString(&name);
    cs = NULL;
    GetDescription((ICharSequence**)&cs);
    String description;
    cs->ToString(&description);
    cs = NULL;
    String status;
    GetStatus((ICharSequence**)&cs);
    cs->ToString(&status);
    AutoPtr<IMediaRouterRouteCategory> rc;
    GetCategory((IMediaRouterRouteCategory **)&rc);
    String category;
    IObject::Probe(rc)->ToString(&category);
    String presentationDisplay;
    IObject::Probe(mPresentationDisplay)->ToString(&presentationDisplay);

    *result = /*getClass().getSimpleName() */ String("{ name=") + name +
            String(", description=") + description +
            String(", status=") + status +
            String(", category=") + category +
            String(", supportedTypes=") + supportedTypes +
            String(", presentationDisplay=") + presentationDisplay + String(" }");

    return NOERROR;
}

AutoPtr<IDisplay> MediaRouterRouteInfo::ChoosePresentationDisplay()
{
    if ((mSupportedTypes & IMediaRouter::ROUTE_TYPE_LIVE_VIDEO) != 0) {
        AutoPtr<ArrayOf<IDisplay*> > displays;
        ((CMediaRouter::Static*)((mHost->sStatic).Get()))->GetAllPresentationDisplays((ArrayOf<IDisplay*>**)&displays);

        // Ensure that the specified display is valid for presentations.
        // This check will normally disallow the default display unless it was
        // configured as a presentation display for some reason.
        assert(displays != NULL);
        Int32 len = displays->GetLength();

        AutoPtr<IDisplay> display;

        if (mPresentationDisplayId >= 0) {
            for (Int32 i = 0; i < len; i++) {
                Int32 displayId;
                (*displays)[i]->GetDisplayId(&displayId);
                if (displayId == mPresentationDisplayId) {
                    display = NULL;
                    display = (*displays)[i];
                    return display;
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
                        display = NULL;
                        display = (*displays)[i];
                        return display;
                    }
                }
            }
            return NULL;
        }

        // For the default route, choose the first presentation display from the list.
        // if (IMediaRouterRouteInfo::Probe(this).Equals(((CMediaRouter::Static*)(mHost->sStatic).Get()))->mDefaultAudioVideo) && len > 0) {
        //     display = NULL;
        //     display = (*displays)[0];
        //     return display;
        // }
    }
    return NULL;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
