
#ifndef __ELASTOS_DROID_CONTENT_PM_CVERIFIERDEVICEIDENTITYHELPER_H__
#define __ELASTOS_DROID_CONTENT_PM_CVERIFIERDEVICEIDENTITYHELPER_H__

#include "_Elastos_Droid_Content_Pm_CVerifierDeviceIdentityHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Security::ISecureRandom;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CVerifierDeviceIdentityHelper)
    , public Singleton
    , public IVerifierDeviceIdentityHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Generate a new device identity.
     *
     * @return random uniformly-distributed device identity
     */
    CARAPI Generate(
        /* [out] */ IVerifierDeviceIdentity** identity);

    CARAPI Parse(
        /* [in] */ const String& deviceIdentity,
        /* [out] */ IVerifierDeviceIdentity** identity);

private:
    /**
     * Generate a new device identity using a provided random number generator
     * class. This is used for testing.
     *
     * @param rng random number generator to retrieve the next long from
     * @return verifier device identity based on the input from the provided
     *         random number generator
     */
    static CARAPI Generate(
        /* [in] */ ISecureRandom* rng,
        /* [out] */ IVerifierDeviceIdentity** identity);

};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_PM_CVERIFIERDEVICEIDENTITYHELPER_H__
