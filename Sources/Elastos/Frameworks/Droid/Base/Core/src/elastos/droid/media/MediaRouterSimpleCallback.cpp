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

#include "elastos/droid/media/MediaRouterSimpleCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {

MediaRouterSimpleCallback::MediaRouterSimpleCallback()
{}

MediaRouterSimpleCallback::~MediaRouterSimpleCallback()
{}

ECode MediaRouterSimpleCallback::constructor()
{
   return NOERROR;
}

CAR_INTERFACE_IMPL_2(MediaRouterSimpleCallback, Object, IMediaRouterCallback, IMediaRouterSimpleCallback)//

ECode MediaRouterSimpleCallback::OnRouteSelected(
  /* [in] */ IMediaRouter* router,
  /* [in] */ Int32 type,
  /* [in] */ IMediaRouterRouteInfo* info)
{
   return NOERROR;
}

ECode MediaRouterSimpleCallback::OnRouteUnselected(
  /* [in] */ IMediaRouter* router,
  /* [in] */ Int32 type,
  /* [in] */ IMediaRouterRouteInfo* info)
{
   return NOERROR;
}

ECode MediaRouterSimpleCallback::OnRouteAdded(
  /* [in] */ IMediaRouter* router,
  /* [in] */ IMediaRouterRouteInfo* info)
{
   return NOERROR;
}

ECode MediaRouterSimpleCallback::OnRouteRemoved(
  /* [in] */ IMediaRouter* router,
  /* [in] */ IMediaRouterRouteInfo* info)
{
   return NOERROR;
}

ECode MediaRouterSimpleCallback::OnRouteChanged(
  /* [in] */ IMediaRouter* router,
  /* [in] */ IMediaRouterRouteInfo* info)
{
   return NOERROR;
}

ECode MediaRouterSimpleCallback::OnRouteGrouped(
  /* [in] */ IMediaRouter* router,
  /* [in] */ IMediaRouterRouteInfo* info,
  /* [in] */ IMediaRouterRouteGroup* group,
  /* [in] */ Int32 index)
{
   return NOERROR;
}

ECode MediaRouterSimpleCallback::OnRouteUngrouped(
  /* [in] */ IMediaRouter* router,
  /* [in] */ IMediaRouterRouteInfo* info,
  /* [in] */ IMediaRouterRouteGroup* group)
{
   return NOERROR;
}

ECode MediaRouterSimpleCallback::OnRouteVolumeChanged(
  /* [in] */ IMediaRouter* router,
  /* [in] */ IMediaRouterRouteInfo* info)
{
   return NOERROR;
}

ECode MediaRouterSimpleCallback::OnRoutePresentationDisplayChanged(
  /* [in] */ IMediaRouter * router,
  /* [in] */ IMediaRouterRouteInfo * info)
{
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos