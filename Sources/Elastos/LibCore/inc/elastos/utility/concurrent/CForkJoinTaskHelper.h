
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CFORKJOINTASKHELPER_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CFORKJOINTASKHELPER_H__

#include "_Elastos_Utility_Concurrent_CForkJoinTaskHelper.h"
#include "Singleton.h"

using Elastos::Core::IRunnable;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CForkJoinTaskHelper)
    , public Singleton
    , public IForkJoinTaskHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI InvokeAll(
        /* [in] */ IForkJoinTask* t1,
        /* [in] */ IForkJoinTask* t2);

    CARAPI InvokeAll(
        /* [in] */ ArrayOf<IForkJoinTask*>* tasks);

    CARAPI InvokeAll(
        /* [in] */ ICollection* tasks,
        /* [out] */ ICollection** outcol);

    CARAPI HelpQuiesce();

    CARAPI GetPool(
        /* [out] */ IForkJoinPool** outpool);

    CARAPI InForkJoinPool(
        /* [out] */ Boolean* value);

    CARAPI GetQueuedTaskCount(
        /* [out] */ Int32* value);

    CARAPI GetSurplusQueuedTaskCount(
        /* [out] */ Int32* value);

    CARAPI Adapt(
        /* [in] */ IRunnable* runnable,
        /* [out] */ IForkJoinTask** outjoin);

    CARAPI Adapt(
        /* [in] */ IRunnable* runnable,
        /* [in] */ IInterface* result,
        /* [out] */ IForkJoinTask** outjoin);

    CARAPI Adapt(
        /* [in] */ ICallable* callable,
        /* [out] */ IForkJoinTask** outjoin);
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CFORKJOINTASKHELPER_H__
