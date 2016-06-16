#ifndef  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__
#define  __ELASTOS_DROID_PHONE_CARRIERLOGO_H__

#include "_Elastos.Droid.Server.Telephony.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Utility class to look up carrier logo resource IDs.
 */
class ImsUtil
{
public:
    /**
     * @return true if this device supports voice calls using the built-in SIP stack.
     */
    static CARAPI_(Boolean) IsImsPhoneSupported();

private:
    ImsUtil() {}

private:
    static Boolean sImsPhoneSupported;

    static AutoPtr<IPhoneGlobals> mApp;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CARRIERLOGO_H__