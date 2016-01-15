
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTHREADPOOLEXECUTOR_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTHREADPOOLEXECUTOR_H__

#include "_Elastos_Utility_Concurrent_CThreadPoolExecutor.h"
#include "ThreadPoolExecutor.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CThreadPoolExecutor)
    , public ThreadPoolExecutor
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CTHREADPOOLEXECUTOR_H__
