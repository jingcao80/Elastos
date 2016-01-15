
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_RECURSIVEACTION_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_RECURSIVEACTION_H__

#include "ForkJoinTask.h"

namespace Elastos {
namespace Utility {
namespace Concurrent {

class RecursiveAction
    : public ForkJoinTask
    , public IRecursiveAction
{
public:
    CAR_INTERFACE_DECL()

    virtual CARAPI Compute() = 0;

    virtual CARAPI GetRawResult(
        /* [out] */ IInterface** outface);

    virtual CARAPI SetRawResult(
        /* [in] */ IInterface* value);

    virtual CARAPI Exec(
        /* [out] */ Boolean* res);

private:
    static Int64 mSerialVersionUID;
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_RECURSIVEACTION_H__
