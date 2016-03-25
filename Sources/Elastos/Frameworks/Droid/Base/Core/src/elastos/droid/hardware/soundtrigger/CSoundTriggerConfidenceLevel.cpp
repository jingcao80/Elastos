
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerConfidenceLevel.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL_2(CSoundTriggerConfidenceLevel, Object, ISoundTriggerConfidenceLevel, IParcelable)

CAR_OBJECT_IMPL(CSoundTriggerConfidenceLevel)

CSoundTriggerConfidenceLevel::CSoundTriggerConfidenceLevel()
    : mUserId(0)
    , mConfidenceLevel(0)
{
}

ECode CSoundTriggerConfidenceLevel::constructor()
{
    return NOERROR;
}

ECode CSoundTriggerConfidenceLevel::constructor(
    /* [in] */ Int32 userId,
    /* [in] */ Int32 confidenceLevel)
{
    mUserId = userId;
    mConfidenceLevel = confidenceLevel;
    return NOERROR;
}

ECode CSoundTriggerConfidenceLevel::FromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ ISoundTriggerConfidenceLevel** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 userId;
    FAIL_RETURN(source->ReadInt32(&userId))
    Int32 confidenceLevel;
    FAIL_RETURN(source->ReadInt32(&confidenceLevel))

    AutoPtr<CSoundTriggerConfidenceLevel> newResult;
    CSoundTriggerConfidenceLevel::NewByFriend(userId, confidenceLevel, (CSoundTriggerConfidenceLevel**)&newResult);
    *result = newResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSoundTriggerConfidenceLevel::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteInt32(mUserId))
    FAIL_RETURN(dest->WriteInt32(mConfidenceLevel))
    return NOERROR;
}

ECode CSoundTriggerConfidenceLevel::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mUserId))
    FAIL_RETURN(source->ReadInt32(&mConfidenceLevel))
    return NOERROR;
}

ECode CSoundTriggerConfidenceLevel::Equals(
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
    if (ECLSID_CSoundTriggerConfidenceLevel != clsid) {
        *equal = FALSE;
        return NOERROR;
    }

    const AutoPtr<CSoundTriggerConfidenceLevel> other = (CSoundTriggerConfidenceLevel*)ISoundTriggerConfidenceLevel::Probe(obj);

    if (mConfidenceLevel != other->mConfidenceLevel) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mUserId != other->mUserId) {
        *equal = FALSE;
        return NOERROR;
    }
    *equal = TRUE;
    return NOERROR;
}

ECode CSoundTriggerConfidenceLevel::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    const Int32 prime = 31;
    Int32 result = 1;
    result = prime * result + mConfidenceLevel;
    result = prime * result + mUserId;
    *hashCode = result;
    return NOERROR;
}

ECode CSoundTriggerConfidenceLevel::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "ConfidenceLevel [userId=";
    sb += mUserId;
    sb += ", confidenceLevel=";
    sb += mConfidenceLevel;
    sb += "]";

    return sb.ToString(str);
}

ECode CSoundTriggerConfidenceLevel::GetUserId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mUserId;
    return NOERROR;
}

ECode CSoundTriggerConfidenceLevel::GetConfidenceLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);

    *level = mConfidenceLevel;
    return NOERROR;
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos
