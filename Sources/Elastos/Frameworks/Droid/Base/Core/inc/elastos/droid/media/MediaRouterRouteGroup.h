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

#ifndef __ELASTOS_DROID_MEDIA_MEDIAROUTERROUTEGROUP_H__
#define __ELASTOS_DROID_MEDIA_MEDIAROUTERROUTEGROUP_H__

#include "elastos/droid/media/MediaRouterRouteInfo.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Media {

class MediaRouterRouteGroup
    : public MediaRouterRouteInfo
    , public IMediaRouterRouteGroup
{
public:
    MediaRouterRouteGroup();

    virtual ~MediaRouterRouteGroup();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IMediaRouterRouteCategory* category);

    using MediaRouterRouteInfo::GetName;

    CARAPI GetName(
        /* [in] */ IResources* res,
        /* [out] */ ICharSequence** result);

    CARAPI AddRoute(
        /* [in] */ IMediaRouterRouteInfo * route);

    CARAPI AddRoute(
        /* [in] */ IMediaRouterRouteInfo * route,
        /* [in] */ Int32 insertAt);

    CARAPI RemoveRoute(
        /* [in] */ IMediaRouterRouteInfo * route);

    CARAPI RemoveRoute(
        /* [in] */ Int32 index);

    CARAPI GetRouteCount(
        /* [out] */ Int32 * result);

    CARAPI GetRouteAt(
        /* [in] */ Int32 index,
        /* [out] */ IMediaRouterRouteInfo ** result);

    CARAPI SetIconDrawable(
        /* [in] */ IDrawable * icon);

    CARAPI SetIconResource(
        /* [in] */ Int32 resId);

    CARAPI RequestSetVolume(
        /* [in] */ Int32 volume);

    CARAPI RequestUpdateVolume(
        /* [in] */ Int32 direction);

    CARAPI_(void) MemberNameChanged(
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ ICharSequence* name);

    CARAPI_(void) MemberStatusChanged(
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ ICharSequence* status);

    CARAPI_(void) MemberVolumeChanged(
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI_(void) UpdateVolume();

    CARAPI RouteUpdated();

    CARAPI_(void) UpdateName();

    CARAPI ToString(
        /* [out] */ String* result);

public:
    AutoPtr<IArrayList> mRoutes;

private:
    Boolean mUpdateName;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIAROUTERROUTEGROUP_H__