
#include "CThreadLocalRandomHelper.h"
#include "CThreadLocalRandom.h"

using Elastos::Utility::Concurrent::CThreadLocalRandom;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CThreadLocalRandomHelper::
//====================================================================
CAR_INTERFACE_IMPL(CThreadLocalRandomHelper, Singleton, IThreadLocalRandomHelper)

CAR_SINGLETON_IMPL(CThreadLocalRandomHelper)

ECode CThreadLocalRandomHelper::GetCurrent(
    /* [out] */ IThreadLocalRandom** threadloc)
{
    VALIDATE_NOT_NULL(threadloc)
    AutoPtr<IThreadLocalRandom> p = CThreadLocalRandom::GetCurrent();
    *threadloc = p.Get();
    REFCOUNT_ADD(*threadloc)
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
