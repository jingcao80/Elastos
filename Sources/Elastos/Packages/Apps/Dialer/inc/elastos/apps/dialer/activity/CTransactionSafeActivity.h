#ifndef __ELASTOS_APPS_DIALER_ACTIVITY_CTRANSACTIONAILACTIVITY_H__
#define __ELASTOS_APPS_DIALER_ACTIVITY_CTRANSACTIONAILACTIVITY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Apps_Dialer_Activity_CTransactionSafeActivity.h"
#include "elastos/apps/dialer/activity/TransactionSafeActivity.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace Activity {

CarClass(CTransactionSafeActivity)
    , public TransactionSafeActivity
{
public:
    CAR_OBJECT_DECL();
};

} // Activity
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_ACTIVITY_CTRANSACTIONAILACTIVITY_H__
