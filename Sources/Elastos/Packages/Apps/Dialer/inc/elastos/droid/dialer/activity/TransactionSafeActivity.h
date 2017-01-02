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

#ifndef __ELASTOS_DROID_DIALER_ACTIVITY_TRANSACTIONAILACTIVITY_H__
#define __ELASTOS_DROID_DIALER_ACTIVITY_TRANSACTIONAILACTIVITY_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/dialerbind/analytics/AnalyticsActivity.h"

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::DialerBind::Analytics::AnalyticsActivity;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Activity {

/**
 * A common superclass that keeps track of whether an {@link Activity} has saved its state yet or
 * not.
 */
class TransactionSafeActivity
    : public AnalyticsActivity
    , public ITransactionSafeActivity
{
public:
    CAR_INTERFACE_DECL();

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
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_ACTIVITY_TRANSACTIONAILACTIVITY_H__
