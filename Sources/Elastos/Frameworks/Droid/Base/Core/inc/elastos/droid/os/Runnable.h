
#ifndef __ELASTOS_DROID_OS_RUNNABLE_H__
#define __ELASTOS_DROID_OS_RUNNABLE_H__

#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Os {

class ECO_PUBLIC Runnable
    : public Object
    , public IRunnable
{
public:
    CAR_INTERFACE_DECL()

    Runnable();

    virtual ~Runnable();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_RUNNABLE_H__
