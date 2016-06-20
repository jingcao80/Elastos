
#include "elastos/apps/dialer/activity/TransactionSafeActivity.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Activity {

// TODO:
// CAR_INTERFACE_IMPL(TransactionSafeActivity, AnalyticsActivity, ITransactionSafeActivity)
CAR_INTERFACE_IMPL(TransactionSafeActivity, Object, ITransactionSafeActivity);

TransactionSafeActivity::~TransactionSafeActivity()
{}

ECode TransactionSafeActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    assert(0 && "TODO");
    // AnalyticsActivity::OnCreate(savedInstanceState);
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode TransactionSafeActivity::OnStart()
{
    assert(0 && "TODO");
    // AnalyticsActivity::OnStart();
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode TransactionSafeActivity::OnResume()
{
    assert(0 && "TODO");
    // AnalyticsActivity::OnResume();
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode TransactionSafeActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    assert(0 && "TODO");
    // AnalyticsActivity::OnSaveInstanceState(outState);
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
} // Apps
} // Elastos

