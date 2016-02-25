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