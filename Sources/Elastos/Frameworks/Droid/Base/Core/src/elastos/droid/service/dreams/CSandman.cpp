
#include "elastos/droid/service/dreams/CSandman.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Dreams {

CAR_SINGLETON_IMPL(CSandman)
CAR_INTERFACE_IMPL(CSandman, Singleton, ISandman)

ECode CSandman::ShouldStartDockApp(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* result)
{
    return Sandman::ShouldStartDockApp(context, intent, result);
}

ECode CSandman::StartDreamByUserRequest(
    /* [in] */ IContext* context)
{
    return Sandman::StartDreamByUserRequest(context);
}

ECode CSandman::StartDreamWhenDockedIfAppropriate(
    /* [in] */ IContext* context)
{
    return Sandman::StartDreamWhenDockedIfAppropriate(context);
}

} // namespace Dreams
} // namespace Service
} // namepsace Droid
} // namespace Elastos
