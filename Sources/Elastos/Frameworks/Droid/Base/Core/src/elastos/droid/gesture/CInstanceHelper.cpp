#include "elastos/droid/gesture/CInstanceHelper.h"
#include "elastos/droid/gesture/Instance.h"

namespace Elastos {
namespace Droid {
namespace Gesture {

CAR_SINGLETON_IMPL(CInstanceHelper)

CAR_INTERFACE_IMPL(CInstanceHelper, Singleton, IInstanceHelper)

ECode CInstanceHelper::CreateInstance(
    /* [in] */ Int32 sequenceType,
    /* [in] */ Int32 orientationType,
    /* [in] */ IGesture *gesture,
    /* [in] */ const String& label,
    /* [out] */ IInstance **instance)
{
    AutoPtr<IInstance> ret;

    ret = Instance::CreateInstance(sequenceType, orientationType, gesture, label);
    *instance = ret;
    REFCOUNT_ADD(*instance);

    return NOERROR;
}

ECode CInstanceHelper::SpatialSampler(
    /* [in] */ IGesture *gesture,
    /* [out, callee] */ ArrayOf<Float> **sampler)
{
    return Instance::SpatialSampler(gesture, sampler);
}

ECode CInstanceHelper::TemporalSampler(
    /* [in] */ Int32 orientationType,
    /* [in] */ IGesture *gesture,
    /* [out, callee] */ ArrayOf<Float> **sampler)
{
    AutoPtr<ArrayOf<Float> > ret;

    ret = Instance::TemporalSampler(orientationType, gesture);
    *sampler = ret;

    return NOERROR;
}

} // namespace Gesture
} // namespace Droid
} // namespace Elastos
