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

#ifndef __ELASTOS_DROID_MEDIA_MEDIAROUTERROUTEINFO_H__
#define __ELASTOS_DROID_MEDIA_MEDIAROUTERROUTEINFO_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/CMediaRouter.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IDisplay;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace Media {

class MediaRouterRouteInfo
    : public Object
    , public IMediaRouterRouteInfo
{
    friend class CMediaRouter;

private:
    class MyRemoteVolumeObserver
        : public Object
        , public IIRemoteVolumeObserver
    {
    public:
        MyRemoteVolumeObserver(
            /* [in] */ MediaRouterRouteInfo* owner);

        ~MyRemoteVolumeObserver();

        CAR_INTERFACE_DECL()

        CARAPI DispatchRemoteVolumeUpdate(
            /* [in] */ Int32 direction,
            /* [in] */ Int32 value);

    private:
        MediaRouterRouteInfo* mOwner;
    };

    class MyRunnable
        : public Object
        , public IRunnable
    {
    public:
        MyRunnable(
            /* [in] */ Int32 direction,
            /* [in] */ Int32 value,
            /* [in] */ MediaRouterRouteInfo* owner);

        ~MyRunnable();

        CAR_INTERFACE_DECL()

        CARAPI Run();

    private:
        Int32 mDirection;
        Int32 mValue;
        MediaRouterRouteInfo* mOwner;
    };

public:
    MediaRouterRouteInfo();

    virtual ~MediaRouterRouteInfo();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IMediaRouterRouteCategory* category,
        /* [in] */ IMediaRouter* host);

    CARAPI GetName(
        /* [out] */ ICharSequence ** result);

    CARAPI SetName(
        /* [in] */ ICharSequence* name);

    CARAPI GetName(
        /* [in] */ IContext * context,
        /* [out] */ ICharSequence ** result);

    CARAPI GetName(
        /* [in] */ IResources* res,
        /* [out] */ ICharSequence** result);

    CARAPI GetDescription(
        /* [out] */ ICharSequence ** result);

    CARAPI GetStatus(
        /* [out] */ ICharSequence ** result);

    CARAPI SetRealStatusCode(
        /* [in] */ Int32 statusCode,
        /* [out] */ Boolean* result);

    CARAPI ResolveStatusCode(
        /* [out] */ Boolean* result);

    CARAPI GetStatusCode(
        /* [out] */ Int32 * result);

    CARAPI GetSupportedTypes(
        /* [out] */ Int32 * result);

    CARAPI SetSupportedTypes(
        /* [in] */ Int32 type);

    CARAPI MatchesTypes(
        /* [in] */ Int32 types,
        /* [out] */ Boolean * result);

    CARAPI GetGroup(
        /* [out] */ IMediaRouterRouteGroup ** result);

    CARAPI GetCategory(
        /* [out] */ IMediaRouterRouteCategory ** result);

    CARAPI GetIconDrawable(
        /* [out] */ IDrawable ** result);

    CARAPI SetTag(
        /* [in] */ IInterface * tag);

    CARAPI GetTag(
        /* [out] */ IInterface ** result);

    CARAPI GetPlaybackType(
        /* [out] */ Int32 * result);

    CARAPI GetPlaybackStream(
        /* [out] */ Int32 * result);

    CARAPI GetVolume(
        /* [out] */ Int32 * result);

    CARAPI RequestSetVolume(
        /* [in] */ Int32 volume);

    CARAPI RequestUpdateVolume(
        /* [in] */ Int32 direction);

    CARAPI GetVolumeMax(
        /* [out] */ Int32 * result);

    CARAPI GetVolumeHandling(
        /* [out] */ Int32 * result);

    CARAPI GetPresentationDisplay(
        /* [out] */ IDisplay ** result);

    CARAPI UpdatePresentationDisplay(
        /* [out] */ Boolean* result);

    CARAPI GetDeviceAddress(
        /* [out] */ String * result);

    CARAPI IsEnabled(
        /* [out] */ Boolean * result);

    CARAPI IsConnecting(
        /* [out] */ Boolean * result);

    CARAPI IsSelected(
        /* [out] */ Boolean * result);

    CARAPI IsDefault(
        /* [out] */ Boolean * result);

    CARAPI Select();

    CARAPI SetStatusInt(
        /* [in] */ ICharSequence* status);

    CARAPI RouteUpdated();

    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI_(AutoPtr<IDisplay>) ChoosePresentationDisplay();

public:
    AutoPtr<ICharSequence> mName;
    Int32 mNameResId;
    AutoPtr<ICharSequence> mDescription;
    AutoPtr<ICharSequence> mStatus;
    Int32 mSupportedTypes;
    AutoPtr<IMediaRouterRouteGroup> mGroup;
    AutoPtr<IMediaRouterRouteCategory> mCategory;
    AutoPtr<IDrawable> mIcon;
    // playback information
    Int32 mPlaybackType;
    Int32 mVolumeMax;
    Int32 mVolume;
    Int32 mVolumeHandling;
    Int32 mPlaybackStream;
    AutoPtr<IMediaRouterVolumeCallbackInfo> mVcb;
    AutoPtr<IDisplay> mPresentationDisplay;
    Int32 mPresentationDisplayId;

    String mDeviceAddress;
    Boolean mEnabled;

    // An id by which the route is known to the media router service.
    // Null if this route only exists as an artifact within this process.
    String mGlobalRouteId;
    AutoPtr<IIRemoteVolumeObserver> mRemoteVolObserver;
    static CMediaRouter* mHost;

private:
    // A predetermined connection status that can override mStatus
    Int32 mRealStatusCode;
    Int32 mResolvedStatusCode;

    Object mTag;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIAROUTERROUTEINFO_H__
