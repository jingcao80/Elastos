
#ifndef __CMANAGEACCOUNTSSETTINGS_H__
#define __CMANAGEACCOUNTSSETTINGS_H__

#include "_CManageAccountsSettings.h"
#include "elastos/droid/app/Activity.h"
#include <malloc.h>
#include <pthread.h>
#include <stdio.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::App::IPendingIntent;

namespace AccountsAndSyncSettings {

class CManageAccountsSettings : public Activity
{
public:
    class MyListener
            : public ElRefBase
            , public IViewOnClickListener
        {
        public:
            MyListener(
                /* [in] */ CManageAccountsSettings* host);

            CAR_INTERFACE_DECL()

            CARAPI OnClick(
                /* [in] */ IView* v);

        private:
            CManageAccountsSettings* mActivity;
        };

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

private:
    CARAPI AddAccount(
        /* [in] */ const String& accountType);

    CARAPI GetAccounts();

    CARAPI UpdateAccount();

    CARAPI RemoveAccount();

    CARAPI GetPassword();

    CARAPI TestpThreadKey();
};

} // namespace AccountsAndSyncSettings
#endif // __CMANAGEACCOUNTSSETTINGS_H__
