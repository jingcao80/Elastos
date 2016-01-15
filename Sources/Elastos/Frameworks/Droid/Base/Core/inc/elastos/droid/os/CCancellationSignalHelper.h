
#ifndef __ELASTOS_DROID_OS_CCANCELLATIONSIGNALHELPER_H__
#define __ELASTOS_DROID_OS_CCANCELLATIONSIGNALHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Os_CCancellationSignalHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CCancellationSignalHelper)
    , public Singleton
    , public ICancellationSignalHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateTransport(
        /* [out] */ IICancellationSignal** cancellationSignal);

    CARAPI FromTransport(
        /* [in] */ IICancellationSignal* transport,
        /* [out] */ ICancellationSignal** cancellationSignal);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CCANCELLATIONSIGNALHELPER_H__
