
#ifndef __ELASTOS_DROID_OS_CSYSTEMSERVICE_H__
#define __ELASTOS_DROID_OS_CSYSTEMSERVICE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Os_CSystemService.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Controls and utilities for low-level {@code init} services.
 *
 * @hide
 */
CarClass(CSystemService)
    , public Singleton
    , public ISystemService
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /** Request that the init daemon start a named service. */
    CARAPI Start(
        /* [in] */ const String& name);

    /** Request that the init daemon stop a named service. */
    CARAPI Stop(
        /* [in] */ const String& name);

    /** Request that the init daemon restart a named service. */
    CARAPI Restart(
        /* [in] */ const String& name);

    /**
     * Return current state of given service.
     */
    CARAPI GetState(
        /* [in] */ const String& service,
        /* [out] */ SystemServiceState* state);

    /**
     * Check if given service is {@link State#STOPPED}.
     */
    CARAPI IsStopped(
        /* [in] */ const String& service,
        /* [out] */ Boolean* isStopped);

    /**
     * Check if given service is {@link State#RUNNING}.
     */
    CARAPI IsRunning(
        /* [in] */ const String& service,
        /* [out] */ Boolean* isRunning);

    /**
     * Wait until given service has entered specific state.
     */
    CARAPI WaitForState(
        /* [in] */ const String& service,
        /* [in] */ SystemServiceState state,
        /* [in] */ Int64 timeoutMillis);

    /**
     * Wait until any of given services enters {@link State#STOPPED}.
     */
    CARAPI WaitForAnyStopped(
        /* [in] */ ArrayOf<String>* services);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSYSTEMSERVICE_H__
