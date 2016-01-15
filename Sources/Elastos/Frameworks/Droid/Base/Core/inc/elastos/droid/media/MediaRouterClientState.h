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

public:
	AutoPtr<IArrayList> mRoutes;
	String mGloballySelectedRouteId;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_MEDIAROUTERCLIENTSTATE_H__