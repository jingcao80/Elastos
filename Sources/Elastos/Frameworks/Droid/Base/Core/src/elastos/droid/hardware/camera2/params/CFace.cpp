
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/hardware/camera2/params/CFace.h"
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CAR_INTERFACE_IMPL(CFace, Object, IFace)

CAR_OBJECT_IMPL(CFace)

CFace::CFace()
    : mScore(0)
    , mId(0)
{
}

CFace::~CFace()
{
}

ECode CFace::constructor()
{
    return NOERROR;
}

ECode CFace::constructor(
    /* [in] */ IRect* bounds,
    /* [in] */ Int32 score,
    /* [in] */ Int32 id,
    /* [in] */ IPoint* leftEyePosition,
    /* [in] */ IPoint* rightEyePosition,
    /* [in] */ IPoint* mouthPosition)
{
    FAIL_RETURN(CheckNotNull(String("bounds"), TO_IINTERFACE(bounds)))

    if (score < IFace::SCORE_MIN || score > IFace::SCORE_MAX) {
        Logger::E("CFace", "Confidence out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (id < 0 && id != IFace::ID_UNSUPPORTED) {
        Logger::E("CFace", "Id out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (id == IFace::ID_UNSUPPORTED) {
        FAIL_RETURN(CheckNull(String("leftEyePosition"), leftEyePosition))
        FAIL_RETURN(CheckNull(String("rightEyePosition"), rightEyePosition))
        FAIL_RETURN(CheckNull(String("mouthPosition"), mouthPosition))
    }

    mBounds = bounds;
    mScore = score;
    mId = id;
    mLeftEye = leftEyePosition;
    mRightEye = rightEyePosition;
    mMouth = mouthPosition;
    return NOERROR;
}

ECode CFace::constructor(
    /* [in] */ IRect* bounds,
    /* [in] */ Int32 score)
{
    return constructor(bounds, score, IFace::ID_UNSUPPORTED,
            /*leftEyePosition*/NULL, /*rightEyePosition*/NULL, /*mouthPosition*/NULL);
}

ECode CFace::GetBounds(
    /* [out] */ IRect** outrect)
{
    VALIDATE_NOT_NULL(outrect)

    *outrect = mBounds;
    REFCOUNT_ADD(*outrect);
    return NOERROR;
}

ECode CFace::GetScore(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mScore;
    return NOERROR;
}

ECode CFace::GetId(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mId;
    return NOERROR;
}

ECode CFace::GetLeftEyePosition(
    /* [out] */ IPoint** outpoint)
{
    VALIDATE_NOT_NULL(outpoint)

    *outpoint = mLeftEye;
    REFCOUNT_ADD(*outpoint);
    return NOERROR;
}

ECode CFace::GetRightEyePosition(
    /* [out] */ IPoint** outpoint)
{
    VALIDATE_NOT_NULL(outpoint)

    *outpoint = mRightEye;
    REFCOUNT_ADD(*outpoint);
    return NOERROR;
}

ECode CFace::GetMouthPosition(
    /* [out] */ IPoint** outpoint)
{
    VALIDATE_NOT_NULL(outpoint)

    *outpoint = mMouth;
    REFCOUNT_ADD(*outpoint);
    return NOERROR;
}

ECode CFace::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    StringBuilder sb;
    sb += "{ bounds: ";
    sb += mBounds;
    sb += ", score: ";
    sb += mScore;
    sb += ", id: ";
    sb += mId;
    sb += ", ";
    sb += "leftEyePosition: ";
    sb += mLeftEye;
    sb += ", rightEyePosition:";
    sb += mRightEye;
    sb += ", mouthPosition: ";
    sb += mMouth;
    sb += " }";

    return sb.ToString(str);
}

ECode CFace::CheckNotNull(
    /* [in] */ const String& name,
    /* [in] */ IInterface* obj)
{
    if (obj == NULL) {
        Logger::E("CFace", String(name) + " was required, but it was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CFace::CheckNull(
    /* [in] */ const String& name,
    /* [in] */ IInterface* obj)
{
    if (obj != NULL) {
        Logger::E("CFace", String(name) + " was required to be null, but it wasn't");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
