#ifndef  __ELASTOS_DROID_PHONE_IMSUTIL_H__
#define  __ELASTOS_DROID_PHONE_IMSUTIL_H__

#include "_Elastos.Droid.TeleService.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * Utility class to look up carrier logo resource IDs.
 */
class ImsUtil
    : public Object
{
public:
    /**
     * @return true if this device supports voice calls using the built-in SIP stack.
     */
    static CARAPI_(Boolean) IsImsPhoneSupported();

private:
    ImsUtil() {}

    static CARAPI_(AutoPtr<PhoneGlobals>) GetApp();

private:
    static Boolean sImsPhoneSupported;

    static AutoPtr<PhoneGlobals> mApp;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_IMSUTIL_H__