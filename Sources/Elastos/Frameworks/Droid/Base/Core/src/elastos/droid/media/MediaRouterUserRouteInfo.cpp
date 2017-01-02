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

#include "elastos/droid/media/MediaRouterUserRouteInfo.h"
#include "elastos/droid/media/MediaRouterRouteGroup.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Media::Session::IMediaSession;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Media {
//================================================================================
//          MediaRouterUserRouteInfo::MySetRequestRunnable
//================================================================================
MediaRouterUserRouteInfo::MySetRequestRunnable::MySetRequestRunnable(
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ Int32 volume)
    : mVolume(volume)
{
    mHost = (MediaRouterUserRouteInfo*)(IMediaRouterUserRouteInfo::Probe(route));
}

MediaRouterUserRouteInfo::MySetRequestRunnable::~MySetRequestRunnable()
{}

CAR_INTERFACE_IMPL(MediaRouterUserRouteInfo::MySetRequestRunnable, Object, IRunnable)

ECode MediaRouterUserRouteInfo::MySetRequestRunnable::Run()
{
    AutoPtr<IMediaRouterVolumeCallbackInfo> vcb = ((MediaRouterRouteInfo*)mHost)->mVcb;
    if (vcb != NULL) {
        AutoPtr<IMediaRouterRouteInfo> route = ((CMediaRouter::VolumeCallbackInfo*)(vcb.Get()))->mRoute;
        AutoPtr<IMediaRouterVolumeCallback> vcb2 = ((CMediaRouter::VolumeCallbackInfo*)(vcb.Get()))->mVcb;
        vcb2->OnVolumeSetRequest(route.Get(), mVolume);
    }
    return NOERROR;
}

//================================================================================
//          MediaRouterUserRouteInfo::MyUpdateRequestRunnable
//================================================================================
MediaRouterUserRouteInfo::MyUpdateRequestRunnable::MyUpdateRequestRunnable(
    /* [in] */ IMediaRouterRouteInfo* host,
    /* [in] */ Int32 direction)
    : mDirection(direction)
{
    mHost = (MediaRouterUserRouteInfo*)IMediaRouterUserRouteInfo::Probe(host);
}

MediaRouterUserRouteInfo::MyUpdateRequestRunnable::~MyUpdateRequestRunnable()
{}

CAR_INTERFACE_IMPL(MediaRouterUserRouteInfo::MyUpdateRequestRunnable, Object, IRunnable)

ECode MediaRouterUserRouteInfo::MyUpdateRequestRunnable::Run()
{
    AutoPtr<IMediaRouterVolumeCallbackInfo> vcb = ((MediaRouterRouteInfo*)mHost)->mVcb;
    if (vcb != NULL) {
        AutoPtr<IMediaRouterRouteInfo> route = ((CMediaRouter::VolumeCallbackInfo*)(vcb.Get()))->mRoute;
        AutoPtr<IMediaRouterVolumeCallback> vcb2 = ((CMediaRouter::VolumeCallbackInfo*)(vcb.Get()))->mVcb;
        vcb2->OnVolumeUpdateRequest(route.Get(), mDirection);
    }
    return NOERROR;
}

//================================================================================
//          MediaRouterUserRouteInfo::SessionVolumeProvider
//================================================================================
MediaRouterUserRouteInfo::SessionVolumeProvider::SessionVolumeProvider(
    /* [in] */ Int32 volumeControl,
    /* [in] */ Int32 maxVolume,
    /* [in] */ Int32 currentVolume,
    /* [in] */ IMediaRouterUserRouteInfo* host)
{
    VolumeProvider::constructor(volumeControl, maxVolume, currentVolume);
    mHost = (MediaRouterUserRouteInfo*)host;
}

MediaRouterUserRouteInfo::SessionVolumeProvider::~SessionVolumeProvider()
{}

CAR_INTERFACE_IMPL(MediaRouterUserRouteInfo::SessionVolumeProvider, Object, IVolumeProvider)

ECode MediaRouterUserRouteInfo::SessionVolumeProvider::OnSetVolumeTo(
    /* [in] */ Int32 volume)
{
    AutoPtr<MySetRequestRunnable> runnable = new MySetRequestRunnable(IMediaRouterRouteInfo::Probe(mHost), volume);
    AutoPtr<CMediaRouter::Static> st =
        (CMediaRouter::Static*)(((mHost->mHost)->sStatic).Get());
    Boolean flag = FALSE;
    return st->mHandler->Post(runnable, &flag);
}

ECode MediaRouterUserRouteInfo::SessionVolumeProvider::OnAdjustVolume(
    /* [in] */ Int32 direction)
{
    AutoPtr<MyUpdateRequestRunnable> runnable = new MyUpdateRequestRunnable(IMediaRouterRouteInfo::Probe(mHost), direction);
    AutoPtr<CMediaRouter::Static> st =
        (CMediaRouter::Static*)(((mHost->mHost)->sStatic).Get());
    Boolean flag = FALSE;
    return st->mHandler->Post(runnable, &flag);
}

//================================================================================
//                      MediaRouterUserRouteInfo
//================================================================================

MediaRouterUserRouteInfo::MediaRouterUserRouteInfo()
{}

MediaRouterUserRouteInfo::~MediaRouterUserRouteInfo()
{}

CAR_INTERFACE_IMPL_2(MediaRouterUserRouteInfo, Object, IMediaRouterUserRouteInfo, IMediaRouterRouteInfo)

ECode MediaRouterUserRouteInfo::constructor(
    /* [in] */ IMediaRouterRouteCategory* category)
{
    MediaRouterRouteInfo::constructor(category, mHost);
    mSupportedTypes = IMediaRouter::ROUTE_TYPE_USER;
    mPlaybackType = IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE;
    mVolumeHandling = IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED;
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::SetName(
    /* [in] */ ICharSequence * name)
{
    mName = name;
    return RouteUpdated();
}

ECode MediaRouterUserRouteInfo::SetName(
    /* [in] */ Int32 resId)
{
    mNameResId = resId;
    mName = NULL;
    return RouteUpdated();
}

ECode MediaRouterUserRouteInfo::SetDescription(
    /* [in] */ ICharSequence * description)
{
    mDescription = description;
    return RouteUpdated();
}

ECode MediaRouterUserRouteInfo::SetStatus(
    /* [in] */ ICharSequence * status)
{
    return MediaRouterRouteInfo::SetStatusInt(status);
}

ECode MediaRouterUserRouteInfo::SetRemoteControlClient(
    /* [in] */ IRemoteControlClient * rcc)
{
    mRcc = rcc;
    UpdatePlaybackInfoOnRcc();
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::GetRemoteControlClient(
    /* [out] */ IRemoteControlClient ** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRcc;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::SetIconDrawable(
    /* [in] */ IDrawable * icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::SetIconResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> dr;
    ((CMediaRouter::Static*)((mHost->sStatic).Get()))->mResources->GetDrawable(resId, (IDrawable**)&dr);
    return SetIconDrawable(dr.Get());
}

ECode MediaRouterUserRouteInfo::SetVolumeCallback(
    /* [in] */ IMediaRouterVolumeCallback * vcb)
{
    AutoPtr<CMediaRouter::VolumeCallbackInfo> v = new CMediaRouter::VolumeCallbackInfo();
    v->constructor(vcb, IMediaRouterRouteInfo::Probe(this));
    MediaRouterRouteInfo::mVcb = (IMediaRouterVolumeCallbackInfo*)v;
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::SetPlaybackType(
    /* [in] */ Int32 type)
{
    if (mPlaybackType != type) {
        mPlaybackType = type;
        ConfigureSessionVolume();
    }
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::SetVolumeHandling(
    /* [in] */ Int32 volumeHandling)
{
    if (mVolumeHandling != volumeHandling) {
        mVolumeHandling = volumeHandling;
        ConfigureSessionVolume();
    }
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::SetVolume(
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
        CMediaRouter::DispatchRouteVolumeChanged(this);
        AutoPtr<MediaRouterRouteGroup> group =
            (MediaRouterRouteGroup*)((((MediaRouterRouteInfo*)IMediaRouterRouteInfo::Probe(this))->mGroup).Get());
        if (group != NULL) {
            group->MemberVolumeChanged(this);
        }
    }
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    if (mVolumeHandling == IMediaRouterRouteInfo::PLAYBACK_VOLUME_VARIABLE) {
        if (mVcb == NULL) {
            Slogger::E(mHost->TAG, "Cannot requestSetVolume on user route - no volume callback set");
            return NOERROR;
        }
        AutoPtr<IMediaRouterVolumeCallback> vcb = ((CMediaRouter::VolumeCallbackInfo*)(mVcb.Get()))->mVcb;
        vcb->OnVolumeSetRequest(this, volume);
    }
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    if (mVolumeHandling == IMediaRouterRouteInfo::PLAYBACK_VOLUME_VARIABLE) {
        if (mVcb == NULL) {
            Slogger::E(mHost->TAG, "Cannot requestChangeVolume on user route - no volumec callback set");
            return NOERROR;
        }
        AutoPtr<IMediaRouterVolumeCallback> vcb = ((CMediaRouter::VolumeCallbackInfo*)(mVcb.Get()))->mVcb;
        vcb->OnVolumeUpdateRequest(this, direction);
    }
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::SetVolumeMax(
    /* [in] */ Int32 volumeMax)
{
    if (mVolumeMax != volumeMax) {
        mVolumeMax = volumeMax;
        ConfigureSessionVolume();
    }
    return NOERROR;
}

ECode MediaRouterUserRouteInfo::SetPlaybackStream(
    /* [in] */ Int32 stream)
{
    if (mPlaybackStream != stream) {
        mPlaybackStream = stream;
        ConfigureSessionVolume();
    }
    return NOERROR;
}

void MediaRouterUserRouteInfo::UpdatePlaybackInfoOnRcc()
{
    ConfigureSessionVolume();
}

void MediaRouterUserRouteInfo::ConfigureSessionVolume()
{
    if (mRcc == NULL) {
        if (mHost->DEBUG) {
            String str;
            MediaRouterRouteInfo::mName->ToString(&str);
            Slogger::D(mHost->TAG, "No Rcc to configure volume for route %s", str.string());
        }
        return;
    }
    AutoPtr<IMediaSession> session;
    mRcc->GetMediaSession((IMediaSession**)&session);
    if (session == NULL) {
        if (mHost->DEBUG) {
            Slogger::D(mHost->TAG, "Rcc has no session to configure volume");
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
        IVolumeProvider::Probe(mSvp)->GetMaxVolume(&vm);
        if (mSvp == NULL || vol != volumeControl
                || vm != mVolumeMax) {
            AutoPtr<SessionVolumeProvider> s =
                new SessionVolumeProvider(volumeControl, mVolumeMax, mVolume, IMediaRouterUserRouteInfo::Probe(this));
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

} // namespace Media
} // namepsace Droid
} // namespace Elastos

