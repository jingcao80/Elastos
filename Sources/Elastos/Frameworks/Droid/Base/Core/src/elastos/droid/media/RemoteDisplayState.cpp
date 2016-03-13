#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/media/RemoteDisplayState.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::CArrayList;

namespace Elastos{
namespace Droid {
namespace Media {
//==========================================================================
//          RemoteDisplayState::RemoteDisplayInfo
//==========================================================================
RemoteDisplayState::RemoteDisplayInfo::RemoteDisplayInfo()
{}

RemoteDisplayState::RemoteDisplayInfo::~RemoteDisplayInfo()
{}

CAR_INTERFACE_IMPL_2(RemoteDisplayState::RemoteDisplayInfo, Object, IParcelable, IRemoteDisplayStateRemoteDisplayInfo)

ECode RemoteDisplayState::RemoteDisplayInfo::constructor()
{
	return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::constructor(
	/* [in] */ const String& id)
{
	mId = id;
    mStatus = STATUS_NOT_AVAILABLE;
    mVolumeHandling = PLAYBACK_VOLUME_FIXED;
    mPresentationDisplayId = -1;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::constructor(
	/* [in] */ IRemoteDisplayStateRemoteDisplayInfo* other_)
{
	AutoPtr<RemoteDisplayInfo> other = (RemoteDisplayInfo*)other_;
    mId = other->mId;
    mName = other->mName;
    mDescription = other->mDescription;
    mStatus = other->mStatus;
    mVolume = other->mVolume;
    mVolumeMax = other->mVolumeMax;
    mVolumeHandling = other->mVolumeHandling;
    mPresentationDisplayId = other->mPresentationDisplayId;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::ReadFromParcel(
	/* [in] */ IParcel* in)
{
	VALIDATE_NOT_NULL(in);
    in->ReadString(&mId);
    in->ReadString(&mName);
    in->ReadString(&mDescription);
    in->ReadInt32(&mStatus);
    in->ReadInt32(&mVolume);
    in->ReadInt32(&mVolumeMax);
    in->ReadInt32(&mVolumeHandling);
    in->ReadInt32(&mPresentationDisplayId);
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);
    dest->WriteString(mId);
    dest->WriteString(mName);
    dest->WriteString(mDescription);
    dest->WriteInt32(mStatus);
    dest->WriteInt32(mVolume);
    dest->WriteInt32(mVolumeMax);
    dest->WriteInt32(mVolumeHandling);
    dest->WriteInt32(mPresentationDisplayId);
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::IsValid(
	/* [out] */ Boolean* result)
{
	VALIDATE_NOT_NULL(result);
    *result = !TextUtils::IsEmpty(mId) && !TextUtils::IsEmpty(mName);
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::DescribeContents(
	/* [out] */ Int32* result)
{
	VALIDATE_NOT_NULL(result);
	*result = 0;
	return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::ToString(
	/* [out] */ String* result)
{
	VALIDATE_NOT_NULL(result);
    *result = String("RemoteDisplayInfo{ id=") + mId
                + ", name=" + mName
                + ", description=" + mDescription
                + ", status=" + mStatus
                + ", volume=" + mVolume
                + ", volumeMax=" + mVolumeMax
                + ", volumeHandling=" + mVolumeHandling
                + ", presentationDisplayId=" + mPresentationDisplayId
                + " }";

    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id)
    *id = mId;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::SetId(
    /* [in] */ const String& id)
{
    mId = id;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::GetDescription(
    /* [out] */ String* description)
{
    VALIDATE_NOT_NULL(description)
    *description = mDescription;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::SetDescription(
    /* [in] */ const String& description)
{
    mDescription = description;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::GetStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status)
    *status = mStatus;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::SetStatus(
    /* [in] */ Int32 status)
{
    mStatus = status;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::GetVolume(
    /* [out] */ Int32* volume)
{
    VALIDATE_NOT_NULL(volume)
    *volume = mVolume;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::SetVolume(
    /* [in] */ Int32 volume)
{
    mVolume = volume;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::GetVolumeMax(
    /* [out] */ Int32* volumeMax)
{
    VALIDATE_NOT_NULL(volumeMax)
    *volumeMax = mVolumeMax;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::SetVolumeMax(
    /* [in] */ Int32 volumeMax)
{
    mVolumeMax = volumeMax;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::GetVolumeHandling(
    /* [out] */ Int32* volumeHandling)
{
    VALIDATE_NOT_NULL(volumeHandling)
    *volumeHandling = mVolumeHandling;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::SetVolumeHandling(
    /* [in] */ Int32 volumeHandling)
{
    mVolumeHandling = volumeHandling;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::GetPresentationDisplayId(
    /* [out] */ Int32* presentationDisplayId)
{
    VALIDATE_NOT_NULL(presentationDisplayId)
    *presentationDisplayId = mPresentationDisplayId;
    return NOERROR;
}

ECode RemoteDisplayState::RemoteDisplayInfo::SetPresentationDisplayId(
    /* [in] */ Int32 presentationDisplayId)
{
    mPresentationDisplayId = presentationDisplayId;
    return NOERROR;
}


//==========================================================================
//          RemoteDisplayState
//==========================================================================
RemoteDisplayState::RemoteDisplayState()
{}

RemoteDisplayState::~RemoteDisplayState()
{}

CAR_INTERFACE_IMPL_2(RemoteDisplayState, Object, IRemoteDisplayState, IParcelable)

ECode RemoteDisplayState::constructor()
{
    FAIL_RETURN(CArrayList::New((IArrayList**)&mDisplays));
    return NOERROR;
}

ECode RemoteDisplayState::ReadFromParcel(
	/* [in] */ IParcel* src)
{
	VALIDATE_NOT_NULL(src);
    return src->ReadInterfacePtr((Handle32*)&mDisplays);
}

ECode RemoteDisplayState::IsValid(
	/* [out] */ Boolean* result)
{
	VALIDATE_NOT_NULL(result);

    if (mDisplays == NULL) {
    	*result = FALSE;
        return NOERROR;
    }
    Int32 count;
    mDisplays->GetSize(&count);
    Boolean flag = FALSE;
    for (Int32 i = 0; i < count; i++) {
    	AutoPtr<IInterface> obj;
    	mDisplays->Get(i, (IInterface**)&obj);
    	AutoPtr<IParcelable> pl = IParcelable::Probe(obj);
    	AutoPtr<RemoteDisplayInfo> rd = (RemoteDisplayInfo*)pl.Get();
    	rd->IsValid(&flag);
        if (!flag) {
        	*result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;

}

ECode RemoteDisplayState::DescribeContents(
	/* [out] */ Int32* result)
{
	VALIDATE_NOT_NULL(result);
	*result = 0;
	return NOERROR;
}

ECode RemoteDisplayState::WriteToParcel(
	/* [in] */ IParcel* dest)
{
	VALIDATE_NOT_NULL(dest);
    return dest->WriteInterfacePtr(IObject::Probe(mDisplays));
}

ECode RemoteDisplayState::GetDisplays(
    /* [out] */ IArrayList** displays)
{
    VALIDATE_NOT_NULL(displays);
    *displays = mDisplays;
    REFCOUNT_ADD(*displays);
    return NOERROR;
}

ECode RemoteDisplayState::SetDisplays(
    /* [in] */ IArrayList* displays)
{
    mDisplays = displays;
    return NOERROR;
}

} // namespace Elastos
} // namespace Droid
} // namespace Media