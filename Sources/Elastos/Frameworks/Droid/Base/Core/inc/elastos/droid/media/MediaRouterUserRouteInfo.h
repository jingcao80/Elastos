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

#ifndef __ELASTOS_DROID_MEDIA_MEDIAROUTERUSERROUTEINFO_H__
#define __ELASTOS_DROID_MEDIA_MEDIAROUTERUSERROUTEINFO_H__

#include "elastos/droid/media/CMediaRouter.h"
#include "elastos/droid/media/MediaRouterRouteInfo.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Media {

class MediaRouterUserRouteInfo
    : public MediaRouterRouteInfo
    , public IMediaRouterUserRouteInfo
{
private:
    class MySetRequestRunnable
        : public Object
        , public IRunnable
    {
    public:
        MySetRequestRunnable(
            /* [in] */ IMediaRouterRouteInfo* route,
            /* [in] */ Int32 volume);

        ~MySetRequestRunnable();

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        MediaRouterUserRouteInfo* mHost;
        Int32 mVolume;
    };

    class MyUpdateRequestRunnable
        : public Object
        , public IRunnable
    {
    public:
        MyUpdateRequestRunnable(
            /* [in] */ IMediaRouterRouteInfo* host,
            /* [in] */ Int32 direction);

        ~MyUpdateRequestRunnable();

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        MediaRouterUserRouteInfo* mHost;
        Int32 mDirection;
    };

public:
    class SessionVolumeProvider
        : public VolumeProvider
    {
    public:
        SessionVolumeProvider(
            /* [in] */ Int32 volumeControl,
            /* [in] */ Int32 maxVolume,
            /* [in] */ Int32 currentVolume,
            /* [in] */ IMediaRouterUserRouteInfo* host);

        ~SessionVolumeProvider();

        CAR_INTERFACE_DECL()

        CARAPI OnSetVolumeTo(
            /* [in] */ Int32 volume);

        CARAPI OnAdjustVolume(
            /* [in] */ Int32 direction);

    private:
        MediaRouterUserRouteInfo* mHost;
    };

public:

    MediaRouterUserRouteInfo();

    virtual ~MediaRouterUserRouteInfo();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IMediaRouterRouteCategory* category);

    CARAPI SetName(
        /* [in] */ ICharSequence * name);

    CARAPI SetName(
        /* [in] */ Int32 resId);

    CARAPI SetDescription(
        /* [in] */ ICharSequence * description);

    CARAPI SetStatus(
        /* [in] */ ICharSequence * status) ;

    CARAPI SetRemoteControlClient(
        /* [in] */ IRemoteControlClient * rcc);

    CARAPI GetRemoteControlClient(
        /* [out] */ IRemoteControlClient ** result);

    CARAPI SetIconDrawable(
        /* [in] */ IDrawable * icon);

    CARAPI SetIconResource(
        /* [in] */ Int32 resId);

    CARAPI SetVolumeCallback(
        /* [in] */ IMediaRouterVolumeCallback * vcb);

    CARAPI SetPlaybackType(
        /* [in] */ Int32 type);

    CARAPI SetVolumeHandling(
        /* [in] */ Int32 volumeHandling);

    CARAPI SetVolume(
        /* [in] */ Int32 volume);

    CARAPI RequestSetVolume(
        /* [in] */ Int32 volume);

    CARAPI RequestUpdateVolume(
        /* [in] */ Int32 direction);

    CARAPI SetVolumeMax(
        /* [in] */ Int32 volumeMax);

    CARAPI SetPlaybackStream(
        /* [in] */ Int32 stream);

private:
    CARAPI_(void) UpdatePlaybackInfoOnRcc();

    CARAPI_(void) ConfigureSessionVolume();

public:
    AutoPtr<IRemoteControlClient> mRcc;
    AutoPtr<SessionVolumeProvider> mSvp;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIAROUTERUSERROUTEINFO_H__
