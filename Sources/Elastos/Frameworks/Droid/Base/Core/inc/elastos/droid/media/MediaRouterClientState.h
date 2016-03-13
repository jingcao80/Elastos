#ifndef _ELASTOS_DROID_MEDIA_MEDIAROUTERCLIENTSTATE_H__
#define _ELASTOS_DROID_MEDIA_MEDIAROUTERCLIENTSTATE_H__

#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;

namespace Elastos{
namespace Droid {
namespace Media {

class MediaRouterClientState
	: public Object
	, public IMediaRouterClientState
	, public IParcelable
{
public:
	class RouteInfo
		: public Object
		, public IMediaRouterClientStateRouteInfo
		, public IParcelable
	{
	public:
		RouteInfo();

		virtual ~RouteInfo();

		CAR_INTERFACE_DECL()

        CARAPI constructor();

		CARAPI constructor(
			/* [in] */ const String& id);

		CARAPI constructor(
			/* [in] */ IMediaRouterClientStateRouteInfo* other);

		CARAPI DescribeContents(
			/* [out] */ Int32* result);

        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

		CARAPI ToString(
			/* [out] */ String* result);

        CARAPI GetId(
            /* [out] */ String* id);

        CARAPI SetId(
            /* [in] */ const String& id);

        CARAPI GetName(
            /* [out] */ String* name);

        CARAPI SetName(
            /* [in] */ const String& name);

        CARAPI GetDescription(
            /* [out] */ String* description);

        CARAPI SetDescription(
            /* [in] */ const String& description);

        CARAPI GetSupportedTypes(
            /* [out] */ Int32* supportedTypes);

        CARAPI SetSupportedTypes(
            /* [in] */ Int32 supportedTypes);

        CARAPI GetEnabled(
            /* [out] */ Boolean* enabled);

        CARAPI SetEnabled(
            /* [in] */ Boolean enabled);

        CARAPI GetStatusCode(
            /* [out] */ Int32* statusCode);

        CARAPI SetStatusCode(
            /* [in] */ Int32 statusCode);

        CARAPI GetPlaybackType(
            /* [out] */ Int32* playbackType);

        CARAPI SetPlaybackType(
            /* [in] */ Int32 playbackType);

        CARAPI GetPlaybackStream(
            /* [out] */ Int32* playbackStream);

        CARAPI SetPlaybackStream(
            /* [in] */ Int32 playbackStream);

        CARAPI GetVolume(
            /* [out] */ Int32* volume);

        CARAPI SetVolume(
            /* [in] */ Int32 volume);

        CARAPI GetVolumeMax(
            /* [out] */ Int32* volumeMax);

        CARAPI SetVolumeMax(
            /* [in] */ Int32 volumeMax);

        CARAPI GetVolumeHandling(
            /* [out] */ Int32* volumeHandling);

        CARAPI SetVolumeHandling(
            /* [in] */ Int32 volumeHandling);

        CARAPI GetPresentationDisplayId(
            /* [out] */ Int32* presentationDisplayId);

        CARAPI SetPresentationDisplayId(
            /* [in] */ Int32 presentationDisplayId);

	public:
        String mId;
        String mName;
        String mDescription;
        Int32 mSupportedTypes;
        Boolean mEnabled;
        Int32 mStatusCode;
        Int32 mPlaybackType;
        Int32 mPlaybackStream;
        Int32 mVolume;
        Int32 mVolumeMax;
        Int32 mVolumeHandling;
        Int32 mPresentationDisplayId;
	};

public:
	MediaRouterClientState();

	virtual ~MediaRouterClientState();

	CAR_INTERFACE_DECL()

	CARAPI constructor();

	CARAPI GetRoute(
		/* [in] */ const String& id,
		/* [out] */ IMediaRouterClientStateRouteInfo** result);

	CARAPI DescribeContents(
		/* [out] */ Int32* result);

	CARAPI ReadFromParcel(
		/* [in] */ IParcel* src);

	CARAPI WriteToParcel(
		/* [in] */ IParcel* dest,
		/* [in] */ Int32 flags);

	//override for compile
	CARAPI WriteToParcel(
    	/* [in] */ IParcel* source);

	CARAPI ToString(
		/* [out] */ String* result);

    CARAPI GetGloballySelectedRouteId(
        /* [out] */ String* routeId);

    CARAPI SetGloballySelectedRouteId(
        /* [in] */ const String& routeId);

    CARAPI GetRoutes(
        /* [out] */ IArrayList** routes);

    CARAPI SetRoutes(
        /* [in] */ IArrayList* routes);

public:
	AutoPtr<IArrayList> mRoutes;
	String mGloballySelectedRouteId;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_MEDIAROUTERCLIENTSTATE_H__