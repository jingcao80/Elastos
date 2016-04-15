#ifndef __ELASTOS_APPS_DIALER_ACTIVITY_CTRANSACTIONAILACTIVITY_H__
#define __ELASTOS_APPS_DIALER_ACTIVITY_CTRANSACTIONAILACTIVITY_H__

#include "_Elastos_Apps_Dialer_CTransactionSafeActivity.h"
#include <AnalyticsActivity.h>

using Elastos::Droid::Os::IBundle;

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Activity {

/**
 * A common superclass that keeps track of whether an {@link Activity} has saved its state yet or
 * not.
 */
CarClass(CTransactionSafeActivity)
    , public AnalyticsActivity
    , public ITransactionSafeActivity
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    /**
     * Returns true if it is safe to commit {@link FragmentTransaction}s at this time, based on
     * whether {@link Activity#onSaveInstanceState} has been called or not.
     *
     * Make sure that the current activity calls into
     * {@link super.onSaveInstanceState(Bundle outState)} (if that method is overridden),
     * so the flag is properly set.
     */
    CARAPI IsSafeToCommitTransactions(
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

private:
    Boolean mIsSafeToCommitTransactions;
};

} // Activity
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_ACTIVITY_CTRANSACTIONAILACTIVITY_H__
