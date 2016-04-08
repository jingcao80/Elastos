
#ifndef __CADDACCOUNTSETTINGS_H__
#define __CADDACCOUNTSETTINGS_H__

#include "_CAddAccountSettings.h"
#include "elastos/droid/app/Activity.h"
//#include "CAccountManagerCallback.h"
//#include "CAccountManagerFuture.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Accounts::IAccountManagerCallback;
using Elastos::Droid::Accounts::IAccountManagerFuture;
using Elastos::Droid::Os::IBundle;

namespace AccountsAndSyncSettings {
/*template<class T>
class CAccountManagerCallback;

template<class T>
class MyCAccountManagerCallback : public CAccountManagerCallback<T> {
    public:
        MyCAccountManagerCallback(CAddAccountSettings* owner);
        void Run(CAccountManagerFuture<T> future);
    private:
        CAddAccountSettings* mOwner;
};

template<>
class MyCAccountManagerCallback2 : public MyCAccountManagerCallback< AutoPtr<IBundle> > {
    public:
        MyCAccountManagerCallback2(CAddAccountSettings* owner);
        void Run(CAccountManagerFuture< AutoPtr<IBundle> > future);
    private:
        CAddAccountSettings* mOwner;
};*/

class MyCAccountManagerCallback
: public IAccountManagerCallback
, public ElRefBase
{
    public:
        CAR_INTERFACE_DECL()
        MyCAccountManagerCallback(CAddAccountSettings* owner);
        CARAPI Run(IAccountManagerFuture* future);
    private:
        CAddAccountSettings* mOwner;
};


class CAddAccountSettings : public Activity
{

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI AddAccount(
        /* [in] */ const String& accountType);

public:
    static const String EXTRA_SELECTED_ACCOUNT;
    static const String EXTRA_HAS_MULTIPLE_USERS;
    static const String KEY_ADD_CALLED;
    static const String KEY_CALLER_IDENTITY;
    static const String TAG;
    static const Int32 CHOOSE_ACCOUNT_REQUEST;
    static const Int32 ADD_ACCOUNT_REQUEST;
    AutoPtr<IPendingIntent> mPendingIntent;
    Boolean mAddAccountCalled;
};

} // namespace AccountsAndSyncSettings
#endif // __CADDACCOUNTSETTINGS_H__
