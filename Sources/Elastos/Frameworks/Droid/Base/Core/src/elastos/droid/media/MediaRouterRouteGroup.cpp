#include "elastos/droid/media/MediaRouterRouteGroup.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

MediaRouterRouteGroup::MediaRouterRouteGroup()
   : mUpdateName(FALSE)
{}

MediaRouterRouteGroup::~MediaRouterRouteGroup()
{}

CAR_INTERFACE_IMPL_2(MediaRouterRouteGroup, Object, IMediaRouterRouteGroup, IMediaRouterRouteInfo)//

ECode MediaRouterRouteGroup::constructor(
    /* [in] */ IMediaRouterRouteCategory* category)
{
    MediaRouterRouteInfo::constructor(category, mHost);
    mGroup = this;
    mVolumeHandling = IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED;
    return NOERROR;
}

ECode MediaRouterRouteGroup::GetName(
    /* [in] */ IResources* res,
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result);

    if (mUpdateName) UpdateName();
    return MediaRouterRouteInfo::GetName(res, result);
}

ECode MediaRouterRouteGroup::AddRoute(
    /* [in] */ IMediaRouterRouteInfo * route)
{
    VALIDATE_NOT_NULL(route);
    AutoPtr<IMediaRouterRouteGroup> mr;
    route->GetGroup((IMediaRouterRouteGroup**)&mr);
    if (mr != NULL) {
        Slogger::E("RouteGroup", "Route %p is already part of a group.", route);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IMediaRouterRouteCategory> rc;
    route->GetCategory((IMediaRouterRouteCategory**)&rc);//

    if (rc != mCategory) {
        Slogger::E("RouteGroup", "Route cannot be added to a group with a different category. (Route category=%p group category=%p)", rc.Get(), mCategory.Get());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;;
    }

    Int32 at;
    mRoutes->GetSize(&at);
    mRoutes->Add(route);
    ((MediaRouterRouteInfo*)route)->mGroup = this;
    mUpdateName = TRUE;
    UpdateVolume();
    RouteUpdated();
    return mHost->DispatchRouteGrouped(route, IMediaRouterRouteGroup::Probe(this), at);
}

ECode MediaRouterRouteGroup::AddRoute(
    /* [in] */ IMediaRouterRouteInfo * route,
    /* [in] */ Int32 insertAt)
{
    VALIDATE_NOT_NULL(route);
    AutoPtr<IMediaRouterRouteGroup> mr;
    route->GetGroup((IMediaRouterRouteGroup**)&mr);
    if (mr != NULL) {
       Slogger::E("RouteGroup", "Route %p is already part of a group.", route);
       return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IMediaRouterRouteCategory> rc;
    route->GetCategory((IMediaRouterRouteCategory**)&rc);//

    if (rc != mCategory) {
       Slogger::E("RouteGroup", "Route cannot be added to a group with a different category. (Route category=%p group category=%p)", rc.Get(), mCategory.Get());
       return E_ILLEGAL_ARGUMENT_EXCEPTION;;
    }
    mRoutes->Add(insertAt, route);
    ((MediaRouterRouteInfo*)route)->mGroup = this;
    mUpdateName = TRUE;
    UpdateVolume();
    RouteUpdated();
    return mHost->DispatchRouteGrouped(route, IMediaRouterRouteGroup::Probe(this), insertAt);
}

ECode MediaRouterRouteGroup::RemoveRoute(
    /* [in] */ IMediaRouterRouteInfo * route)
{
    VALIDATE_NOT_NULL(route);
    AutoPtr<IMediaRouterRouteGroup> mr;
    route->GetGroup((IMediaRouterRouteGroup**)&mr);
    if (mr.Get() != IMediaRouterRouteGroup::Probe(this)) {
       Slogger::E("RouteGroup", "Route %p is not a member of this group.", route);
       return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mRoutes->Remove(route);
    ((MediaRouterRouteInfo*)route)->mGroup = NULL;
    mUpdateName = TRUE;
    UpdateVolume();
    mHost->DispatchRouteUngrouped(route, this);
    return RouteUpdated();
}

ECode MediaRouterRouteGroup::RemoveRoute(
    /* [in] */ Int32 index)
{
    AutoPtr<IInterface> obj;
    mRoutes->Remove(index, (IInterface**)&obj);
    AutoPtr<MediaRouterRouteInfo> route = (MediaRouterRouteInfo*)IMediaRouterRouteInfo::Probe(obj);
    route->mGroup = NULL;
    mUpdateName = TRUE;
    UpdateVolume();
    mHost->DispatchRouteUngrouped(route, IMediaRouterRouteGroup::Probe(this));
    return RouteUpdated();
}

ECode MediaRouterRouteGroup::GetRouteCount(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result);
    Int32 size;
    mRoutes->GetSize(&size);
    *result = size;
    return NOERROR;
}

ECode MediaRouterRouteGroup::GetRouteAt(
    /* [in] */ Int32 index,
    /* [out] */ IMediaRouterRouteInfo ** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IInterface> obj;
    mRoutes->Get(index, (IInterface**)&obj);
    *result = IMediaRouterRouteInfo::Probe(obj);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode MediaRouterRouteGroup::SetIconDrawable(
    /* [in] */ IDrawable * icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode MediaRouterRouteGroup::SetIconResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> drawable;
    ((CMediaRouter::Static*)((mHost->sStatic).Get()))->mResources->GetDrawable(resId, (IDrawable**)&drawable);
    return SetIconDrawable(drawable.Get());
}

ECode MediaRouterRouteGroup::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    Int32 maxVol;
    GetVolumeMax(&maxVol);
    if (maxVol == 0) {
       return NOERROR;
    }

    Float scaledVolume = (Float) volume / maxVol;
    Int32 routeCount;
    GetRouteCount(&routeCount);
    for (Int32 i = 0; i < routeCount; i++) {
       AutoPtr<IMediaRouterRouteInfo> mr;
       GetRouteAt(i, (IMediaRouterRouteInfo**)&mr);
       AutoPtr<MediaRouterRouteInfo> route = (MediaRouterRouteInfo*)mr.Get();
       Int32 vol;
       route->GetVolumeMax(&vol);
       Int32 routeVol = (Int32) (scaledVolume * vol);
       route->RequestSetVolume(routeVol);
    }
    if (volume != mVolume) {
       mVolume = volume;
       mHost->DispatchRouteVolumeChanged(this);
    }
    return NOERROR;
}

ECode MediaRouterRouteGroup::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    Int32 maxVol;
    GetVolumeMax(&maxVol);
    if (maxVol == 0) {
       return NOERROR;
    }//

    Int32 routeCount;
    GetRouteCount(&routeCount);
    Int32 volume = 0;
    for (Int32 i = 0; i < routeCount; i++) {
       AutoPtr<IMediaRouterRouteInfo> r;
       GetRouteAt(i, (IMediaRouterRouteInfo**)&r);
       AutoPtr<MediaRouterRouteInfo> route = (MediaRouterRouteInfo*)r.Get();
       route->RequestUpdateVolume(direction);
       Int32 routeVol;
       route->GetVolume(&routeVol);
       if (routeVol > volume) {
           volume = routeVol;
       }
    }
    if (volume != mVolume) {
       mVolume = volume;
       mHost->DispatchRouteVolumeChanged(this);
    }

    return NOERROR;
}

void MediaRouterRouteGroup::MemberNameChanged(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ ICharSequence* name)
{
    mUpdateName = TRUE;
    RouteUpdated();
}

void MediaRouterRouteGroup::MemberStatusChanged(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ ICharSequence* status)
{
    SetStatusInt(status);
}

void MediaRouterRouteGroup::MemberVolumeChanged(
    /* [in] */ IMediaRouterRouteInfo* info)
{
    UpdateVolume();
}

void MediaRouterRouteGroup::UpdateVolume()
{
    // A group always represents the highest component volume value.
    Int32 routeCount;
    GetRouteCount(&routeCount);
    Int32 volume = 0;
    for (Int32 i = 0; i < routeCount; i++) {
       AutoPtr<IMediaRouterRouteInfo> mr;
       GetRouteAt(i, (IMediaRouterRouteInfo**)&mr);
       Int32 routeVol;
       mr->GetVolume(&routeVol);
       if (routeVol > volume) {
           volume = routeVol;
       }
    }
    if (volume != mVolume) {
       mVolume = volume;
       mHost->DispatchRouteVolumeChanged(this);
    }
}

ECode MediaRouterRouteGroup::RouteUpdated()
{
    Int32 types = 0;
    Int32 count;
    mRoutes->GetSize(&count);
    if (count == 0) {
    // Don't keep empty groups in the router.
    assert(0 && "TODO");
    // return CMediaRouterHelper::RemoveRouteStatic(this);
    }

    Int32 maxVolume = 0;
    Boolean isLocal = TRUE;
    Boolean isFixedVolume = TRUE;
    AutoPtr<IInterface> interf;
    AutoPtr<IMediaRouterRouteInfo> obj;
    AutoPtr<MediaRouterRouteInfo> route;
    for (Int32 i = 0; i < count; i++) {
    interf = NULL;
    obj = NULL;
    mRoutes->Get(i, (IInterface**)&interf);
    obj = IMediaRouterRouteInfo::Probe(interf);
    route = NULL;
    route = (MediaRouterRouteInfo*)obj.Get();
    types |= route->mSupportedTypes;
    Int32 routeMaxVolume;
    route->GetVolumeMax(&routeMaxVolume);
    if (routeMaxVolume > maxVolume) {
       maxVolume = routeMaxVolume;
    }
    Int32 vol;
    isLocal &= (route->GetPlaybackType(&vol), vol) == IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL;
    Int32 handling;
    isFixedVolume &= (route->GetVolumeHandling(&handling), handling) == IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED;
    }
    mPlaybackType = isLocal ? IMediaRouterRouteInfo::PLAYBACK_TYPE_LOCAL : IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE;
    mVolumeHandling = isFixedVolume ? IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED : IMediaRouterRouteInfo::PLAYBACK_VOLUME_VARIABLE;
    mSupportedTypes = types;
    mVolumeMax = maxVolume;
    interf = NULL;
    mRoutes->Get(0, (IInterface**)&interf);
    AutoPtr<IDrawable> dr;
    IMediaRouterRouteInfo::Probe(interf)->GetIconDrawable((IDrawable**)&dr);
    mIcon = count == 1 ? dr : NULL;
    return MediaRouterRouteInfo::RouteUpdated();
}

void MediaRouterRouteGroup::UpdateName()
{
    StringBuilder sb;
    Int32 count;
    mRoutes->GetSize(&count);
    AutoPtr<IInterface> obj;
    AutoPtr<MediaRouterRouteInfo> info;
    for (Int32 i = 0; i < count; i++) {
       mRoutes->Get(i, (IInterface**)&obj);
       info = NULL;
       info = (MediaRouterRouteInfo*)IMediaRouterRouteInfo::Probe(obj);
       // TODO: There's probably a much more correct way to localize this.
       if (i > 0) sb.Append(String(", "));
       sb.Append((info->mName).Get());
    }
    mName = sb.ToCharSequence();
    mUpdateName = FALSE;
}

ECode MediaRouterRouteGroup::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String routeInfoStr;
    MediaRouterRouteInfo::ToString(&routeInfoStr);
    StringBuilder sb(routeInfoStr);
    sb.AppendChar('[');
    Int32 count;
    mRoutes->GetSize(&count);
    AutoPtr<IInterface> obj;
    AutoPtr<IMediaRouterRouteInfo> rf;
    for (Int32 i = 0; i < count; i++) {
       if (i > 0) sb.Append(String(", "));
       rf = NULL;
       mRoutes->Get(i, (IInterface**)&obj);
       rf = NULL;
       rf = IMediaRouterRouteInfo::Probe(obj);
       sb.Append(rf.Get());
    }
    sb.AppendChar(']');
    return sb.ToString(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos