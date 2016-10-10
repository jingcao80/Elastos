#ifndef __ELASTOS_DROID_OS_CCANCELLATIONSIGNALTRANSPORT_H__
#define __ELASTOS_DROID_OS_CCANCELLATIONSIGNALTRANSPORT_H__

#include "_Elastos_Droid_Os_CCancellationSignalTransport.h"
#include "elastos/droid/os/CCancellationSignal.h"

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CCancellationSignalTransport)
    , public CCancellationSignal::Transport
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CCANCELLATIONSIGNALTRANSPORT_H__
