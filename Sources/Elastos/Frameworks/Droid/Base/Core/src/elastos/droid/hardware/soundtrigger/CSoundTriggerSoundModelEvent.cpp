
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerSoundModelEvent.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL_2(CSoundTriggerSoundModelEvent, Object, ISoundTriggerSoundModelEvent, IParcelable)

CAR_OBJECT_IMPL(CSoundTriggerSoundModelEvent)

CSoundTriggerSoundModelEvent::CSoundTriggerSoundModelEvent()
    : mStatus(0)
    , mSoundModelHandle(0)
    , mData(NULL)
{
}

ECode CSoundTriggerSoundModelEvent::constructor()
{
    return NOERROR;
}

ECode CSoundTriggerSoundModelEvent::constructor(
    /* [in] */ Int32 status,
    /* [in] */ Int32 soundModelHandle,
    /* [in] */ ArrayOf<Byte>* data)
{
    mStatus = status;
    mSoundModelHandle = soundModelHandle;
    mData = data;
    return NOERROR;
}

ECode CSoundTriggerSoundModelEvent::FromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ ISoundTriggerSoundModelEvent** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 status;
    FAIL_RETURN(source->ReadInt32(&status))
    Int32 soundModelHandle;
    FAIL_RETURN(source->ReadInt32(&soundModelHandle))
    AutoPtr<ArrayOf<Byte> > data;
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&data))

    AutoPtr<CSoundTriggerSoundModelEvent> newResult;
    CSoundTriggerSoundModelEvent::NewByFriend(status, soundModelHandle, data, (CSoundTriggerSoundModelEvent**)&newResult);
    *result = newResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSoundTriggerSoundModelEvent::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteInt32(mStatus))
    FAIL_RETURN(dest->WriteInt32(mSoundModelHandle))
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mData.Get()))
    return NOERROR;
}

ECode CSoundTriggerSoundModelEvent::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mStatus))
    FAIL_RETURN(source->ReadInt32(&mSoundModelHandle))
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mData))
    return NOERROR;
}

ECode CSoundTriggerSoundModelEvent::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal);

    if (TO_IINTERFACE(this) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }

    ClassID clsid;
    IObject::Probe(obj)->GetClassID(&clsid);
    if (ECLSID_CSoundTriggerSoundModelEvent != clsid) {
        *equal = FALSE;
        return NOERROR;
    }

    const AutoPtr<CSoundTriggerSoundModelEvent> other = (CSoundTriggerSoundModelEvent*)ISoundTriggerSoundModelEvent::Probe(obj);
    if (!Arrays::Equals(mData, other->mData)) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mSoundModelHandle != other->mSoundModelHandle) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mStatus != other->mStatus) {
        *equal = FALSE;
        return NOERROR;
    }
    *equal = TRUE;
    return NOERROR;
}

ECode CSoundTriggerSoundModelEvent::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    const Int32 prime = 31;
    Int32 result = 1;
    result = prime * result + Arrays::GetHashCode(mData);
    result = prime * result + mSoundModelHandle;
    result = prime * result + mStatus;
    *hashCode = result;
    return NOERROR;
}

ECode CSoundTriggerSoundModelEvent::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "SoundModelEvent [status=";
    sb += mStatus;
    sb += ", soundModelHandle=";
    sb += mSoundModelHandle;
    sb += ", data=";
    sb +=  (mData == NULL ? 0 : mData->GetLength());
    sb += "]";

    return sb.ToString(str);
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos
