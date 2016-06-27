
#include "elastos/droid/teleservice/services/telephony/TelephonyGlobals.h"
#include <elastos/core/AutoLock.h>

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Services {
namespace Telephony {

AutoPtr<TelephonyGlobals> TelephonyGlobals::sInstance;

Object TelephonyGlobals::slock;

TelephonyGlobals::TelephonyGlobals(
    /* [in] */ IContext* context)
{
    context->GetApplicationContext((IContext**)&mContext);
}

AutoPtr<TelephonyGlobals> TelephonyGlobals::GetInstance(
    /* [in] */ IContext* context)
{
    {
        AutoLock syncLock(slock);

        if (sInstance == NULL) {
            sInstance = new TelephonyGlobals(context);
        }
        return sInstance;
    }
}

ECode TelephonyGlobals::OnCreate()
{
    assert(0 && "need class PhoneFactory");
    // // TODO: Make this work with Multi-SIM devices
    // AutoPtr<IPhone> phone;
    // PhoneFactory::GetDefaultPhone((IPhone**)&phone);
    // if (phone != NULL) {
    //     mTtyManager = new TtyManager(mContext, phone);
    // }

    // AutoPtr<TelecomAccountRegistry> registry = TelecomAccountRegistry::GetInstance(mContext);
    // return registry->SetupOnBoot();
    return NOERROR;
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos