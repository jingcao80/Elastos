
#ifndef __ELASTOS_UTILITY_COBSERVABLE_H__
#define __ELASTOS_UTILITY_COBSERVABLE_H__

#include "_Elastos_Utility_CObservable.h"
#include "Observable.h"

namespace Elastos {
namespace Utility {

CarClass(CObservable)
    , public Observable
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Utility
} // namespace Elastos

#endif // __ELASTOS_UTILITY_COBSERVABLE_H__
