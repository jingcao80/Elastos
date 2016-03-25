
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerKeyphraseRecognitionExtra.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL_2(CSoundTriggerKeyphraseRecognitionExtra, Object, ISoundTriggerKeyphraseRecognitionExtra, IParcelable)

CAR_OBJECT_IMPL(CSoundTriggerKeyphraseRecognitionExtra)

CSoundTriggerKeyphraseRecognitionExtra::CSoundTriggerKeyphraseRecognitionExtra()
    : mId(0)
    , mRecognitionModes(0)
    , mCoarseConfidenceLevel(0)
    , mConfidenceLevels(NULL)
{
}

ECode CSoundTriggerKeyphraseRecognitionExtra::constructor()
{
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::constructor(
    /* [in] */ Int32 id,
    /* [in] */ Int32 recognitionModes,
    /* [in] */ Int32 coarseConfidenceLevel,
    /* [in] */ ArrayOf<ISoundTriggerConfidenceLevel*>* confidenceLevels)
{
    mId = id;
    mRecognitionModes = recognitionModes;
    mCoarseConfidenceLevel = coarseConfidenceLevel;
    mConfidenceLevels = confidenceLevels;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::FromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ ISoundTriggerKeyphraseRecognitionExtra** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    FAIL_RETURN(source->ReadInt32(&id))
    Int32 recognitionModes;
    FAIL_RETURN(source->ReadInt32(&recognitionModes))
    Int32 coarseConfidenceLevel;
    FAIL_RETURN(source->ReadInt32(&coarseConfidenceLevel))
    //ConfidenceLevel[] confidenceLevels = in.createTypedArray(ConfidenceLevel.CREATOR);
    assert(0 && "TODO: ConfidenceLevel.CREATOR");
    AutoPtr<ArrayOf<ISoundTriggerConfidenceLevel*> > confidenceLevels;
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&confidenceLevels))

    AutoPtr<CSoundTriggerKeyphraseRecognitionExtra> newResult;
    CSoundTriggerKeyphraseRecognitionExtra::NewByFriend(id, recognitionModes, coarseConfidenceLevel,
            confidenceLevels, (CSoundTriggerKeyphraseRecognitionExtra**)&newResult);
    *result = newResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteInt32(mId))
    FAIL_RETURN(dest->WriteInt32(mRecognitionModes))
    FAIL_RETURN(dest->WriteInt32(mCoarseConfidenceLevel))
    //dest.writeTypedArray(confidenceLevels, flags);
    assert(0 && "TODO: writeTypedArray");
    FAIL_RETURN(dest->WriteArrayOf((Handle32)mConfidenceLevels.Get()))
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mId))
    FAIL_RETURN(source->ReadInt32(&mRecognitionModes))
    FAIL_RETURN(source->ReadInt32(&mCoarseConfidenceLevel))
    //ConfidenceLevel[] confidenceLevels = in.createTypedArray(ConfidenceLevel.CREATOR);
    assert(0 && "TODO: ConfidenceLevel.CREATOR");
    FAIL_RETURN(source->ReadArrayOf((Handle32*)&mConfidenceLevels))
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::Equals(
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
    if (ECLSID_CSoundTriggerKeyphraseRecognitionExtra != clsid) {
        *equal = FALSE;
        return NOERROR;
    }

    const AutoPtr<CSoundTriggerKeyphraseRecognitionExtra> other = (CSoundTriggerKeyphraseRecognitionExtra*)ISoundTriggerKeyphraseRecognitionExtra::Probe(obj);
    if (!Arrays::Equals(mConfidenceLevels, other->mConfidenceLevels)) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mId != other->mId) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mRecognitionModes != other->mRecognitionModes) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mCoarseConfidenceLevel != other->mCoarseConfidenceLevel) {
        *equal = FALSE;
        return NOERROR;
    }
    *equal = TRUE;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    const Int32 prime = 31;
    Int32 result = 1;
    result = prime * result + Arrays::GetHashCode(mConfidenceLevels);
    result = prime * result + mId;
    result = prime * result + mRecognitionModes;
    result = prime * result + mCoarseConfidenceLevel;
    *hashCode = result;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "KeyphraseRecognitionExtra [id=";
    sb += mId;
    sb += ", recognitionModes=";
    sb += mRecognitionModes;
    sb += ", coarseConfidenceLevel=";
    sb += mCoarseConfidenceLevel;
    sb += ", confidenceLevels=";
    sb += Arrays::ToString(mConfidenceLevels);
    sb += "]";

    return sb.ToString(str);
}

ECode CSoundTriggerKeyphraseRecognitionExtra::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mId;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::GetRecognitionModes(
    /* [out] */ Int32* modes)
{
    VALIDATE_NOT_NULL(modes);

    *modes = mRecognitionModes;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::GetCoarseConfidenceLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);

    *level = mCoarseConfidenceLevel;
    return NOERROR;
}

ECode CSoundTriggerKeyphraseRecognitionExtra::GetConfidenceLevels(
    /* [out, callee] */ ArrayOf<ISoundTriggerConfidenceLevel*>** levels)
{
    VALIDATE_NOT_NULL(levels);

    *levels = mConfidenceLevels;
    REFCOUNT_ADD(*levels);
    return NOERROR;
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos
