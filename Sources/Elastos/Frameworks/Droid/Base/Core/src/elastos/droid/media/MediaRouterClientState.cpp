#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/media/MediaRouterClientState.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Utility::CArrayList;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos{
namespace Droid {
namespace Media {
//====================================================================
//          MediaRouterClientState::RouteInfo
//====================================================================
MediaRouterClientState::RouteInfo::RouteInfo()
{}

MediaRouterClientState::RouteInfo::~RouteInfo()
{}

CAR_INTERFACE_IMPL_2(MediaRouterClientState::RouteInfo, Object, IMediaRouterClientStateRouteInfo, IParcelable)

ECode MediaRouterClientState::RouteInfo::constructor()
{
    return NOERROR;
}

ECode MediaRouterClientState::RouteInfo::constructor(
    /* [in] */ const String& id)
{
    mId = id;
    mEnabled = TRUE;
    mStatusCode = IMediaRouterRouteInfo::STATUS_NONE;
    mPlaybackType = IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE;
    mPlaybackStream = -1;
    mVolumeHandling = IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED;
    mPresentationDisplayId = -1;
    return NOERROR;
}


ECode MediaRouterClientState::RouteInfo::constructor(
    /* [in] */ IMediaRouterClientStateRouteInfo* other)
{
    AutoPtr<RouteInfo> r = (RouteInfo*)other;
    mId = r->mId;
    mName = r->mName;
    mDescription = r->mDescription;
    mSupportedTypes = r->mSupportedTypes;
    mEnabled = r->mEnabled;
    mStatusCode = r->mStatusCode;
    mPlaybackType = r->mPlaybackType;
    mPlaybackStream = r->mPlaybackStream;
    mVolume = r->mVolume;
    mVolumeHandling = r->mVolumeHandling;
    mPresentationDisplayId = r->mPresentationDisplayId;
    return NOERROR;
}

ECode MediaRouterClientState::RouteInfo::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    VALIDATE_NOT_NULL(in);
    in->ReadString(&mId);
    in->ReadString(&mName);
    in->ReadString(&mDescription);
    in->ReadInt32(&mSupportedTypes);
    Int32 value;
    mEnabled = (in->ReadInt32(&value), value != 0);
    in->ReadInt32(&mStatusCode);
    in->ReadInt32(&mPlaybackType);
    in->ReadInt32(&mPlaybackStream);
    in->ReadInt32(&mVolume);
    in->ReadInt32(&mVolumeMax);
    in->ReadInt32(&mVolumeHandling);
    in->ReadInt32(&mPresentationDisplayId);
    return NOERROR;
}

ECode MediaRouterClientState::RouteInfo::DescribeContents(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode MediaRouterClientState::RouteInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteString(mId);
    dest->WriteString(mName);
    dest->WriteString(mDescription);
    dest->WriteInt32(mSupportedTypes);
    dest->WriteInt32(mEnabled ? 1 : 0);
    dest->WriteInt32(mStatusCode);
    dest->WriteInt32(mPlaybackType);
    dest->WriteInt32(mPlaybackStream);
    dest->WriteInt32(mVolume);
    dest->WriteInt32(mVolumeMax);
    dest->WriteInt32(mVolumeHandling);
    dest->WriteInt32(mPresentationDisplayId);
    return NOERROR;
}

ECode MediaRouterClientState::RouteInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb;
    sb += "RouteInfo{ id=";
    sb += mId;
    sb += ", name=";
    sb += mName;
    sb += ", description=";
    sb += mDescription;
    sb += ", supportedTypes=0x";
    sb += StringUtils::ToHexString(mSupportedTypes);
    sb += ", enabled=";
    sb += mEnabled;
    sb += ", statusCode=";
    sb += mStatusCode;
    sb += ", playbackType=";
    sb += mPlaybackType;
    sb += ", playbackStream=";
    sb += mPlaybackStream;
    sb += ", volume=";
    sb += mVolume;
    sb += ", volumeMax=";
    sb += mVolumeMax;
    sb += ", volumeHandling=";
    sb += mVolumeHandling;
    sb += ", presentationDisplayId=";
    sb += mPresentationDisplayId;
    sb += " }";
    *result = sb.ToString();
    return NOERROR;
}

//====================================================================
//          MediaRouterClientState
//====================================================================
MediaRouterClientState::MediaRouterClientState()
{}

MediaRouterClientState::~MediaRouterClientState()
{}

CAR_INTERFACE_IMPL_2(MediaRouterClientState, Object, IMediaRouterClientState, IParcelable)

ECode MediaRouterClientState::constructor()
{
    CArrayList::New((IArrayList**)&mRoutes);
    return NOERROR;
}

ECode MediaRouterClientState::GetRoute(
	/* [in] */ const String& id,
	/* [out] */ IMediaRouterClientStateRouteInfo** result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count;
    mRoutes->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        mRoutes->Get(i, (IInterface**)&obj);
        AutoPtr<RouteInfo> route = (RouteInfo*)IMediaRouterClientStateRouteInfo::Probe(obj);
        if (route->mId.Equals(id)) {
            *result = route;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    *result = NULL;
    return NOERROR;
}

ECode MediaRouterClientState::DescribeContents(
	/* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode MediaRouterClientState::ReadFromParcel(
    /* [in] */ IParcel* src)
{
    // routes = src.createTypedArrayList(RouteInfo.CREATOR);
    src->ReadInterfacePtr((Handle32*)&mRoutes);
    return src->ReadString(&mGloballySelectedRouteId);
}

ECode MediaRouterClientState::WriteToParcel(
	/* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteInterfacePtr(mRoutes.Get());
    return dest->WriteString(mGloballySelectedRouteId);
}

ECode MediaRouterClientState::ToString(
	/* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String str;
    IObject::Probe(mRoutes)->ToString(&str);
    *result = String("MediaRouterClientState{ globallySelectedRouteId=")
                + mGloballySelectedRouteId
                + String(", routes=")
                + str.string()
                + String(" }");
    return NOERROR;
}

} // namespace Elastos
} // namespace Droid
} // namespace Media