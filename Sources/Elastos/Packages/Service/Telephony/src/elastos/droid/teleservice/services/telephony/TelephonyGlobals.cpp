
#include "elastos/droid/teleservice/services/telephony/TelephonyGlobals.h"
#include "elastos/droid/teleservice/services/telephony/TelecomAccountRegistry.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Utility::Logging::Logger;

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
    // // TODO: Make this work with Multi-SIM devices
    AutoPtr<IPhone> phone;
    AutoPtr<IPhoneFactory> helper;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&helper);
    helper->GetDefaultPhone((IPhone**)&phone);
    if (phone != NULL) {
        mTtyManager = new TtyManager(mContext, phone);
    }

    AutoPtr<TelecomAccountRegistry> registry = TelecomAccountRegistry::GetInstance(mContext);
    return registry->SetupOnBoot();
}

} // namespace Telephony
} // namespace Services
} // namespace TeleService
} // namespace Droid
} // namespace Elastos