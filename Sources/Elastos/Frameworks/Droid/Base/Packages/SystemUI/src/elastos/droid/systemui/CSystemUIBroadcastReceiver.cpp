
#include "elastos/droid/systemui/CSystemUIBroadcastReceiver.h"
#include "elastos/droid/systemui/CSystemUIApplication.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CAR_OBJECT_IMPL(CSystemUIBroadcastReceiver);
ECode CSystemUIBroadcastReceiver::constructor(
    /* [in] */ ISystemUIApplication* host)
{
    mHost = (CSystemUIApplication*)host;
    return NOERROR;
}

ECode CSystemUIBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (mHost->mBootCompleted) return NOERROR;

    if (mHost->DEBUG) Logger::V(mHost->TAG, "BOOT_COMPLETED received");
    mHost->UnregisterReceiver(this);
    mHost->mBootCompleted = TRUE;
    if (mHost->mServicesStarted) {
        Int32 N = mHost->mServices->GetLength();
        for (Int32 i = 0; i < N; i++) {
            (*mHost->mServices)[i]->OnBootCompleted();
        }
    }

    return NOERROR;
}


} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
