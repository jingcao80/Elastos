
#ifndef __ELASTOS_DROID_SYSTEM_CZYGOTEHOOKS_H__
#define __ELASTOS_DROID_SYSTEM_CZYGOTEHOOKS_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CZygoteHooks.h"

using Elastos::Net::IInetAddress;

namespace Elastos {
namespace Droid {
namespace System {

/**
 * Corresponds to C's {@code struct group_req}.
 */
CarClass(CZygoteHooks)
    , public Object
    , public IZygoteHooks
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CZygoteHooks();

    CARAPI PreFork();

    CARAPI PostForkChild(
        /* [in] */ Int32 debugFlags,
        /* [in] */ const String& instructionSet);

    CARAPI PostForkCommon();

private:
    static CARAPI_(Int64) NativePreFork();

    static CARAPI_(void) NativePostForkChild(
        /* [in] */ Int64 token,
        /* [in] */ Int32 debugFlags,
        /* [in] */ const String& instructionSet);

    /**
     * We must not fork until we're single-threaded again. Wait until /proc shows we're
     * down to just one thread.
     */
    static CARAPI_(void) WaitUntilAllThreadsStopped();

private:
    Int64 mToken;
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CZYGOTEHOOKS_H__
