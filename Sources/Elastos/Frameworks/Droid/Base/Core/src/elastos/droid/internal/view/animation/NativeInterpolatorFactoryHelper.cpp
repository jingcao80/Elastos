
#include "elastos/droid/internal/view/animation/NativeInterpolatorFactoryHelper.h"
#include <hwui/Interpolator.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Animation {

Int64 NativeInterpolatorFactoryHelper::CreateAccelerateDecelerateInterpolator()
{
    return reinterpret_cast<Int64>(new android::uirenderer::AccelerateDecelerateInterpolator());
}

Int64 NativeInterpolatorFactoryHelper::CreateAccelerateInterpolator(
    /* [in] */ Float factor)
{
    return reinterpret_cast<Int64>(new android::uirenderer::AccelerateInterpolator(factor));
}

Int64 NativeInterpolatorFactoryHelper::CreateAnticipateInterpolator(
    /* [in] */ Float tension)
{
    return reinterpret_cast<Int64>(new android::uirenderer::AnticipateInterpolator(tension));
}

Int64 NativeInterpolatorFactoryHelper::CreateAnticipateOvershootInterpolator(
    /* [in] */ Float tension)
{
    return reinterpret_cast<Int64>(new android::uirenderer::AnticipateOvershootInterpolator(tension));
}

Int64 NativeInterpolatorFactoryHelper::CreateBounceInterpolator()
{
    return reinterpret_cast<Int64>(new android::uirenderer::BounceInterpolator());
}

Int64 NativeInterpolatorFactoryHelper::CreateCycleInterpolator(
    /* [in] */ Float cycles)
{
    return reinterpret_cast<Int64>(new android::uirenderer::CycleInterpolator(cycles));
}

Int64 NativeInterpolatorFactoryHelper::CreateDecelerateInterpolator(
    /* [in] */ Float factor)
{
    return reinterpret_cast<Int64>(new android::uirenderer::DecelerateInterpolator(factor));
}

Int64 NativeInterpolatorFactoryHelper::CreateLinearInterpolator()
{
    return reinterpret_cast<Int64>(new android::uirenderer::LinearInterpolator());
}

Int64 NativeInterpolatorFactoryHelper::CreateOvershootInterpolator(
    /* [in] */ Float tension)
{
    return reinterpret_cast<Int64>(new android::uirenderer::OvershootInterpolator(tension));
}

Int64 NativeInterpolatorFactoryHelper::CreateLutInterpolator(
    /* [in] */ ArrayOf<Float>* values)
{
    Int32 len = values->GetLength();
    if (len <= 0) {
        return 0;
    }
    float* lut = new float[len];
    // env->GetFloatArrayRegion(jlut, 0, len, lut);
    memcpy(lut, values->GetPayload(), len * sizeof(float));
    return reinterpret_cast<Int64>(new android::uirenderer::LUTInterpolator(lut, len));
}

}   //namespace Animation
}   //namespace View
}   //namespace Internal
}   //namespace Droid
}   //namespace Elastos
