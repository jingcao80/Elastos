
#include "elastos/droid/hardware/input/TouchCalibration.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Input {

const AutoPtr<ITouchCalibration> TouchCalibration::IDENTITY = new TouchCalibration();

CAR_INTERFACE_IMPL_2(TouchCalibration, Object, ITouchCalibration, IParcelable)

TouchCalibration::TouchCalibration()
    :TouchCalibration(1,0,0,0,1,0)
{
}

TouchCalibration::TouchCalibration(
    /* [in] */ Float xScale,
    /* [in] */ Float xyMix,
    /* [in] */ Float xOffset,
    /* [in] */ Float yxMix,
    /* [in] */ Float yScale,
    /* [in] */ Float yOffset)
    : mXScale(xScale)
    , mXYMix(xyMix)
    , mXOffset(xOffset)
    , mYXMix(yxMix)
    , mYScale(yScale)
    , mYOffset(yOffset)
{
}

TouchCalibration::TouchCalibration(
   /* [in] */ IParcel* source)
{
    source->ReadFloat(&mXScale);
    source->ReadFloat(&mXYMix);
    source->ReadFloat(&mXOffset);
    source->ReadFloat(&mYXMix);
    source->ReadFloat(&mYScale);
    source->ReadFloat(&mYOffset);
}

ECode TouchCalibration::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadFloat(&mXScale))
    FAIL_RETURN(source->ReadFloat(&mXYMix))
    FAIL_RETURN(source->ReadFloat(&mXOffset))
    FAIL_RETURN(source->ReadFloat(&mYXMix))
    FAIL_RETURN(source->ReadFloat(&mYScale))
    FAIL_RETURN(source->ReadFloat(&mYOffset))

    return NOERROR;
}

ECode TouchCalibration::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteFloat(mXScale));
    FAIL_RETURN(dest->WriteFloat(mXYMix));
    FAIL_RETURN(dest->WriteFloat(mXOffset));
    FAIL_RETURN(dest->WriteFloat(mYXMix));
    FAIL_RETURN(dest->WriteFloat(mYScale));
    FAIL_RETURN(dest->WriteFloat(mYOffset));

    return NOERROR;
}

ECode TouchCalibration::GetAffineTransform(
    /* [out, callee] */ ArrayOf<Float>** transform)
{
    VALIDATE_NOT_NULL(transform);

    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(6);
    (*array)[0] = mXScale;
    (*array)[1] = mXYMix;
    (*array)[2] = mXOffset;
    (*array)[3] = mYXMix;
    (*array)[4] = mYScale;
    (*array)[5] = mYOffset;

    *transform = array;
    REFCOUNT_ADD(*transform);
    return NOERROR;
}

//@Override
ECode TouchCalibration::Equals(
    /* [in] */ IInterface* another,
    /* [out] */ Boolean* result)
{
    if (another == THIS_PROBE(IInterface)) {
        *result = TRUE;
        return NOERROR;
    } else if (ITouchCalibration::Probe(another) != NULL) {
        AutoPtr<TouchCalibration> cal = (TouchCalibration*)ITouchCalibration::Probe(another);

        *result = (cal->mXScale == mXScale)  &&
                (cal->mXYMix   == mXYMix)   &&
                (cal->mXOffset == mXOffset) &&
                (cal->mYXMix   == mYXMix)   &&
                (cal->mYScale  == mYScale)  &&
                (cal->mYOffset == mYOffset);
        return NOERROR;
    } else {
        *result = FALSE;
        return NOERROR;
    }
}

ECode TouchCalibration::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    assert(0 && "TODO floatToIntBits");
    // *code = Float.floatToIntBits(mXScale)  ^
    //         Float.floatToIntBits(mXYMix)   ^
    //         Float.floatToIntBits(mXOffset) ^
    //         Float.floatToIntBits(mYXMix)   ^
    //         Float.floatToIntBits(mYScale)  ^
    //         Float.floatToIntBits(mYOffset);
    return NOERROR;
}

} // namespace Input
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos
