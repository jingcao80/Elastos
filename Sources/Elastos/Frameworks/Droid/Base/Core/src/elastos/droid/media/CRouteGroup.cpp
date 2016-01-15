
#include "elastos/droid/media/CRouteGroup.h"

#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Media {

const String CRouteGroup::TAG("CRouteGroup");

CRouteGroup::CRouteGroup()
    : mUpdateName(FALSE)
{
}

ECode CRouteGroup::constructor(
    /* [in] */ IRouteCategory* category)
{
    MediaRouteInfo::Init(category);
    mGroup = this;
    mVolumeHandling = IRouteInfo::PLAYBACK_VOLUME_FIXED;
    return NOERROR;
}

PInterface CRouteGroup::Probe(
    /* [in] */ REIID riid)
{
    return _CRouteGroup::Probe(riid);
}

ECode CRouteGroup::AddRoute(
    /* [in] */ IRouteInfo* route)
{
    VALIDATE_NOT_NULL(route);

    AutoPtr<IRouteGroup> routeGroup;
    route->GetGroup((IRouteGroup**)&routeGroup);
    if (routeGroup != NULL) {
        String info;
        route->ToString(&info);
        Logger::E(TAG, "Route %s is already part of a group.", info.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IRouteCategory> routeCategory;
    route->GetCategory((IRouteCategory**)&routeCategory);
    if (routeCategory != mCategory) {
        String lhs, rhs;
        if (routeCategory) routeCategory->ToString(&lhs);
        if (mCategory) mCategory->ToString(&rhs);
        Logger::E(TAG, "Route cannot be added to a group with a different category. (Route category=%s group category=%s",
            lhs.string(), rhs.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 at = mRoutes.GetSize();
    mRoutes.PushBack(route);
    routeGroup = THIS_PROBE(IRouteGroup);
    mUpdateName = TRUE;
    UpdateVolume();
    RouteUpdated();
    CMediaRouter::DispatchRouteGrouped(route, this, at);
    return NOERROR;
}

ECode CRouteGroup::AddRoute(
    /* [in] */ IRouteInfo* route,
    /* [in] */ Int32 insertAt)
{
    VALIDATE_NOT_NULL(route);

    AutoPtr<IRouteGroup> routeGroup;
    route->GetGroup((IRouteGroup**)&routeGroup);
    if (routeGroup != NULL) {
        String info;
        route->ToString(&info);
        Logger::E(TAG, "Route %s is already part of a group.", info.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IRouteCategory> routeCategory;
    route->GetCategory((IRouteCategory**)&routeCategory);
    if (routeCategory != mCategory) {
        String lhs, rhs;
        if (routeCategory) routeCategory->ToString(&lhs);
        if (mCategory) mCategory->ToString(&rhs);
        Logger::E(TAG, "Route cannot be added to a group with a different category. (Route category=%s group category=%s",
            lhs.string(), rhs.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mRoutes.Insert(insertAt, route);
    routeGroup = THIS_PROBE(IRouteGroup);
    mUpdateName = TRUE;
    UpdateVolume();
    RouteUpdated();
    CMediaRouter::DispatchRouteGrouped(route, THIS_PROBE(IRouteGroup), insertAt);
    return NOERROR;
}

ECode CRouteGroup::RemoveRoute(
    /* [in] */ IRouteInfo* route)
{
    VALIDATE_NOT_NULL(route);

    AutoPtr<IRouteGroup> routeGroup;
    route->GetGroup((IRouteGroup**)&routeGroup);
    if (routeGroup.Get() != THIS_PROBE(IRouteGroup)) {
        String info;
        route->ToString(&info);
        Logger::E(TAG, "Route %s is not a member of this group.", info.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IRouteInfo> temp(route);
    mRoutes.Remove(temp);
    routeGroup = NULL;
    mUpdateName = TRUE;
    UpdateVolume();
    CMediaRouter::DispatchRouteUngrouped(route, THIS_PROBE(IRouteGroup));
    RouteUpdated();
    return NOERROR;
}

ECode CRouteGroup::RemoveRoute(
    /* [in] */ Int32 index)
{
    if (index < 0 || index >= mRoutes.GetSize())
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    List< AutoPtr<IRouteInfo> >::Iterator it = mRoutes.Begin();
    for (Int32 i = 0; i <= index && it != mRoutes.End(); ++i) {
        ++it;
    }

    if (it != mRoutes.End()) {
        AutoPtr<IRouteInfo> route = *it;
        mRoutes.Erase(it);
        route->SetGroup(NULL);

        mUpdateName = TRUE;
        UpdateVolume();
        CMediaRouter::DispatchRouteUngrouped(route, THIS_PROBE(IRouteGroup));
        RouteUpdated();
    }

    return NOERROR;
}

ECode CRouteGroup::GetRouteCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRoutes.GetSize();
    return NOERROR;
}

ECode CRouteGroup::GetRouteAt(
    /* [in] */ Int32 index,
    /* [out] */ IRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (index < 0 || index >= mRoutes.GetSize())
        return E_ILLEGAL_ARGUMENT_EXCEPTION;

    *result = mRoutes[index];
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRouteGroup::SetIconDrawable(
    /* [in] */ IDrawable* icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode CRouteGroup::SetIconResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IDrawable> drawable;
    CMediaRouter::sStatic->mResources->GetDrawable(resId, (IDrawable**)&drawable);
    SetIconDrawable(drawable);
    return NOERROR;
}

ECode CRouteGroup::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    Int32 maxVol;
    MediaRouteInfo::GetVolumeMax(&maxVol);
    if (maxVol == 0) {
        return NOERROR;
    }

    Float scaledVolume = (Float) volume / maxVol;
    Int32 routeCount, tempValue, routeVol;
    GetRouteCount(&routeCount);
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IRouteInfo> route;
        GetRouteAt(i, (IRouteInfo**)&route);
        route->GetVolumeMax(&tempValue);
        routeVol = (Int32) (scaledVolume * tempValue);
        route->RequestSetVolume(routeVol);
    }

    if (volume != mVolume) {
        mVolume = volume;
        CMediaRouter::DispatchRouteVolumeChanged(THIS_PROBE(IRouteInfo));
    }
    return NOERROR;
}

ECode CRouteGroup::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    Int32 maxVol;
    MediaRouteInfo::GetVolumeMax(&maxVol);
    if (maxVol == 0) {
        return NOERROR;
    }

    Int32 routeCount;
    GetRouteCount(&routeCount);
    Int32 volume = 0, routeVol;
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IRouteInfo> route;
        GetRouteAt(i, (IRouteInfo**)&route);
        route->RequestUpdateVolume(direction);
        route->GetVolume(&routeVol);
        if (routeVol > volume) {
            volume = routeVol;
        }
    }

    if (volume != mVolume) {
        mVolume = volume;
        CMediaRouter::DispatchRouteVolumeChanged(THIS_PROBE(IRouteInfo));
    }
    return NOERROR;
}

ECode CRouteGroup::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    String tempText;
    MediaRouteInfo::ToString(&tempText);
    StringBuilder sb(tempText);

    sb += '[';
    String info;
    IRouteInfo* route;
    List< AutoPtr<IRouteInfo> >::Iterator it = mRoutes.Begin();
    for (; it != mRoutes.End(); ++it) {
        if (it != mRoutes.Begin()) {
            sb += ", ";
        }

        route = *it;
        if (route) {
            route->ToString(&info);
            sb += info;
        }
        else {
            sb += "NULL";
        }
    }
    sb += ']';
    sb.ToString(result);
    return NOERROR;
}

ECode CRouteGroup::MemberStatusChanged(
    /* [in] */ IRouteInfo* info,
    /* [in] */ ICharSequence* status)
{
    MediaRouteInfo::SetStatusInt(status);
    return NOERROR;
}

ECode CRouteGroup::MemberVolumeChanged(
    /* [in] */ IRouteInfo* info)
{
    UpdateVolume();
    return NOERROR;
}

AutoPtr<ICharSequence> CRouteGroup::GetName(
    /* [in] */ IResources* res)
{
    if (mUpdateName) {
        UpdateName();
    }
    return MediaRouteInfo::GetName(res);
}

ECode CRouteGroup::MemberNameChanged(
    /* [in] */ IRouteInfo* info,
    /* [in] */ ICharSequence* name)
{
    mUpdateName = TRUE;
    RouteUpdated();
    return NOERROR;
}

void CRouteGroup::UpdateVolume()
{
    // A group always represents the highest component volume value.
    Int32 routeCount;
    GetRouteCount(&routeCount);
    Int32 volume = 0, routeVol;
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IRouteInfo> route;
        GetRouteAt(i, (IRouteInfo**)&route);
        route->GetVolume(&routeVol);
        if (routeVol > volume) {
            volume = routeVol;
        }
    }
    if (volume != mVolume) {
        mVolume = volume;
        CMediaRouter::DispatchRouteVolumeChanged(THIS_PROBE(IRouteInfo));
    }
}

void CRouteGroup::RouteUpdated()
{
    Int32 types = 0;
    if (mRoutes.IsEmpty()) {
        // Don't keep empty groups in the router.
        CMediaRouter::RemoveRoute(THIS_PROBE(IRouteInfo));
        return;
    }

    Int32 maxVolume = 0, tempValue, routeMaxVolume;
    Boolean isLocal = TRUE;
    Boolean isFixedVolume = TRUE;
    List< AutoPtr<IRouteInfo> >::Iterator it = mRoutes.Begin();
    for (; it != mRoutes.End(); ++it) {
        AutoPtr<IRouteInfo> route = *it;
        route->GetSupportedTypes(&tempValue);
        types |= tempValue;
        route->GetVolumeMax(&tempValue);
        routeMaxVolume = tempValue;
        if (routeMaxVolume > maxVolume) {
            maxVolume = routeMaxVolume;
        }
        isLocal &= (route->GetPlaybackType(&tempValue), tempValue) == IRouteInfo::PLAYBACK_TYPE_LOCAL;
        isFixedVolume &= (route->GetVolumeHandling(&tempValue), tempValue) == IRouteInfo::PLAYBACK_VOLUME_FIXED;
    }

    mPlaybackType = isLocal ? IRouteInfo::PLAYBACK_TYPE_LOCAL : IRouteInfo::PLAYBACK_TYPE_REMOTE;
    mVolumeHandling = isFixedVolume ? IRouteInfo::PLAYBACK_VOLUME_FIXED : IRouteInfo::PLAYBACK_VOLUME_VARIABLE;
    mSupportedTypes = types;
    mVolumeMax = maxVolume;
    AutoPtr<IDrawable> drawable;
    mIcon = mRoutes.GetSize() == 1 ? (mRoutes[0]->GetIconDrawable((IDrawable**)&drawable), drawable) : NULL;
    MediaRouteInfo::RouteUpdated();
}

void CRouteGroup::UpdateName()
{
    StringBuilder sb;
    List< AutoPtr<IRouteInfo> >::Iterator it = mRoutes.Begin();
    for (; it != mRoutes.End(); ++it) {
        AutoPtr<IRouteInfo> info = *it;
        if (it != mRoutes.Begin()) {
            sb += ", ";
        }
        AutoPtr<ICharSequence> name;
        info->GetName((ICharSequence**)&name);
        sb += name;
    }
    String tempText;
    sb.ToString(&tempText);
    AutoPtr<ICharSequence> charSequence;
    CString::New(tempText, (ICharSequence**)&charSequence);
    mName = charSequence;
    mUpdateName = FALSE;
}


//-------------------------------
//    MediaRouteInfo
//-------------------------------

ECode CRouteGroup::GetName(
    /* [out] */ ICharSequence** result)
{
    return MediaRouteInfo::GetName(result);
}

ECode CRouteGroup::GetName(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    return MediaRouteInfo::GetName(context, result);
}

ECode CRouteGroup::GetStatus(
    /* [out] */ ICharSequence** result)
{
    return MediaRouteInfo::GetStatus(result);
}

ECode CRouteGroup::GetStatusCode(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetStatusCode(result);
}

ECode CRouteGroup::GetSupportedTypes(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetSupportedTypes(result);
}

ECode CRouteGroup::SetGroup(
    /* [in] */ IRouteGroup* group)
{
    return MediaRouteInfo::SetGroup(group);
}

ECode CRouteGroup::GetGroup(
    /* [out] */ IRouteGroup** result)
{
    return MediaRouteInfo::GetGroup(result);
}

ECode CRouteGroup::SetCategory(
    /* [in] */ IRouteCategory* category)
{
    return MediaRouteInfo::SetCategory(category);
}

ECode CRouteGroup::GetCategory(
    /* [out] */ IRouteCategory** result)
{
    return MediaRouteInfo::GetCategory(result);
}

ECode CRouteGroup::GetIconDrawable(
    /* [out] */ IDrawable** result)
{
    return MediaRouteInfo::GetIconDrawable(result);
}

ECode CRouteGroup::SetTag(
    /* [in] */ IInterface* tag)
{
    return MediaRouteInfo::SetTag(tag);
}

ECode CRouteGroup::GetTag(
    /* [out] */ IInterface** result)
{
    return MediaRouteInfo::GetTag(result);
}

ECode CRouteGroup::GetPlaybackType(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetPlaybackType(result);
}

ECode CRouteGroup::GetPlaybackStream(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetPlaybackStream(result);
}

ECode CRouteGroup::GetVolume(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetVolume(result);
}

ECode CRouteGroup::GetVolumeMax(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetVolumeMax(result);
}

ECode CRouteGroup::GetVolumeHandling(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetVolumeHandling(result);
}

ECode CRouteGroup::GetPresentationDisplay(
    /* [out] */ IDisplay** result)
{
    return MediaRouteInfo::GetPresentationDisplay(result);
}

ECode CRouteGroup::IsEnabled(
    /* [out] */ Boolean* result)
{
    return MediaRouteInfo::IsEnabled(result);
}

ECode CRouteGroup::SetName(
    /* [in] */ Int32 nameResId)
{
    return MediaRouteInfo::SetName(nameResId);
}

ECode CRouteGroup::SetSupportedTypes(
    /* [in] */ Int32 supportedTypes)
{
    return MediaRouteInfo::SetSupportedTypes(supportedTypes);
}

ECode CRouteGroup::SetPresentationDisplay(
    /* [in] */ IDisplay* presentationDisplay)
{
    return MediaRouteInfo::SetPresentationDisplay(presentationDisplay);
}

ECode CRouteGroup::SetName(
    /* [in] */ ICharSequence* name)
{
    return MediaRouteInfo::SetName(name);
}

ECode CRouteGroup::GetDeviceAddress(
    /* [out] */ String* result)
{
    return MediaRouteInfo::GetDeviceAddress(result);
}

ECode CRouteGroup::SetDeviceAddress(
    /* [in] */ const String& deviceAddress)
{
    return MediaRouteInfo::SetDeviceAddress(deviceAddress);
}

ECode CRouteGroup::SetVolumeHandling(
    /* [in] */ Int32 volumeHandling)
{
    return MediaRouteInfo::SetVolumeHandling(volumeHandling);
}

ECode CRouteGroup::SetPlaybackType(
    /* [in] */ Int32 playbackType)
{
    return MediaRouteInfo::SetPlaybackType(playbackType);
}

ECode CRouteGroup::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return MediaRouteInfo::SetEnabled(enabled);
}

ECode CRouteGroup::GetEnabled(
    /* [out] */ Boolean* result)
{
    return MediaRouteInfo::GetEnabled(result);
}

ECode CRouteGroup::SetStatusCode(
    /* [in] */ Int32 statusCode,
    /* [out] */ Boolean* result)
{
    return MediaRouteInfo::SetStatusCode(statusCode, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
