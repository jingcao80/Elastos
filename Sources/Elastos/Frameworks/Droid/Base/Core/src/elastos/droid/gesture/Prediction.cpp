#include "elastos/droid/gesture/Prediction.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_INTERFACE_IMPL(Prediction, Object, IPrediction);

Prediction::Prediction()
{}

Prediction::~Prediction()
{}

ECode Prediction::constructor(
    /* [in] */ const String& label,
    /* [in] */ Double predictionScore)
{
    mName = label;
    mScore = predictionScore;
    return NOERROR;
}

ECode Prediction::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode Prediction::GetScore(
    /* [out] */ Double* score)
{
    VALIDATE_NOT_NULL(score);

    *score = mScore;
    return NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
