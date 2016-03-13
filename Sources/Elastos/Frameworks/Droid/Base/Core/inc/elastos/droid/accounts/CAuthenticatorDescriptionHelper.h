
#ifndef __ELASTOS_DROID_ACCOUNTS_CAUTHENTICATORDESCRIPTIONHELPER_H__
#define __ELASTOS_DROID_ACCOUNTS_CAUTHENTICATORDESCRIPTIONHELPER_H__

#include "_Elastos_Droid_Accounts_CAuthenticatorDescriptionHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CAuthenticatorDescriptionHelper)
    , public Singleton
    , public IAuthenticatorDescriptionHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI NewKey(
        /* [in] */ const String& type,
        /* [out] */ IAuthenticatorDescription** description);
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__CAUTHENTICATORDESCRIPTION_H__
