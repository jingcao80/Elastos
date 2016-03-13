#ifndef __ELASTOS_DROID_OS_BINDERPROXY_H__
#define __ELASTOS_DROID_OS_BINDERPROXY_H__

#include "Elastos.Droid.Os.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

class BinderProxy
    : public Object
    , public IBinderProxy
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    BinderProxy();

    ~BinderProxy();

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);
private:
    CARAPI_(void) Destroy();

public:
    Int32 mObject;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_BINDERPROXY_H__
