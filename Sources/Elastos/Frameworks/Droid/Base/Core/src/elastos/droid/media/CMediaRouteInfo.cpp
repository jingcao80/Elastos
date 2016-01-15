
#include "elastos/droid/media/CMediaRouteInfo.h"

namespace Elastos {
namespace Droid {
namespace Media {

ECode CMediaRouteInfo::constructor(
    /* [in] */ IRouteCategory* category)
{
    return MediaRouteInfo::Init(category);
}

PInterface CMediaRouteInfo::Probe(
    /* [in] */ REIID riid)
{
    return _CMediaRouteInfo::Probe(riid);
}

ECode CMediaRouteInfo::GetName(
    /* [out] */ ICharSequence** result)
{
    return MediaRouteInfo::GetName(result);
}

ECode CMediaRouteInfo::GetName(
    /* [in] */ IContext* context,
    /* [out] */ ICharSequence** result)
{
    return MediaRouteInfo::GetName(context, result);
}

ECode CMediaRouteInfo::GetStatus(
    /* [out] */ ICharSequence** result)
{
    return MediaRouteInfo::GetStatus(result);
}

ECode CMediaRouteInfo::GetStatusCode(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetStatusCode(result);
}

ECode CMediaRouteInfo::GetSupportedTypes(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetSupportedTypes(result);
}

ECode CMediaRouteInfo::SetGroup(
    /* [in] */ IRouteGroup* group)
{
    return MediaRouteInfo::SetGroup(group);
}

ECode CMediaRouteInfo::GetGroup(
    /* [out] */ IRouteGroup** result)
{
    return MediaRouteInfo::GetGroup(result);
}

ECode CMediaRouteInfo::SetCategory(
    /* [in] */ IRouteCategory* category)
{
    return MediaRouteInfo::SetCategory(category);
}

ECode CMediaRouteInfo::GetCategory(
    /* [out] */ IRouteCategory** result)
{
    return MediaRouteInfo::GetCategory(result);
}

ECode CMediaRouteInfo::GetIconDrawable(
    /* [out] */ IDrawable** result)
{
    return MediaRouteInfo::GetIconDrawable(result);
}

ECode CMediaRouteInfo::SetTag(
    /* [in] */ IInterface* tag)
{
    return MediaRouteInfo::SetTag(tag);
}

ECode CMediaRouteInfo::GetTag(
    /* [out] */ IInterface** result)
{
    return MediaRouteInfo::GetTag(result);
}

ECode CMediaRouteInfo::GetPlaybackType(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetPlaybackType(result);
}

ECode CMediaRouteInfo::GetPlaybackStream(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetPlaybackStream(result);
}

ECode CMediaRouteInfo::GetVolume(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetVolume(result);
}

ECode CMediaRouteInfo::RequestSetVolume(
    /* [in] */ Int32 volume)
{
    return MediaRouteInfo::RequestSetVolume(volume);
}

ECode CMediaRouteInfo::RequestUpdateVolume(
    /* [in] */ Int32 direction)
{
    return MediaRouteInfo::RequestUpdateVolume(direction);
}

ECode CMediaRouteInfo::GetVolumeMax(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetVolumeMax(result);
}

ECode CMediaRouteInfo::GetVolumeHandling(
    /* [out] */ Int32* result)
{
    return MediaRouteInfo::GetVolumeHandling(result);
}

ECode CMediaRouteInfo::GetPresentationDisplay(
    /* [out] */ IDisplay** result)
{
    return MediaRouteInfo::GetPresentationDisplay(result);
}

ECode CMediaRouteInfo::IsEnabled(
    /* [out] */ Boolean* result)
{
    return MediaRouteInfo::IsEnabled(result);
}

ECode CMediaRouteInfo::ToString(
    /* [out] */ String* result)
{
    return MediaRouteInfo::ToString(result);
}

ECode CMediaRouteInfo::SetName(
    /* [in] */ Int32 nameResId)
{
    return MediaRouteInfo::SetName(nameResId);
}

ECode CMediaRouteInfo::SetSupportedTypes(
    /* [in] */ Int32 supportedTypes)
{
    return MediaRouteInfo::SetSupportedTypes(supportedTypes);
}

ECode CMediaRouteInfo::SetPresentationDisplay(
    /* [in] */ IDisplay* presentationDisplay)
{
    return MediaRouteInfo::SetPresentationDisplay(presentationDisplay);
}

ECode CMediaRouteInfo::SetName(
    /* [in] */ ICharSequence* name)
{
    return MediaRouteInfo::SetName(name);
}

ECode CMediaRouteInfo::GetDeviceAddress(
    /* [out] */ String* result)
{
    return MediaRouteInfo::GetDeviceAddress(result);
}

ECode CMediaRouteInfo::SetDeviceAddress(
    /* [in] */ const String& deviceAddress)
{
    return MediaRouteInfo::SetDeviceAddress(deviceAddress);
}

ECode CMediaRouteInfo::SetVolumeHandling(
    /* [in] */ Int32 volumeHandling)
{
    return MediaRouteInfo::SetVolumeHandling(volumeHandling);
}

ECode CMediaRouteInfo::SetPlaybackType(
    /* [in] */ Int32 playbackType)
{
    return MediaRouteInfo::SetPlaybackType(playbackType);
}

ECode CMediaRouteInfo::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return MediaRouteInfo::SetEnabled(enabled);
}

ECode CMediaRouteInfo::GetEnabled(
    /* [out] */ Boolean* result)
{
    return MediaRouteInfo::GetEnabled(result);
}

ECode CMediaRouteInfo::SetStatusCode(
    /* [in] */ Int32 statusCode,
    /* [out] */ Boolean* result)
{
    return MediaRouteInfo::SetStatusCode(statusCode, result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
