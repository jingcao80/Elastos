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

#ifndef __ELASTOS_DROID_MEDIA_MEDIAROUTERSIMPLECALLBACK_H__
#define __ELASTOS_DROID_MEDIA_MEDIAROUTERSIMPLECALLBACK_H__

#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "Elastos.Droid.Media.h"

namespace Elastos {
namespace Droid {
namespace Media {

class ECO_PUBLIC MediaRouterSimpleCallback
    : public Object
    , public IMediaRouterCallback
    , public IMediaRouterSimpleCallback
{
public:
    CAR_INTERFACE_DECL()

    MediaRouterSimpleCallback();

    virtual ~MediaRouterSimpleCallback();

    CARAPI constructor();

    CARAPI OnRouteSelected(
        /* [in] */ IMediaRouter* router,
        /* [in] */ Int32 type,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteUnselected(
        /* [in] */ IMediaRouter* router,
        /* [in] */ Int32 type,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteAdded(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteRemoved(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteChanged(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRouteGrouped(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ IMediaRouterRouteGroup* group,
        /* [in] */ Int32 index);

    CARAPI OnRouteUngrouped(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info,
        /* [in] */ IMediaRouterRouteGroup* group);

    CARAPI OnRouteVolumeChanged(
        /* [in] */ IMediaRouter* router,
        /* [in] */ IMediaRouterRouteInfo* info);

    CARAPI OnRoutePresentationDisplayChanged(
        /* [in] */ IMediaRouter * router,
        /* [in] */ IMediaRouterRouteInfo * info);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_MEDIAROUTERSIMPLECALLBACK_H__