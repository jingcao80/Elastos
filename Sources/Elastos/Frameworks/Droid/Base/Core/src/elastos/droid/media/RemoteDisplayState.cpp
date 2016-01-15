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
                + String(", name=") + mName
                + String(", description=") + mDescription
                + String(", status=") + mStatus
                + String(", volume=") + mVolume
                + String(", volumeMax=") + mVolumeMax
                + String(", volumeHandling=") + mVolumeHandling
                + String(", presentationDisplayId=") + mPresentationDisplayId
                + String(" }");

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