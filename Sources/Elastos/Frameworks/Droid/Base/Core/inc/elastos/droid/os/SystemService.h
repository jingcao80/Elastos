
#ifndef __ELASTOS_DROID_OS_SYSTEMSERVICE_H__
#define __ELASTOS_DROID_OS_SYSTEMSERVICE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Object.h>
#include "elastos/droid/os/Runnable.h"

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Controls and utilities for low-level {@code init} services.
 *
 * @hide
 */
class SystemService
{
public:
    /** Request that the init daemon start a named service. */
    static CARAPI Start(
        /* [in] */ const String& name);

    /** Request that the init daemon stop a named service. */
    static CARAPI Stop(
        /* [in] */ const String& name);

    /** Request that the init daemon restart a named service. */
    static CARAPI Restart(
        /* [in] */ const String& name);

    /**
     * Return current state of given service.
     */
    static CARAPI_(SystemServiceState) GetState(
        /* [in] */ const String& service);

    /**
     * Check if given service is {@link State#STOPPED}.
     */
    static CARAPI_(Boolean) IsStopped(
        /* [in] */ const String& service);

    /**
     * Check if given service is {@link State#RUNNING}.
     */
    static CARAPI_(Boolean) IsRunning(
        /* [in] */ const String& service);

    /**
     * Wait until given service has entered specific state.
     */
    static CARAPI WaitForState(
        /* [in] */ const String& service,
        /* [in] */ SystemServiceState state,
        /* [in] */ Int64 timeoutMillis);

    /**
     * Wait until any of given services enters {@link State#STOPPED}.
     */
    static CARAPI WaitForAnyStopped(
        /* [in] */ ArrayOf<String>* services);

private:

    static CARAPI_(Boolean) Init();

    class MyRunnable
        : public Runnable {
    public:
        CARAPI Run();
    };

private:
    friend class MyRunnable;

    static HashMap<String, SystemServiceState> sStates;
    static Object sPropertyLock;
    static Boolean sInitialized;

private:
    SystemService();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_SYSTEMSERVICE_H__
