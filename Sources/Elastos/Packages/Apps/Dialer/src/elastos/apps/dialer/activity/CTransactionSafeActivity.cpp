
#include "CTransactionSafeActivity.h"

namespace Elastos{
namespace Apps{
namespace Dialer {
namespace Activity {

CAR_INTERFACE_IMPL(CTransactionSafeActivity, AnalyticsActivity, ITransactionSafeActivity)

CAR_OBJECT_IMPL(CTransactionSafeActivity)

ECode CTransactionSafeActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    AnalyticsActivity::OnCreate(savedInstanceState);
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode CTransactionSafeActivity::OnStart()
{
    AnalyticsActivity::OnStart();
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode CTransactionSafeActivity::OnResume()
{
    AnalyticsActivity::OnResume();
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode CTransactionSafeActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    AnalyticsActivity::OnSaveInstanceState(outState);
    mIsSafeToCommitTransactions = TRUE;
    return NOERROR;
}

ECode CTransactionSafeActivity::IsSafeToCommitTransactions(
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

