
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTHREADLOCALRANDOMHELPER_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTHREADLOCALRANDOMHELPER_H__

#include "_Elastos_Utility_Concurrent_CThreadLocalRandomHelper.h"
#include "Singleton.h"

using Elastos::Core::IRunnable;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CThreadLocalRandomHelper)
    , public Singleton
    , public IThreadLocalRandomHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetCurrent(
        /* [out] */ IThreadLocalRandom** threadloc);
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTHREADLOCALRANDOMHELPER_H__
