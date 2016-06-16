
#include "elastos/droid/phone/ImsUtil.h"

namespace Elastos {
namespace Droid {
namespace Phone {

Boolean ImsUtil::sImsPhoneSupported = FALSE;

static AutoPtr<IPhoneGlobals> GetApp()
{
    if (mApp != NULL) {
        sImsPhoneSupported = TRUE;
        return mApp;
    }
    else {
        AutoPtr<IPhoneGlobals> app;
        PhoneGlobals::GetInstance((IPhoneGlobals**)&app);
        sImsPhoneSupported = TRUE;
        return app;
    }
}

AutoPtr<IPhoneGlobals> ImsUtil::mApp = GetApp();

Boolean ImsUtil::IsImsPhoneSupported()
{
    return sImsPhoneSupported;
}

} // namespace Phone
} // namespace Droid
} // namespace Elastos