
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_RECURSIVETASK_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_RECURSIVETASK_H__

#include "ForkJoinTask.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

class RecursiveTask
    : public ForkJoinTask
    , public IRecursiveTask
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI Compute(
        /* [out] */ IInterface** outface) = 0;

    virtual CARAPI GetRawResult(
        /* [out] */ IInterface** outface);

    virtual CARAPI SetRawResult(
        /* [in] */ IInterface* value);

    virtual CARAPI Exec(
        /* [out] */ Boolean* res);

public:
    static Int64 mSerialVersionUID;

    /**
     * The result of the computation.
     */
    AutoPtr<IInterface> mResult;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_RECURSIVETASK_H__
