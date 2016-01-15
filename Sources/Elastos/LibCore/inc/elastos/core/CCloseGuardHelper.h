
#ifndef __ELASTOS_CORE_CCLOSEGUARDHELPER_H__
#define __ELASTOS_CORE_CCLOSEGUARDHELPER_H__

#include "_Elastos_Core_CCloseGuardHelper.h"
#include "Singleton.h"

namespace Elastos {
namespace Core {

CarClass(CCloseGuardHelper)
    , public Singleton
    , public ICloseGuardHelper
{
public:

    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a CloseGuard instance. If CloseGuard is enabled, {@code
     * #open(String)} can be used to set up the instance to warn on
     * failure to close. If CloseGuard is disabled, a non-null no-op
     * instance is returned.
     */
    CARAPI Get(
        /* [out] */ ICloseGuard** guard);

    /**
     * Used to enable or disable CloseGuard. Note that CloseGuard only
     * warns if it is enabled for both allocation and finalization.
     */
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Used to replace default Reporter used to warn of CloseGuard
     * violations. Must be non-null.
     */
    CARAPI SetReporter(
        /* [in] */ ICloseGuardReporter* reporter);

    /**
     * Returns non-null CloseGuard.Reporter.
     */
    CARAPI GetReporter(
        /* [out] */ ICloseGuardReporter** report);
};

} // namespace Core
} // namespace Elastos

#endif //__ELASTOS_CORE_CCLOSEGUARDHELPER_H__
