
#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTLINGER_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTLINGER_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CStructLinger.h"


namespace Elastos {
namespace Droid {
namespace System {

/**
 * Corresponds to C's {@code struct linger} from
 * <a href="http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/sys_socket.h.html">&lt;sys/socket.h&gt;</a>
 */
CarClass(CStructLinger)
    , public Object
    , public IStructLinger
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetOnoff(
        /* [out] */ Int32* onoff);

    CARAPI GetLinger(
        /* [out] */ Int32* linger);

    CARAPI IsOn(
        /* [out] */ Boolean* isOn);

    CARAPI constructor(
        /* [in] */ Int32 onoff,
        /* [in] */ Int32 linger);

private:
    /** Whether or not linger is enabled. Non-zero is on. */
    Int32 mOnoff;

    /** Linger time in seconds. */
    Int32 mLinger;
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CSTRUCTLINGER_H__
