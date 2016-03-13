#ifndef _ELASTOS_DROID_MEDIA_RemoteDisplayState_H__
#define _ELASTOS_DROID_MEDIA_RemoteDisplayState_H__

#include "Elastos.Droid.Media.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IArrayList;

namespace Elastos{
namespace Droid {
namespace Media {

class RemoteDisplayState
	: public Object
	, public IRemoteDisplayState
	, public IParcelable
{
public:
	class RemoteDisplayInfo
		: public Object
		, public IParcelable
		, public IRemoteDisplayStateRemoteDisplayInfo
	{
	public:
		RemoteDisplayInfo();

		virtual ~RemoteDisplayInfo();

		CAR_INTERFACE_DECL()

		CARAPI constructor();

		CARAPI constructor(
			/* [in] */ const String& id);

		CARAPI constructor(
			/* [in] */ IRemoteDisplayStateRemoteDisplayInfo* other_);

		CARAPI WriteToParcel(
			/* [in] */ IParcel* dest);

		CARAPI ReadFromParcel(
			/* [in] */ IParcel* in);

		CARAPI IsValid(
			/* [out] */ Boolean* result);

		CARAPI DescribeContents(
			/* [out] */ Int32* result);

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

        CARAPI GetStatus(
            /* [out] */ Int32* status);

        CARAPI SetStatus(
            /* [in] */ Int32 status);

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
        Int32 mStatus;
        Int32 mVolume;
        Int32 mVolumeMax;
        Int32 mVolumeHandling;
        Int32 mPresentationDisplayId;
	};

public:
	RemoteDisplayState();

	virtual ~RemoteDisplayState();

	CAR_INTERFACE_DECL()

	CARAPI constructor();

	CARAPI WriteToParcel(
		/* [in] */ IParcel* dest);

	CARAPI ReadFromParcel(
		/* [in] */ IParcel* src);

	CARAPI IsValid(
		/* [out] */ Boolean* result);

	CARAPI DescribeContents(
		/* [out] */ Int32* result);

    CARAPI GetDisplays(
        /* [out] */ IArrayList** displays);

    CARAPI SetDisplays(
        /* [in] */ IArrayList* displays);

public:
    /**
     * A list of all remote displays.
     */
    AutoPtr<IArrayList> mDisplays;
};

} // namespace Elastos
} // namespace Droid
} // namespace Media

#endif // _ELASTOS_DROID_MEDIA_RemoteDisplayState_H__