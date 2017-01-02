//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

