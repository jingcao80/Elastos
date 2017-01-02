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

#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTERROUTEGROUP_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTERROUTEGROUP_H__

#include "_Elastos_Droid_Media_CMediaRouterRouteGroup.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/media/MediaRouterRouteGroup.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * Information about a route that consists of multiple other routes in a group.
 */
CarClass(CMediaRouterRouteGroup)
    , public MediaRouterRouteGroup
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTERROUTEGROUP_H__
