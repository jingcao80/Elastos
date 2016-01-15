
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/CChoreographerHelper.h"
// #include "elastos/droid/view/Choreographer.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CChoreographerHelper, Singleton, IChoreographerHelper)
CAR_SINGLETON_IMPL(CChoreographerHelper)

ECode CChoreographerHelper::GetInstance(
    /* [out] */ IChoreographer** choreographer)
{
    // VALIDATE_NOT_NULL(choreographer);
    // AutoPtr<Choreographer> temp = Choreographer::GetInstance();
    // *choreographer = temp.Get();
    // REFCOUNT_ADD(*choreographer);
    return NOERROR;
}

ECode CChoreographerHelper::GetFrameDelay(
    /* [out] */ Int64* frameDelay)
{
    // VALIDATE_NOT_NULL(frameDelay);
    // *frameDelay = Choreographer::GetFrameDelay();
    return NOERROR;
}

ECode CChoreographerHelper::SetFrameDelay(
    /* [in] */ Int64 frameDelay)
{
    // Choreographer::SetFrameDelay(frameDelay);
    return NOERROR;
}

ECode CChoreographerHelper::SubtractFrameDelay(
    /* [in] */ Int64 delayMillis,
    /* [out] */ Int64* frameDelay)
{
    VALIDATE_NOT_NULL(frameDelay);
    // *frameDelay = Choreographer::SubtractFrameDelay(delayMillis);
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
