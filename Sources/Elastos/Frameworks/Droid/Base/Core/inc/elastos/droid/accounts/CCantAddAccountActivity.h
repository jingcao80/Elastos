
#ifndef __ELASTOS_DROID_ACCOUNTS_CCANTADDACCOUNTACTIVITY_H__
#define __ELASTOS_DROID_ACCOUNTS_CCANTADDACCOUNTACTIVITY_H__

#include "_Elastos_Droid_Accounts_CCantAddAccountActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace Accounts {

CarClass(CCantAddAccountActivity)
    , public Activity
    , public ICantAddAccountActivity
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnCancelButtonClicked(
        /* [in] */ IView* view);
};

} // namespace Accounts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_ACCOUNTS_CCANTADDACCOUNTACTIVITY_H__
