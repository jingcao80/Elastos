
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/CAccelerateDecelerateInterpolator.h"
#include "elastos/droid/internal/view/animation/NativeInterpolatorFactoryHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Internal::View::Animation::EIID_INativeInterpolatorFactory;
using Elastos::Droid::Internal::View::Animation::NativeInterpolatorFactoryHelper;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

CAR_OBJECT_IMPL(CAccelerateDecelerateInterpolator);

CAR_INTERFACE_IMPL_4(CAccelerateDecelerateInterpolator, Object,
        IAccelerateDecelerateInterpolator, INativeInterpolatorFactory,
        IInterpolator, ITimeInterpolator);

CAccelerateDecelerateInterpolator::CAccelerateDecelerateInterpolator()
{}

CAccelerateDecelerateInterpolator::~CAccelerateDecelerateInterpolator()
{}

ECode CAccelerateDecelerateInterpolator::constructor()
{
    return NOERROR;
}

ECode CAccelerateDecelerateInterpolator::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return NOERROR;
}

ECode CAccelerateDecelerateInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* output)
{
    VALIDATE_NOT_NULL(output);
    *output = (Float)(Elastos::Core::Math::Cos((input + 1) * Elastos::Core::Math::PI) / 2.0f) + 0.5f;
    return NOERROR;
}

ECode CAccelerateDecelerateInterpolator::CreateNativeInterpolator(
    /* [out] */ Int64* interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = NativeInterpolatorFactoryHelper::CreateAccelerateDecelerateInterpolator();
    return NOERROR;
}

ECode CAccelerateDecelerateInterpolator::HasNativeInterpolator(
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
