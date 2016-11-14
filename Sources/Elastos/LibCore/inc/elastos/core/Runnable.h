
#ifndef __ELASTOS_CORE_RUNNABLE_H__
#define __ELASTOS_CORE_RUNNABLE_H__

#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Core {

class ECO_PUBLIC Runnable
    : public Object
    , public IRunnable
{
public:
    CAR_INTERFACE_DECL()

    Runnable();

    virtual ~Runnable();
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_RUNNABLE_H__
