
#include "elastos/droid/app/CProfileManagerHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CProfileManagerHelper, Singleton, IProfileManagerHelper)

CAR_SINGLETON_IMPL(CProfileManagerHelper)

ECode CProfileManagerHelper::GetNoProfile(
    /* [out] */ IUUID** noprofile)
{
    VALIDATE_NOT_NULL(noprofile);
    *noprofile = ProfileManager::NO_PROFILE;
    REFCOUNT_ADD(*noprofile);
    return NOERROR;
}

ECode CProfileManagerHelper::GetService(
    /* [out] */ IIProfileManager** service)
{
    VALIDATE_NOT_NULL(service);
    AutoPtr<IIProfileManager> temp = ProfileManager::GetService();
    *service = temp;
    REFCOUNT_ADD(*service);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
