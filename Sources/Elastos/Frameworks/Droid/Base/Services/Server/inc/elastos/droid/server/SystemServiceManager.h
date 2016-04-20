
#ifndef __ELASTOS_DROID_SERVER_SYSTEM_SERVER_MANAGER_H__
#define __ELASTOS_DROID_SERVER_SYSTEM_SERVER_MANAGER_H__

#include <elastos/droid/ext/frameworkext.h>
#include <_Elastos.Droid.Server.h>
#include <Elastos.Droid.Content.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Manages creating, starting, and other lifecycle events of
 * {@link com.android.server.SystemService system services}.
 *
 * {@hide}
 */
class SystemServiceManager
    : public Object
    , public ISystemServiceManager
{
public:
    CAR_INTERFACE_DECL()

    SystemServiceManager(
        /* [in] */ IContext* context);

    CARAPI StartService(
        /* [in] */ ISystemService* systemService);

    /**
     * Starts a service by class name.
     *
     * @return The service instance.
     */
    // @SuppressWarnings("unchecked")
    CARAPI StartService(
        /* [in] */ const String& className,
        /* [out] */ ISystemService** service);

    /**
     * Creates and starts a system service. The class must be a subclass of
     * {@link com.android.server.SystemService}.
     *
     * @param serviceClass A Java class that implements the SystemService interface.
     * @return The service instance, never null.
     * @throws RuntimeException if the service fails to start.
     */
    // @SuppressWarnings("unchecked")
    CARAPI StartService(
        /* [in */ IClassInfo* serviceClass,
        /* [out] */ ISystemService** service);

    /**
     * Starts the specified boot phase for all system services that have been started up to
     * this point.
     *
     * @param phase The boot phase to start.
     */
    CARAPI StartBootPhase(
        /* [in] */ Int32 phase);

    CARAPI StartUser(
        /* [in] */ Int32 userHandle);

    CARAPI SwitchUser(
        /* [in] */ Int32 userHandle);

    CARAPI StopUser(
        /* [in] */ Int32 userHandle);

    CARAPI CleanupUser(
        /* [in] */ Int32 userHandle);

    /** Sets the safe mode flag for services to query. */
    CARAPI SetSafeMode(
        /* [in] */ Boolean safeMode);

    /**
     * Returns whether we are booting into safe mode.
     * @return safe mode flag
     */
    CARAPI IsSafeMode(
        /* [out] */ Boolean* result);

    /**
     * Outputs the state of this manager to the System log.
     */
    CARAPI Dump();

private:
    static const String TAG;
    static const String sConstructorSignature;

    AutoPtr<IContext> mContext;
    Boolean mSafeMode;

    // Services that should receive lifecycle events.
    List<AutoPtr<ISystemService> > mServices;

    Int32 mCurrentPhase;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__SYSTEM_SERVER_MANAGER_H__
