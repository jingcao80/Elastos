
#include "elastos/droid/teleservice/phone/ImsUtil.h"

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

Boolean ImsUtil::sImsPhoneSupported = FALSE;

AutoPtr<PhoneGlobals> ImsUtil::GetApp()
{
    if (mApp != NULL) {
        sImsPhoneSupported = TRUE;
        return mApp;
    }
    else {
        AutoPtr<PhoneGlobals> app;
        PhoneGlobals::GetInstance((PhoneGlobals**)&app);
        sImsPhoneSupported = TRUE;
        return app;
    }
}

AutoPtr<PhoneGlobals> ImsUtil::mApp = GetApp();

Boolean ImsUtil::IsImsPhoneSupported()
{
    return sImsPhoneSupported;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos