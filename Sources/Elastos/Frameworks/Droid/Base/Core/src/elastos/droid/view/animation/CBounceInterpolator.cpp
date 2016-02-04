
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CBounceInterpolator.h"
#include "elastos/droid/internal/view/animation/NativeInterpolatorFactoryHelper.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Internal::View::Animation::EIID_INativeInterpolatorFactory;
using Elastos::Droid::Internal::View::Animation::NativeInterpolatorFactoryHelper;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_OBJECT_IMPL(CBounceInterpolator);

CAR_INTERFACE_IMPL_4(CBounceInterpolator, Object, IBounceInterpolator,
        INativeInterpolatorFactory, IInterpolator, ITimeInterpolator);

CBounceInterpolator::CBounceInterpolator()
{}

CBounceInterpolator::~CBounceInterpolator()
{}

ECode CBounceInterpolator::constructor()
{
    return NOERROR;
}

ECode CBounceInterpolator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NOERROR;
}

static Float Bounce(
    /* [in] */ Float t)
{
    return t * t * 8.0f;
}

ECode CBounceInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output);

    // _b(t) = t * t * 8
    // bs(t) = _b(t) for t < 0.3535
    // bs(t) = _b(t - 0.54719) + 0.7 for t < 0.7408
    // bs(t) = _b(t - 0.8526) + 0.9 for t < 0.9644
    // bs(t) = _b(t - 1.0435) + 0.95 for t <= 1.0
    // b(t) = bs(t * 1.1226)
    input *= 1.1226f;
    if (input < 0.3535f) {
        *output = Bounce(input);
    }
    else if (input < 0.7408f) {
        *output = Bounce(input - 0.54719f) + 0.7f;
    }
    else if (input < 0.9644f) {
        *output = Bounce(input - 0.8526f) + 0.9f;
    }
    else {
        *output = Bounce(input - 1.0435f) + 0.95f;
    }

    return NOERROR;
}

ECode CBounceInterpolator::CreateNativeInterpolator(
    /* [out] */ Int64* interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = NativeInterpolatorFactoryHelper::CreateBounceInterpolator();
    return NOERROR;
}

ECode CBounceInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = TRUE;
    return NOERROR;
}

}// namespace Animation
}// namespace View
}// namespace Droid
}// namespace Elastos
