
#include "elastos/droid/dialer/activity/TransactionSafeActivity.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Activity {

CAR_INTERFACE_IMPL(TransactionSafeActivity, AnalyticsActivity, ITransactionSafeActivity)

ECode TransactionSafeActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(AnalyticsActivity::OnCreate(savedInstanceState));
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode TransactionSafeActivity::OnStart()
{
    FAIL_RETURN(AnalyticsActivity::OnStart());
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode TransactionSafeActivity::OnResume()
{
    FAIL_RETURN(AnalyticsActivity::OnResume());
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode TransactionSafeActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    FAIL_RETURN(AnalyticsActivity::OnSaveInstanceState(outState));
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode TransactionSafeActivity::IsSafeToCommitTransactions(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsSafeToCommitTransactions;
    return NOERROR;
}

} // Activity
} // Dialer
} // Droid
} // Elastos

